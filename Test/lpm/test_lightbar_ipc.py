# test_lightbar_ipc.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

"""
Unit tests for controlling the lightbar through the LPM using IPC.
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

import random
import threading
import time

import pytest

import AutoLpmServiceMessages_pb2 as AutoIPCMessages
import LpmServiceMessages_pb2 as IPCMessages
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

# Logging utility.
logger = get_logger(__file__)

# Used to wait for asynchronus requests to come back.
lbcs_xfer_event = threading.Event()

# Animation data that will be populated via a DB request.
# animation_data = None

# Used to verify that the anim we requested to be played was played.
verify_animation_id = None


def start_animation_request(animation_id, repeat, lpm_ipc):
    """
    Issue a start animation request on the LPM Service.

    :param: animId (integer) Id of the animation
    :param: repeatFlag (bool) True to play the animation repeatedly
    """
    animation = AutoIPCMessages.LBCSStartAnimation_t()
    animation.animationId = animation_id
    animation.immediateFlag = True
    animation.repeatFlag = repeat

    lbcs_xfer_event.clear()

    lpm_ipc.LBCSStartAnim(animation, start_animation_request_cb)

    lbcs_xfer_event.wait(10)


def abort_animation_request(lpm_ipc):
    """
    Issue an abort animation request on the LPM Service.
    """

    lbcs_xfer_event.clear()
    lpm_ipc.LBCSAbortAnimation(stop_animation_request_cb)	# Reusing stop cb since these are identical cases
    lbcs_xfer_event.wait(10)


def stop_animation_request(lpm_ipc):
    """
    Issue a stop animation request on the LPM Service.
    """
    lbcs_xfer_event.clear()
    lpm_ipc.LBCSStopAnimation(stop_animation_request_cb)
    lbcs_xfer_event.wait(10)


def get_animation_db_version_cb(resp):
    """
    Callback for LBCSAnimDBVersionReq.

    :param: resp Response from service in form of LBCSAnimationDBVersion_t.
    """
    logger.info("LBCS animation db version: {0}.{1} valid checksum: {2}"
                .format(resp.major, resp.minor, resp.animDBChecksumPassed))
    assert (resp.major > 0 or resp.minor > 0), "Invalid major/minor version of animation DB."
    assert resp.animDBChecksumPassed, "Animation DB 'checksum passed' field is false."

    lbcs_xfer_event.set()


def get_animation_db_data_cb(resp):
    """
    Callback for IPC_LBCS_DB_INDEX_DATA_RESP.

    :param: resp Response from sevice in form of LBCSDbIndexDataResp_t
    """
    # global animation_data

    assert resp, "No valid animation data response received."
    animation_data = resp
    print(animation_data)
    lbcs_xfer_event.set()


def start_animation_request_cb(resp):
    """
    Start animation command completed successfully.
    """
    global verify_animation_id

    if verify_animation_id is not None:
        assert (resp.animationId == verify_animation_id), \
            "Current animation ID %i does not match expected %i for 'start animation'." \
            % (resp.animationId, verify_animation_id)
        verify_animation_id = None

    lbcs_xfer_event.set()


def stop_animation_request_cb(resp):
    """
    Stop animation command completed successfully.
    Note that this cb is used for both "stop" and "abort".
    """
    global verify_animation_id

    if verify_animation_id is not None:
        assert (resp.animationId == verify_animation_id), \
            "Current animation ID %i does not match expected %i for 'stop animation'." \
            % (resp.animationId, verify_animation_id)
        verify_animation_id = None

    lbcs_xfer_event.set()


@pytest.mark.usefixtures('lpm_ipc_client', 'riviera_enabled_ipc')
def test_get_animation_db_version(lpm_ipc_client):
    """
    Request animation DB version and status.
    Waits for request to complete using _lbcs_xfer_event.
    """

    lbcs_xfer_event.clear()
    lpm_ipc_client.LBCSAnimDBVersionReq(get_animation_db_version_cb)
    lbcs_xfer_event.wait(10)


@pytest.mark.usefixtures('riviera_enabled_ipc', 'lpm_ipc_client')
def test_get_animation_db_data(lpm_ipc_client):
    """
    Request animation DB data.
    Waits for request to complete using _lbcs_xfer_event.

    :param client:
    """

    lpm_ipc_client.RegisterEvent(AutoIPCMessages.IPC_LBCS_DB_INDEX_DATA_RESP,
                          IPCMessages.LBCSDbIndexDataResp_t,
                          get_animation_db_data_cb)

    lbcs_xfer_event.clear()
    lpm_ipc_client.LBCSGetDBIndexData()
    lbcs_xfer_event.wait(10)


@pytest.mark.usefixtures('animation_data', 'lpm_ipc_client')
def test_stop_animation(animation_data, lpm_ipc_client):
    """
    Test that the stop animation command works correctly.

    :param client:
    """
    global verify_animation_id

    assert animation_data, "No animation data loaded."

    rndAnimationId = random.randint(1, len(animation_data.indexEntries))

    verify_animation_id = rndAnimationId
    start_animation_request(rndAnimationId, True, lpm_ipc_client)
    time.sleep(1.0)

    verify_animation_id = rndAnimationId
    stop_animation_request(lpm_ipc_client)


@pytest.mark.usefixtures('riviera_enabled_ipc', 'lpm_ipc_client', 'animation_data')
def test_abort_animation(animation_data, lpm_ipc_client):
    """
    Test that the abort animation command works correctly.
    """
    global verify_animation_id

    assert animation_data, "No animation data loaded."

    rndAnimationId = random.randint(1, len(animation_data.indexEntries))

    verify_animation_id = rndAnimationId
    start_animation_request(rndAnimationId, True, lpm_ipc_client)
    time.sleep(1.0)

    verify_animation_id = rndAnimationId
    abort_animation_request(lpm_ipc_client)


@pytest.mark.usefixtures('riviera_enabled_ipc', 'animation_data', 'lpm_ipc_client')
def test_play_all_animations(animation_data, lpm_ipc_client):
    """
    Starts every animation and plays it for a few seconds.
    _animationData must be previously populate (ideally via test_get_animation_db_version())
    """
    global verify_animation_id

    assert animation_data, "No animation data loaded."

    lastAnimationId = None

    for entry in animation_data.indexEntries:
        logger.info("Playing animation {0}: \"{1}\"".format(entry.animationId, entry.animationName))

        verify_animation_id = entry.animationId
        start_animation_request(entry.animationId, True, lpm_ipc_client)

        time.sleep(3.0)

    # Wait a bit for things to settle down before clearing this animation.
    time.sleep(2.0)
    stop_animation_request(lpm_ipc_client)


@pytest.mark.usefixtures('animation_data', 'lpm_ipc_client', 'riviera_enabled_ipc')
def test_play_all_animations_super_fast(animation_data, lpm_ipc_client):
    """
    Starts every animation and plays it for a few seconds.
    _animationData must be previously populate (ideally via test_get_animation_db_version())
    """
    global verify_animation_id

    assert animation_data, "No animation data loaded."

    for entry in animation_data.indexEntries:
        logger.info("Playing animation {0}: \"{1}\"".format(entry.animationId, entry.animationName))

        verify_animation_id = entry.animationId
        start_animation_request(entry.animationId, True, lpm_ipc_client)

        # 75MS is the minimum time it takes to load the longest animation.
        time.sleep(0.075)

    # Wait a bit for things to settle down before clearing this animation.
    time.sleep(2.0)
    stop_animation_request(lpm_ipc_client)
