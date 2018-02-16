# KeyEvent.py
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
from pyadb import ADB
from configure import conf
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LoggerUtils.BSerialLogger import getdatabuf, startdatacapture, stopdatacapture
from CastleTestUtils.RivieraUtils.hardware.keys.keypress import press_key

logger = get_logger(__file__)
adb = ADB(conf["adb_path"])

# Useful CLI Key commands
RAW_KEY_CLI = 'raw_key'
ORIGIN = '0'
PRESS = '1'
RELEASE = '0'
ASYNC_RESPONSE = 'ar'
PROMPT = '->'
BLANK = ''
# Commands response constant values
ASYNC_RESPONSE_ON = 'async_responses on'
SUCCESS = 'Success'
OK = 'OK'

def key_single(keydata, tap, serial_handler):
    """
    In key_single event, single key and one timeout value is expected that is used
    to trigger an action based on how long key is held.
    :param keydata : Dictionary containing the key id and timeout value
                            (Ex:keydata={"Origin": 0,
                                     "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap : pexpect Telnet tap client
    :param serial_handler : APQ serial logs instance
    :return log_data : list of APQ serial logs
    """
    # key number
    input_keys = keydata["keyList"]
    logger.info("Performing execution of key_single event for key %s" % input_keys)
    timeout = float(keydata["timeOutList"][0])
    logger.info("Timeout value is %s ms" % timeout)

    # Below command is used for async responses
    # and to ensure for tap prompt before executing CLI commands
    tap.expect(PROMPT)
    tap.sendline(ASYNC_RESPONSE)
    tap.expect(ASYNC_RESPONSE_ON)

    # Start to capture the serial_logs
    startdatacapture(serial_handler)

    press_key(tap, input_keys, timeout, True)

    # Duration for generate the intent value
    sleep(4)

    # Stop the serial logs.
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)
    return log_data

def key_multiple(keydata, tap, serial_handler):
    """
    In key_multiple event, multiple keys and one timeout is expected and action is triggered
    when a key is pressed and held for a timeout.
    :param keydata : Dictionary  containing the key id and timeout value
                           (Ex:keydata={"Origin": 0,
                                    "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap : pexpect Telnet tap client
    :param serial_handler : APQ serial logs instance
    :return log_data : list of APQ serial logs
    """
    # key number
    input_keys = keydata["keyList"]
    logger.info("Performing execution of key_multiple event for keys %s" % input_keys)
    timeout = float(keydata["timeOutList"][0])
    logger.debug("Timeout value is %s ms" % timeout)

    # Below command is used for async responses
    # and ensure for tap prompt before executing CLI commands
    tap.expect(PROMPT)
    tap.sendline(ASYNC_RESPONSE)
    tap.expect(ASYNC_RESPONSE_ON)

    startdatacapture(serial_handler)

    press_key(tap, input_keys, timeout, True)

    # Duration for generate the intent value
    sleep(2)

    # Stop the serial logs
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)
    return log_data

def key_repetitive(keydata, tap, serial_handler):
    """
    In key_repetitive event, one key and will be repetitively pressed according to two
    timeout values in timeOutList.
    First value used for intial duration for hold and second value used for repetitive.
    :param keydata : Dictionary  containing the key id and timeout value
                           (Ex: keydata={"Origin": 0,
                                    "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap : pexpect Telnet tap client
    :param serial_handler : APQ serial logs instance
    :return log_data : list of Serial logs
    """
    # key number
    input_keys = keydata["keyList"]
    logger.info("Performing execution of key_repetitive event for keys %s" % input_keys)
    timeouts = keydata["timeOutList"]

    # Below command is used for async responses
    # and ensure for tap prompt before executing CLI commands
    tap.expect(PROMPT)
    tap.sendline(ASYNC_RESPONSE)
    tap.expect(ASYNC_RESPONSE_ON)

    # Start to capture the serial_logs
    startdatacapture(serial_handler)

    for timeout in timeouts:
        press_key(tap, input_keys, float(timeout), True)

    # Stop the serial logs
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)
    return log_data

def key_burst(keydata, tap, serial_handler):
    """
    In key_burst event, burst key press and one timeout is expected to trigger an action.
    The timeout defines the maximum duration between the burst of keys.
    :param keydata        : Dictionary  containing the key id and timeout value
                           (Ex: keydata={"Origin": 0,
                                       "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap            : pexpect Telnet tap client
    :param serial_handler : APQ serial logs instance
    : return log_data     : list of Serial logs
    """
    # Key number
    input_keys = keydata["keyList"]
    logger.info("Performing execution of key_burst event for keys %s" % input_keys)

    # Convert timeout value from  milliseconds to seconds
    timeout = (keydata["timeOutList"][0]/1000)/(len(input_keys) * 2)

    # Below command is used for async responses
    # and ensure for tap prompt before executing CLI commands
    tap.expect(PROMPT)
    tap.sendline(ASYNC_RESPONSE)
    tap.expect(ASYNC_RESPONSE_ON)
    startdatacapture(serial_handler)

   # Press and release within timeout interval and convert timeout from millisecond to second
    for input_key in  input_keys:
        press_key(tap, input_key, timeout, True)
        # Sleep used for delay to generate intent value for CLI key
        sleep(timeout+0.01)

    # sleep used for delay to generate intent value for CLI key
    sleep(0.5)
    # Stop the serial logs
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)
    return log_data

def key_release_always(keydata, tap, serial_handler):
    """
    In key_release_always event, it is used to detect the key release event
    for the key described in keydata["keyList"] and time0utList value
    is not  required for the release check of key.
    :param keydata : Dictionary  containing the key id and timeout value
                            (Ex: keydata={"Origin": 0,
                                       "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
    :param tap : Instance of pexpect client
    :param serial_handler : APQ serial logs instance
    :return log_data : list of Serial logs
    """
    # key number
    input_keys = keydata["keyList"]
    logger.info("Performing execution of key_release_always event for key %s" % input_keys)
    timeout = 0
    # Below command is used for async responses
    # and ensure for tap prompt before executing CLI commands
    tap.expect(PROMPT)
    tap.sendline(ASYNC_RESPONSE)
    tap.expect(ASYNC_RESPONSE_ON)

    # Start to capture the serial_logs
    startdatacapture(serial_handler)

    press_key(tap, input_keys, timeout, True)

    # sleep used for delay to generate intent value for CLI key
    sleep(1)

    # Stop the serial logs
    stopdatacapture(serial_handler)
    log_data = getdatabuf(serial_handler)
    return log_data

def compute_intent(serial_logs):
    """
    Check the intent value generated from serial logs with the expected intent value.
    : param serial_logs : list of captured serial logs
    : return intent_generated : List of generated intent values
    """
    intent_generated = []
    # Check if intent value generated in serial_logs
    if any('Translated intent' in serial_log for serial_log in serial_logs):
        for serial_log in serial_logs:
            if 'Translated intent' in serial_log:
                current_intent = serial_log.strip('\r').split(' ')
                current_intent = current_intent[-1]
                logger.debug('Current intent value is %s' % str(current_intent))
                intent_generated.append(current_intent)
        intent_generated = [int(intent) for intent in intent_generated]
        return intent_generated
    else:
        return intent_generated
