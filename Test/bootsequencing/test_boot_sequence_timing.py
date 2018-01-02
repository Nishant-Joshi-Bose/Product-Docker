# test_boot_sequence_timing.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
"""
A series of Pytests that determine how long services on Eddie take to start.
At the conclusion of testing, all information is transferred to our
Graphite database and viewable in Grafana.
"""

import logging

import pytest

from conf_bootsequencing import CONFIG
from stateutils import state_checker, UNKNOWN

from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.MemoryUtils.graphite import generate_time_message, Graphite


logger = get_logger(__name__, logLevel=logging.DEBUG)


@pytest.mark.usefixtures("rebooted_and_networked_device", "adb_versions")
def test_multi_state(rebooted_and_networked_device, adb_versions, delay=0, maximum_time=30):
    """
    Acquires the time to get to states through the FrontDoorAPI, then posts them to the
    Graphite Server.

    :param rebooted_and_networked_device: Fixture that sets up the ADB device to be rebooted
        and networked. Will return the time associated to do these tasks.
    :param adb_versions: Fixture that returns the version information of the device  that is
        used.
    :param delay: The delay before starting the FrontDoorApi
    :param maximum_time: The maximum time to collect data.

    :return: None
    """
    logger.debug('ADB Device Version Information: {}'.format(adb_versions))
    data = rebooted_and_networked_device
    logger.debug('Initial Reboot and Network Information: {}'.format(data))

    # Check Reboot information
    reboot_time = data['reboot']['duration']
    assert reboot_time > 0, \
        'Reboot time is not less than specification. Actual: {0:.2f}s.'.format(reboot_time)

    # Check IP Address information
    ip_address = rebooted_and_networked_device['ip']['address']
    assert ip_address != UNKNOWN, 'IP Address never acquired.'

    state_data = state_checker(ip_address, maximum_time, dict(), delay)

    rows = generate_time_message(dict(data.items() + state_data.items()), CONFIG['Grafana']['table_name'],
                                 CONFIG['Grafana']['sub_table_name'])

    logger.debug('Posting the following information: {}'.format(rows))

    graphite = Graphite(CONFIG['Grafana']['server_ip'], CONFIG['Grafana']['server_port'],
                  CONFIG['Grafana']['event_port'])
    graphite.send_message('\n'.join(rows) + '\n')

    # Ensure connection is closed
    if graphite:
        graphite.close()


@pytest.mark.usefixtures("rebooted_and_networked_device", "adb_versions")
def test_network_time(rebooted_and_networked_device, adb_versions):
    """
    This test will determine the time from issuing a reboot command to when it
        is has acquired a network IP address using the
        rebooted_and_networked_device fixture.

    Resulting data will be posted to the Graphite DB based upon the module
        configuration.

    :param rebooted_and_networked_device: Fixture that sets up the ADB device to be rebooted
        and networked. Will return the time associated to do these tasks.
    :param adb_versions: Fixture that returns the version information of the device  that is
        used.

    :return: None
    """
    logger.debug('ADB Device Version Information: {}'.format(adb_versions))
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

    logger.debug('Posting the following information: {}'.format(rows))

    graphite = Graphite(CONFIG['Grafana']['server_ip'], CONFIG['Grafana']['server_port'],
                        CONFIG['Grafana']['event_port'])
    graphite.send_message('\n'.join(rows) + '\n')

    # Ensure connection is closed
    if graphite:
        graphite.close()


@pytest.mark.usefixtures("rebooted_device", "adb_versions")
def test_eddie_base_reboot_time(rebooted_device, adb_versions):
    """
    This test will determine the time from issuing a reboot command to when it
    is ready using the rebooted_device fixture.

    Resulting data will be posted to the Graphite DB based upon the module configuration.

    :param rebooted_device: Fixture that sets up the ADB device to be rebooted
        and networked. Will return the time associated to do these tasks.
    :param adb_versions: Fixture that returns the version information of the device  that is
        used.
    :return: None
    """
    logger.debug('ADB Device Version Information: {}'.format(adb_versions))
    data = rebooted_device

    # Check Reboot Time information
    reboot_time = data['reboot']['duration']
    assert reboot_time > 0, \
        'Reboot time is not less than specification. Actual: {0:.2f}s.'.format(reboot_time)

    rows = generate_time_message(data, CONFIG['Grafana']['table_name'],
                                 CONFIG['Grafana']['sub_table_name'])

    logger.debug('Posting the following information: {}'.format(rows))

    graphite = Graphite(CONFIG['Grafana']['server_ip'], CONFIG['Grafana']['server_port'],
                        CONFIG['Grafana']['event_port'])
    graphite.send_message('\n'.join(rows) + '\n')

    # Ensure connection is closed
    if graphite:
        graphite.close()
