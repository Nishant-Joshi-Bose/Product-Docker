# stress_utils.py
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
Utility class for working with Eddie stress test.
"""

import time
import logging
import re
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from ..ProductControllerAPI import eddie_helper

LOGGER = get_logger(__name__, "StressTest.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)


def wait_for_product_state(adb, expected_state, on_exit=False, timeout=30):
    """
    Function to wait until device state set to expected_state.

    :param adb: ADBCommunication object.
    :param expected_state: expected device state.
    :param on_exit: Flag for checking state on enter or on exit
    :param timeout: Max time to wait for expected response.
    :return: Current device state.
    """
    device_state = None
    start_time = time.time()

    if on_exit:
        LOGGER.info("Waiting for device to exit %s state.", expected_state)
    else:
        LOGGER.info("Waiting for device to enter %s state.", expected_state)

    while time.time() < timeout + start_time:
        response = adb.executeCommand("echo 'getproductstate' | nc 0 17000")
        response_state = re.search(r'(.*Current State: )([a-zA-Z]+)(.*)', response)
        if response_state:
            device_state = response_state.group(2)
            LOGGER.debug('Got state: %s', device_state)
            if (device_state == expected_state and not on_exit) or (device_state != expected_state and on_exit):
                break
        time.sleep(0.5)

    return device_state


def wait_for_booting_state(adb, on_exit=False, timeout=30):
    """
    Function to wait until Eddie service starts and device state set to 'Booting'.

    :param adb: ADBCommunication object.
    :param on_exit: Flag for checking state on enter or on exit
    :param timeout: Max time to wait for expected response.
    :return: Current device state.
    """
    start_time = time.time()

    LOGGER.info("Waiting for CLI server to start...")
    while time.time() < timeout + start_time:
        response = adb.executeCommand("echo '?' | nc 0 17000 | grep 'getproductstate'")
        if response and 'getproductstate' in response.strip():
            break
        time.sleep(1)

    device_state = wait_for_product_state(adb, eddie_helper.BOOTING, on_exit=on_exit, timeout=timeout)
    return device_state


def get_last_chime(adb):
    """
    Function to get last chime played on device.

    :param adb: ADBCommunication object.
    :return: last chime details.
    """
    cmd = "echo 'chimes history' | nc 0 17000"

    output = adb.executeCommand(cmd)
    if output:
        chime = output.split('\r\n')[0]
        LOGGER.info("Got last chime information : %s", chime)
        return chime


def compare_light_bar_animations(lb_expected, lb_actual):
    """
    Function to compare expected and actual light_bar animations.

    :param lb_expected: list of expected light_bar animations played on device.
    :param lb_actual: list of actual light_bar animations played on device.
    :return: False/True.

    """
    if len(lb_expected) > len(lb_actual):
        return False
    for (lb_expected_entry, lb_actual_entry) in zip(lb_expected, lb_actual):
        if cmp(lb_expected_entry, lb_actual_entry):
            return False
    return True


def get_lb_expected_for_first_boot():
    """
    Function to get expected light_bar animations.

    :return: list of dictionaries for light_bar animations info.

    """
    LOGGER.info("Getting expected light_bar animations...")
    lb_names = ['BOOTUP',
                'FIRST_BOOT_ENTER',
                'FIRST_BOOT_EXIT']
    lb_states = ['STARTED', 'STOPPED']
    lb_list = list()
    for (lb_name, lb_state) in \
            [(lb_name, lb_state) for lb_name in lb_names for lb_state in lb_states]:
        lb_dict = dict()
        lb_dict['name'] = lb_name
        lb_dict['state'] = lb_state
        lb_list.append(lb_dict)
    return lb_list


def get_lb_history(adb):
    """
    Function to get light_bar animations played on device.

    :param adb: ADBCommunication object.
    :return: list of dictionaries for light_bar animations played on device.
    """
    LOGGER.info("Getting light_bar animations played on device...")
    cmd = "echo 'lpm pt \\\"lb history\\\"' | nc 0 17000"

    output = adb.executeCommand(cmd)
    if output:
        lb_output = re.findall('Timestamp:.*\r', output, re.MULTILINE)
        if lb_output:
            lb_list = list()
            for lb_iter in lb_output:
                lb_dict = dict()
                lb_re = lb_iter.split(',')
                lb_dict['name'] = lb_re[2].split(':')[1].strip()
                lb_dict['state'] = lb_re[3].split('\r')[0].strip()
                # Skip DEFAULT_BLANK animation
                if lb_dict['name'] != 'DEFAULT_BLANK':
                    lb_list.append(lb_dict)
            return lb_list
