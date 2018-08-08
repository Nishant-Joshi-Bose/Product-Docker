# conftest.py
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
conftest file for Memory Consumption tests
"""
import json
import pytest
import pexpect

from conf_memoryConsumption import CONFIG
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils

adb = ADBCommunication()

logger = get_logger(__name__)

def pytest_addoption(parser):
    """
    Command line arguments
    """

    parser.addoption("--IsMandatoryDataPush", action="store", default=False,
                     help="Data Push to Graphite")

    parser.addoption("--IsMultiroomSetup", action="store", default=False,
                     help="MultiRoom Setup for Master and Slave")

    parser.addoption('--branch', action="store", default="Master",
                     help='Branch Name')

    parser.addoption("--tar_files", action="store_true", default=False,
                     help="look for the tar files available to flash on device")

    parser.addoption("--network-interface", action="store", default="wlan0",
                     help="network interface to choose")

    parser.addoption("--test-duration",
                     action="store",
                     default="60",
                     help="Duration of the test in minutes")

    parser.addoption("--device",
                     default=None,
                     help="Run the test scenario on passed device")

    parser.addoption('--telnet-port', action='store', type=int, default=17000,
                     help='Port used to connect to the Riviera object through Telnet for LPM/Tap')

@pytest.fixture(scope="session")
def frontdoor(request):
    """
    fixture to get frontdoorobj
    return: frontdoorobj object
    """
    device = request.config.getoption("--device")
    networkbase = NetworkBase(None, device=device)
    interface = request.config.getoption("--network-interface")
    ip_address = networkbase.check_inf_presence(interface)
    frontdoorobj = FrontDoorAPI(ip_address, email="your_email@bose.com", password="password")

    def tear():
        """Closing frontdoor Object"""
        frontdoorobj.close()
    request.addfinalizer(tear)

    return frontdoorobj

@pytest.fixture(scope="function")
def pkill_webkit_process(request):
    """
    Kill the Process and verify the status code 0
    """
    cmd = CONFIG['WEBKIT']['CMD1']
    result = adb.isProcessRunning(cmd)
    logger.debug(result)
    if result[0] is True:
        adb.killProcess(cmd)
    else:
        logger.info("No Webkit Process to kill - %s", result[0])

@pytest.fixture(scope='function', name='tap')
def telnet_client(request):
    """
    A PExpect object connected to Riviera through telnet.

    :param request: PyTest command line request option
    :return: PExpect Telnet connection
    """
    device = request.config.getoption("--device")
    networkbase = NetworkBase(None, device=device)
    interface = request.config.getoption("--network-interface")
    ip_address = networkbase.check_inf_presence(interface)
    port = request.config.getoption('--telnet-port')
    logger.info('Spawning a Telnet connection to %s at %s', ip_address, port)
    child = pexpect.spawn('telnet {} {}'.format(ip_address, port))
    yield child

    # Finally close the Telnet connect through PExpect
    if child:
        child.close()

@pytest.fixture(scope='class')
def perform_cloud_sync(frontdoor):
    """
    Perform cloud sync
    param: frontdoor: fixture returns frontdoor object
    """
    logger.info("perform_cloud_sync")
    sync_message = json.dumps({"update":["ALL"]}, indent=4)
    frontdoor.cloudSync(sync_message)

@pytest.fixture(scope='session')
def riviera_utils(device):
    """
    param: device - Device fixture gets device id from command line
    Return RivieraUtils object
    """
    return RivieraUtils('ADB', device=device)

@pytest.fixture(scope='session')
def device_ip(device, request):
    """
    Use device id to get device ip
    param: device - Device fixture gets device id from command line
    """
    adb.setCommunicationDetail(device)
    network_iface = request.config.getoption("--network-interface")
    logger.info("IP Address is %s", adb.getIPAddress(network_iface))
    return adb.getIPAddress()

@pytest.fixture(scope='session')
def device(request):
    """
    Use request object to get device id from command line
    """
    device = request.config.getoption("--device")
    if device is None:
        pytest.fail("Provide valid Device Id")
    return device

@pytest.fixture(scope='session')
def read_write_device_path(device):
    """
    Fixture to perform read/write on Eddie 
    param: device - Use request object to get device id from command line
    """
    adb = ADBCommunication(logger=logger)
    adb.setCommunicationDetail(device)
    adb.executeCommand("/opt/Bose/bin/rw")
