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

import pytest

from CastleTestUtils.FrontDoorAPI.FrontDoorQueue import FrontDoorQueue
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import device_utils, adb_utils
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils

# Subcode
SUBCODE_INVALID_KEY = 2005
SUBCODE_INVALID_ARGS = 0

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
DESELECTED = "DESELECTED"
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
    assert header["status"] == status_code

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


def wait_for_device_commands(device_id, adb):
    """
    This method will wait for device to accept CLI commands

    :param device_id: device_id
    :param adb: ADB Instance
    :return: telnet_status (True or False)
    """
    telnet_status = False
    for _ in range(device_utils.TIMEOUT):
        status = adb.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status:
            LOGGER.debug("Telnet serive started in the device")
            telnet_status = True
            if adb.executeCommand("echo '?' | nc 0 17000 | grep 'getproductstate'"):
                device_state = adb_utils.adb_telnet_cmd('getproductstate',
                                                        expect_after='Current State: ',
                                                        timeout=30, device_id=device_id)
                LOGGER.info("Device State: %s", device_state)
                assert (device_state in [FIRSTBOOTGREETING, BOOTING, SETUPOTHER, PLAYINGDESELECTED]), \
                    'Device not in expected state. Current state: {}.'.format(device_state)
                break

        time.sleep(1)
    return telnet_status


def wait_for_setup_state(device_id):
    """
    This method will wait for device be in set up state after reboot

    :param device_id: device_id
    :return: device_state: Device state
    """
    LOGGER.info("Waiting for Setup device state after booting")
    time.sleep(device_utils.TIMEOUT)
    for _ in range(device_utils.TIMEOUT):
        device_state = adb_utils.adb_telnet_cmd('getproductstate',
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
    interface = request.config.getoption("--network-iface")
    router = request.config.getoption("--router")
    ssid = wifi_config.get(router, 'ssid')
    security = wifi_config.get(router, 'security')
    password = wifi_config.get(router, 'password')
    device_id = request.config.getoption("--device-id")
    ip_address = device_utils.get_ip_address(
        device_id, interface, ssid, security, password)
    riviera_utils = RivieraUtils('ADB', device=device_id)
    assert riviera_utils.wait_for_galapagos_activation(timeout=120)
    frontdoor = FrontDoorQueue(ip_address)
    return frontdoor
