# coding=utf-8
# test_system_setup.py
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
PyTest based validation tests for the "/system/setup" API.
"""
import time
import json
import os
import pytest
from CastleTestUtils.RivieraUtils import adb_utils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import eddie_helper

LOGGER = get_logger(os.path.basename(__file__))


def get_and_set_system_setup(front_door_queue):
    """
    Common function to get system state and verify response.
    Test Steps:
    1. Get system setup information and verify response.
    2. Verify notification of system state which should be "IDLE".
    3. Configure OOB setup done using PUT method.
    4. Verify notification of system setup information.
    """
    # 1. Get system setup information and verify response.
    LOGGER.info("Testing get system setup information ")
    response = eddie_helper.get_system_setup(front_door_queue)

    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_SETUP_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    # 2. Verify device setup which should be False.
    assert response["body"]["isSetupCompleted"] is False, \
        'setup value should be False. Current value : {}'.format(response["body"]["isSetupCompleted"])

    assert response["body"]["networkConfigured"] is True, \
        'networkConfigured value should be True. Current value : {}'.format(response["body"]["networkConfigured"])

    # 3. Configure OOB setup done using PUT method.
    LOGGER.info("Testing of set system setup value to true")
    data_request = dict()
    data_request["isSetupCompleted"] = True
    data = json.dumps(data_request)
    response = eddie_helper.set_system_setup(front_door_queue, data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_SETUP_API, eddie_helper.METHOD_PUT,
                                                 eddie_helper.STATUS_OK)
    time.sleep(2)

    # 4. Verify notification of system setup information.
    notification = eddie_helper.get_last_notification(front_door_queue, eddie_helper.SYSTEM_SETUP_API)
    assert notification["isSetupCompleted"] is True, \
        'setup value should be True. Current value : {}'.format(notification["isSetupCompleted"])
    assert notification["networkConfigured"] is True, \
        'networkConfigured value should be True. Current value : {}'.format(
            notification["networkConfigured"])


def set_and_verify_invalid_system_setup(front_door_queue, request_data, error_code):
    """
    Common function to get system state and verify response.
    Test Steps:
    1. Configure OOB setup using PUT method.
    2. Verify error subcode which should be "1".
    3. Get system setup information and verify response.
    4. Verify device setup which should be False and networkConfigured should be True.
    """
    # 1. Configure OOB setup using PUT method.
    response = eddie_helper.set_system_setup(front_door_queue, request_data)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_SETUP_API,
                                                 eddie_helper.METHOD_PUT, eddie_helper.STATUS_ERROR, is_error=True)
    # 2. Verify error subcode which should be "1".
    assert response["error"]["subcode"] == error_code, \
        'Subcode should be {} for invalid key. Subcode got : {}'.format(error_code, response["error"]["subcode"])
    time.sleep(2)

    # 3. Get system setup information and verify response.
    response = eddie_helper.get_system_setup(front_door_queue)
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_SETUP_API, eddie_helper.METHOD_GET,
                                                 eddie_helper.STATUS_OK)

    # 4. Verify device setup which should be False and networkConfigured should be True.
    assert response["body"]["isSetupCompleted"] is False, \
        'setup value should be False. Current value : {}'.format(response["body"]["isSetupCompleted"])

    assert response["body"]["networkConfigured"] is True, \
        'networkConfigured value should be True. Current value : {}'.format(response["body"]["networkConfigured"])


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'front_door_queue')
def test_system_setup_from_setup_state(front_door_queue):
    """
    Test for GET method of system setup api after rebooting the device and from SetupOther state
    Test Steps:
    1. Check that endpoint is returned in capabilities.
    2. Get and set system setup information and verify response.
    3. Verify device state which should be "DESELECTED".
    """
    # 1. Check that endpoint is returned in capabilities.
    eddie_helper.check_if_end_point_exists(front_door_queue, eddie_helper.SYSTEM_SETUP_API)

    # 2. Get and set system setup information and verify response.
    get_and_set_system_setup(front_door_queue)

    # 3. Verify device state which should be "DESELECTED".
    state = front_door_queue.getState()
    assert state == eddie_helper.DESELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.DESELECTED, state)


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'front_door_queue', 'device_in_aux')
def test_system_setup_from_selected_state(front_door_queue):
    """
    Test for GET method of system setup api while playing from AUX
    Test Steps:
    1. Change playing source to AUX and verifies the device state.
    2. Get and set system setup information and verify response.
    3. Verify device state which should be "SELECTED".
    """
    # 1. Change playing source to AUX and verifies the device state from fixture.

    # 2. Get and set system setup information and verify response.
    get_and_set_system_setup(front_door_queue)

    # 3. Verify device state which should be "SELECTED".
    state = front_door_queue.getState()
    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'front_door_queue', 'device_in_aux')
def test_system_setup_from_idle_state(front_door_queue):
    """
    Test for GET method of system setup api while playing from AUX
    Test Steps:
    1. Change device state to Idle.
    2. Verify device state which should be "IDLE".
    3. Get and set system state and verify response.
    4. Verify device state which should be "IDLE".
    """
    # 1. Change device state to Idle.
    LOGGER.info("Setting device state to Idle")
    data_request = dict()
    data_request["power"] = eddie_helper.POWER_OFF
    data = json.dumps(data_request)
    eddie_helper.set_system_power_control(front_door_queue, data)
    time.sleep(2)

    # 2. Verify device state which should be "IDLE".
    state = front_door_queue.getState()
    assert state == eddie_helper.IDLE, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.IDLE, state)

    # 3. Get and set system state and verify response.
    get_and_set_system_setup(front_door_queue)

    # 4. Verify device state which should be "IDLE".
    state = front_door_queue.getState()
    assert state == eddie_helper.IDLE, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.IDLE, state)


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'deviceid', 'front_door_queue')
def test_system_setup_cli_command(deviceid, front_door_queue):
    """
    Test for GET method of system setup api after rebooting the device and from SetupOther state
    Test Steps:
    1. Set OOB Setup completed using CLI command.
    2. Verify notification of system setup information.
    3. Verify device state which should be "DESELECTED".
    """
    # 1. Set OOB Setup completed using CLI command.
    adb_utils.adb_telnet_cmd('setoobsetupcompleted', device_id=deviceid)
    time.sleep(2)

    # 2. Verify notification of system setup information.
    notification = eddie_helper.get_last_notification(front_door_queue, eddie_helper.SYSTEM_SETUP_API)
    assert notification["isSetupCompleted"] is True, \
        'setup value should be True. Current value : {}'.format(notification["isSetupCompleted"])
    assert notification["networkConfigured"] is True, \
        'networkConfigured value should be True. Current value : {}'.format(
            notification["networkConfigured"])

    # 3. Verify device state which should be "DESELECTED".
    state = front_door_queue.getState()
    assert state == eddie_helper.DESELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.DESELECTED, state)


@pytest.mark.usefixtures('remove_oob_setup_state_and_reboot_device', 'front_door_queue', 'device_in_aux')
def test_system_setup_errors(front_door_queue):
    """
    Test for PUT method of system setup api with negative scenarios.
    Test Steps:
    1. Validate error message by sending invalid setup value.
    2. Validate error message by sending only networkConfigured value.
    3. Validate error message by sending networkConfigured value and invalid setup value.
    4. Validate error message by sending invalid key.
    """
    LOGGER.info("Testing set invalid system setup information")

    # 1. Validate error message by sending invalid setup value.
    data_request = dict()
    data_request["isSetupCompleted"] = False
    data = json.dumps(data_request)
    set_and_verify_invalid_system_setup(front_door_queue, data, eddie_helper.SUBCODE_INVALID_ARGS)

    # 2. Validate error message by sending only networkConfigured value.
    data_request = dict()
    data_request["networkConfigured"] = True
    data = json.dumps(data_request)
    set_and_verify_invalid_system_setup(front_door_queue, data, eddie_helper.SUBCODE_INVALID_ARGS)

    # 3. Validate error message by sending networkConfigured value and invalid setup value.
    data_request = dict()
    data_request["networkConfigured"] = True
    data_request["isSetupCompleted"] = False
    data = json.dumps(data_request)
    set_and_verify_invalid_system_setup(front_door_queue, data, eddie_helper.SUBCODE_INVALID_ARGS)

    # 4. Validate error message by sending invalid key.
    data_request = dict()
    data_request["invalidKey"] = True
    data = json.dumps(data_request)
    set_and_verify_invalid_system_setup(front_door_queue, data, eddie_helper.SUBCODE_INVALID_KEY)
