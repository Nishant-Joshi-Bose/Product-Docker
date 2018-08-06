# test_boot_sequence_timing.py
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
A series of PyTests that determine how long services on Eddie take to start.
At the conclusion of testing, all information is transferred to our
Graphite database and viewable in Grafana.
"""

import logging
import time
import multiprocessing
from collections import namedtuple

import pytest
from pytest_testrail.plugin import pytestrail

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.MemoryUtils.graphite import generate_time_message
from CastleTestUtils.LpmUtils.LpmException import LpmException
from CastleTestUtils.NetworkUtils.network_base import NetworkBase

from conf_bootsequencing import CONFIG
from stateutils import state_checker, UNKNOWN


LOGGER = get_logger(__name__, level=logging.DEBUG)

State = namedtuple('State', 'current_time elapsed_time system soc power')


@pytestrail.case('C658164')
@pytest.mark.usefixtures('rebooted_device', 'graphite')
def test_base_reboot_time(rebooted_device, graphite):
    """
    This test will determine the time from issuing a reboot command to when it
    is ready using the rebooted_device fixture.

    Resulting data will be posted to the Graphite DB based upon the module configuration.

    :param rebooted_device: Fixture that sets up the ADB device to be rebooted
        and networked. Will return the time associated to do these tasks.
    :return: None
    """
    data = rebooted_device

    # Check Reboot Time information
    reboot_time = data['reboot']['duration']
    assert reboot_time > 0, \
        'Reboot time is not less than specification. Actual: {0:.2f}s.'.format(reboot_time)

    rows = generate_time_message(data, CONFIG['Grafana']['table_name'],
                                 CONFIG['Grafana']['sub_table_name'])

    LOGGER.debug('Posting the following information: %s', rows)
    graphite.send_message('\n'.join(rows) + '\n')
    time.sleep(5)


@pytestrail.case('C658165')
@pytest.mark.usefixtures('rebooted_and_networked_device', 'graphite')
def test_network_connection_time(rebooted_and_networked_device, graphite):
    """
    This test will determine the time from issuing a reboot command to when it
        is has acquired a network IP address using the
        rebooted_and_networked_device fixture.

    Resulting data will be posted to the Graphite DB based upon the module
        configuration.

    :param rebooted_and_networked_device: Fixture that sets up the ADB device to be rebooted
        and networked. Will return the time associated to do these tasks.

    :return: None
    """
    data = rebooted_and_networked_device

    # Check Reboot information
    reboot_time = data['reboot']['duration']
    assert reboot_time > 0, \
        'Reboot time is not less than specification. Actual: {0:.2f}s.'.format(reboot_time)

    # Check IP Address information
    ip_address = rebooted_and_networked_device['ip']['address']
    assert ip_address != UNKNOWN, 'IP Address never acquired.'

    rows = generate_time_message(data, CONFIG['Grafana']['table_name'],
                                 CONFIG['Grafana']['sub_table_name'])

    LOGGER.debug('Posting the following information: %s', rows)
    graphite.send_message('\n'.join(rows) + '\n')
    time.sleep(5)


@pytestrail.case('C658166')
@pytest.mark.usefixtures('rebooted_and_networked_device', 'graphite', 'user_account', 'environment', 'passport_user_with_device')
def test_boot_state_time(rebooted_and_networked_device, graphite, user_account, environment, maximum_time=30):
    """
    Acquires the time to get to states through the FrontDoorAPI, then posts them to the
    Graphite Server.

    :param rebooted_and_networked_device: Fixture that sets up the ADB device to be rebooted
        and networked. Will return the time associated to do these tasks.
    :param maximum_time: The maximum time to collect data.

    :return: None
    """
    data = rebooted_and_networked_device
    LOGGER.debug('Initial Reboot and Network Information: %s', data)

    # Check Reboot information
    reboot_time = data['reboot']['duration']
    assert reboot_time > 0, \
        'Reboot time is not less than specification. Actual: {0:.2f}s.'.format(reboot_time)

    # Check IP Address information
    ip_address = rebooted_and_networked_device['ip']['address']
    LOGGER.debug("State Checking will be done with IP: %s", ip_address)
    assert ip_address != UNKNOWN, 'IP Address never acquired ({}).'.format(ip_address)

    state_data = state_checker(ip_address, maximum_time, dict(), user_account, environment)

    rows = generate_time_message(dict(data.items() + state_data.items()), CONFIG['Grafana']['table_name'],
                                 CONFIG['Grafana']['sub_table_name'])

    LOGGER.debug('Posting the following information: %s', rows)
    graphite.send_message('\n'.join(rows) + '\n')
    time.sleep(5)


@pytest.mark.usefixtures('lpm_serial_client', 'riviera', 'device_id', 'ip_address_wlan')
def test_timing_reboot_state(lpm_serial_client, riviera, device_id):
    """
    Similar to test_base_reboot_time, this will gather state information via the Serial connection

    This test is meant to show functionality for future tests.

    :param lpm_serial_client: A serial connection to the LPM
    :param riviera: A Riviera object for the device under test
    :param device_id: The ADB Device ID for the device under test
    """
    if not lpm_serial_client:
        pytest.skip("Serial connection failed. Skipping test.")

    class LpmStates(multiprocessing.Process):
        """
        Gathers LPM state information.

        Stores Epoch, Delta Time, System State, SOC State, and Power State
        into the passed queue object as a Tuple
        """
        def __init__(self, stop, lpm, queue):
            super(LpmStates, self).__init__()
            self._stop = stop
            self._queue = queue
            self._lpm = lpm

        def run(self):
            start_time = time.time()
            # while not self._stop.is_set():
            while not stop.is_set():
                try:
                    current_time = time.time()
                    system = self._lpm.get_system_state()
                    soc = self._lpm.get_soc_state()
                    power = self._lpm.get_power_state()
                    elapsed_time = current_time - start_time
                    self._queue.put(State(current_time=current_time,
                                          elapsed_time=elapsed_time,
                                          system=system,
                                          soc=soc,
                                          power=power))
                except LpmException:
                    pass

    LOGGER.debug("Starting LPM State collection Thread.")
    stop = multiprocessing.Event()
    q = multiprocessing.Queue()
    t = LpmStates(stop, lpm_serial_client, q)
    t.daemon
    t.start()

    LOGGER.debug("Rebooting Device.")
    reset_start_time = time.time()
    riviera.communication.executeCommand('/opt/Bose/bin/PlatformReset')
    riviera.communication.waitForRebootDevice()
    reset_stop_time = time.time()
    LOGGER.debug("Reboot time: {}".format(reset_stop_time - reset_start_time))
    LOGGER.debug("Ensure we have network connection.")
    network = NetworkBase(None, device_id)
    ip_start_time = time.time()
    ip_addresss = network.check_inf_presence('wlan0', timeout=120)
    ip_stop_time = time.time()
    LOGGER.debug("Network time: {}".format(ip_stop_time - ip_start_time))
    LOGGER.debug("Found IP Address: %s", ip_addresss)

    LOGGER.debug("Stopping LPM State collection Thread.")
    stop.set()
    LOGGER.debug("Stop value is %s", stop.is_set())
    t.terminate()
    t.join()

    LOGGER.debug("Processing Queue for Storage.")
    results = list()
    while not q.empty():
        results.append(q.get())
    # Assert State Transitions
    transition_count = 0
    for iteration, point in enumerate(results):
        # Skip the first one
        if not iteration:
            pass
        # These are the state transition times
        if (point.system != results[iteration - 1].system) or (point.soc != results[iteration - 1].soc) or (point.power != results[iteration - 1].power):
            transition_count += 1

    assert transition_count, "Not able to transition system states. Please review: {}".format(results)
    time.sleep(5)
