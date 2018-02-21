"""
Entering and exiting low power standby LPM unit test.
This puts the just the LPM into low power standby and then simulates a wake-up.
IPC messages are used to initiate the state changes and wait for key press events.
LPM tap is only used to verify entered state and simulate wake-up key press.

Note: This requires the RivieraLpmService's Python folder to be in the path.

Note: In order to use IPC communications you must follow the setup instructions 
in the README from the Python folder in RivieraLpmService.
"""

import pexpect
import pytest
import threading
import time

import lpmtestutils

import RivieraLpmService.lpm_client as LpmClient
import RivieraLpmService.AutoLpmServiceMessages_pb2 as AutoIPCMessages
import RivieraLpmService.LpmServiceMessages_pb2 as IPCMessages

from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm

_logger = get_logger(__file__)

_lpm_tap_port = pytest.config.getoption('--lpm-port')
if _lpm_tap_port is None:
    pytest.fail("LPM port is required: pytest -sv <test.py> --lpm_port </dev/tty.usb-foo>")

# Required parameter: ip address of IPC server.
_ip_address = pytest.config.getoption('--ip-address')
if _ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")

# Used to wait for asynchronus requests to come back.
_thread_lock = threading.Event()

# LPM tap client.
_lpm_tap = Lpm(_lpm_tap_port)

# The remote LPM Service with which we will communicate.
_ipc_client = LpmClient.LpmClient(_ip_address)
assert _ipc_client, "Failed to open LPM IPC interface at ip address %s" % _ip_address

# Used to verify that the state requested to enter.
_verifySystemStateId = None

# Waiting for key press to resume
_waitingForKeyPress = True

#
# Callbacks.
#

def set_system_state_cb(resp):
	"""
	Callback for SetSystemState IPC transaction.
	:param: resp Response from service in form of IpcLpmStateResponse_t.
	"""

	if _verifySystemStateId != None:
		assert (_verifySystemStateId == resp.sysState), \
		"System state %i does not match expected state %i" % (resp.sysState, _verifySystemStateId)

	_thread_lock.set()

def wake_on_key_event_cb(resp):
	"""
	Return to standby state. Normally the APQ would tell the LPM to change
	out of low power state. We play that role here.
	"""
	global _verifySystemStateId
	global _waitingForKeyPress

	if _waitingForKeyPress:

		_waitingForKeyPress = False
		_logger.info(resp)

		systemStateMsg = AutoIPCMessages.IpcLpmSystemStateSet_t()
		systemStateMsg.state = AutoIPCMessages.SYSTEM_STATE_STANDBY # SYSTEM_STATE_STANDBY

		_verifySystemStateId = systemStateMsg.state
		_ipc_client.SetSystemState(systemStateMsg, set_system_state_cb)

#
# Test entry points.
#

def test_low_power_standby():
	"""
	Enter standby state using IPC message, wait for key press, then resume.
	"""
	global _verifySystemStateId

	currentState = _lpm_tap.get_system_state()

	#
	# Reboot to put LPM in a good state, if necessary

	if not lpmtestutils.is_in_good_state_for_testing(_lpm_tap):
		_logger.info("LPM in incompatible state. Rebooting.")
		lpmtestutils.reboot_and_wait(_lpm_tap)
		time.sleep(5)

	assert (_lpm_tap.get_system_state() != Lpm.SystemState.LowPower), \
		"LPM alread in LowPower state. Test aborted"

	# Pretend we are the 
	_ipc_client.RegisterEvent(AutoIPCMessages.IPC_KEY, 
		AutoIPCMessages.IpcKeyInformation_t, wake_on_key_event_cb)

	#
	# Enter low power system state.

	_logger.info("Putting LPM in low power state.")

	systemStateMsg = AutoIPCMessages.IpcLpmSystemStateSet_t()
	systemStateMsg.state = AutoIPCMessages.SYSTEM_STATE_LOW_POWER # SYSTEM_STATE_LOW_POWER

	_thread_lock.clear()
	_verifySystemStateId = systemStateMsg.state
	_ipc_client.SetSystemState(systemStateMsg, set_system_state_cb)
	_thread_lock.wait(10)

	time.sleep(3)	# Give some time just in case.

	#
	# Verify low power system state.

	assert (_lpm_tap.wait_for_system_state([Lpm.SystemState.LowPower], 10)), \
		"Failed to enter LowPower system state."

	_logger.info("Waking LPM")

	#
	# Wake up.

	# Generate a fake key press. This will come in to us via IPC as if we are the APQ.
	_lpm_tap.button_tap(5, 100)

	#
	# Wait for and verify standby system state.

	assert(_lpm_tap.wait_for_system_state([Lpm.SystemState.Standby], 10)), \
		"Failed to resume into Standby system state."
