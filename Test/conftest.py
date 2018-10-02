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

import json
import os
import time

import pytest
import pexpect

from pytest_testrail.plugin import pytestrail
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.FrontDoorAPI.FrontDoorQueue import FrontDoorQueue
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LoggerUtils.logreadLogger import LogreadLogger
from CastleTestUtils.LpmUtils.Lpm import Lpm
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.OAuthUtils.OAuthUtils import UserAccount
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.PassportUtils.passport_utils import get_passport_url
from CastleTestUtils.RivieraUtils import adb_utils, rivieraCommunication, rivieraUtils
from CastleTestUtils.RivieraUtils.device_utils import PRODUCT_STATE

from bootsequencing.stateutils import network_checker, UNKNOWN
from services import SERVICES
from ProductControllerAPI import eddie_helper


from Test.testrail import retrieve_test_cases

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

    parser.addoption("--router",
                     action="store",
                     help="router: Specify which router from Configs/conf_wifiProfiles.ini is used to connect.")

    parser.addoption('--api-key',
                     default='9zf6kcZgF5IEsXbrKU6fvG8vFGWzF1Ih',
                     help='Passport API KEY')

    parser.addoption("--environment",
                     default="integration",
                     help="Pass the Galapagos environment for frontdoor api object")

@pytest.hookimpl
def pytest_collection_modifyitems(config, items):
    """
    Python hook function - https://docs.pytest.org/en/latest/writing_plugins.html
    config - The command line arguments
    items - The list of collected test items
    """
    if config.getoption('--testrail'):
        tests = retrieve_test_cases()
        for item in items:
            for test in tests:
                for testid, testname in test.iteritems():
                    if item.name == testname:
                        item.add_marker(pytestrail.case(testid))

def ping(ip):
    """ Pings a given IP Address """
    return os.system("ping -q -c 5 -i 0.2 -w 2 " + ip) == 0


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


@pytest.mark.usefixtures('request')
@pytest.fixture(scope='session')
def environment(request):
    """
    Galapagos/Passport Environment for the test session.
    :param request: PyTest command line request option
    :return: Environment string
    """
    yield request.config.getoption("--environment")


@pytest.fixture(scope="function")
def frontDoor_without_user(ip_address_wlan, user_details):
    """
    Get the frontDoor Instance
    :param ip_address_wlan: Fixture returning ip address
    :param passport_user_details: Fixture returnng passport user credential
    :param front_door: Instance of FronDoorAPI
    """
    LOGGER.info("frontDoor_without_user")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    front_door = FrontDoorAPI(ip_address_wlan, email=user_details["email"],
                              password=user_details["password"])

    yield front_door

    # Close the FrontDoor
    if front_door:
        front_door.close()


@pytest.fixture(scope="function")
@pytest.mark.usesfixtures('ip_address_wlan', 'user_account', 'environment', 'passport_user_with_device')
def frontdoor_wlan(ip_address_wlan, user_account, environment, passport_user_with_device):
    """
    Frontdoor instance of the device connected to wlan.
    """
    LOGGER.info("frontDoorQueue")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    front_door = FrontDoorQueue(ip_address_wlan, access_token=user_account['access_token'], env=environment)

    # Need to subscribe to notifications
    front_door.send("PUT", '/subscription', json.dumps({"notifications": [{"resource": "*", "version": 1}]}))

    yield front_door

    if front_door:
        front_door.close()


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('request', 'environment', 'user_account')
def passport_user(request, environment, user_account):
    """
    A Passport User object based upon the environment and Gigya User Account.
    """
    LOGGER.info("passport_user")
    passport_base_url = get_passport_url(environment)
    apikey = request.config.getoption('--api-key')

    _bosepersonID = user_account["bosePersonID"]
    _access_token = user_account["access_token"]

    LOGGER.info("Bose Person ID : %s ", _bosepersonID)

    passportUser = PassportAPIUsers(_bosepersonID, apikey, _access_token, passport_base_url)
    LOGGER.debug("Passport User created: %s", passportUser)

    yield passportUser

    response = passportUser.delete_users()
    assert response, "Failed to Delete User: {}".format(passportUser)


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('device_guid', 'passport_user')
def passport_user_with_device(device_guid, passport_user):
    """
    Uses the defined Passport User and adds local Eddie to its account
    """
    LOGGER.info("Adding device (%s) to ", device_guid)

    add_response = passport_user.add_product(device_guid, "Eco2Products")

    LOGGER.debug("Add Product Response: %s", add_response)

    yield passport_user


@pytest.fixture(scope='session')
def user_details():
    """
    Generate Automation User details.
    """
    LOGGER.info("Generating Automation User details")

    first_name = "eddie"
    last_name = "automation"
    time_stamp = int(time.time() * 1000)

    email = "Eddie_Automation_{}@bose.com".format(time_stamp)
    LOGGER.debug("email is: %s", email)

    user_dict = {'email': email, 'password': 'bose901',
                 'first_name': first_name, 'last_name': last_name}

    return user_dict


@pytest.fixture(scope='function')
def user_account(environment, user_details):
    """
    Creates a User Account (Gigya) object with Session scoping
    """
    LOGGER.info("Generating User Account with Session Scope")
    gigya_url = 'https://ingress-platform.live-aws-useast1.bose.io/dev/svc-id-gen-pub/{}/id-user-accounts-core/userAccounts/'.format(environment)

    user_account = UserAccount(url=gigya_url, logger=LOGGER)
    user_account.create_user_account(user_details['email'],
                                     user_details['password'],
                                     user_details['last_name'],
                                     user_details['first_name'])

    response = user_account.authenticate_user_account(user_details['email'], user_details['password'])
    LOGGER.debug("bosePersonID: %s", response['bosePersonID'])
    LOGGER.debug("access_token: %s", response['access_token'])
    LOGGER.debug("refresh_token: %s", response['refresh_token'])
    LOGGER.debug("expires_in: %s", response['expires_in'])

    yield response

    # Delete the user account at the end.
    LOGGER.info("Deleting User-Account")
    deletion_response = user_account.delete_user_account(user_details["email"], user_details["password"])
    assert deletion_response.status_code == 204, \
        "Failed to Delete account: {}".format(response["bosePersonID"])


@pytest.fixture(scope='function')
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

    retries = 25
    while retries > 0:
        response = riviera_device.communication.executeCommand("echo '?' | nc 0 17000 | grep 'network'")
        if response and 'network' in response:
            break
        retries -= 1
        time.sleep(1)
    assert 'network' in response, "Network CLI service never came up."

    if not device_ip_address:
        # Clear any WiFi profiles on the device
        # Clear any WiFi profiles on the device
        clear_profiles = ' '.join(['network', 'wifi', 'profiles', 'clear'])
        clear_profiles = "echo {} | nc 0 17000".format(clear_profiles)
        LOGGER.info("Clearing Network Profiles: {}".format(clear_profiles))

        for _ in range(60):
            network_response = riviera_device.communication.executeCommand(clear_profiles)
            LOGGER.info("Clear wifi profiles response is %s" % network_response)
            if 'Profiles Deleted' in network_response:
                LOGGER.info("Cleared wifi profiles")
                break
            time.sleep(1)

        # Acquire the Router information
        router = request.config.getoption("--router")
        router_name = wifi_config.get(router, 'ssid')
        security = wifi_config.get(router, 'security')
        password = wifi_config.get(router, 'password')

        time.sleep(1)

        # Add Router information to the Device
        add_profile = ' '.join(['network', 'wifi', 'profiles', 'add',
                                router_name, security.upper(), password])
        add_profile = "echo {} | nc 0 17000".format(add_profile)
        LOGGER.info("Adding Network Profile: {}".format(add_profile))

        add_profile_response = riviera_device.communication.executeCommand(add_profile)
        LOGGER.info("Adding Network Profile Response : {}".format(add_profile_response))

        check_profile = ' '.join(['network', 'wifi', 'status'])
        check_profile = "echo {} | nc 0 17000".format(check_profile)

        for _ in range(60):
            network_response = riviera_device.communication.executeCommand(check_profile)
            LOGGER.info("Check wifi profiles response is %s" % network_response)
            if 'WIFI_STATION_CONNECTED' in network_response:
                LOGGER.info("Added wifi profiles")
                break
            time.sleep(1)

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
def force_rndis(adb, device_id, request, wifi_config):
    """
    This fixture enables ADB in PTS mode by creating /mnt/nv/force-rndis
    Reboots the system and checks for rndis0 in the "ifconfig" list
    param: adb - ADB instance
    param: device_id - This fixture will return the device id
    param: wifi_config - Wifi Configuration details
    """
    LOGGER.info("force_rndis")
    command = "touch /mnt/nv/force-rndis"
    adb.executeCommand(command)
    adb.rebootDevice()
    adb.waitforDevice()

    ip_address = ip_address_wlan(request, device_id, wifi_config)
    LOGGER.info("IP address is %s", ip_address)

    network_interface_list_command = "ifconfig | sed 's/[ \t].*//;/^$/d'"
    output = adb.executeCommand(network_interface_list_command)
    network_list = output.strip().replace('\r', '').split('\n')
    LOGGER.info(network_list)
    assert 'rndis0' in network_list, "rndis0 interface not in the network interface list"


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
    rebooted_and_out_of_booting_state_device(device_id)

    # 3. Wait until product state set to 'SetupOther' or 'SetupNetwork' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd(PRODUCT_STATE, expect_after='Current State: ', device_id=device_id, timeout=120)
        if device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK]:
            break
        time.sleep(1)

    device_state = adb_utils.adb_telnet_cmd(PRODUCT_STATE, expect_after='Current State: ', device_id=device_id)
    assert device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK], \
        'Device should be in {} or {} state. Current state : {}'.format(eddie_helper.SETUPOTHER,
                                                                        eddie_helper.SETUPNETWORK, device_state)
    yield

    # 4. At the end of test case revert "NoAudioTimeout" to 20 minutes.
    riviera_comm.executeCommand("/opt/Bose/bin/rw")
    riviera_comm.executeCommand("sed 's/\\\"NoAudioTimeout\\\": 1,/\\\"NoAudioTimeout\\\": 20,/' -i {}".format(timer_file))
    riviera_comm.executeCommand('sync')
    # 5. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(device_id)


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
    rebooted_and_out_of_booting_state_device(device_id)
    LOGGER.info(riviera_comm.executeCommand("cat {}".format(timer_file)))

    yield

    # 3. At the end of test case revert "NoAudioTimeout" to 20 minutes and "NoNetworkConfiguredTimeout" to 120 minutes.
    riviera_comm.executeCommand("/opt/Bose/bin/rw")
    riviera_comm.executeCommand("sed 's/\\\"NoNetworkConfiguredTimeout\\\": 2,/\\\"NoNetworkConfiguredTimeout\\\": 120,/;"
                                "s/\\\"NoAudioTimeout\\\": 1,/\\\"NoAudioTimeout\\\": 20,/' -i {}".format(timer_file))
    riviera_comm.executeCommand('sync')
    LOGGER.info(riviera_comm.executeCommand("cat {}".format(timer_file)))

    # 4. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(device_id)


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('device_id', 'riviera')
def rebooted_and_out_of_booting_state_device(device_id):
    """
    This fixture is used to reboot the device and wait until device come out of 'Booting' state.
    """
    riviera_communication = rivieraCommunication.getCommunicationType('ADB')
    riviera_communication.setCommunicationDetail(device_id)
    riviera_communication.executeCommand('/opt/Bose/bin/PlatformReset')

    # Wait for CLI-Server to start and listens on 17000 port.
    status = None
    for _ in range(90):
        status = riviera_communication.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status and status.strip() == 'OK':
            break
        time.sleep(1)
    assert status, "CLIServer not started within 30s."
    assert (status.strip() == 'OK'), 'CLIServer is not stated even after 30 seconds'
    LOGGER.debug("CLIServer started within 30s.")

    time.sleep(2)

    command = "echo '?' | nc 0 17000 | grep {}".format(PRODUCT_STATE)

    for _ in range(30):
        if riviera_communication.executeCommand(command):
            break
        time.sleep(1)
    LOGGER.debug("product state command is registered to CLI-Server.")

    # Wait until product state come out from 'Booting' state.
    for _ in range(90):
        device_state = adb_utils.adb_telnet_cmd(PRODUCT_STATE, expect_after='Current State: ',
                                                device_id=device_id, timeout=120)
        if device_state != eddie_helper.BOOTING:
            LOGGER.debug("Current device state : %s", device_state)
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
    rebooted_and_out_of_booting_state_device(device_id)

    # Wait until product state set to 'SetupOther' or 'SetupNetwork' state.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd(PRODUCT_STATE, expect_after='Current State: ',
                                                device_id=device_id, timeout=120)
        if device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK]:
            break
        time.sleep(1)

    device_state = adb_utils.adb_telnet_cmd(PRODUCT_STATE, expect_after='Current State: ',
                                            device_id=device_id, timeout=120)
    assert device_state in [eddie_helper.SETUPOTHER, eddie_helper.SETUPNETWORK], \
        'Device should be in {} or {} state. Current state : {}'.format(eddie_helper.SETUPOTHER,
                                                                        eddie_helper.SETUPNETWORK, device_state)


@pytest.fixture(scope='function')
def factory_defaulted_device(riviera):
    """
    Performs factory default on device
    :param riviera: Riviera connection throught ADB
    """
    LOGGER.info("Perform factory default")

    # Perform factory default
    riviera.perform_factory_default()

    yield riviera


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
def tap(ip_address_wlan):
    """
    This fixture is used to get the pexpect client for performing tap commands of CLI keys.
    """
    LOGGER.info("tap - pexpect telnet client")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    client = pexpect.spawn('telnet %s 17000' % ip_address_wlan)
    yield client
    client.close()


@pytest.fixture(scope="module")
def clear_wifi_profiles(device_id):
    """
    This fixture will clear all wifi profiles from the device before test starts
    """
    # Clear any WiFi profiles on the device
    clear_profiles = "network wifi profiles clear"
    LOGGER.debug("Clearing Network Profiles: %s", clear_profiles)
    adb_utils.adb_telnet_cmd(clear_profiles, expect_after='Profiles Deleted', device_id=device_id)


@pytest.fixture(scope='class')
def reboot_device_at_end(request, device_id):
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
        rebooted_and_out_of_booting_state_device(device_id)
    request.addfinalizer(teardown)
