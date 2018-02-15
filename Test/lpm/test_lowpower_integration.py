"""
Entering and exiting low power standby integration test.
This puts the entire system into low power standby and then wakes it up.
"""

import pexpect
import pytest
import time

from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm
from ApqTap import ApqTap

__logger = get_logger(__file__)

__ip_address = pytest.config.getoption('--ip-address')
if __ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")

__lpm_port = pytest.config.getoption('--lpm_port')
if __lpm_port is None:
    pytest.fail("LPM port is required: pytest -sv <test.py> --lpm_port </dev/tty.usb-foo>")

# LPM tap client.
__lpm = Lpm(__lpm_port)


def test_low_power_standby():
	"""
	Enter system into low power standby state from APQ tap.
	"""

	if __lpm.get_system_state() != Lpm.SystemState.On:
		# Start fresh from standby state.
		__logger.info("Rebooting LPM and waiting for On state...")
		__lpm.reboot()
		time.sleep(30)
		assert __lpm.wait_for_system_state([Lpm.SystemState.On], 10)

	# Need to wait some more time before low power will work.
	time.sleep(5)

	__logger.info("Entering Low Power Standby...")
	# Use APQ tap to put the system into low power.
	with ApqTap(__ip_address, 'adb') as tap:
		tap.send("lowpower suspend", "Success")

	time.sleep(10)

	assert __lpm.wait_for_system_state([Lpm.SystemState.LowPower], 10)

	# Simulate a key press to wake it up.
	__logger.info("Simulating a key press...")
	__lpm.button_tap(5, 100)

	# Verify we are back in standby state.
	__logger.info("Waiting to resume into On state...")
	assert __lpm.wait_for_system_state([Lpm.PowerState.On], 10)
