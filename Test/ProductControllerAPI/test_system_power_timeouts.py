# coding=utf-8
# test_system_power_timeouts.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
"""
PyTest based validation tests for the "/system/power/timeouts" API.
"""
import time
import json
import os
import pytest
import eddie_helper
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

LOGGER = get_logger(os.path.basename(__file__))


@pytest.mark.usefixtures('front_door_queue', 'device_in_aux')
def test_system_power_timeouts_errors(front_door_queue):
    """
    Test for PUT method of system power timeouts api with negative scenarios.
    Test Steps:
    1. Check that endpoint is returned in capabilities.
    2. Validate error request by sending invalid key.
    3. Validate error request by sending invalid value of noAudio.
    4. Validate error request by sending invalid value of noVideo.
    """
    LOGGER.info("Testing set invalid system power timeouts")

    # 1. Check that endpoint is returned in capabilities.
    eddie_helper.check_if_end_point_exists(front_door_queue, eddie_helper.SYSTEM_POWER_TIMEOUTS_API)

    # 2. Validate error request by sending invalid key.
    data_request = dict()
    data_request["notValid"] = False
    data = json.dumps(data_request)

    response = front_door_queue.setSystemPowerTimeouts(data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_TIMEOUTS_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_ERROR, is_error=True)
    # Verify error subcode which should be "2005".
    assert response["error"]["subcode"] == eddie_helper.SUBCODE_INVALID_KEY, \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])
    time.sleep(2)

    # 3. Validate error request by sending invalid value of noAudio.
    data_request = dict()
    data_request["noAudio"] = "invalidValue"
    data = json.dumps(data_request)

    response = front_door_queue.setSystemPowerTimeouts(data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_TIMEOUTS_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_ERROR, is_error=True)
    # Verify error subcode which should be "2005".
    assert response["error"]["subcode"] == eddie_helper.SUBCODE_INVALID_KEY, \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])
    time.sleep(2)

    # 4. Validate error request by sending invalid value of noVideo.
    data_request = dict()
    data_request["noVideo"] = "invalidValue"
    data = json.dumps(data_request)

    response = front_door_queue.setSystemPowerTimeouts(data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_TIMEOUTS_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_ERROR, is_error=True)
    # Verify error subcode which should be "2005".
    assert response["error"]["subcode"] == eddie_helper.SUBCODE_INVALID_KEY, \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'front_door_queue')
def test_system_power_timeouts_from_setup_state(front_door_queue):
    """
    Test for system power timeouts api after rebooting the device and from SetupOther state
    Test Steps:
    1. Get system power timeouts information and verify response.
    2. Verify no_video_enabled value should be False.
    3. Set system power timeouts with inverted value of noAudio.
    4. Verify response of system power timeouts.
    5. Verify notification of system power timeouts.
    """
    # 1. Get system power timeouts information and verify response.
    LOGGER.info("Testing get system power timeouts")

    response = front_door_queue.getSystemPowerTimeouts()

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_TIMEOUTS_API,
                                                 eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    no_audio = response["body"]["noAudio"]
    no_video = response["body"]["noVideo"]

    # 2. Verify no_video_enabled value should be False.
    assert no_video is False, \
        'Video timeout should be False in Eddie product. Current value : {}'.format(no_video)

    for _ in range(2):
        # 3. Set system power timeouts with inverted value of noAudio.
        no_audio = not no_audio
        LOGGER.info("Testing of set noAudio to %s", no_audio)
        data_request = dict()
        data_request["noAudio"] = no_audio

        response = front_door_queue.setSystemPowerTimeouts(json.dumps(data_request))
        eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_TIMEOUTS_API,
                                                     eddie_helper.METHOD_PUT, eddie_helper.STATUS_OK)

        # 4. Verify response of system power timeouts.
        assert response["body"]["noVideo"] is False, \
            'Video timeout should be False in Eddie product. Current value : {}'.format(response["body"]["noVideo"])
        assert response["body"]["noAudio"] == no_audio, \
            'Audio timeout should be {}. Current value : {}'.format(no_audio, response["body"]["noAudio"])

        time.sleep(10)

        # 5. Verify notification of system power timeouts.
        notification = eddie_helper.get_last_notification(front_door_queue, eddie_helper.SYSTEM_POWER_TIMEOUTS_API)
        assert notification["noVideo"] is False, \
            'Video timeout should be False in Eddie product. Current value : {}'.format(notification["noVideo"])
        assert notification["noAudio"] == no_audio, \
            'Audio timeout should be {}. Current value : {}'.format(no_audio, notification["noAudio"])


@pytest.mark.usefixtures('front_door_queue', 'device_in_aux')
def test_system_power_timeouts_from_selected_state(front_door_queue):
    """
    Test for system power timeouts api while playing from AUX
    Test Steps:
    1. Change playing source to AUX and verifies the device state.
    2. Verify device state which should be "SELECTED".
    3. Get system power timeouts information and verify response.
    4. Verify no_video_enabled value should be False.
    5. Set system power timeouts with inverted value of noAudio.
    6. Verify response of system power timeouts.
    7. Verify notification of system power timeouts.
    """
    # 1. Change playing source to AUX and verifies the device state from fixture.

    # 2. Verify device state which should be "SELECTED".
    for _ in range(25):
        state = front_door_queue.getState()
        if state == eddie_helper.SELECTED:
            break
        time.sleep(1)

    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)

    # [3-7]. Get and Set system power timeouts information and verify responses.
    test_system_power_timeouts_from_setup_state(front_door_queue)


@pytest.mark.usefixtures('front_door_queue', 'device_in_aux')
def test_system_power_timeouts_from_idle_state(front_door_queue):
    """
    Test for system power timeouts api from Idle state
    Test Steps:
    1. Change device state to Idle.
    2. Verify device state which should be "IDLE".
    3. Get system power timeouts information and verify response.
    4. Verify no_video_enabled value should be False.
    5. Set system power timeouts with inverted value of noAudio.
    6. Verify response of system power timeouts.
    7. Verify notification of system power timeouts.
    """
    # 1. Change device state to Idle.
    LOGGER.info("Setting device state to Idle")
    data_request = dict()
    data_request["power"] = eddie_helper.POWER_OFF
    data = json.dumps(data_request)
    front_door_queue.setSystemPowerControl(data)
    for _ in range(25):
        state = front_door_queue.getState()
        if state == eddie_helper.IDLE:
            break
        time.sleep(1)

    # 2. Verify device state which should be "IDLE".
    assert state == eddie_helper.IDLE, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.IDLE, state)

    # [3-7]. Get and Set system power timeouts information and verify responses.
    test_system_power_timeouts_from_setup_state(front_door_queue)


@pytest.mark.usefixtures('set_no_audio_timeout', 'front_door_queue', 'device_in_aux')
def test_system_power_timeouts_behaviour(front_door_queue):
    """
    Test for system power timeouts api behaviour
    Test Steps:
    1. Change device state to Idle.
    2. Verify device state which should be "IDLE".
    3. Set noAudio timeout to False.
    4. Verify response of system power timeouts.
    5. Wait for device state to transit into NetworkStandby state.
    6. Verify device state which should be "IDLE".
    7. Set noAudio timeout to True.
    8. Verify response of system power timeouts.
    9. Wait for device state to transit into NetworkStandby state.
    10. Verify device state which should be "NETWORK_STANDBY".
    """
    # Change device state to Idle.
    LOGGER.info("Setting device state to Idle")
    data_request = dict()
    data_request["power"] = eddie_helper.POWER_OFF
    data = json.dumps(data_request)
    front_door_queue.setSystemPowerControl(data)
    time.sleep(2)
    state = front_door_queue.getState()
    # Set noAudio timeout to False.
    no_audio = False

    for _ in range(2):
        # Verify device state which should be "IDLE".
        for _ in range(25):
            state = front_door_queue.getState()
            if state == eddie_helper.IDLE:
                break
            time.sleep(1)
        assert state == eddie_helper.IDLE, \
            'Device should be in {} state. Current state : {}'.format(eddie_helper.IDLE, state)

        # Set noAudio timeout
        LOGGER.info("Testing of set noAudio to %s", no_audio)
        data_request = dict()
        data_request["noAudio"] = no_audio

        response = front_door_queue.setSystemPowerTimeouts(json.dumps(data_request))
        eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_TIMEOUTS_API,
                                                     eddie_helper.METHOD_PUT, eddie_helper.STATUS_OK)

        # Verify response of system power timeouts.
        assert response["body"]["noVideo"] is False, \
            'Video timeout should be False in Eddie product. Current value : {}'.format(response["body"]["noVideo"])
        assert response["body"]["noAudio"] == no_audio, \
            'Audio timeout should be {}. Current value : {}'.format(no_audio, response["body"]["noAudio"])

        # Wait for device state to transit into NetworkStandby state.
        time.sleep(60)

        # Set noAudio timeout to True.
        no_audio = True

    # Verify device state which should be "NETWORK_STANDBY".
    state = front_door_queue.getState()
    assert state == eddie_helper.NETWORK_STANDBY, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.NETWORK_STANDBY, state)
