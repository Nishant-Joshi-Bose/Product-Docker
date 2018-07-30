# eddie_helper.py
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
Utility functions for testing frontdoor APIs provided by Eddie Product.
"""
import time
import os
import json

import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import device_utils, adb_utils
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from CastleTestUtils.FrontDoorAPI.FrontDoorQueue import FrontDoorQueue
from CastleTestUtils.RivieraUtils.device_utils import PRODUCT_STATE

# Subcode
SUBCODE_INVALID_KEY = 2005
SUBCODE_INVALID_ARGS = 0
SUBCODE_INVALID_PARAMETER = 1

# Status code
STATUS_OK = 200
STATUS_ERROR = 500

# API Methods
METHOD_GET = "GET"
METHOD_SET = "SET"
METHOD_PUT = "PUT"
METHOD_POST = "POST"
METHOD_DELETE = "DELETE"

# Product Controller APIs
ACCESSORIES_API = "/accessories"
ADAPTIQ_API = "/adaptiq"
AUDIO_AVSYNC_API = "/audio/avSync"
AUDIO_BALANCE_API = "/audio/balance"
AUDIO_BASS_API = "/audio/bass"
AUDIO_CENTER_API = "/audio/center"
AUDIO_CONTENTTYPE_API = "/audio/contentType"
AUDIO_DUALMONOSELECT_API = "/audio/dualMonoSelect"
AUDIO_EQSELECT_API = "/audio/eqSelect"
AUDIO_FORMAT_API = "/audio/format"
AUDIO_GAINOFFSET_API = "/audio/gainOffset"
AUDIO_MICROPHONE_API = "/audio/microphone"
AUDIO_MODE_API = "/audio/mode"
AUDIO_MOUNTORIENTATION_API = "/audio/mountOrientation"
AUDIO_SUBWOOFERGAIN_API = "/audio/subwooferGain"
AUDIO_SUBWOOFERPOLARITY_API = "/audio/subwooferPolarity"
AUDIO_SURROUND_API = "/audio/surround"
AUDIO_SURROUNDDELAY_API = "/audio/surroundDelay"
AUDIO_TREBLE_API = "/audio/treble"
CEC_API = "/cec"
CLOCK_API = "/clock"
HOSPITALITY_API = "/hospitality"
INJECTKEY_API = "/injectKey"
SYSTEM_FACTORYDEFAULT_API = "/system/factorydefault"
SYSTEM_INFO_API = "/system/info"
SYSTEM_NAME_API = "/system/name"
SYSTEM_POWER_API = "/system/power"
SYSTEM_POWER_CONTROL_API = "/system/power/control"
SYSTEM_POWER_MACRO_API = "/system/power/macro"
SYSTEM_POWER_MODE__API = "/system/power/mode/"
SYSTEM_POWER_MODE_FASTACTIVATION_API = "/system/power/mode/fastActivation"
SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API = "/system/power/mode/opticalAutoWake"
SYSTEM_POWER_TIMEOUTS_API = "/system/power/timeouts"
SYSTEM_PRODUCTSETTINGS_API = "/system/productSettings"
SYSTEM_RESET_API = "/system/reset"
SYSTEM_SETUP_API = "/system/setup"
SYSTEM_STATE_API = "/system/state"
SYSTEM_UPDATE_START_API = "/system/update/start"

# Device states
BOOTING = "Booting"
SETUPOTHER = "SetupOther"
FIRSTBOOTGREETING = "FirstBootGreeting"
SETUPNETWORK = "SetupNetwork"
STATE_SILENT = "PlayingSelectedSilent"
STATE_NOT_SILENT = "PlayingSelectedNotSilent"
STATE_NW_CONFIGURED = "NetworkConfigured"
STATE_NW_NOT_CONFIGURED = "NetworkNotConfigured"
STATE_VC_CONFIGURED = "VoiceConfigured"
STATE_VC_NOT_CONFIGURED = "VoiceNotConfigured"
SELECTED = "SELECTED"
DESELECTED = ["PLAYING_SOURCE_OFF", "DESELECTED"]
IDLE = "IDLE"
NETWORK_STANDBY = "NETWORK_STANDBY"
FACTORY_DEFAULT = "FACTORY_DEFAULT"
PLAYINGDESELECTED = "PlayingDeselected"

# System Power options
POWER_ON = "ON"
POWER_OFF = "OFF"
POWER_TOGGLE = "TOGGLE"

LOGGER = get_logger(os.path.basename(__file__))


def get_notification_in_list(fdq, notif_id):
    """
    function to get specific notification list.

    :param fdq: FrontDoorQueue instance connected to device
    :param notif_id: Notification is received as resource in header
    :return notif_list: list of received notifications of notif_id
    """
    # Giving the option to just enter one id or a list
    if not isinstance(notif_id, list):
        notif_id = [notif_id]

    notif_list = list()
    while not fdq.queueEmpty():
        notification = fdq.getNotification()
        if notification["header"]["resource"] in notif_id:
            notif_list.append(notification)

    return notif_list


def get_last_notification(fdq, notif_id, retry=5):
    """
    function to get last system state notification and return.

    :param fdq: FrontDoorQueue instance connected to device
    :param notif_id: Notification is received as resource in header
    :param retry: No of retry to get Notifications
    :return sys_state: system state received in notification
    """
    notif_list = get_notification_in_list(fdq, notif_id)
    for _ in range(retry):
        if len(notif_list):
            notification = notif_list.pop()
            return notification["body"]
        time.sleep(1)


def check_error_and_response_header(response, api, method=METHOD_GET, status_code=STATUS_OK, is_error=False):
    """
    Check the response of FrontDoor API
    :param response: response of FrontDoor API
    :param api: API endpoint
    :param method: method required for API
    :param status_code: status code of API
    :param is_error: check for error in API response
    """
    assert response, 'Not getting any response'

    header = response["header"]

    assert header["resource"] == api
    assert header["msgtype"] == "RESPONSE"
    assert header["method"] == method

    if is_error:
        assert "error" in response.keys(), 'Response should contain error'
        assert "code" in response["error"].keys(), 'Error Response should contain code'
        assert "subcode" in response["error"].keys(), 'Error Response should contain subcode'
        assert "message" in response["error"].keys(), 'Error Response should contain message'
    else:
        with pytest.raises(KeyError) as key_error:
            error = response["error"]
            assert not key_error, \
                "Got Error for API:{} method:{} Error:{}".format(api, method, error["message"])


def check_if_end_point_exists(frontdoor, endpoint):
    """
    function to check if API exists or not
    """
    LOGGER.info("Checking endpoint: " + endpoint)
    capabilities = frontdoor.getCapabilities()

    group_name = capabilities["body"]["group"]

    for capability in group_name:
        if str(capability["apiGroup"]) is "Unknown" or "ProductController":
            endpoints_name = capability["endpoints"]
            for name in endpoints_name:
                if str(name["endpoint"]) == endpoint:
                    return

    assert False, "Front door end point {} does not exists".format(endpoint)


def wait_for_setup_state(device_id):
    """
    This method will wait for device be in set up state after reboot

    :param device_id: device_id
    :return: device_state: Device state
    """
    LOGGER.info("Waiting for Setup device state after booting")
    time.sleep(device_utils.TIMEOUT)
    for _ in range(device_utils.TIMEOUT):
        device_state = adb_utils.adb_telnet_cmd(PRODUCT_STATE,
                                                expect_after='Current State: ',
                                                timeout=30,
                                                device_id=device_id)
        if device_state != BOOTING:
            LOGGER.info("Current device_state is %s ", device_state)
            break
        time.sleep(1)
    return device_state


def get_frontdoor_instance(request, wifi_config):
    """
    Get frontDoorAPI instance

    :param request: A request for a fixture from a test or fixture function
    :param wifi_config: config parser instance of wifi profiles
    :return: frontdoor instance
    """
    # get ip address and open frontdoor instance
    galapagos_env = request.config.getoption("--galapagos-env")
    interface = request.config.getoption("--network-iface")
    router = request.config.getoption("--router")
    ssid = wifi_config.get(router, 'ssid')
    security = wifi_config.get(router, 'security')
    password = wifi_config.get(router, 'password')
    device_id = request.config.getoption("--device-id")

    ip_address = device_utils.get_ip_address(device_id, interface, ssid, security, password)
    riviera_utils = RivieraUtils('ADB', device=device_id)

    assert riviera_utils.wait_for_galapagos_activation(timeout=120), "galapagos activation is not done yet."
    frontdoor = FrontDoorQueue(ip_address)

    return frontdoor


def verify_audio_get_api(frontdoor, audio_api):
    """
    This method verifies the get method for audio treble or audio bass api

    :param frontdoor: Instance of frontdoor API
    :param audio_api: /audio/bass or /audio/treble api for get request
    :return response: response from get /audio/bass or /audio/treble api
    """
    LOGGER.info("Test of get method for %s api", audio_api)

    if audio_api == AUDIO_BASS_API:
        # execute get method for audio bass api
        response = frontdoor.getBassLevel()
    else:
        # execute get method for audio treble api
        response = frontdoor.getTreble()

    check_error_and_response_header(response, audio_api, METHOD_GET, STATUS_OK)

    # verify response of audio get api
    assert response["body"]["persistence"],\
        "persistence parameter not in response of get api {}".format(response["body"])

    assert response["body"]["properties"],\
        "properties parameter not in response of get api {}".format(response["body"])

    assert response["body"]["properties"]["max"],\
        "properties max parameter not in response of get api {}".format(response["body"])

    assert response["body"]["properties"]["min"],\
        "properties min parameter not in response of get api {}".format(response["body"])

    assert response["body"]["properties"]["step"],\
        "properties step parameter not in response of get api {}".format(response["body"])

    assert response["body"]["properties"]["supportedPersistence"],\
        "properties persistence values not in response of get api {}".format(response["body"])

    assert response["body"]["value"] is not None,\
        "properties audio value not in response of get api {}".format(response["body"])

    return response


def verify_audio_put_api(frontdoor, audio_api, audio_info):
    """
    This method verifies the put method for audio treble or audio bass api

    :param frontdoor: Instance of frontdoor API
    :param audio_api: /audio/bass or /audio/treble api for put request
    :param audio_info: response from get /audio/bass or /audio/treble api
    :return: None
    """
    LOGGER.info("Test of put method for %s api", audio_api)

    # capture data for put api verification
    persistence_params = audio_info["properties"]["supportedPersistence"]
    max_value = int(audio_info["properties"]["max"]) + 1
    min_value = int(audio_info["properties"]["min"])
    step = int(audio_info["properties"]["step"])

    # Verify put audio api for all persistence, max, min value and step
    for item, value in [(item, value) for item in persistence_params for value in range(min_value,
                                                                                        max_value,
                                                                                        step)]:
        LOGGER.info("Testing of set value for persistence : {} and value : {}".format(item, value))
        audio_data = dict()
        audio_data["persistence"] = item
        audio_data["value"] = value
        data = json.dumps(audio_data)

        # Execute put request
        if audio_api == AUDIO_BASS_API:
            # Execute put method for audio bass api
            response = frontdoor.setBassLevel(data)
        else:
            # Execute put method for audio treble api
            response = frontdoor.setTreble(data)

        check_error_and_response_header(response, audio_api, METHOD_PUT, STATUS_OK)

        # Verify persistence and value is changed
        assert response["body"]["persistence"] == audio_data["persistence"],\
            "persistence value not set in put api {}".format(response["body"]["persistence"])
        assert response["body"]["value"] == audio_data["value"],\
            "audio value not set in put api {}".format(response["body"]["value"])
        time.sleep(1)

        # Verify notify response for persistence and value
        notif_resp = get_last_notification(frontdoor, audio_api)
        assert notif_resp["persistence"] == audio_data["persistence"],\
            "persistence value not set in notification api {}".format(notif_resp["persistence"])
        assert notif_resp["value"] == audio_data["value"],\
            "audio value not set in notification api {}".format(notif_resp["persistence"])


def set_initial_audio_values(initial_audio_data, frontdoor, audio_api):
    """
    This method sets the initial values back from response of audio api

    :param initial_audio_data: response from get audio api
    :param frontdoor: Instance of frontdoor API
    :param audio_api: /audio/bass or /audio/treble api
    :return: None
    """
    # Execute put request for audio api to set data
    data = json.dumps(initial_audio_data)

    if audio_api == AUDIO_BASS_API:
        # Execute put method for audio bass api
        response = frontdoor.setBassLevel(data)
    else:
        # Execute put method for audio treble api
        response = frontdoor.setTreble(data)

    check_error_and_response_header(response, audio_api, METHOD_PUT, STATUS_OK)

    # Verify value and persistence are set in response
    assert response["body"]["persistence"] == initial_audio_data["persistence"],\
        "initial persistence not set {}".format(response["body"]["persistence"])
    assert response["body"]["value"] == initial_audio_data["value"],\
        "initial audio value not set {}".format(response["body"]["value"])


def verify_audio_invalid_args(frontdoor, audio_info, audio_api):
    """
    This method verifies the put method for audio api

    :param frontdoor: Instance of frontdoor API
    :param audio_api: /audio/bass or /audio/treble api for put request
    :param audio_info: response from get /audio/bass or /audio/treble api
    :return: None
    """
    LOGGER.info("Test of put method for %s api for invalid arguments", audio_api)

    # Capture current persistence
    persistence_params = audio_info["properties"]["supportedPersistence"]

    # Set error values after max and min value for error verification
    max_value = int(audio_info["properties"]["max"]) + 1
    min_value = int(audio_info["properties"]["min"]) - 1

    # Verify that value does not exceed after max and min value of treble
    for item, value in [(item, value) for item in persistence_params for value in (min_value,
                                                                                   max_value)]:
        LOGGER.info("Testing of set treble value for persistence : {} and value : {}".format(
            item, value))
        data_invalid_value = dict()
        data_invalid_value["persistence"] = item
        data_invalid_value["value"] = value
        data = json.dumps(data_invalid_value)
        # Execute put request
        if audio_api == AUDIO_BASS_API:
            response = frontdoor.setBassLevel(data)
        else:
            response = frontdoor.setTreble(data)
        check_error_and_response_header(response, audio_api, METHOD_PUT, STATUS_ERROR,
                                        is_error=True)

        # Verify error code
        assert response["error"]["subcode"] == SUBCODE_INVALID_PARAMETER,\
            'Subcode should be {} for invalid parameter. Subcode got : {}'.format(
                SUBCODE_INVALID_PARAMETER, response["error"]["subcode"])


def verify_audio_keyerror(frontdoor, data_keyerror, audio_api):
    """
    This method verifies the keyerror for put method of audio api

    :param frontdoor: Instance of frontdoor API
    :param data_keyerror: keyerror value of audio bass or audio treble api
    :param audio_api: /audio/bass or /audio/treble api for put request
    :return: None
    """
    LOGGER.info("Test of put method for %s api for invalid keys", audio_api)

    # Set keyerror values
    data = json.dumps(data_keyerror)
    if audio_api == AUDIO_BASS_API:
        response = frontdoor.setBassLevel(data)
    else:
        response = frontdoor.setTreble(data)
    check_error_and_response_header(response, audio_api,
                                    METHOD_PUT,
                                    STATUS_ERROR, is_error=True)

    # Verify error code
    assert response["error"]["subcode"] == SUBCODE_INVALID_KEY,\
        'Subcode should be {} for invalid key. Subcode got : {}'.format(
            SUBCODE_INVALID_KEY, response["error"]["subcode"])
