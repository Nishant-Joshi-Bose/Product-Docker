"""
Entering and exiting low power standby integration test.
This puts the entire system into low power standby and then wakes it up.

This test requires connections to both LPM (via tap) and APQ (via adb).
"""

import pexpect
import pytest
import time

import lpmtestutils

from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm
from ApqTap import ApqTap

_logger = get_logger(__file__)

_ip_address = pytest.config.getoption('--ip-address')
if _ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")

_lpm_tap_port = pytest.config.getoption('--lpm-port')
if _lpm_tap_port is None:
    pytest.fail("LPM port is required: pytest -sv <test.py> --lpm_port </dev/tty.usb-foo>")

# LPM tap client.
_lpm_tap = Lpm(_lpm_tap_port)


def test_low_power_standby():
	"""
	Enter system into low power standby state from APQ tap.
	"""

	#
	# Reboot to put LPM in a good state, if necessary

	if not lpmtestutils.is_in_good_state_for_testing(_lpm_tap):
		_logger.info("LPM in incompatible state. Rebooting.")
		lpmtestutils.reboot_and_wait(_lpm_tap)
		time.sleep(5)

	# Need to wait some more time before low power will work.
	time.sleep(5)

	_logger.info("Entering Low Power Standby...")
	# Use APQ tap to put the system into low power.
	with ApqTap(_ip_address, 'adb') as tap:
		tap.send("lowpower suspend", "Success")

	time.sleep(10)

	assert _lpm_tap.wait_for_system_state([Lpm.SystemState.LowPower], 10)

	# Simulate a key press to wake it up.
	_logger.info("Simulating a key press...")
	_lpm_tap.button_tap(5, 100)

	# Verify we are back in standby state.
	_logger.info("Waiting to resume...")
	assert _lpm_tap.wait_for_system_state([Lpm.PowerState.Standby], 10)
