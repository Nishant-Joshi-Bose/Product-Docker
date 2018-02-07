# bootsequencing/conftest.py
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
PyTest configuration and fixtures used by the Boot Sequencing Tests.

"""

import pytest
from CastleTestUtils.MemoryUtils.graphite import Graphite

from conf_bootsequencing import CONFIG


@pytest.fixture(scope='session')
def graphite():
    """
    Uses the local configuration to generate a session fixture to a Graphite Database

    :return: Graphite connection object.
    """
    graphite_session = Graphite(CONFIG['Grafana']['server_ip'], CONFIG['Grafana']['server_port'],
                                CONFIG['Grafana']['event_port'])
    yield graphite_session

    if graphite_session:
        graphite_session.close()


@pytest.mark.usefixtures('device_id')
@pytest.fixture(scope='function')
def rebooted_device(device_id):
    """
    This will put the device into a rebooted state and yield information about
        how long it took.

    :param device_id: Command Line Android Device ID
    :return: None
    """
    import time
    from pyadb import ADB

    adb = ADB('/usr/bin/adb')
    # Version 0.1.1 has a problem with iterating of None list,
    # thus the `get_devices()` first.
    adb.get_devices()
    adb.set_target_device(device=device_id)

    start_time = time.time()

    adb.run_cmd('reboot')
    adb.wait_for_device()

    end_time = time.time()

    yield {'reboot': {'start': start_time, 'end': end_time,
                      'duration': end_time - start_time}}


@pytest.mark.usesfixtures('rebooted_device', 'device_id')
@pytest.fixture(scope='function')
def rebooted_and_networked_device(rebooted_device, device_id, request):
    """
    This will put the device into a rebooted state with network up and yield
        information about how long it took.

    :param request: PyTest command line request options
    :return: None
    """
    from multiprocessing import Process, Manager

    from stateutils import network_checker, UNKNOWN

    reboot_information = rebooted_device

    manager = Manager()
    collection_dict = manager.dict()
    maximum_time = 30
    network_connection = request.config.getoption("--network-iface") \
        if request.config.getoption("--network-iface") else 'eth0'

    # Network
    network_process = Process(target=network_checker,
                              args=(network_connection, maximum_time, collection_dict, device_id))
    network_process.daemon = True
    network_process.start()

    ip_address = None
    while ip_address is None:
        try:
            ip_address = collection_dict['ip']['address']
        except KeyError:
            pass
    assert ip_address is not UNKNOWN, \
        'Could not locate find network connection after {:.2f}'.format(maximum_time)

    # Added the Thread-safe dictionary information to the reboot dictionary.
    reboot_information.update(collection_dict.copy())

    yield reboot_information
