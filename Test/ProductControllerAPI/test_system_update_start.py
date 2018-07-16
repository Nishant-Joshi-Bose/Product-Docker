# coding=utf-8
# test_system_update_start.py
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
PyTest based validation tests for the "/system/update/start" API.
"""
import time
import json
import os
import pytest
import eddie_helper
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

LOGGER = get_logger(os.path.basename(__file__))


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'frontdoor_wlan')
def test_system_update_start_from_setup_state(frontdoor_wlan):
    """
    Test for system update start api from SetupOther state
    Test Steps:
    1. Check that endpoint is returned in capabilities.
    2. Request system update start API with PUT method.
    """
    # 1. Check that endpoint is returned in capabilities.
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.SYSTEM_UPDATE_START_API)

    # 2. Request system update start API with PUT method.
    LOGGER.info("Testing of system update start API")
    delay = 0
    response = frontdoor_wlan.sendSystemUpdateStartRequest(delay)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_UPDATE_START_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_OK)


@pytest.mark.usefixtures('frontdoor_wlan', 'device_in_aux')
def test_system_update_start_playing_from_aux(frontdoor_wlan):
    """
    Test for system update start api while playing from aux
    Test Steps:
    1. Put device in AUX mode using fixture.
    2. Request system update start API with PUT method.
    """
    # 1. Put device in AUX mode using fixture.

    # 2. Request system update start API with PUT method.
    LOGGER.info("Testing of system update start API")

    delay = 0
    response = frontdoor_wlan.sendSystemUpdateStartRequest(delay)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_UPDATE_START_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_OK)


@pytest.mark.usefixtures('frontdoor_wlan', 'device_in_aux')
def test_system_update_start_from_idle_state(frontdoor_wlan):
    """
    Test for system update start api from Idle state
    Test Steps:
    1. Change device state to Idle.
    2. Verify device state which should be "IDLE".
    3. Request system update start API with PUT method.
    """
    # 1. Change device state to Idle.
    LOGGER.info("Setting device state to Idle")
    data_request = dict()
    data_request["power"] = eddie_helper.POWER_OFF
    data = json.dumps(data_request)

    frontdoor_wlan.setSystemPowerControl(data)
    for _ in range(25):
        state = frontdoor_wlan.getState()
        if state == eddie_helper.IDLE:
            break
        time.sleep(1)

    # 2. Verify device state which should be "IDLE".
    assert state == eddie_helper.IDLE, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.IDLE, state)

    time.sleep(2)

    # 3. Request system update start API with PUT method.
    LOGGER.info("Testing of system update start API")

    delay = 0
    response = frontdoor_wlan.sendSystemUpdateStartRequest(delay)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_UPDATE_START_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_OK)


@pytest.mark.usefixtures('frontdoor_wlan')
def test_system_update_start_errors(frontdoor_wlan):
    """
    Test for PUT method of system update start api with negative scenarios.
    Test Steps:
    1. Validate error message by sending invalid delay value type.
    2. Verify error subcode which should be "2005".
    3. Validate error message by sending invalid key.
    4. Verify error subcode which should be "2005".
    """

    LOGGER.info("Testing of system update start API with invalid data")

    # 1. Validate error message by sending invalid delay value type.
    delay = "Invalid"
    response = frontdoor_wlan.sendSystemUpdateStartRequest(delay)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_UPDATE_START_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_ERROR, is_error=True)
    # 2. Verify error subcode which should be "2005".
    assert response["error"]["subcode"] == eddie_helper.SUBCODE_INVALID_KEY, \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])

    # 3. Validate error message by sending invalid key.
    data_request = dict()
    data_request["Invalid"] = 5
    data = json.dumps(data_request)
    response = frontdoor_wlan.sendSystemUpdateStartRequest(data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_UPDATE_START_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_ERROR, is_error=True)
    # 4. Verify error subcode which should be "2005".
    assert response["error"]["subcode"] == eddie_helper.SUBCODE_INVALID_KEY, \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(eddie_helper.SUBCODE_INVALID_KEY,
                                                                        response["error"]["subcode"])
