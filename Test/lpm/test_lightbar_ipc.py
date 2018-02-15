import pytest
import random
import threading
import time

import RivieraLpmService.lpm_client as LpmClient
import RivieraLpmService.AutoLpmServiceMessages_pb2 as AutoIPCMessages
import RivieraLpmService.LpmServiceMessages_pb2 as IPCMessages

from CastleTestUtils.LoggerUtils.log_setup import get_logger

# Required parameter: ip address of IPC server.
__ip_address = pytest.config.getoption('--ip-address')
if __ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")

# Logging utility.
__logger = get_logger(__file__)

# Used to wait for asynchronus requests to come back.
__lbcs_xfer_event = threading.Event()

# The remote LPM Service with which we will communicate.
__client = LpmClient.LpmClient(__ip_address)
assert (__client)

# Animation data that will be populated via a DB request.
__animationData = None

# Used to verify that the anim we requested to be played was played.
__verifyAnimationId = None;

#
# Utilities and helpers.
#

def start_animation_request(animId, repeatFlag):
	"""
	Issue a start animation request on the LPM Service.
	:param: animId (integer) Id of the animation 
	:param: repeatFlag (bool) True to play the animation repeatedly
	"""
	anim 				= AutoIPCMessages.LBCSStartAnimation_t()
	anim.animationId 	= animId
	anim.immediateFlag 	= True
	anim.repeatFlag 	= repeatFlag

	__lbcs_xfer_event.clear()

	__client.LBCSStartAnim(anim, start_animation_request_cb)

	__lbcs_xfer_event.wait(10)

def abort_animation_request():
	"""
	Issue an abort animation request on the LPM Service.
	"""

	__lbcs_xfer_event.clear()
	__client.LBCSAbortAnimation(stop_animation_request_cb)	# Reusing stop cb since these are identical cases
	__lbcs_xfer_event.wait(10)

def stop_animation_request():
	"""
	Issue a stop animation request on the LPM Service.
	"""

	__lbcs_xfer_event.clear()
	__client.LBCSStopAnimation(stop_animation_request_cb)
	__lbcs_xfer_event.wait(10)

#
# Callbacks.
#

def get_animation_db_version_cb(resp):
	"""
	Callback for LBCSAnimDBVersionReq.
	:param: resp Response from service in form of LBCSAnimationDBVersion_t.
	"""
	print("LBCS animation db version: {0}.{1} valid checksum: {2}"
		.format(resp.major, resp.minor, resp.animDBChecksumPassed))
	assert (resp.major > 0 or resp.minor > 0)
	assert (resp.animDBChecksumPassed)

	__lbcs_xfer_event.set()

def get_animation_db_data_cb(resp):
	"""
	Callback for IPC_LBCS_DB_INDEX_DATA_RESP.
	:param: resp Response from sevice in form of LBCSDbIndexDataResp_t
	"""
	global __animationData

	assert (resp)
	__animationData = resp
	print(__animationData)
	__lbcs_xfer_event.set()

def start_animation_request_cb(resp):
	"""
	Start animation command completed successfully.
	"""
	global __verifyAnimationId

	if __verifyAnimationId != None:
		assert (resp.animationId == __verifyAnimationId)
		__verifyAnimationId = None

	__lbcs_xfer_event.set()

def stop_animation_request_cb(resp):
	"""
	Stop animation command completed successfully.
	Note that this cb is used for both "stop" and "abort".
	"""
	global __verifyAnimationId

	if __verifyAnimationId != None:
		assert (resp.animationId == __verifyAnimationId)
		__verifyAnimationId = None

	__lbcs_xfer_event.set()

#
# Test entry points.
#

def test_get_animation_db_version():
	"""
	Request animation DB version and status.
	Waits for request to complete using __lbcs_xfer_event.
	"""

	__lbcs_xfer_event.clear()
	__client.LBCSAnimDBVersionReq(get_animation_db_version_cb)
	__lbcs_xfer_event.wait(10)

def test_get_animation_db_data():
	"""
	Request animation DB data.
	Waits for request to complete using __lbcs_xfer_event.
	"""
	client = LpmClient.LpmClient(__ip_address)
	assert (client)

	__client.RegisterEvent(AutoIPCMessages.IPC_LBCS_DB_INDEX_DATA_RESP, 
	IPCMessages.LBCSDbIndexDataResp_t, get_animation_db_data_cb)

	__lbcs_xfer_event.clear()
	__client.LBCSGetDBIndexData()
	__lbcs_xfer_event.wait(10)

def test_stop_animation():
	"""
	Test that the stop animation command works correctly.
	"""
	global __verifyAnimationId

	assert (__animationData)

	rndAnimationId = random.randint(1, len(__animationData.indexEntries))

	__verifyAnimationId = rndAnimationId
	start_animation_request(rndAnimationId, True)
	time.sleep(1.0)

	__verifyAnimationId = rndAnimationId
	stop_animation_request()

def test_abort_animation():
	"""
	Test that the abort animation command works correctly.
	"""
	global __verifyAnimationId

	assert (__animationData)

	rndAnimationId = random.randint(1, len(__animationData.indexEntries))

	__verifyAnimationId = rndAnimationId
	start_animation_request(rndAnimationId, True)
	time.sleep(1.0)

	__verifyAnimationId = rndAnimationId
	abort_animation_request()

@pytest.mark.skip("")
def test_play_all_animations():
	"""
	Starts every animation and plays it for a few seconds.
	__animationData must be previously populate (ideally via test_get_animation_db_version())
	"""
	global __verifyAnimationId

	assert (__animationData)

	lastAnimationId = None

	for entry in __animationData.indexEntries:
		__logger.info("Playing animation {0}: \"{1}\"".format(entry.animationId, entry.animationName))
		
		__verifyAnimationId = entry.animationId
		start_animation_request(entry.animationId, True)
		
		time.sleep(3.0)

	stop_animation_request()

def test_play_all_animations_super_fast():
	"""
	Starts every animation and plays it for a few seconds.
	__animationData must be previously populate (ideally via test_get_animation_db_version())
	"""
	global __verifyAnimationId

	assert (__animationData)

	for entry in __animationData.indexEntries:
		__logger.info("Playing animation {0}: \"{1}\"".format(entry.animationId, entry.animationName))

		__verifyAnimationId = entry.animationId
		start_animation_request(entry.animationId, True)

		time.sleep(0.075)

	time.sleep(1.0)
	stop_animation_request()
