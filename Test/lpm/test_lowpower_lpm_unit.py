"""
Entering and exiting low power standby LPM unit test.
This puts the just the LPM into low power standby and then simulates a wake-up.
IPC messages are used to initiate the state changes and wait for key press events.
LPM tap is only used to verify entered state and simulate wake-up key press.
"""

import pexpect
import pytest
import threading
import time

import RivieraLpmService.lpm_client as LpmClient
import RivieraLpmService.AutoLpmServiceMessages_pb2 as AutoIPCMessages
import RivieraLpmService.LpmServiceMessages_pb2 as IPCMessages

from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm

__logger = get_logger(__file__)

__lpm_tap_port = pytest.config.getoption('--lpm_port')
if __lpm_tap_port is None:
    pytest.fail("LPM port is required: pytest -sv <test.py> --lpm_port </dev/tty.usb-foo>")

# Required parameter: ip address of IPC server.
__ip_address = pytest.config.getoption('--ip-address')
if __ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")

# Used to wait for asynchronus requests to come back.
__thread_lock = threading.Event()

# LPM tap client.
__lpm_tap = Lpm(__lpm_tap_port)

# The remote LPM Service with which we will communicate.
__ipc_client = LpmClient.LpmClient(__ip_address)
assert __ipc_client

# Used to verify that the state requested to enter.
__verifySystemStateId = None

# Waiting for key press to resume
__waitingForKeyPress = True

#
# Utilities.
#

def is_in_good_state_for_testing(lpm_tap):
	"""
	Returns true If the LPM is in a good system state for testing.
	"""
	s = lpm_tap.get_system_state()
	return (s == Lpm.SystemState.Standby 
		or s == Lpm.SystemState.Idle
		or s == Lpm.SystemState.On)

def reboot_and_wait(lpm_tap):
	"""
	Reboots the LPM and waits for it to get into a good state.
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

#
# Callbacks.
#

def set_system_state_cb(resp):
	"""
	Callback for SetSystemState IPC transaction.
	:param: resp Response from service in form of IpcLpmStateResponse_t.
	"""

	if __verifySystemStateId != None:
		assert (__verifySystemStateId == resp.sysState)

	__thread_lock.set()

def wake_on_key_event_cb(resp):
	"""
	Return to standby state. Normally the APQ would tell the LPM to change
	out of low power state. We play that role here.
	"""
	global __verifySystemStateId
	global __waitingForKeyPress

	if __waitingForKeyPress:

		__waitingForKeyPress = False
		__logger.info(resp)

		systemStateMsg = AutoIPCMessages.IpcLpmSystemStateSet_t()
		systemStateMsg.state = AutoIPCMessages.SYSTEM_STATE_STANDBY # SYSTEM_STATE_STANDBY

		__verifySystemStateId = systemStateMsg.state
		__ipc_client.SetSystemState(systemStateMsg, set_system_state_cb)

#
# Test entry points.
#

def test_low_power_standby():
	"""
	Enter standby state using IPC message, wait for key press, then resume.
	"""
	global __verifySystemStateId

	currentState = __lpm_tap.get_system_state()

	#
	# Reboot to put LPM in a good state, if necessary

	if not is_in_good_state_for_testing(__lpm_tap):
		__logger.info("LPM in incompatible state. Rebooting.")
		reboot_and_wait(__lpm_tap)
		time.sleep(5)

	assert (__lpm_tap.get_system_state() != Lpm.SystemState.LowPower), \
		"LPM alread in LowPower state. Test aborted"

	# Pretend we are the 
	__ipc_client.RegisterEvent(AutoIPCMessages.IPC_KEY, 
		AutoIPCMessages.IpcKeyInformation_t, wake_on_key_event_cb)

	#
	# Enter low power system state.

	__logger.info("Putting LPM in low power state.")

	systemStateMsg = AutoIPCMessages.IpcLpmSystemStateSet_t()
	systemStateMsg.state = AutoIPCMessages.SYSTEM_STATE_LOW_POWER # SYSTEM_STATE_LOW_POWER

	__thread_lock.clear()
	__verifySystemStateId = systemStateMsg.state
	__ipc_client.SetSystemState(systemStateMsg, set_system_state_cb)
	__thread_lock.wait(10)

	time.sleep(3)	# Give some time just in case.

	#
	# Verify low power system state.

	assert (__lpm_tap.wait_for_system_state([Lpm.SystemState.LowPower], 10))

	__logger.info("Waking LPM")

	#
	# Wake up.

	# Generate a fake key press. This will come in to us via IPC as if we are the APQ.
	__lpm_tap.button_tap(5, 100)

	#
	# Wait for and verify standby system state.

	assert(__lpm_tap.wait_for_system_state([Lpm.SystemState.Standby], 10))
