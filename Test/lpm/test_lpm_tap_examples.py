"""
Examples of using Python hooks for executing LPM TAP commands on Eddie.
"""

import pytest
import time
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm

_logger = get_logger(__file__)

_lpm_port = pytest.config.getoption('--lpm-port')
if _lpm_port is None:
    pytest.fail("LPM port is required: pytest -sv <test.py> --lpm_port </dev/tty.usb-foo>")

_lpm_tap = Lpm(_lpm_port)


@pytest.mark.skip("Example/test functionality")
def test_amp_fault_on():
	""" 
	Manually set LPM into an amp fault state.
	"""
	lpmVersion = _lpm_tap.get_version()
	assert lpmVersion
	_logger.info("LPM found at version %s", lpmVersion)

	_lpm_tap.amp_fault_set_state(True)

@pytest.mark.skip("Example/test functionality")
def test_amp_fault_induce():
	""" 
	Induce an amp fault.
	"""
	lpmVersion = _lpm_tap.get_version()
	assert lpmVersion
	_logger.info("LPM found at version %s", lpmVersion)

	_logger.info("Please wait. Inducing an amp fault takes about 15 seconds...")
	_lpm_tap.amp_fault_induce()
	time.sleep(2) # amp fault will block until it is done, but just in case, let's wait a bit
	system_state = _lpm_tap.get_system_state()
	assert (system_state == Lpm.SystemState.Error)
	# Amp fault lasts until reboot
	_lpm_tap.reboot()

@pytest.mark.skip("Example/test functionality")
def test_low_power_standby():
	"""
	Use a TAP command to put the LPM in low power standby.
	This only effects the LPM.
	"""
	originalState = _lpm_tap.get_power_state()
	_logger.info("Current power state: %s", originalState)

	_lpm_tap.set_power_state("LowPower")
	# Wait up to 2 seconds to enter LowPower state. Note that this only effects
	# the "power state" on the LPM and will not fully enter low power state
	# for the whole device.
	_lpm_tap.wait_for_power_state(["LowPower"], 2)

	_logger.info("Now in LowPower state")

	# Restore the original state.
	_lpm_tap.set_power_state(originalState)
	_lpm_tap.wait_for_power_state([originalState], 2)

	_logger.info("Restored power state: %s", originalState)

#@pytest.mark.skip("Example/test functionality")
def test_some_keys():
	""" 
	Test some key events. 
	These route down to the PSoC so it tests the entire chain up from PSoC through SoC.
	"""

	lpmVersion = _lpm_tap.get_version()
	assert lpmVersion
	_logger.info("LPM found at version %s", lpmVersion)

	#
	# Button examples
	#

	# Volume down twice. 100 ms between down and up of the key. 
	# Events with too small of an activation time will be ignored as noise.
	_lpm_tap.button_tap(5, 100)
	_lpm_tap.button_tap(5, 100)

	# Hold volume up
	_lpm_tap.button_down(3)
	time.sleep(5)
	_lpm_tap.button_up(3)

	#
	# Slider examples
	# See https://wiki.bose.com/display/WSSW/Capsense+Strip+Input+Handling for mapping
	# of slider x position to presets on Eddie
	#

	# Activate preset 2
	_lpm_tap.slider_tap(15, 100);

	# Start on preset 2, drag to preset 3. Should do nothing.
	_lpm_tap.slider_down(15)
	_lpm_tap.slider_move(25)
	_lpm_tap.slider_move(35)
	_lpm_tap.slider_move(40)
	_lpm_tap.slider_up(45)

	# Start on preset2, drag around in 2, hold to store preset
	_lpm_tap.slider_down(15)
	time.sleep(0.5)
	_lpm_tap.slider_move(21)
	time.sleep(0.5)
	_lpm_tap.slider_move(23)
	time.sleep(1)
	_lpm_tap.slider_move(30)
	time.sleep(1)
	_lpm_tap.slider_up(28)

