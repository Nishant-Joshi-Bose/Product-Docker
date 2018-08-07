# cli_key_helper.py
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
Different CLI key events and the method to execute them.
"""
from time import sleep

from CastleTestUtils.LoggerUtils.BSerialLogger import getdatabuf, startdatacapture, stopdatacapture
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys.keypress import press_key, key_press_only
from CastleTestUtils.RivieraUtils import device_utils

LOGGER = get_logger(__file__)

# Commands response constant values
INVALID_SOURCE = 'INVALID_SOURCE()'
SETUP_SOURCE = 'SETUP(SETUP)'
PRODUCT_SETUP = 'PRODUCT(SETUP)'
FACTORY_RESET_SOURCE = [PRODUCT_SETUP, INVALID_SOURCE, SETUP_SOURCE]
INTENT_TIMEOUT = 4
KEY_TIMEOUT = 0.5


def key_single_or_multiple(keydata, tap, serial_handler, press_hold=False):
    """
    In key_single event, single key and one timeout value is expected that is used
    to trigger an action based on how long key is held.

    :param keydata: Dictionary containing the key id and timeout value
                            (Ex:keydata={"Origin": 0,
                                     "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap: Instance of PExpect client
    :param serial_handler: APQ serial logs instance
    :param press_hold: flag for press and hold keys
    :return log_data: list of APQ serial logs
    """
    # key number
    input_keys = keydata["keyList"]
    LOGGER.info("Performing execution of key_single event for key %s", input_keys)
    timeout = float(keydata["timeOutList"][0])
    LOGGER.info("Timeout value is %s ms", timeout)

    # Start to capture the serial_logs
    startdatacapture(serial_handler)
    if press_hold:
        timeout = timeout / 1000.0
        key_press_only(tap, input_keys, True)
        sleep(timeout)
    else:
        press_key(tap, input_keys, timeout, True)

    # Duration for generate the intent value
    sleep(INTENT_TIMEOUT)

    # Stop the serial logs.
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)

    return log_data


def key_repetitive(keydata, tap, serial_handler, press_hold=False):
    """
    In key_repetitive event, one key and will be repetitively pressed according to two
    timeout values in timeOutList.
    First value used for initial duration for hold and second value used for repetitive.

    :param keydata: Dictionary  containing the key id and timeout value
                           (Ex: keydata={"Origin": 0,
                                    "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap: Instance of PExpect client
    :param press_hold: flag for press and hold keys
    :param serial_handler: APQ serial logs instance
    :return log_data: list of Serial logs
    """
    # key number
    input_keys = keydata["keyList"]
    LOGGER.info("Performing execution of key_repetitive event for keys %s", input_keys)
    timeouts = keydata["timeOutList"]

    # Start to capture the serial_logs
    startdatacapture(serial_handler)
    for timeout in timeouts:
        if press_hold:
            timeout = timeout / 1000.0
            key_press_only(tap, input_keys, True)
            sleep(timeout + 4)
        else:
            press_key(tap, input_keys, float(timeout), True)

    # Stop the serial logs
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)

    return log_data


def key_burst(keydata, tap, serial_handler, press_hold=False):
    """
    In key_burst event, burst key press and one timeout is expected to trigger an action.
    The timeout defines the maximum duration between the burst of keys.

    :param keydata: Dictionary  containing the key id and timeout value
                           (Ex: keydata={"Origin": 0,
                                       "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap: Instance of PExpect client
    :param press_hold: flag for press and hold keys
    :param serial_handler: APQ serial logs instance
    :return log_data: list of Serial logs
    """
    # Key number
    input_keys = keydata["keyList"]
    LOGGER.info("Performing execution of key_burst event for keys %s", input_keys)

    # Convert timeout value from  milliseconds to seconds
    timeout = (keydata["timeOutList"][0]/1000)/(len(input_keys) * 2)

    startdatacapture(serial_handler)

    # Press and release within timeout interval and convert timeout from millisecond to second
    for input_key in input_keys:
        if press_hold:
            key_press_only(tap, input_key, True)
            sleep(timeout)
            sleep(timeout + 0.01)
        else:
            press_key(tap, input_key, timeout, True)
            sleep(timeout + 0.01)
        # sleep used for delay to generate intent value for CLI key
        sleep(timeout + 0.01)

    # sleep used to generate intent value for CLI key
    sleep(KEY_TIMEOUT)

    # Stop the serial logs
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)

    return log_data


def key_release_always(keydata, tap, serial_handler, press_hold=False):
    """
    In key_release_always event, it is used to detect the key release event
    for the key described in keydata["keyList"] and time0utList value
    is not  required for the release check of key.

    :param keydata: Dictionary  containing the key id and timeout value
                            (Ex: keydata={"Origin": 0,
                                       "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap: Instance of PExpect client
    :param press_hold: flag for press and hold keys
    :param serial_handler: APQ serial logs instance
    :return log_data: list of Serial logs
    """
    # key number
    input_keys = keydata["keyList"]
    LOGGER.info("Performing execution of key_release_always event for key %s", input_keys)
    timeout = 0

    # Start to capture the serial_logs
    startdatacapture(serial_handler)

    if press_hold:
        key_press_only(tap, input_keys, True)
        sleep(timeout)
    else:
        press_key(tap, input_keys, timeout, True)

    # sleep used for delay to generate intent value for CLI key
    sleep(INTENT_TIMEOUT)

    # Stop the serial logs
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)

    return log_data


def get_network_ipaddr(request, wifi_config, device_id, interface):
    """
    This function restores the ip address of the device
    :param request: A request for a fixture from a test or fixture function.
    :param wifi_config: fixture returning wifi configurations
    :param device_id: ADB device id
    :param interface: Network interface
    :return ip_address : wlan ip address of device
    """
    # get wifi credentials
    router = request.config.getoption("--router")
    ssid = wifi_config.get(router, 'ssid')
    security = wifi_config.get(router, 'security')
    password = wifi_config.get(router, 'password')

    LOGGER.debug("Connecting to ssid %s", ssid)
    ip_address = device_utils.get_ip_address(device_id, interface,
                                             ssid, security, password)

    LOGGER.debug("Restored Ip address is %s", ip_address)
    return ip_address
