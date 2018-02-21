"""
Some miscellaneous utility functions used by the scriptis in this folder.
"""

import time

from CastleTestUtils.LpmUtils.Lpm import Lpm

def is_in_good_state_for_testing(lpm_tap):
	"""
	Returns true If the LPM is in a good system state for testing.
	:param: lpm_tap A connection to the LPM tap via CastleTestUtils.LpmUtils.Lpm
	"""
	s = lpm_tap.get_system_state()
	return (s == Lpm.SystemState.Standby 
		or s == Lpm.SystemState.Idle
		or s == Lpm.SystemState.On)

def reboot_and_wait(lpm_tap):
	"""
	Reboots the LPM and waits for it to get into a good state.
	:param: lpm_tap A connection to the LPM tap via CastleTestUtils.LpmUtils.Lpm
	"""

	lpm_tap.reboot()

	retries = 6
	wait_time = 5

	while (retries > 0):
		retries = retries - 1
		time.sleep(wait_time)

		if is_in_good_state_for_testing(lpm_tap):
			break
		elif retries == 0:
			assert (False), "Timeout waiting for LPM to reboot."