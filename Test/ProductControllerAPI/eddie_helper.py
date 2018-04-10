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
import json
import os
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

""" Subcode """
SUBCODE_INVALID_KEY = 2005
SUBCODE_INVALID_ARGS = 1

""" Status code """
STATUS_OK = 200
STATUS_ERROR = 500

""" API Methods """
METHOD_GET = "GET"
METHOD_SET = "SET"
METHOD_PUT = "PUT"
METHOD_POST = "POST"
METHOD_DELETE = "DELETE"

""" Product Controller APIs """
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

logger = get_logger(os.path.basename(__file__))


def get_notification_in_list(fdq, notif_id):
    """
    function to get specific notification list.

    :param fdq: FrontDoorQueue instance connected to device
    :param notif_id: Notification is reseived as resource in header
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
    :param notif_id: Notification is reseived as resource in header
    :param retry: No of retry to get Notifications
    :return sys_state: system state received in notification
    """
    notif_list = get_notification_in_list(fdq, notif_id)
    for count in range(retry):
        if len(notif_list):
            notification = notif_list.pop()
            return notification["body"]
        time.sleep(1)


def check_error_and_response_header(response, api, method=METHOD_GET, status_code=STATUS_OK, is_error=False):
    assert response, 'Not getting any response'

    header = response["header"]

    assert header["resource"] == api
    assert header["msgtype"] == "RESPONSE"
    assert header["method"] == method
    assert header["status"] == status_code

    if is_error:
        assert response["error"]
        assert response["error"]["code"]
        assert response["error"]["subcode"]
        assert response["error"]["message"]
    else:
        with pytest.raises(KeyError) as key_error:
            error = response["error"]
            assert not key_error, 'Got Error for API:{} method:{} Error:{}'.format(api, method, error["message"])


def check_if_end_point_exists(frontdoor, endpoint):
    """
    function to check if API exists or not
    """
    logger.info("Checking endpoint: " + endpoint)
    capabilities = frontdoor.getCapabilities()

    group_name = capabilities["body"]["group"]

    for capability in group_name:
        if str(capability["apiGroup"]) is "Unknown" or "ProductController":
            endpoints_name = capability["endpoints"]
            for name in endpoints_name:
                if str(name["endpoint"]) == endpoint:
                    return

    assert False, "Front door end point %s does not exists" % endpoint


def get_system_info(frontdoor):
    """ Get System info """
    logger.info("Getting system info status")
    result = frontdoor.send(METHOD_GET, SYSTEM_INFO_API)
    return json.loads(result)


def get_system_power_control(frontdoor):
    """ Get System Power state """
    logger.info("Getting system power state")
    result = frontdoor.send(METHOD_GET, SYSTEM_POWER_CONTROL_API)
    return json.loads(result)


def set_system_power_control(frontdoor, data):
    """ Get System Power state """
    logger.info("Getting system power state")
    result = frontdoor.send(METHOD_POST, SYSTEM_POWER_CONTROL_API, data)
    return json.loads(result)


def get_audio_bass(frontdoor):
    """ Get Audio Bass """
    logger.info("Getting Audio Bass")
    result = frontdoor.send(METHOD_GET, AUDIO_BASS_API)
    return json.loads(result)


def set_audio_bass(frontdoor, data):
    """ Set Audio Bass """
    logger.info("Setting Audio Bass")
    result = frontdoor.send(METHOD_PUT, AUDIO_BASS_API, data)
    return json.loads(result)


def get_audio_treble(frontdoor):
    """ Get Audio Treble """
    logger.info("Getting Audio Treble")
    result = frontdoor.send(METHOD_GET, AUDIO_TREBLE_API)
    return json.loads(result)


def set_audio_treble(frontdoor, data):
    """ Set Audio Treble """
    logger.info("Setting Audio Treble")
    result = frontdoor.send(METHOD_PUT, AUDIO_TREBLE_API, data)
    return json.loads(result)


def get_product_settings(frontdoor):
    """ Get Product Settings """
    logger.info("Getting Product Settings")
    result = frontdoor.send(METHOD_GET, SYSTEM_PRODUCTSETTINGS_API)
    return json.loads(result)


def system_reset(frontdoor):
    """ System Reset API """
    logger.info("System Reset")
    result = frontdoor.send(METHOD_GET, SYSTEM_RESET_API)
    return json.loads(result)


def get_setup_state(frontdoor):
    """ Get Setup State """
    logger.info("Getting Setup State")
    result = frontdoor.send(METHOD_GET, SYSTEM_SETUP_API)
    return json.loads(result)


def set_setup_state(frontdoor, data):
    """ Set Setup State """
    logger.info("Setting Setup State")
    result = frontdoor.send(METHOD_PUT, SYSTEM_SETUP_API, data)
    return json.loads(result)


def get_system_state(frontdoor):
    """ Get System State """
    logger.info("Getting System State")
    result = frontdoor.send(METHOD_GET, SYSTEM_STATE_API)
    return json.loads(result)


def set_system_state(frontdoor, data):
    """ Set System State """
    logger.info("Setting System State")
    result = frontdoor.send(METHOD_PUT, SYSTEM_STATE_API, data)
    return json.loads(result)


def system_update_start(frontdoor, data):
    """ System Update Start """
    logger.info("System update start")
    result = frontdoor.send(METHOD_PUT, SYSTEM_UPDATE_START_API, data)
    return json.loads(result)
