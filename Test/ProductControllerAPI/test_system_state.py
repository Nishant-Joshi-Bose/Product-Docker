# coding=utf-8
# test_system_state.py
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
PyTest based validation tests for the "/system/state" API.
"""
import time
import os
import pytest
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.RivieraUtils import adb_utils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import eddie_helper

logger = get_logger(os.path.basename(__file__))


def get_and_verify_system_state(deviceid, frontdoor):
    """
    Common function to get system state and verify response.
    Test Steps:
    1. Get system state and verify response.
    2. Verify device state which should be "SELECTED".
    3. Change state to Idle by pressing Play/Pause button for 2 seconds.
    4. Verify notification of system state which should be "IDLE".
    5. Change state to Selected again by pressing Play/Pause button for 2 seconds.
    6. Verify notification of system state which should be "SELECTED".
    """
    # 1. Get system state and verify response.
    logger.info("Testing get system state")
    response = eddie_helper.get_system_state(frontdoor)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_STATE_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert response["body"]["state"], 'Not able to get state information. Got response : {}'.format(response["body"])

    # 2. Verify device state which should be "SELECTED".
    assert response["body"]["state"] == "SELECTED", \
        'Device should be in "SELECTED" state. Current state : {}'.format(response["body"]["state"])

    # 3. Change state to Idle by pressing Play/Pause button for 2 seconds.
    logger.info("Testing notification of system state for IDLE")
    tap = adb_utils.adb_telnet_tap(deviceid)
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)

    # 4. Verify notification of system state which should be "IDLE".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_STATE_API)
    assert notif_resp["state"] == "IDLE", \
        'Device should be in "IDLE" state. Current state : {}'.format(notif_resp["state"])

    # 5. Change state to Selected again by pressing Play/Pause button for 2 seconds.
    logger.info("Testing notification of system state for SELECTED")
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)

    # 6. Verify notification of system state which should be "SELECTED".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_STATE_API)
    assert notif_resp["state"] == "SELECTED", \
        'Device should be in "SELECTED" state. Current state : {}'.format(notif_resp["state"])


@pytest.mark.usefixtures('deviceid', 'device_playing_from_amazon')
def test_system_state_playing_from_amazon(deviceid, device_playing_from_amazon):
    """
    Test for GET method of system state api while playing from Amazon MSP
    Test Steps:
    1. Configure Amazon MSP account and play music.
    2. Get system state and verify response.
    3. Verify MSP source is available and wait for music to play.
    """
    # 1. Configure Amazon MSP account and play music.
    frontdoor, common_behavior_handler, service_name, get_config = device_playing_from_amazon
    time.sleep(5)

    # 2. Get system info and verify response.
    get_and_verify_system_state(deviceid, frontdoor)

    # 3. Verify MSP source is available and wait for music to play.
    logger.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    logger.info("verify play status")
    now_playing = common_behavior_handler.checkPlayStatus(playStatus='play')
    logger.debug("Now Playing : " + str(now_playing))


@pytest.mark.usefixtures('deviceid', 'frontdoor', 'device_in_aux')
def test_system_state_playing_from_aux(deviceid, frontdoor):
    """
    Test for GET method of system state api while playing from AUX
    Test Steps:
    1. Change playing source to AUX and verifies the device state.
    2. Get system state and verify response.
    """
    # 1. Change playing source to AUX and verifies the device state from fixture.

    # 2. Get system state and verify response.
    get_and_verify_system_state(deviceid, frontdoor)


@pytest.mark.usefixtures('set_no_audio_timeout', 'deviceid', 'frontdoor')
def test_system_state_network_standby(deviceid, frontdoor):
    """
    Test for GET method of system state api while in network standby
    Test Steps:
    1. Get system state and verify response.
    2. Verify device state which should be "SELECTED".
    3. Change state to Idle by pressing Play/Pause button for 2 seconds.
    4. Verify notification of system state which should be "IDLE".
    5. Get system state and verify response.
    6. Wait for device state to transit into NetworkStandby state.
    7. Verify notification of system state which should be "NETWORK_STANDBY".
    8. Verify device state which should be "NETWORK_STANDBY".
    """
    # 1. Get system state and verify response.
    logger.info("Testing get system state")
    response = eddie_helper.get_system_state(frontdoor)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_STATE_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert response["body"]["state"], 'Not able to get state information. Got response : {}'.format(response["body"])

    # 2. Verify device state which should be "SELECTED".
    assert response["body"]["state"] == "SELECTED", \
        'Device should be in "SELECTED" state. Current state : {}'.format(response["body"]["state"])

    # 3. Change state to Idle by pressing Play/Pause button for 2 seconds.
    logger.info("Testing notification of system state for IDLE")
    tap = adb_utils.adb_telnet_tap(deviceid)
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)

    # 4. Verify notification of system state which should be "IDLE".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_STATE_API)
    assert notif_resp["state"] == "IDLE", \
        'Device should be in "IDLE" state. Current state : {}'.format(notif_resp["state"])

    # 5. Get system state and verify response.
    response = eddie_helper.get_system_state(frontdoor)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_STATE_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert response["body"]["state"] == "IDLE", \
        'Device should be in "IDLE" state. Current state : {}'.format(response["body"]["state"])

    # 6. Wait for device state to transit into NetworkStandby state.
    time.sleep(60)

    # 7. Verify notification of system state which should be "NETWORK_STANDBY".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_STATE_API)
    assert notif_resp["state"] == "NETWORK_STANDBY", \
        'Device should be in "NETWORK_STANDBY" state. Current state : {}'.format(notif_resp["state"])

    # 8. Verify device state which should be "NETWORK_STANDBY".
    response = eddie_helper.get_system_state(frontdoor)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_STATE_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert response["body"]["state"] == "NETWORK_STANDBY", \
        'Device should be in "NETWORK_STANDBY" state. Current state : {}'.format(response["body"]["state"])


@pytest.mark.usefixtures('deviceid', 'adb', 'frontdoor')
def test_system_state_factory_default(deviceid, adb, frontdoor):
    """
    Test for system state notification for factory default state
    Test Steps:
    1. Get system state and verify response.
    2. Verify device state which should be "SELECTED".
    3. Generate factoryDefault action using CLI Command.
    4. Wait for minimum 10 seconds to launch factoryDefault action.
    5. Verify notification of system state which should be "FACTORY_DEFAULT".
    6. Wait for factoryDefault action to complete.
    7. Wait for device to become online after reboot and come out from 'Booting' state.
    """
    # 1. Get system state and verify response.
    logger.info("Testing get system state")
    response = eddie_helper.get_system_state(frontdoor)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_STATE_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    assert response["body"]["state"], 'Not able to get state information. Got response : {}'.format(response["body"])

    # 2. Verify device state which should be "SELECTED".
    assert response["body"]["state"] == "SELECTED", \
        'Device should be in "SELECTED" state. Current state : {}'.format(response["body"]["state"])

    # 3. Generate factoryDefault action using CLI Command.
    tap = adb_utils.adb_telnet_tap(deviceid)
    keypress.key_press_only(tap, [Keys.AUX.value, Keys.VOLUME_DOWN.value], async_response=True)

    # 4. Wait for minimum 10 seconds to launch factoryDefault action.
    time.sleep(11)

    # 5. Verify notification of system state which should be "FACTORY_DEFAULT".
    notif_resp = eddie_helper.get_last_notification(frontdoor, eddie_helper.SYSTEM_STATE_API)
    assert notif_resp["state"] == "FACTORY_DEFAULT", \
        'Device should be in "FACTORY_DEFAULT" state. Current state : {}'.format(notif_resp["state"])

    # 6. Wait for factoryDefault action to complete.
    time.sleep(10)
    # 7. Wait for device to become online after reboot and come out from 'Booting' state.
    adb.waitForRebootDevice()

    status = None
    for count in range(30):
        status = adb.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status and status.strip() == 'OK':
            break
        time.sleep(1)
    assert status, "CLIServer not started within 30s."
    assert (status.strip() == 'OK'), 'CLIServer is not stated even after 30 seconds'

    time.sleep(2)

    for count in range(10):
        if adb.executeCommand("echo '?' | nc 0 17000 | grep 'getproductstate'"):
            break
        time.sleep(1)

    for count in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
        if device_state != 'Booting':
            break
        time.sleep(1)


@pytest.mark.usefixtures('rebooted_and_out_of_booting_state_device', 'deviceid', 'frontdoor')
def test_system_state_setup_state(deviceid, frontdoor):
    """
    Test for GET method of system state api after rebooting the device and from SetupOther state
    Test Steps:
    1. Check that endpoint is returned in capabilities.
    2. Get system state and verify response.
    """
    # 1. Check that endpoint is returned in capabilities.
    eddie_helper.check_if_end_point_exists(frontdoor, eddie_helper.SYSTEM_STATE_API)

    # 2. Get system state and verify response.
    get_and_verify_system_state(deviceid, frontdoor)
