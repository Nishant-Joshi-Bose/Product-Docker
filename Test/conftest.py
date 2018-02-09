# conftest.py
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
Parent conftest.py for the Eddie repository
"""

import os
import datetime
import pytest
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.RivieraUtils import rivieraCommunication
from CastleTestUtils.SoftwareUpdateUtils.FastbootFixture.riviera_flash import flash_device
from commonData import keyConfig

_log = None
logger = get_logger(__name__)

def pytest_addoption(parser):
    """
    Command line options for the pytest tests in this module.

    :param parser: Parser used for method.
    :return: None
    """
    parser.addoption("--device-id", action="store", default=None, help="device-id: Device Id")
    parser.addoption("--target", action="store", default="native", help="target: [native/device], Specify whether the tests need to be executed on native or on device")
    parser.addoption("--log-dir", action="store", default="SCMLogs", help="Where to store logs.")
    parser.addoption("--log-type", action="store", default="useSerial", help="logging : [useSerial / ipBased ]")
    parser.addoption("--network-iface", action="store", default="wlan0", help="network interface to choose")
    parser.addoption("--ip-address", action="store", default=None, help="IP Address of Target under test")
    parser.addoption("--lpm_port", action="store", default=None, help="serial port of the device")
    parser.addoption("--flash_param",default='f', help="Choose '-a'(APQ) / 'l'(LPM) / '-f'(Complete Flash)")
    parser.addoption("--flash_binary", default='product_flash',help="Binary file to be used - productFlash")
    parser.addoption("--lpm_update", action="store_true", default=False, help="Set True to Update LPM")

    parser.addoption("--timeout",
                     action="store",
                     default=30,
                     type=int,
                     help="Timeout for most of the commands")

def ping(ip):
    """ Pings a given IP Address """
    return os.system("ping -q -c 5 -i 0.2 -w 2 " + ip) == 0

@pytest.fixture(scope='session')
def scm_ip(request):
    """ Get the IP address of Device under Test """
    return request.config.getoption("--scm-ip")

@pytest.fixture(scope='session')
def software_update(flash_device):
    """
    For now; this only calls the Software-Update fixture
    """
    logger.info("Finished Updating Software on the Device")

@pytest.fixture(scope='function')
def save_speaker_log(request, device_ip):
    """
    This fixture collects the device log and save to given location.
    """
    logger.info("save_speaker_log")
    from CastleTestUtils.LoggerUtils.logreadLogger import LogreadLogger
    logreadlogger = LogreadLogger(device_ip)
    try:
        logreadlogger.start_log_collection(testName=request.function.__name__, path="./SpeakerLogs", saveperiodically=True)
    except:
        logger.info("Error while start: The log from the speaker will not be saved.")

    def teardown():
        """ Stop Speaker Log  Collection """
        logger.info("teardown")
        try:
            logreadlogger.stop_log_collection()
        except:
            logger.info("Error while stop: The log from the speaker will not be saved.")

    request.addfinalizer(teardown)

@pytest.fixture(scope='class')
def device_ip(request):
    """
    This fixture gets the device IP
    :return: device ip
    """
    logger.info("device_ip")
    if request.config.getoption("--target").lower() == 'device':
        networkbaseObj = NetworkBase(None)
        iface = request.config.getoption("--network-iface")
        device_ip = networkbaseObj.check_inf_presence(iface)
        return device_ip

@pytest.fixture(scope="class")
def frontDoor(device_ip):
    """
    Get FrontDoorAPI instance.
    """
    logger.info("frontDoor")
    if device_ip is None:
        pytest.fail("No valid device IP")
    frontdoorObj = FrontDoorAPI(device_ip)
    return frontdoorObj

@pytest.fixture(scope='function', autouse=True)
def test_log_banner(request):
    """
    Log start and completed test banner in console output.
    """
    testName = request.function.__name__
    logger.info("\n%s\n----- Start test:    %s\n%s\n", "-" * 60, testName, "-" * 60)

    def teardown():
        """ log banner ends """
        logger.info("\n%s\n----- Completed test:    %s\n%s\n", "-" * 60, testName, "-" * 60)

    request.addfinalizer(teardown)

def create_log_dir(foldername):
    """
    Create a directory to store logs.
    :return: dir - name of the log directory.
    """
    if not os.path.exists(foldername):
        os.mkdir(foldername)
    subfolder = foldername +"/Logs-"+str(datetime.date.today())
    if not os.path.exists(subfolder):
        os.makedirs(subfolder)
    return subfolder

@pytest.fixture(scope='class')
def adb(request):
    """
    Get adb instance
    """
    adb = rivieraCommunication.getCommunicationType('ADB')
    adb.setCommunicationDetail(request.config.getoption("--device-id"))
    return adb


@pytest.fixture(scope='function')
def rebooted_device():
    """
    This will put the device into a rebooted state and yield information about
        how long it took.

    :return: None
    """
    import time
    from pyadb import ADB

    adb = ADB('/usr/bin/adb')
    start_time = time.time()
    adb.run_cmd('reboot')
    adb.wait_for_device()
    end_time = time.time()

    yield {'reboot': {'start': start_time, 'end': end_time,
                      'duration': end_time - start_time}}


@pytest.mark.usesfixtures("rebooted_device")
@pytest.fixture(scope='function')
def rebooted_and_networked_device(rebooted_device, request):
    """
    This will put the device into a rebooted state with network up and yield
        information about how long it took.

    :return: None
    """
    from multiprocessing import Process, Manager

    from bootsequencing.stateutils import network_checker, UNKNOWN

    reboot_information = rebooted_device

    manager = Manager()
    collection_dict = manager.dict()
    maximum_time = 30
    network_connection = request.config.getoption("--network-iface") \
        if request.config.getoption("--network-iface") else 'eth0'

    # Network
    network_process = Process(target=network_checker,
                              args=(network_connection, maximum_time, collection_dict))
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


@pytest.fixture(scope='function')
def adb_versions():
    """
    This fixture will return information regarding version information on the
        ADB system

    :return:
    """
    import json

    from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils

    riviera = RivieraUtils(communicationType='ADB')
    versions = {}

    # Get Product information
    bose_version = riviera.getDeviceBoseVersion()
    versions['bose'] = json.loads(bose_version)
    # Get FS information
    versions['fs'] = riviera.getDeviceFsVersion()

    yield versions


@pytest.fixture(scope='function')
def eddie_master_latest_directory(tmpdir):
    """
    Fixture to download the latest Continuous Master Eddie build locally.

    :param tmpdir: Pytest temporary directory fixture

    :yield: The location of the Eddie Tar and Flash
    """
    import glob
    import os
    import shutil

    # The IP2 Release latest directory
    eddie_latest = os.path.join('/', 'home', 'softlib', 'verisoft', 'Eddie', 'Continuous', 'master', 'latest')

    eddie_flash = 'eddie_flash'
    eddie_tar = 'eddie_*.tar'

    # Copy Eddie Flash File
    eddie_flash_latest = glob.glob(os.path.join(eddie_latest, eddie_flash))[0]
    shutil.copy(eddie_flash_latest, os.path.join(str(tmpdir), 'eddie_flash'))

    # Copy Eddie Tar File
    eddie_tar_latest = glob.glob(os.path.join(eddie_latest, eddie_tar))[0]
    eddie_tar_name = os.path.basename(eddie_tar_latest)
    shutil.copy(eddie_tar_latest, os.path.join(str(tmpdir), eddie_tar_name))

    # Change to tmpdir
    cwd = os.getcwd()
    os.chdir(str(tmpdir))

    yield tmpdir, {'eddie_tar': os.path.join(str(tmpdir), eddie_tar_name),
                   'eddie_flash': os.path.join(str(tmpdir), eddie_flash)}

    # Go back to original working directory
    os.chdir(cwd)
    # Remove everything in the tmpdir
    tmpdir.remove()

@pytest.fixture(scope="session")
def keyConfig():
    keyConfigData = None
    keyConfigData = keyConfig["keyTable"]
    return keyConfigData