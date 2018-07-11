# ProductControllerAPI/test_system_reset.py
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
PyTest based validation tests for the "/system/reset" API.
"""
import time
import json
import pytest

import eddie_helper

from CastleTestUtils.RivieraUtils import device_utils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger


LOGGER = get_logger(__file__)


@pytest.mark.usefixtures("adb", "ip_address_wlan")
@pytest.mark.parametrize("device_state", ["ON", "OFF"])
def test_system_reset_device_state(frontdoor_wlan, request, adb, device_state):
    """
    This test will verify device state after system reset api
        while device states are ON and OFF.

    Test Steps:
    1. Change the device state(ON, OFF)
    2. Check the FrontDoor API exists
    3. Reset the device by calling /system/reset
    4. Verify the device state

    :param frontdoor_wlan: Instance of FrontDoor API
    :param request: A request for a fixture from a test or fixture function.
    :param adb: ADB Instance
    :param device_state: device state (ON, OFF)
    :return: None
    """
    LOGGER.info("Testing system reset")

    # change the device state
    device_id = request.config.getoption("--device-id")
    device_utils.change_current_device_state(device_state, device_id)

    # check for frontDoor API exist
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.SYSTEM_RESET_API)

    # verify the GET response
    response = frontdoor_wlan.getSystemReset()
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_RESET_API, eddie_helper.METHOD_GET, 200)
    assert not response["body"]["resetProduct"], "Error in response of API: {}".format(response)

    # verify the PUT response
    data = {"resetProduct": "true"}
    response = frontdoor_wlan.setSystemReset(json.dumps(data))
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_RESET_API, eddie_helper.METHOD_PUT, 200)
    assert response["body"]["resetProduct"], "Device doesn't reset: {}".format(response)

    # Wait for factoryDefault action to complete.
    LOGGER.debug("Waiting for the factoryDefault device")
    reboot_status = False

    for _ in range(device_utils.TIMEOUT):
        LOGGER.debug("Waiting for the device to reboot")
        if not adb.isDeviceAvailable():
            LOGGER.debug("Waiting for the device to reboot")
            reboot_status = True
            break
        time.sleep(1)
    assert reboot_status, "Device didn't reboot after system reset. Availability: {}".format(adb.isDeviceAvailable())

    adb.waitForRebootDevice()
    LOGGER.debug("Device rebooted & waiting for telnet service to get started")

    telnet_status = eddie_helper.wait_for_device_commands(device_id, adb)

    assert telnet_status, "CLIServer not started. Telnet status: {}".format(telnet_status)

    # Wait for next device state after booting
    device_state = eddie_helper.wait_for_setup_state(device_id)

    assert device_state != eddie_helper.BOOTING, "Device is still in Booting ({}) state.".format(device_state)


@pytest.mark.usefixtures("adb", "ip_address_wlan")
@pytest.mark.parametrize("device_source", ["AUX", "BLUETOOTH"])
def test_system_reset_device_source(frontdoor_wlan, request, adb, device_source):
    """
    This test will verify device state after system reset api
        while device sources are AUX and BLUETOOTH.

    Test Steps:
    1. Change the device source(AUX, BLUETOOTH)
    2. Check the FrontDoor API exists
    3. Reset the device by calling /system/reset
    4. Verify the device state by /system/reset

    :param frontdoor_wlan: Instance of FrontDoor API
    :param request: A request for a fixture from a test or fixture function.
    :param adb: ADB Instance
    :param device_source: device source (AUX, BLUETOOTH)
    :return: None
    """
    LOGGER.info("Testing system reset")

    # change the device source
    device_id = request.config.getoption("--device-id")
    device_utils.set_device_source(device_source, device_id)

    # check for frontDoor API exist
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.SYSTEM_RESET_API)

    # verify the GET response
    response = frontdoor_wlan.getSystemReset()
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_RESET_API, eddie_helper.METHOD_GET, 200)
    assert not response["body"]["resetProduct"], "Error in response of API and response is {}".format(response)

    # verify the PUT response
    data = {"resetProduct": "true"}
    response = frontdoor_wlan.setSystemReset(json.dumps(data))
    eddie_helper.check_error_and_response_header(response, eddie_helper.SYSTEM_RESET_API, eddie_helper.METHOD_PUT, 200)
    assert response["body"]["resetProduct"], "Device doesn't reset. Response: {}".format(response)

    # Wait for factoryDefault action to complete.
    LOGGER.debug("Waiting for the factoryDefault device")
    reboot_status = False

    for _ in range(device_utils.TIMEOUT):
        if not adb.isDeviceAvailable():
            LOGGER.debug("Waiting for the device to reboot")
            reboot_status = True
            break
        time.sleep(1)
    assert reboot_status, "Device (availability {}) reboot after system reset failed.".format(adb.isDeviceAvailable())

    adb.waitForRebootDevice()
    LOGGER.debug("Device rebooted and waiting for telnet service to get started")

    # Wait for CLI-Server to start and listens on 17000 port.
    telnet_status = eddie_helper.wait_for_device_commands(device_id, adb)
    assert telnet_status, "CLIServer not started. Telnet status: {}".format(telnet_status)

    # Wait for next device state after booting
    device_state = eddie_helper.wait_for_setup_state(device_id)

    assert device_state != eddie_helper.BOOTING, "Device is still in '{}' state.".format(eddie_helper.BOOTING)
