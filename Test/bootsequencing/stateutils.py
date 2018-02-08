# stateutils.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

"""
Utilities for working with state timing.
"""

import logging
import socket
import time

from pyadb import ADB

from CastleTestUtils.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.RivieraUtils.commonException import ADBCommandFailure
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication

android_device = ADB('/usr/bin/adb')
logger = get_logger(__name__, logLevel=logging.DEBUG)

UNKNOWN = 'UNKNOWN'


def is_ip_address(address):
    """
    Attempts to determine if the input string IP Address is valid.

    :param address: The string address to check.
    :return: True/False if is a proper address
    """

    # Check if we are in a None input state
    if not address:
        return False
    try:
        socket.inet_aton(address)
        # We have correctly parsed the address
        return True
    except socket.error:
        # Exceptions parsing will cause a failure
        return False


def network_checker(network_connection, maximum_time, data):
    """
    Determines how long it takes from execution start to acquiring an
    IP address on the connection type given. Data for this is stored
    in a shared thread-safe dictionary.

    :param network_connection: The network connection type (*nix style)
    :param maximum_time: The timeout for trying to acquire an IP Address
    :param data: The thread-safe shared dictionary
    :return: The updated dictionary with times
    """
    adbc = ADBCommunication()
    ip_address = None
    start_time = time.time()
    elapsed_time = 0
    while not ip_address and elapsed_time < maximum_time and not is_ip_address(ip_address):
        try:
            ip_address = adbc.getIPAddress(interfaceName=network_connection)
        except ADBCommandFailure:
            elapsed_time = time.time() - start_time
    end_time = time.time()
    run_time = end_time - start_time

    if not ip_address:
        ip_address = UNKNOWN

    logger.info('Acquired IP {} in {:.2f}s.'.format(ip_address, run_time))

    data['ip'] = {'address': ip_address, 'start': start_time, 'end': end_time,
                  'duration': run_time}

    return data


def state_checker(ip_address, run_time, data, delay=5):
    """
    This will collect the states seen by the system, the time it was first seen,
    and the time it was last seen. The collected information will be saved
    in a thread-safe dictionary.

    :param ip_address: The IP address of the device that will be
    :param run_time: How long (in seconds) we want to run
    :param data: The shared thread safe dictionary
    :param delay: How long to wait prior to starting the collection
    :return: A dictionary with state names, time first seen, time last seen,
        and number of times seen
    """

    if 'state' not in data:
        data['state'] = {}
    time.sleep(delay)

    api = FrontDoorAPI.FrontDoorAPI(ip_address)
    start_time = time.time()
    data['state']['start'] = start_time

    # Iterate while we are still less than the run-time
    while (time.time() - start_time) < run_time:
        current_time = time.time()
        state = str(api.getState())

        # Put all information regarding the state into the thread safe dictionary
        if state in data['state'].keys():
            data['state'][state]['last_seen'] = current_time
            data['state'][state]['count'] = data['state'][state]['count'] + 1
        else:
            data['state'][state] = {'first_seen': current_time, 'count': 1,
                                    'last_seen': current_time}

        # I don't want to hit the API too hard. This might be slowing things down.
        time.sleep(0.1)

    data['state']['end'] = time.time()
    number_states = len([key for key in data['state'].keys() if key not in ['start', 'end']])
    logger.info('State collection ran for {0:.2f}s and collected {1} state(s).'
                .format(data['state']['end'] - data['state']['start'], number_states))

    # We need to close the Websocket correctly
    if api:
        api.close()

    return data


def reboot_checker(data):
    """
    Will attempt to reboot the system and acquire the time taken to do it.
    Reboot is only based upon the ADB being able to see the device.

    :param data: A dictionary that is shared for Multiprocessing
    :return:  None
    """
    reboot_start = time.time()
    android_device.run_cmd('reboot')
    android_device.wait_for_device()
    reboot_end = time.time()
    reboot_time = reboot_end - reboot_start
    data['reboot'] = {'start': reboot_start, 'end': reboot_end, 'duration': reboot_time}
    logger.info('Reboot of target completed and took {:.2f}s'.format(reboot_time))

    return data
