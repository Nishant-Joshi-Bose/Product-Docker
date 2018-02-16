"""
Unit tests for constrolling the lightbar through the LPM using IPC.
Use this to verify that lightbar animations work from the LPM on down.

Tested in this script:
	* Retrieve animations DB version and integrity
	* Retrieve animation DB data
	* Stop animation
	* Abort animation
	* Play all animations (3s between each) for visual inspection
	* Play all animations with no delay for brute force test

Note: This requires the RivieraLpmService's Python folder to be in the path.

Note: In order to use IPC communications you must follow the setup instructions 
in the README from the Python folder in RivieraLpmService.
"""

import pytest
import random
import threading
import time

import RivieraLpmService.lpm_client as LpmClient
import RivieraLpmService.AutoLpmServiceMessages_pb2 as AutoIPCMessages
import RivieraLpmService.LpmServiceMessages_pb2 as IPCMessages

from CastleTestUtils.LoggerUtils.log_setup import get_logger

# Required parameter: ip address of IPC server.
_ip_address = pytest.config.getoption('--ip-address')
if _ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")

# Logging utility.
_logger = get_logger(__file__)

# Used to wait for asynchronus requests to come back.
_lbcs_xfer_event = threading.Event()

# The remote LPM Service with which we will communicate.
_lpm_ipc = LpmClient.LpmClient(_ip_address)
assert (_lpm_ipc)

# Animation data that will be populated via a DB request.
_animationData = None

# Used to verify that the anim we requested to be played was played.
_verifyAnimationId = None;

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

	_lbcs_xfer_event.clear()

	_lpm_ipc.LBCSStartAnim(anim, start_animation_request_cb)

	_lbcs_xfer_event.wait(10)

def abort_animation_request():
	"""
	Issue an abort animation request on the LPM Service.
	"""

	_lbcs_xfer_event.clear()
	_lpm_ipc.LBCSAbortAnimation(stop_animation_request_cb)	# Reusing stop cb since these are identical cases
	_lbcs_xfer_event.wait(10)

def stop_animation_request():
	"""
	Issue a stop animation request on the LPM Service.
	"""

	_lbcs_xfer_event.clear()
	_lpm_ipc.LBCSStopAnimation(stop_animation_request_cb)
	_lbcs_xfer_event.wait(10)

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

	_lbcs_xfer_event.set()

def get_animation_db_data_cb(resp):
	"""
	Callback for IPC_LBCS_DB_INDEX_DATA_RESP.
	:param: resp Response from sevice in form of LBCSDbIndexDataResp_t
	"""
	global _animationData

	assert (resp)
	_animationData = resp
	print(_animationData)
	_lbcs_xfer_event.set()

def start_animation_request_cb(resp):
	"""
	Start animation command completed successfully.
	"""
	global _verifyAnimationId

	if _verifyAnimationId != None:
		assert (resp.animationId == _verifyAnimationId)
		_verifyAnimationId = None

	_lbcs_xfer_event.set()

def stop_animation_request_cb(resp):
	"""
	Stop animation command completed successfully.
	Note that this cb is used for both "stop" and "abort".
	"""
	global _verifyAnimationId

	if _verifyAnimationId != None:
		assert (resp.animationId == _verifyAnimationId)
		_verifyAnimationId = None

	_lbcs_xfer_event.set()

#
# Test entry points.
#

def test_get_animation_db_version():
	"""
	Request animation DB version and status.
	Waits for request to complete using _lbcs_xfer_event.
	"""

	_lbcs_xfer_event.clear()
	_lpm_ipc.LBCSAnimDBVersionReq(get_animation_db_version_cb)
	_lbcs_xfer_event.wait(10)

def test_get_animation_db_data():
	"""
	Request animation DB data.
	Waits for request to complete using _lbcs_xfer_event.
	"""
	client = LpmClient.LpmClient(_ip_address)
	assert (client)

	_lpm_ipc.RegisterEvent(AutoIPCMessages.IPC_LBCS_DB_INDEX_DATA_RESP, 
	IPCMessages.LBCSDbIndexDataResp_t, get_animation_db_data_cb)

	_lbcs_xfer_event.clear()
	_lpm_ipc.LBCSGetDBIndexData()
	_lbcs_xfer_event.wait(10)

def test_stop_animation():
	"""
	Test that the stop animation command works correctly.
	"""
	global _verifyAnimationId

	assert (_animationData)

	rndAnimationId = random.randint(1, len(_animationData.indexEntries))

	_verifyAnimationId = rndAnimationId
	start_animation_request(rndAnimationId, True)
	time.sleep(1.0)

	_verifyAnimationId = rndAnimationId
	stop_animation_request()

def test_abort_animation():
	"""
	Test that the abort animation command works correctly.
	"""
	global _verifyAnimationId

	assert (_animationData)

	rndAnimationId = random.randint(1, len(_animationData.indexEntries))

	_verifyAnimationId = rndAnimationId
	start_animation_request(rndAnimationId, True)
	time.sleep(1.0)

	_verifyAnimationId = rndAnimationId
	abort_animation_request()

def test_play_all_animations():
	"""
	Starts every animation and plays it for a few seconds.
	_animationData must be previously populate (ideally via test_get_animation_db_version())
	"""
	global _verifyAnimationId

	assert (_animationData)

	lastAnimationId = None

	for entry in _animationData.indexEntries:
		_logger.info("Playing animation {0}: \"{1}\"".format(entry.animationId, entry.animationName))
		
		_verifyAnimationId = entry.animationId
		start_animation_request(entry.animationId, True)
		
		time.sleep(3.0)

	# Wait a bit for things to settle down before clearing this animation.
	time.sleep(2.0)
	stop_animation_request()

def test_play_all_animations_super_fast():
	"""
	Starts every animation and plays it for a few seconds.
	_animationData must be previously populate (ideally via test_get_animation_db_version())
	"""
	global _verifyAnimationId

	assert (_animationData)

	for entry in _animationData.indexEntries:
		_logger.info("Playing animation {0}: \"{1}\"".format(entry.animationId, entry.animationName))

		_verifyAnimationId = entry.animationId
		start_animation_request(entry.animationId, True)

		# 75MS is the minimum time it takes to load the longest animation.
		time.sleep(0.075)

	# Wait a bit for things to settle down before clearing this animation.
	time.sleep(2.0)
	stop_animation_request()
