# test_product_settings.py
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
PyTest based validation tests for the "/system/productSettings" API.
"""
import pytest
import eddie_helper
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import device_utils, adb_utils, rivieraUtils
from CastleTestUtils.RivieraUtils.hardware.keys.keypress import press_key
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys

LOGGER = get_logger(__file__)


@pytest.mark.usefixtures("frontdoor_wlan")
@pytest.mark.parametrize("device_state", ["ON", "OFF"])
def test_product_settings_with_device_state(frontdoor_wlan, device_state, device_id):
    """
    This test will verify response for /system/productSettings API
        while device states are ON, OFF, SETUP.

    Test Steps:
    1. Change the device state(ON, OFF, SETUP)
    2. Check the FrontDoor API exists
    3. get response by calling /system/productSettings
    4. verify product settings get and notify api from device json file

    :param frontdoor_wlan: Instance of FrontDoor API
    :param device_state: device state (ON, OFF, SETUP)
    :param device_id: Device id
    :return: None
    """
    LOGGER.info("Testing system product settings")

    # change the device state
    device_utils.change_current_device_state(device_state, device_id, timeout=120)

    eddie_helper.check_if_end_point_exists(
        frontdoor_wlan, eddie_helper.SYSTEM_PRODUCTSETTINGS_API)

    LOGGER.info("Testing get product settings")
    response = frontdoor_wlan.getProductSettings()

    eddie_helper.check_error_and_response_header(
        response, eddie_helper.SYSTEM_PRODUCTSETTINGS_API,
        eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"] and response["body"]["language"]\
        and response["body"]["productName"], "Incorrect\
         Response Response status is {}".format(response)

    LOGGER.info("Getting product settings from device json file.")

    adb = rivieraUtils.RivieraUtils('ADB', device=device_id)
    response_device = adb.get_product_settings()
    notify_response = _verify_response(frontdoor_wlan, response_device)
    assert not notify_response, "Notification Response is not valid"


@pytest.mark.usefixtures("frontdoor_wlan")
@pytest.mark.parametrize("device_source", ["AUX", "BLUETOOTH"])
def test_product_settings_with_device_source(frontdoor_wlan, device_source, device_id):
    """
     This test will verify response for /system/productSettings API
        while device resources are AUX and BLUETOOTH.


    Test Steps:
    1. Change the device state(AUX and BLUETOOTH)
    2. Check the FrontDoor API exists
    3. get response by calling /system/productSettings
    4. verify product settings get and notify api from device json file

    :param frontdoor_wlan: Instance of FrontDoor API
    :param device_source: device source (AUX, BLUETOOTH)
    :param device_id: Device id
    :return: None
    """
    LOGGER.info("Testing system product settings")

    # change the device source
    device_utils.set_device_source(device_source, device_id)

    # check for front_door API exist
    eddie_helper.check_if_end_point_exists(
        frontdoor_wlan, eddie_helper.SYSTEM_PRODUCTSETTINGS_API)

    LOGGER.info("Testing get product settings")
    response = frontdoor_wlan.getProductSettings()

    eddie_helper.check_error_and_response_header(
        response, eddie_helper.SYSTEM_PRODUCTSETTINGS_API,
        eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"] and response["body"]["language"] \
        and response["body"]["productName"], "Incorrect\
         Response Response status is {}".format(response)

    LOGGER.info("Getting product settings from device json file.")
    adb = rivieraUtils.RivieraUtils('ADB', device=device_id)
    response_device = adb.get_product_settings()

    notify_response = _verify_response(frontdoor_wlan, response_device)

    assert not notify_response, "Notification Response is not valid"


@pytest.mark.usefixtures("request", "device_playing_from_amazon")
def test_product_settings_play_amazon(frontdoor_wlan, device_id):
    """
     This test will verify response for /system/productSettings API
        while play music from AMAZON.


    Test Steps:
    1. Check the FrontDoor API exists
    2. get response by calling /system/productSettings
    3. set preset while playing music.
    3. verify product settings get and notify api from device json file

    :param frontdoor_wlan: Instance of FrontDoor API
    :param device_id: Device id
    :return: None
    """
    LOGGER.info("Testing system product settings")

    eddie_helper.check_if_end_point_exists(
        frontdoor_wlan, eddie_helper.SYSTEM_PRODUCTSETTINGS_API)

    LOGGER.info("Testing get product settings")
    response = frontdoor_wlan.getProductSettings()

    eddie_helper.check_error_and_response_header(
        response, eddie_helper.SYSTEM_PRODUCTSETTINGS_API,
        eddie_helper.METHOD_GET, eddie_helper.STATUS_OK)

    assert response["body"] and response["body"]["language"]\
        and response["body"]["presets"]\
        and response["body"]["productName"], "Incorrect\
         Response Response status is {}".format(response)

    tap = adb_utils.adb_telnet_tap(device_id=device_id)
    press_key(tap, Keys.PRESET_1.value, 5000, True)

    frontdoor_wlan.stopPlaybackRequest()

    LOGGER.info("Getting product settings from device json file.")
    adb = rivieraUtils.RivieraUtils('ADB', device=device_id)
    response_device = adb.get_product_settings()

    assert response_device and response_device["ProductSettings"], "Incorrect response \
                {}".format(response_device)

    notify_response = _verify_response(frontdoor_wlan, response_device)

    assert cmp(response_device["ProductSettings"], notify_response["presets"])


def _verify_response(frontdoor_wlan, response_device):
    """
     method to verify api respnse

    :param frontdoor_wlan: Instance of frontDoor API
    :param response_device : json file response
    :return: notify_response :  Notification Response
    """
    LOGGER.info("Response to verify is %s", response_device)
    assert response_device, "Error in productSettings response fromt device"
    assert response_device["ProductSettings"], "Error in ProductSettings key"

    # verify notify response for persistence and value
    notify_response = eddie_helper.get_last_notification(
        frontdoor_wlan, eddie_helper.SYSTEM_PRODUCTSETTINGS_API)

    LOGGER.info("Notify Response : {}".format(notify_response))
    return notify_response
