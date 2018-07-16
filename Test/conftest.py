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
Parent conftest.py for the Eddie repository
"""
import datetime
from multiprocessing import Process, Manager
import ConfigParser

import glob
import json
import os
import shutil
import time

import pytest
import pexpect

from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.FrontDoorAPI.FrontDoorQueue import FrontDoorQueue
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LoggerUtils.logreadLogger import LogreadLogger
from CastleTestUtils.LpmUtils.Lpm import Lpm
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.OAuthUtils.OAuthUtils import UserAccount
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.RivieraUtils import adb_utils, rivieraCommunication, rivieraUtils

from commonData import keyConfig
from bootsequencing.stateutils import network_checker, UNKNOWN
from services import SERVICES
from ProductControllerAPI import eddie_helper


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

    parser.addoption("--pts-iface",
                     action="store",
                     default="rndis1",
                     help="rndis interface to choose")

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
                     help="router: Specify which router from Configs/conf_wifiProfiles.ini is used to connect.")

    parser.addoption('--passport-base-url',
                     default='https://test.users.api.bose.io/latest/passport-core/',
                     help='Passport base URL')

    parser.addoption('--api-key',
                     default='9zf6kcZgF5IEsXbrKU6fvG8vFGWzF1Ih',
                     help='Passport API KEY')

    parser.addoption("--galapagos-env",
                     default="latest",
                     help="Pass the Galapagos environment for frontdoor api object")


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
        ip_address = network_base.check_inf_presence(interface)
        return ip_address


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
        """
        Close the frontDoor.
        """
        if front_door:
            front_door.close()
    request.addfinalizer(tear)

    return front_door


@pytest.fixture(scope='session')
def passport_user_details(request):
    """
    Get User details.
    """
    LOGGER.info("passport_user_details")
    fname = "eddie"
    lname = "automation"
    env = request.config.getoption('--galapagos-env')
    _dname = str(time.time()*1000)
    LOGGER.debug("_dname is: %s", _dname)
    email = "Eddie_Automation" + _dname + "@bose.com"
    LOGGER.debug("email is: %s", email)
    user_dict = {"email": email, "password": "bose901", "fname": fname, "lname": lname, "env": env}
    return user_dict


@pytest.fixture(scope='session')
def create_passport_user(request, passport_user_details):
    """
    Get passport user object
    """
    LOGGER.info("passport_user_withoutFD")
    gigya_url = "https://ingress-platform.live-aws-useast1.bose.io/dev/svc-id-gen-pub/" + passport_user_details["env"] + "/id-user-accounts-core/userAccounts/"
    user_account = UserAccount(url=gigya_url, logger=LOGGER)
    user_account.create_user_account(passport_user_details["email"],
                                     passport_user_details["password"],
                                     passport_user_details["lname"],
                                     passport_user_details["fname"])
    response = user_account.authenticate_user_account(passport_user_details["email"], passport_user_details["password"])
    LOGGER.info("The bosePersonID is: %s", response["bosePersonID"])
    LOGGER.info("The access token is: %s", response["access_token"])
    LOGGER.info("The refresh token is: %s", response["refresh_token"])
    LOGGER.info("The time to live is: %s", response["expires_in"])

    def tear():
        """
        Delete the user account at the end.
        """
        user_account = UserAccount(url=gigya_url, logger=LOGGER)
        LOGGER.info("Deleting User-Account")
        user_account.delete_user_account(passport_user_details["email"], passport_user_details["password"])
    request.addfinalizer(tear)

    return response


@pytest.mark.usefixtures('request')
@pytest.fixture(scope='session')
def environment(request):
    """
    Galapagos/Passport Environment for the test session.

    :param request: PyTest command line request option
    :return: Environment string
    """
    yield request.config.getoption("--galapagos-env")


@pytest.fixture(scope="function")
def frontDoor_without_user(request, ip_address_wlan, passport_user_details):
    """
    Get the frontDoor Instance
    :param request: pytest request object
    :param ip_address_wlan: Fixture returning ip address
    :param passport_user_details: Fixture returnng passport user credential
    :param front_door: Instance of FronDoorAPI
    """
    LOGGER.info("frontDoor_without_user")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    front_door = FrontDoorAPI(ip_address_wlan, email=passport_user_details["email"],
                              password=passport_user_details["password"])

    def tear():
        """
        Close the FrontDoor
        """
        if front_door:
            front_door.close()
    request.addfinalizer(tear)

    return front_door


@pytest.fixture(scope='session')
def passport_user(request, create_passport_user):
    LOGGER.info("passport_user")
    passport_base_url = request.config.getoption('--passport-base-url')
    apikey = request.config.getoption('--api-key')
    _bosepersonID = create_passport_user["bosePersonID"]
    _access_token = create_passport_user["access_token"]
    LOGGER.info("Bose Person ID : %s ", _bosepersonID)
    passportUser = PassportAPIUsers(_bosepersonID, apikey, _access_token, passport_base_url, LOGGER)
    return passportUser


@pytest.fixture(scope='session')
def music_service_account(request, passport_user):
    """
    Add music service account
    :param request: pytest request fixture
    :param passport_user: fixture returns reference to current PassportAPIUsers
    :param get_config: fixture returns dictionary of current config loaded from either global resources or config file
    :param common_behavior_handler: fixture returns reference to CommonBehaviorHandler
    """
    LOGGER.info("music_service_account")
    service_accountid = []
    for service in SERVICES:
        LOGGER.debug("Adding {} to {}".format(service, passport_user))
        account = SERVICES[service]['account']
        if account['provider'] != 'TUNEIN':
            accountId = passport_user.add_service_account(accountType=account['account_type'],
                                                          service=account['provider'],
                                                          accountID=account['name'],
                                                          account_name=account['name'],
                                                          refresh_token=account['secret'])
            service_accountid.append(accountId)
            assert accountId and accountId != "", "Fail to add music service account."

    def remove_music_service():
        """
        Delete the music services
        """
        LOGGER.info("remove_music_service")
        # get account id for music service
        for account_id in service_accountid:
            assert passport_user.remove_service_account(account_id), "Fail to remove music account from passport account."
    request.addfinalizer(remove_music_service)


@pytest.fixture(scope='function')
def create_passport_user_function(request, passport_user_details):
    """
    Get passport user object
    """
    LOGGER.info("create passport user at function scope")
    gigya_url = "https://ingress-platform.live-aws-useast1.bose.io/dev/svc-id-gen-pub/" + passport_user_details["env"] + "/id-user-accounts-core/userAccounts/"
    user_account = UserAccount(url=gigya_url, logger=LOGGER)
    user_account.create_user_account(passport_user_details["email"],
                                     passport_user_details["password"],
                                     passport_user_details["lname"],
                                     passport_user_details["fname"])
    response = user_account.authenticate_user_account(passport_user_details["email"], passport_user_details["password"])
    LOGGER.info("The bosePersonID is: %s", response["bosePersonID"])
    LOGGER.info("The access token is: %s", response["access_token"])
    LOGGER.info("The refresh token is: %s", response["refresh_token"])
    LOGGER.info("The time to live is: %s", response["expires_in"])

    def tear():
        """
        Delete the user account at the end.
        """
        user_account = UserAccount(url=gigya_url, logger=LOGGER)
        LOGGER.info("Deleting User-Account")
        user_account.delete_user_account(passport_user_details["email"], passport_user_details["password"])
    request.addfinalizer(tear)

    return response


@pytest.fixture(scope='session')
def riviera(device_id):
    """
    Get RivieraUtil instance.
    """
    return rivieraUtils.RivieraUtils('ADB', device=device_id)


@pytest.fixture(scope='session')
@pytest.mark.usefixtures('riviera')
def device_guid(riviera):
    """
    Acquires the GUID for the Device under test
    :param riviera: Instance of RivieraUtils
    :return: GUID string
    """
    guid = riviera.communication.executeCommand('mfgdata get guid').strip()
    LOGGER.debug("GUID is: %s", guid)
    return guid


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('passport_user', 'device_guid')
def frontDoor_with_user(environment, riviera, create_passport_user_function, passport_user_details, ip_address_wlan):
    """
    Adding the product using passportAPI
    """
    LOGGER.info("Link the product after user account")
    product_response = riviera.link_product(environment=environment, ip_address=ip_address_wlan,
                                            email=passport_user_details["email"], password=passport_user_details["password"])
    LOGGER.debug("Product response is %s", product_response)


@pytest.fixture(scope='function', autouse=False)
def test_log_banner(request):
    """
    Log start and completed test banner in console output.
    """
    test_name = request.function.__name__
    LOGGER.info("\n%s\n----- Start test:    %s\n%s\n", "-" * 60, test_name, "-" * 60)

    def teardown():
        """ log banner ends """
        LOGGER.info("\n%s\n----- Completed test:    %s\n%s\n", "-" * 60, test_name, "-" * 60)

    request.addfinalizer(teardown)


def create_log_dir(foldername):
    """
    Create a directory to store logs.
    :return: dir - name of the log directory.
    """
    if not os.path.exists(foldername):
        os.mkdir(foldername)
    subfolder = foldername + "/Logs-" + str(datetime.date.today())
    if not os.path.exists(subfolder):
        os.makedirs(subfolder)
    return subfolder


@pytest.mark.usefixture('device_id')
@pytest.fixture(scope='class')
def adb(device_id):
    """
    Get adb instance
    """
    adb = rivieraCommunication.getCommunicationType('ADB')
    adb.setCommunicationDetail(device_id)
    return adb


@pytest.fixture(scope='session')
def device_id(request):
    """
    Acquires the Command line Device ID.

    :param request: PyTest command line request options
    :return: String Device ID used when starting the test
    """
    return request.config.getoption('--device-id')


@pytest.mark.usefixture('riviera')
@pytest.fixture(scope='function')
def adb_versions(riviera):
    """
    This fixture will return information regarding version information on the
        ADB system

    :param riviera: Riviera connection throught ADB
    :return: Dictionary of Version information of the device
    """
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
def key_config():
    """
    Get the CLI key event details
    """
    key_data = keyConfig["keyTable"]
    return key_data


@pytest.fixture(scope='module')
def wifi_config():
    """
    Generates a ConfigParser object of locally stored wireless base
    station profiles.

    See ./Configs/conf_wifiProiles.ini for details and configuration
    setups.

    :return: ConfigParser object of Wireless configs
    """
    LOGGER.info("wifi_config")
    config = ConfigParser.SafeConfigParser()
    current_path = os.path.dirname(os.path.realpath(__file__))
    wifi_ini_file = '{}/Configs/conf_wifiProfiles.ini'.format(current_path)

    config.read(wifi_ini_file)

    yield config


@pytest.mark.usefixture('request', 'device_id', 'wifi_config')
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
    interface = request.config.getoption("--network-iface")
    device_ip_address = None
    try:
        device_ip_address = network_base.check_inf_presence(interface, timeout=5)
        LOGGER.info("Found Device IP: %s", device_ip_address)
    except UnboundLocalError as exception:
        LOGGER.warning("Not able to acquire IP Address: %s", exception)

    LOGGER.info("Device IP Address: %s", repr(device_ip_address))

    # Wait until LPM connection is available
    status = None
    query = '(netstat -tnl | grep -q 17000) && echo OK'
    for _ in range(90):
        status = riviera_device.communication.executeCommand(query)
        if status and status.strip() == 'OK':
            break
        time.sleep(1)
    assert status and (status.strip() == 'OK'), "CLIServer not started within 90s."

    # Check whether the LPM and CLI services are running or not over ADB connection
    retries = 25
    lpm_state = None
    cli_state = None
    while retries > 0:
        lpm_state = False
        cli_state = False
        out = riviera_device.communication.executeCommand("\"ps -a | grep LPM\"")
        if "LPMService" in out:
            lpm_state = True
        out = riviera_device.communication.executeCommand("\"ps -a | grep CLI\"")
        if "CLIServer" in out:
            cli_state = True
        if lpm_state and cli_state:
            break
        retries -= 1
        time.sleep(1)
    assert lpm_state and cli_state, "LPM ({}) and CLI ({}) not activated.".format(lpm_state, cli_state)

    if not device_ip_address:
        # Clear any WiFi profiles on the device
        clear_profiles = ' '.join(['network', 'wifi', 'profiles', 'clear'])
        LOGGER.info("Clearing Network Profiles: %s", clear_profiles)
        adb_utils.adb_telnet_cmd(clear_profiles, expect_after='Profiles Deleted',
                                 device_id=device_id, timeout=120)

        # Acquire the Router information
        router = request.config.getoption("--router")
        LOGGER.debug("Router Connection Name: %s", router)
        router_name = wifi_config.get(router, 'ssid')
        security = wifi_config.get(router, 'security')
        password = wifi_config.get(router, 'password')

        # Add Router information to the Device
        add_profile = ' '.join(['network', 'wifi', 'profiles', 'add',
                                router_name, security.upper(), password])
        LOGGER.info("Adding Network Profile: %s", add_profile)

        adb_utils.adb_telnet_cmd(add_profile, expect_after='->OK', expect_last='ADD_PROFILE_SUCCEEDED',
                                 async_response=True, device_id=device_id, timeout=120)

        device_ip_address = network_base.check_inf_presence(interface, timeout=20)

        if not device_ip_address:
            LOGGER.debug("Rebooting device to ensure added profile retains.")
            riviera_device.communication.executeCommand('/opt/Bose/bin/PlatformReset')
            device_ip_address = network_base.check_inf_presence(interface, timeout=60)

            if not device_ip_address:
                pytest.fail("Failed to acquire network connection through: {}".format(interface))

    # Wait for galapagos activation even if network is already configured.
    assert riviera_device.wait_for_galapagos_activation(), 'galapagos activation is not done yet.'

    LOGGER.debug("Found IP Address (%s) for Device (%s).", device_ip_address, device_id)
    return device_ip_address


@pytest.fixture(scope='module')
def add_wifi_at_end(request, device_id, wifi_config):
    """
    Add the wifi network at the end.
    """
    LOGGER.debug("add_wifi_at_end")

    yield

    if not request.config.getoption('--router'):
        LOGGER.info("No router defined.")
        return

    LOGGER.info("Executing after yield")
    riviera_comm = rivieraCommunication.getCommunicationType('ADB')
    riviera_comm.setCommunicationDetail(device_id)

    status = None
    for _ in range(90):
        status = riviera_comm.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status and status.strip() == 'OK':
            break
        time.sleep(1)

    assert status, "CLIServer not started within 90s."
    assert (status.strip() == 'OK'), 'CLIServer is not stated even after 90 seconds'

    time.sleep(2)

    LOGGER.info("Executing ip_address_wlan")

    ip_address_wlan(request, device_id, wifi_config)


@pytest.mark.usefixtures('adb')
@pytest.fixture(scope='function')
def rebooted_device(adb):
    """
    This will put the device into a rebooted state and yield information about
        how long it took.

    :param adb: ADB Communication Object
    :return: None
    """
    start_time = time.time()
    adb.executeCommand('/opt/Bose/bin/PlatformReset')
    adb.waitforDevice()
    end_time = time.time()

    duration = end_time - start_time
    LOGGER.debug("Reboot took %.2f", duration)

    yield {'reboot': {'start': start_time, 'end': end_time,
                      'duration': end_time - start_time}}


@pytest.mark.usesfixtures('request', 'adb', 'device_id', 'ip_address_wlan')
@pytest.fixture(scope='function')
def rebooted_and_networked_device(request, adb, device_id, ip_address_wlan):
    """
    This will put the device into a rebooted state with network up and yield
        information about how long it took.

    :param request: PyTest command line request options
    :return: Rebooted Information
    """
    LOGGER.debug("Have an IP Address of %s.", ip_address_wlan)

    LOGGER.info("Rebooting Device %s.", device_id)
    reboot_information = {}
    start_time = time.time()
    adb.executeCommand('/opt/Bose/bin/PlatformReset')
    adb.waitforDevice()
    end_time = time.time()

    duration = end_time - start_time
    LOGGER.debug("Reboot took %.2f", duration)

    reboot_information = {'reboot': {'start': start_time, 'end': end_time,
                                     'duration': end_time - start_time}}

    manager = Manager()
    collection_dict = manager.dict()
    maximum_time = 60
    network_connection = request.config.getoption("--network-iface") \
        if request.config.getoption("--network-iface") else 'wlan0'
    LOGGER.debug("Looking for IP Address on %s", network_connection)

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


@pytest.fixture(scope='function')
def force_rndis(adb, device_id, request):
    """
    This fixture enables ADB in PTS mode by creating /mnt/nv/force-rndis
    Reboots the system and checks for rndis0 in the "ifconfig" list
    param: adb - ADB instance
    param: device_id - This fixture will return the device id
    """
    LOGGER.info("force_rndis")
    command = "touch /mnt/nv/force-rndis"
    adb.executeCommand(command)
    adb.rebootDevice()
    adb.waitforDevice()
    network_base = NetworkBase(None, device=device_id, logger=LOGGER)
    interface = request.config.getoption("--network-iface")
    device_ip_address = None
    try:
        device_ip_address = network_base.check_inf_presence(interface, timeout=300)
        LOGGER.info("Found Device IP: %s", device_ip_address)
    except UnboundLocalError as exception:
        LOGGER.warning("Not able to acquire IP Address: %s", exception)
    network_interface_list_command = "ifconfig | sed 's/[ \t].*//;/^$/d'"
    output = adb.executeCommand(network_interface_list_command)
    network_list = output.strip().replace('\r', '').split('\n')
    LOGGER.info(network_list)
    if 'rndis0' not in network_list:
        raise Exception('rndis0 interface not in the list')


@pytest.fixture(scope="function")
@pytest.mark.usesfixtures('ip_address_wlan', 'passport_user_details')
def frontdoor_wlan(ip_address_wlan, frontDoor_with_user, passport_user_details):
    """
    Frontdoor instance of the device connected to wlan.
    """
    LOGGER.info("frontDoorQueue")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    _frontdoor = FrontDoorQueue(ip_address_wlan, email=passport_user_details["email"],
                                password=passport_user_details["password"])

    yield _frontdoor

    if _frontdoor:
        _frontdoor.close()


@pytest.fixture(scope="function")
def set_no_audio_timeout(device_id):
    """
    This fixture changes "NoAudioTimeout" param to test NetworkStandby state transition.
    Test steps:
    1. Change "NoAudioTimeout" to 1 minute.
    2. Reboot and wait for CLI-Server to start and device state get out of Booting.
    3. Wait until product state set to 'SetupOther' or 'SetupNetwork' state.
    4. At the end of test case revert "NoAudioTimeout" to 20 minutes.
    5. Reboot and wait for CLI-Server to start and device state get out of Booting.

    :param device_id: ADB Device ID of the device under test
    """
    riviera_comm = rivieraCommunication.getCommunicationType('ADB')
    riviera_comm.setCommunicationDetail(device_id)
    timer_file = '/opt/Bose/etc/InActivityTimer.json'
    # 1. Change "NoAudioTimeout" to 1 minute.
    riviera_comm.executeCommand("/opt/Bose/bin/rw")
    riviera_comm.executeCommand("sed 's/\\\"NoAudioTimeout\\\": 20,/\\\"NoAudioTimeout\\\": 1,/' -i {}".format(timer_file))
    riviera_comm.executeCommand('sync')
    # 2. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(device_id, riviera_comm)

    # 3. Wait until product state set to 'SetupOther' or 'SetupNetwork' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id, timeout=120)
        if device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK]:
            break
        time.sleep(1)

    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
    assert device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK], \
        'Device should be in {} or {} state. Current state : {}'.format(eddie_helper.SETUPOTHER,
                                                                        eddie_helper.SETUPNETWORK, device_state)
    yield

    # 4. At the end of test case revert "NoAudioTimeout" to 20 minutes.
    riviera_comm.executeCommand("/opt/Bose/bin/rw")
    riviera_comm.executeCommand("sed 's/\\\"NoAudioTimeout\\\": 1,/\\\"NoAudioTimeout\\\": 20,/' -i {}".format(timer_file))
    riviera_comm.executeCommand('sync')
    # 5. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(device_id, riviera_comm)


@pytest.fixture(scope="function")
def set_lps_timeout(device_id):
    """
    This fixture changes "NoAudioTimeout" and "NoNetworkConfiguredTimeout" params to test Low power state transition.
    Test steps:
    1. Change "NoAudioTimeout" to 1 minute and "NoNetworkConfiguredTimeout"
       to 2 minutes.
    2. Reboot and wait for CLI-Server to start and
       device state get out of Booting.
    3. At the end of test case revert "NoAudioTimeout" to 20 minutes and
       "NoNetworkConfiguredTimeout" to 120 minutes.
    4. Reboot and wait for CLI-Server to start and device state get out of Booting.

    :param device_id: ADB Device ID of the device under test
    """
    riviera_comm = rivieraCommunication.getCommunicationType('ADB')
    riviera_comm.setCommunicationDetail(device_id)
    timer_file = '/opt/Bose/etc/InActivityTimer.json'
    # 1. Change "NoAudioTimeout" to 1 minute and "NoNetworkConfiguredTimeout" to 2 minutes.
    riviera_comm.executeCommand("/opt/Bose/bin/rw")
    riviera_comm.executeCommand("sed 's/\\\"NoNetworkConfiguredTimeout\\\": 120,/\\\"NoNetworkConfiguredTimeout\\\": 2,/;"
                                "s/\\\"NoAudioTimeout\\\": 20,/\\\"NoAudioTimeout\\\": 1,/' -i {}".format(timer_file))
    riviera_comm.executeCommand('sync')
    LOGGER.info(riviera_comm.executeCommand("cat {}".format(timer_file)))
    # 2. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(device_id, adb)
    LOGGER.info(riviera_comm.executeCommand("cat {}".format(timer_file)))

    yield

    # 3. At the end of test case revert "NoAudioTimeout" to 20 minutes and "NoNetworkConfiguredTimeout" to 120 minutes.
    riviera_comm.executeCommand("/opt/Bose/bin/rw")
    riviera_comm.executeCommand("sed 's/\\\"NoNetworkConfiguredTimeout\\\": 2,/\\\"NoNetworkConfiguredTimeout\\\": 120,/;"
                                "s/\\\"NoAudioTimeout\\\": 1,/\\\"NoAudioTimeout\\\": 20,/' -i {}".format(timer_file))
    riviera_comm.executeCommand('sync')
    LOGGER.info(riviera_comm.executeCommand("cat {}".format(timer_file)))

    # 4. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(device_id, adb)


@pytest.fixture(scope="function")
def rebooted_and_out_of_booting_state_device(device_id, adb):
    """
    This fixture is used to reboot the device and wait until device come out of 'Booting' state.
    """
    adb.rebootDevice_adb()

    # Wait for CLI-Server to start and listens on 17000 port.
    status = None
    for _ in range(90):
        status = adb.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status and status.strip() == 'OK':
            break
        time.sleep(1)
    assert status, "CLIServer not started within 30s."
    assert (status.strip() == 'OK'), 'CLIServer is not stated even after 30 seconds'
    LOGGER.debug("CLIServer started within 30s.")

    time.sleep(2)

    for _ in range(10):
        if adb.executeCommand("echo '?' | nc 0 17000 | grep 'getproductstate'"):
            break
        time.sleep(1)
    LOGGER.debug("getproductstate command is registered to CLI-Server.")

    # Wait until product state come out from 'Booting' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ',
                                                device_id=device_id, timeout=120)
        if device_state != eddie_helper.BOOTING:
            break
        LOGGER.debug("Current device state : %s", device_state)
        time.sleep(1)


@pytest.fixture(scope="function")
def remove_oob_setup_state_and_reboot_device(device_id, adb):
    """
    This fixture is used to remove "/mnt/nv/product-persistence/SystemSetupDone" file and reboot the device.
    """
    oob_setup_file = '/mnt/nv/product-persistence/SystemSetupDone'
    # 1. Remove "/mnt/nv/product-persistence/SystemSetupDone" file.
    adb.executeCommand("/opt/Bose/bin/rw")
    adb.executeCommand("rm {}".format(oob_setup_file))
    adb.executeCommand('sync')
    rebooted_and_out_of_booting_state_device(device_id, adb)

    # Wait until product state set to 'SetupOther' or 'SetupNetwork' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ',
                                                device_id=device_id, timeout=120)
        if device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK]:
            break
        time.sleep(1)

    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ',
                                            device_id=device_id, timeout=120)
    assert device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK], \
        'Device should be in {} or {} state. Current state : {}'.format(eddie_helper.SETUPOTHER,
                                                                        eddie_helper.SETUPNETWORK, device_state)


@pytest.fixture(scope='session')
def lpm_serial_client(request):
    """
    Generates an LPM Serial Client using the supplied port.

    :param request: PyTest command line request object
    :yield: LPM Serial Client connection
    """
    lpm_serial_path = request.config.getoption('--lpm-port')

    # LPM tap client.
    lpm_serial = Lpm(lpm_serial_path)

    yield lpm_serial

    # Close the client
    del lpm_serial


@pytest.fixture(scope="function")
def tap(device_ip):
    """
    This fixture is used to get the pexpect client for performing tap commands of CLI keys.
    """
    LOGGER.info("tap - pexpect telnet client")
    if device_ip is None:
        pytest.fail("No valid device IP")
    client = pexpect.spawn('telnet %s 17000' % device_ip)
    yield client
    client.close()


@pytest.fixture(scope='class')
def reboot_device_at_end(request, device_id, adb):
    """
    Reboots the device at the end.

    :param request: PyTest command line request object
    :param device_id: ADB Device ID of the device under test
    :param adb: Get adb instance
    """
    LOGGER.info("reboot_device_at_end")
    def teardown():
        """ Teardown module to reboot device """
        LOGGER.info("Rebooting device in teardown")
        rebooted_and_out_of_booting_state_device(device_id, adb)
    request.addfinalizer(teardown)
