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
import datetime
import glob
import json
import os
import shutil
import time
import ConfigParser
from multiprocessing import process, Manager

import pytest
from pyadb import ADB

from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LoggerUtils.logreadLogger import LogreadLogger
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.RivieraUtils import rivieraCommunication, rivieraUtils
from CastleTestUtils.SoftwareUpdateUtils.FastbootFixture.riviera_flash import flash_device

from commonData import keyConfig
from bootsequencing.stateutils import network_checker, UNKNOWN


LOGGER = get_logger(__name__)


def pytest_addoption(parser):
    """
    Command line options for the pytest tests in this module.
    :param parser: Parser used for method.
    :return: None
    """
    parser.addoption("--device-id",
                     action="store",
                     default=None,
                     help="device-id: Device Id")

    parser.addoption("--target",
                     action="store",
                     default="native",
                     help="target: [native/device], \
                            Specify whether the tests need to be executed on native or on device")

    parser.addoption("--log-dir",
                     action="store",
                     default="SCMLogs",
                     help="Where to store logs.")

    parser.addoption("--log-type",
                     action="store",
                     default="useSerial",
                     help="logging : [useSerial / ipBased ]")

    parser.addoption("--network-iface",
                     action="store",
                     default="wlan0",
                     help="network interface to choose")

    parser.addoption("--usb-iface",
                     action="store",
                     default="usb2",
                     help="USB interface to choose")

    parser.addoption("--ip-address",
                     action="store",
                     default=None,
                     help="IP Address of Target under test")

    parser.addoption("--lpm-port",
                     action="store",
                     default=None,
                     help="serial port of the device")

    parser.addoption("--timeout",
                     action="store",
                     default=30,
                     type=int,
                     help="Timeout for most of the commands")

    parser.addoption("--router",
                     action="store",
                     default="testRouter",
                     help="router: Specify which router from Configs/conf_wifiProfiles.ini is used to connect.")

    parser.addoption('--passport-base-url',
                     default='https://test.users.api.bose.io/latest/passport-core/',
                     help='Passport base URL')

    parser.addoption('--api-key',
                     default='9zf6kcZgF5IEsXbrKU6fvG8vFGWzF1Ih',
                     help='Passport API KEY')

def ping(ip):
    """ Pings a given IP Address """
    return os.system("ping -q -c 5 -i 0.2 -w 2 " + ip) == 0

@pytest.fixture(scope='session')
def scm_ip(request):
    """ Get the IP address of Device under Test """
    return request.config.getoption("--scm-ip")

@pytest.fixture(scope='function')
def save_speaker_log(request, device_ip):
    """
    This fixture collects the device log and save to given location.
    """
    LOGGER.info("Starting to save Device logs")
    logreadlogger = LogreadLogger(device_ip)
    try:
        logreadlogger.start_log_collection(testName=request.function.__name__, path="./SpeakerLogs",
                                           saveperiodically=True)
    except:
        LOGGER.info("Error while start: The log from the speaker will not be saved.")

    def teardown():
        """ Stop Speaker Log  Collection """
        LOGGER.info("teardown")
        try:
            logreadlogger.stop_log_collection()
        except:
            LOGGER.info("Error while stop: The log from the speaker will not be saved.")

    request.addfinalizer(teardown)

@pytest.fixture(scope='class')
def device_ip(request, device_id):
    """
    This fixture gets the device IP
    :return: device ip
    """
    LOGGER.info("Trying to retrieve the IP-Address of the device")
    if request.config.getoption("--target").lower() == 'device':
        network_base = NetworkBase(None, device_id)
        interface = request.config.getoption("--network-iface")
        device_ip = network_base.check_inf_presence(interface)
        return device_ip

@pytest.fixture(scope="class")
def frontDoor(device_ip, request):
    """
    Get FrontDoorAPI instance.
    """
    LOGGER.info("Spawning a front door object")
    if device_ip is None:
        pytest.fail("No valid device IP")
    front_door = FrontDoorAPI(device_ip)

    def tear():
        if front_door:
            front_door.close()
    request.addfinalizer(tear)

    return front_door


@pytest.fixture(scope='class')
def riviera(device_id):
    """
    Get RivieraUtil instance.
    """
    return rivieraUtils.RivieraUtils('ADB', device=device_id)


@pytest.fixture(scope='class')
def device_guid(frontDoor):
    """
    Use front door API to obtain device GUID
    """
    LOGGER.info("Getting the GUID of the device")
    device_guid = frontDoor.getInfo()["body"]["guid"]
    assert device_guid != None
    LOGGER.debug("GUID is: %s", device_guid)
    return device_guid

@pytest.fixture(scope='function', autouse=False)
def test_log_banner(request):
    """
    Log start and completed test banner in console output.
    """
    testName = request.function.__name__
    LOGGER.info("\n%s\n----- Start test:    %s\n%s\n", "-" * 60, testName, "-" * 60)

    def teardown():
        """ log banner ends """
        LOGGER.info("\n%s\n----- Completed test:    %s\n%s\n", "-" * 60, testName, "-" * 60)

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


@pytest.fixture(scope='session')
def device_id(request):
    """
    Acquires the Command line Device ID.

    :param request: PyTest command line request options
    :return: String Device ID used when starting the test
    """
    return request.config.getoption('--device-id')


@pytest.mark.usefixture('device_id')
@pytest.fixture(scope='function')
def adb_versions(device_id):
    """
    This fixture will return information regarding version information on the
        ADB system

    :param device_id: Command Line Android Device ID
    :return: Dictionary of Version information of the device
    """
    riviera = rivieraUtils.RivieraUtils(communicationType='ADB', device=device_id)

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

@pytest.fixture(scope='session')
def deviceid(request):
    """
    This fixture will return the device-id
    :return: device-id
    """
    try:
        return request.config.getoption("--device-id")
    except Exception as exception:
        LOGGER.info("Getting device id.... " + str(exception))
    return False

@pytest.fixture(scope='module')
def wifi_config():
    """
    Get config parser instance of wifi profiles.
    """
    LOGGER.info("wifi_config")
    cfg = ConfigParser.SafeConfigParser()
    current_path = os.path.dirname(os.path.realpath(__file__))
    wifi_ini_file = '{}/Configs/conf_wifiProfiles.ini'.format(current_path)
    cfg.read(wifi_ini_file)
    yield cfg


@pytest.fixture(scope="function")
def ip_address_wlan(request, device_id, wifi_config):
    """
    IP address of the device connected to WLAN.
    Removes any configuration on the Device if not connected.
    :param request: PyTest command line request option
    :param device_id: The ADB Device ID of the device under test
    :param wifi_config: ConfigParser object of Wireless configs
    :return: The IP Address of the attached Riviera Device
    """
    riviera_device = rivieraUtils.RivieraUtils('ADB', device=device_id, logger=LOGGER)
    network_base = NetworkBase(None, device=device_id, logger=LOGGER)

    interface = 'wlan0'
    device_ip_address = None
    try:
        device_ip_address = network_base.check_inf_presence(interface, timeout=5)
        LOGGER.info("Found Device IP: %s", device_ip_address)
    except UnboundLocalError as exception:
        LOGGER.warning("Not able to acquire IP Address: %s", exception)
    if not device_ip_address:
        # Clear any WiFi profiles on the device
        clear_profiles = ' '.join(['network', 'wifi', 'profiles', 'clear'])
        clear_profiles = "echo {} | nc 0 17000".format(clear_profiles)
        LOGGER.info("Clearing Network Profiles: %s", clear_profiles)
        riviera_device.communication.executeCommand(clear_profiles)

        # Acquire the Router information
        router = request.config.getoption("--router")
        router_name = wifi_config.get(router, 'ssid')
        security = wifi_config.get(router, 'security')
        password = wifi_config.get(router, 'password')

        # Add Router information to the Device
        add_profile = ' '.join(['network', 'wifi', 'profiles', 'add',
                                router_name, security.upper(), password])
        add_profile = "echo {} | nc 0 17000".format(add_profile)
        LOGGER.info("Adding Network Profile: %s", add_profile)
        riviera_device.communication.executeCommand(add_profile)
        time.sleep(5)
        LOGGER.debug("Rebooting device to ensure added profile retains.")
        riviera_device.communication.executeCommand('/opt/Bose/bin/PlatformReset')
        time.sleep(20)

    device_ip_address = network_base.check_inf_presence(interface, timeout=20)
    if not device_ip_address:
        raise SystemError("Failed to acquire network connection through: {}".format(interface))

    return device_ip_address
