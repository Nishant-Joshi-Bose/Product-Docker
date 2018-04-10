# coding=utf-8
# test_system_power_control.py
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
PyTest based validation tests for the "/system/power/control" API.
"""
import time
import json
import os
import pytest
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.RivieraUtils import adb_utils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import eddie_helper

logger = get_logger(os.path.basename(__file__))


@pytest.mark.usefixtures('frontdoor')
def test_system_power_control_error(frontdoor):
    """
    Test for system power control api with error scenarios.
    Test Steps:
    1. Check that endpoint is returned in capabilities.
    2. Get system power control info and verify response.
    3. Validate error message by sending invalid key.
    4. Validate error message by sending invalid value.
    5. Validate error message by sending extra key and value.
    """
    # 1. Check that endpoint is returned in capabilities.
    eddie_helper.check_if_end_point_exists(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)

    # 2. Get system power control info and verify response.
    logger.info("Testing get system power state")
    response = eddie_helper.get_system_power_control(frontdoor)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    # 3. Validate error message by sending invalid key.
    logger.info("Testing of set system power state with invalid key")
    data_invalid_key = dict()
    data_invalid_key["fake_key"] = "OFF"
    data = json.dumps(data_invalid_key)
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_ERROR, is_error=True)

    assert response["error"]["subcode"] == str(eddie_helper.SUBCODE_INVALID_KEY), \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])
    time.sleep(1)

    # 4. Validate error message by sending invalid value.
    logger.info("Testing of set system power state with invalid value")
    data_invalid_value = dict()
    data_invalid_value["power"] = "Invalid"
    data = json.dumps(data_invalid_value)
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_ERROR, is_error=True)

    assert response["error"]["subcode"] == str(eddie_helper.SUBCODE_INVALID_KEY), \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])
    time.sleep(1)

    # Validate error message by sending extra key and value.
    logger.info("Testing of set system power state to OFF with extra key and value")
    data_invalid = dict()
    data_invalid["power"] = "OFF"
    data_invalid["fake_key"] = "Invalid"
    data = json.dumps(data_invalid)
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_ERROR, is_error=True)

    assert response["error"]["subcode"] == str(eddie_helper.SUBCODE_INVALID_KEY), \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])
    time.sleep(1)


@pytest.mark.usefixtures('frontdoor', 'device_in_aux')
def test_system_power_control_aux_play(frontdoor):
    """
    Test for system power control api while playing from AUX
    Test Steps:
    1. Configure device to play music from AUX.
    2. Get system power control info and verify response.
    3. Set system power control to "OFF" and verify response, notification and device state.
    4. Set system power control to "ON" and verify response, notification and device state.
    5. Set system power control to "TOGGLE" and verify response, notification and device state.
    6. Set system power control to "TOGGLE" again and verify response, notification and device state.
    """
    # 1. Configure device to play music from AUX.

    # 2. Get system power control info and verify response.
    logger.info("Testing get system power state")
    response = eddie_helper.get_system_power_control(frontdoor)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    # 3. Set system power control to "OFF" and verify response, notification and device state.
    logger.info("Testing of set system power state to OFF")
    data_dict = dict()
    data_dict["power"] = "OFF"
    data = json.dumps(data_dict)
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "OFF", 'Device power notification should be "OFF".'

    # Verify device state which should be "IDLE".
    state = frontdoor.getState()
    assert state == "IDLE", 'Device should be in "IDLE" state. Current state : {}'.format(state)

    # 4. Set system power control to "ON" and verify response, notification and device state.
    logger.info("Testing of set system power state to ON")
    data_dict["power"] = "ON"
    data = json.dumps(data_dict)
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "OFF", 'Device power response should be "OFF".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "ON", 'Device power notification should be "ON".'

    # Verify device state which should be "SELECTED".
    state = frontdoor.getState()
    assert state == "SELECTED", 'Device should be in "SELECTED" state. Current state : {}'.format(state)

    # 5. Set system power control to "TOGGLE" and verify response, notification and device state.
    logger.info("Testing of set system power state to TOGGLE")
    data_dict["power"] = "TOGGLE"
    data = json.dumps(data_dict)
    response = eddie_helper.set_system_power_control(frontdoor, data)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, 200)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "OFF", 'Device power notification should be "OFF".'

    # Verify device state which should be "IDLE".
    state = frontdoor.getState()
    assert state == "IDLE", 'Device should be in "IDLE" state. Current state : {}'.format(state)

    # 6. Set system power control to "TOGGLE" again and verify response, notification and device state.
    logger.info("Testing of set system power state to TOGGLE second time")
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "OFF", 'Device power response should be "OFF".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "ON", 'Device power notification should be "ON".'

    # Verify device state which should be "SELECTED".
    state = frontdoor.getState()
    assert state == "SELECTED", 'Device should be in "SELECTED" state. Current state : {}'.format(state)


@pytest.mark.usefixtures('device_playing_from_amazon')
def test_system_power_control_sts_playing(device_playing_from_amazon):
    """
    Test for system power control api while playing from Amazon MSP.
    Test Steps:
    1. Configure Amazon MSP account and play music.
    2. Get system power control info and verify response.
    3. Set system power control to "OFF" and verify response, notification and device state.
    4. Set system power control to "ON" and verify response, notification and device state.
    5. Verify MSP source is available and wait for music to play.
    6. Set system power control to "TOGGLE" and verify response, notification and device state.
    7. Set system power control to "TOGGLE" again and verify response, notification and device state.
    8. Verify MSP source is available and wait for music to play.
    """
    # 1. Configure Amazon MSP account and play music.
    frontdoor, common_behavior_handler, service_name, get_config = device_playing_from_amazon
    time.sleep(5)

    # 2. Get system power control info and verify response.
    logger.info("Testing get system power state")
    response = eddie_helper.get_system_power_control(frontdoor)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    # 4. Set system power control to "OFF" and verify response, notification and device state.
    logger.info("Testing of set system power state to OFF")
    data_dict = dict()
    data_dict["power"] = "OFF"
    data = json.dumps(data_dict)
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "OFF", 'Device power notification should be "OFF".'

    # Verify device state which should be "IDLE".
    state = frontdoor.getState()
    assert state == "IDLE", 'Device should be in "IDLE" state. Current state : {}'.format(state)

    # 5. Set system power control to "ON" and verify response, notification and device state.
    logger.info("Testing of set system power state to ON")
    data_dict["power"] = "ON"
    data = json.dumps(data_dict)
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "OFF", 'Device power response should be "OFF".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "ON", 'Device power notification should be "ON".'

    # Verify device state which should be "SELECTED".
    state = frontdoor.getState()
    assert state == "SELECTED", 'Device should be in "SELECTED" state. Current state : {}'.format(state)

    # 6. Verify MSP source is available and wait for music to play.
    logger.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    logger.info("verify play status")
    now_playing = common_behavior_handler.checkPlayStatus(playStatus='play')
    logger.debug("Now Playing : " + str(now_playing))
    time.sleep(5)

    # 7. Set system power control to "TOGGLE" and verify response, notification and device state.
    logger.info("Testing of set system power state to TOGGLE")
    data_dict["power"] = "TOGGLE"
    data = json.dumps(data_dict)
    response = eddie_helper.set_system_power_control(frontdoor, data)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "OFF", 'Device power notification should be "OFF".'

    # Verify device state which should be "IDLE".
    state = frontdoor.getState()
    assert state == "IDLE", 'Device should be in "IDLE" state. Current state : {}'.format(state)

    # 8. Set system power control to "TOGGLE" again and verify response, notification and device state.
    logger.info("Testing of set system power state to TOGGLE second time")
    response = eddie_helper.set_system_power_control(frontdoor, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_POST, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "OFF", 'Device power response should be "OFF".'
    time.sleep(1)

    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "ON", 'Device power notification should be "ON".'

    # Verify device state which should be "SELECTED".
    state = frontdoor.getState()
    assert state == "SELECTED", 'Device should be in "SELECTED" state. Current state : {}'.format(state)

    # 9. Verify MSP source is available and wait for music to play.
    logger.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    logger.info("verify play status")
    now_playing = common_behavior_handler.checkPlayStatus(playStatus='play')
    logger.debug("Now Playing : " + str(now_playing))


@pytest.mark.usefixtures('deviceid', 'frontdoor', 'device_in_aux')
def test_system_power_control_notification_in_aux(deviceid, frontdoor):
    """
    Test for system power control api notification while playing from AUX and change state using keys.
    Test Steps:
    1. Get system power control info and verify response.
    2. Change state to Idle by pressing Play/Pause button for 2 seconds.
    3. Verify notification of system power value which should be "OFF".
    4. Verify device state which should be "IDLE".
    5. Change state to Selected again by pressing Play/Pause button for 2 seconds.
    6. Verify notification of system power value which should be "ON".
    7. Verify device state which should be "SELECTED".
    """
    # 1. Get system power control info and verify response.
    logger.info("Testing get system power state")
    response = eddie_helper.get_system_power_control(frontdoor)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    # 2. Change state to Idle by pressing Play/Pause button for 2 seconds.
    logger.info("Testing notification of system power state for value OFF")
    tap = adb_utils.adb_telnet_tap(deviceid)
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)

    # 3. Verify notification of system power value which should be "OFF".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "OFF", 'Device power notification should be "OFF".'

    # 4. Verify device state which should be "IDLE".
    state = frontdoor.getState()
    assert state == "IDLE", 'Device should be in "IDLE" state. Current state : {}'.format(state)

    # 5. Change state to Selected again by pressing Play/Pause button for 2 seconds.
    logger.info("Testing notification of system power state for value ON")
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)

    # 6. Verify notification of system power value which should be "ON".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "ON", 'Device power notification should be "ON".'

    # 7. Verify device state which should be "SELECTED".
    state = frontdoor.getState()
    assert state == "SELECTED", 'Device should be in "SELECTED" state. Current state : {}'.format(state)


@pytest.mark.usefixtures('deviceid', 'device_playing_from_amazon')
def test_system_power_control_notification_in_sts(deviceid, device_playing_from_amazon):
    """
    Test for system power control api notification while playing playing from Amazon MSP and change state using keys.
    Test Steps:
    1. Configure Amazon MSP account and play music.
    2. Get system power control info and verify response.
    3. Change state to Idle by pressing Play/Pause button for 2 seconds.
    4. Verify notification of system power value which should be "OFF".
    5. Verify device state which should be "IDLE".
    6. Change state to Selected again by pressing Play/Pause button for 2 seconds.
    7. Verify notification of system power value which should be "ON".
    8. Verify device state which should be "SELECTED".
    9. Verify MSP source is available and wait for music to play.
    """
    # 1. Configure Amazon MSP account and play music.
    frontdoor, common_behavior_handler, service_name, get_config = device_playing_from_amazon
    time.sleep(5)

    # 2. Get system power control info and verify response.
    logger.info("Testing get system power state")
    response = eddie_helper.get_system_power_control(frontdoor)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_POWER_CONTROL_API,
                                                 eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"]["power"] == "ON", 'Device power response should be "ON".'
    time.sleep(1)

    # 3. Change state to Idle by pressing Play/Pause button for 2 seconds.
    logger.info("Testing notification of system power state for value OFF")
    tap = adb_utils.adb_telnet_tap(deviceid)
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)

    # 4. Verify notification of system power value which should be "OFF".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "OFF", 'Device power notification should be "OFF".'

    # 5. Verify device state which should be "IDLE".
    state = frontdoor.getState()
    assert state == "IDLE", 'Device should be in "IDLE" state. Current state : {}'.format(state)

    # 6. Change state to Selected again by pressing Play/Pause button for 2 seconds.
    logger.info("Testing notification of system power state for value ON")
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)

    # 7. Verify notification of system power value which should be "ON".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_POWER_CONTROL_API)
    assert notif_resp["power"] == "ON", 'Device power notification should be "ON".'

    # 8. Verify device state which should be "SELECTED".
    state = frontdoor.getState()
    assert state == "SELECTED", 'Device should be in "SELECTED" state. Current state : {}'.format(state)

    # 9. Verify MSP source is available and wait for music to play.
    logger.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    logger.info("verify play status")
    now_playing = common_behavior_handler.checkPlayStatus(playStatus='play')
    logger.debug("Now Playing : " + str(now_playing))
