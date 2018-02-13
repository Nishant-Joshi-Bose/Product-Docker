"""
Examples of using Python hooks for executing LPM TAP commands on Eddie.
"""

import pytest
import time
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm

__logger = get_logger(__file__)

__lpmPort = pytest.config.getoption('--lpm_port')
if __lpmPort is None:
    pytest.fail("LPM port is required: pytest -sv <test.py> --lmp_port </dev/tty.usb-foo>")

__lpm = Lpm(__lpmPort)


@pytest.mark.skip("Example/test functionality")
def test_amp_fault_on():
	""" 
	Manually set LPM into an amp fault state.
	"""
	lpmVersion = __lpm.get_version()
	assert lpmVersion
	__logger.info("LPM found at version %s", lpmVersion)

	__lpm.amp_fault_set_state(True)

#@pytest.mark.skip("Example/test functionality")
def test_amp_fault_induce():
	""" 
	Induce an amp fault.
	"""
	lpmVersion = __lpm.get_version()
	assert lpmVersion
	__logger.info("LPM found at version %s", lpmVersion)

	__logger.info("Please wait. Inducing an amp fault takes about 15 seconds...")
	__lpm.amp_fault_induce()
	time.sleep(2) # amp fault will block until it is done, but just in case, let's wait a bit
	system_state = __lpm.get_system_state()
	assert (system_state == Lpm.SystemState.Error)
	# Amp fault lasts until reboot
	__lpm.reboot()

@pytest.mark.skip("Example/test functionality")
def test_low_power_standby():
	"""
	Use a TAP command to put the LPM in low power standby.
	This only effects the LPM.
	"""
	originalState = __lpm.get_power_state()
	__logger.info("Current power state: %s", originalState)

	__lpm.set_power_state("LowPower")
	# Wait up to 2 seconds to enter LowPower state. Note that this only effects
	# the "power state" on the LPM and will not fully enter low power state
	# for the whole device.
	__lpm.wait_for_power_state(["LowPower"], 2)

	__logger.info("Now in LowPower state")

	# Restore the original state.
	__lpm.set_power_state(originalState)
	__lpm.wait_for_power_state([originalState], 2)

	__logger.info("Restored power state: %s", originalState)

@pytest.mark.skip("Example/test functionality")
def test_some_keys():
	""" 
	Test some key events. 
	These route down to the PSoC so it tests the entire chain up from PSoC through SoC.
	"""

	lpmVersion = __lpm.get_version()
	assert lpmVersion
	__logger.info("LPM found at version %s", lpmVersion)

	#
	# Button examples
	#

	# Volume down twice. 100 ms between down and up of the key. 
	# Events with too small of an activation time will be ignored as noise.
	__lpm.button_tap(5, 100)
	__lpm.button_tap(5, 100)

	# Hold volume up
	__lpm.button_down(3)
	time.sleep(5)
	__lpm.button_up(3)

	#
	# Slider examples
	# See https://wiki.bose.com/display/WSSW/Capsense+Strip+Input+Handling for mapping
	# of slider x position to presets on Eddie
	#

	# Activate preset 2
	__lpm.slider_tap(15, 100);

	# Start on preset 2, drag to preset 3. Should do nothing.
	__lpm.slider_down(15)
	__lpm.slider_move(25)
	__lpm.slider_move(35)
	__lpm.slider_move(40)
	__lpm.slider_up(45)

	# Start on preset2, drag around in 2, hold to store preset
	__lpm.slider_down(15)
	time.sleep(0.5)
	__lpm.slider_move(21)
	time.sleep(0.5)
	__lpm.slider_move(23)
	time.sleep(1)
	__lpm.slider_move(30)
	time.sleep(1)
	__lpm.slider_up(28)

