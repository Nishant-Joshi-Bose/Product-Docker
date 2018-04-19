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
PyTest Configuration & Fixtures for the Eddie Factory Default tests.
"""
import time
import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import adb_utils, rivieraCommunication
from CastleTestUtils.LpmUtils.Lpm import Lpm
from CastleTestUtils.FrontDoorAPI.FrontDoorQueue import FrontDoorQueue
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator
from CastleTestUtils.CAPSUtils.TransportUtils.responseHandler import ResponseHandler
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.scripts.config_madrid import RESOURCES

LOGGER = get_logger(__name__)


@pytest.fixture(scope="module")
def lpm_tap(request):
    """
    This fixture is used to get the pexpect client for performing tap commands of CLI keys.
    """
    _lpm_port = request.config.getoption('--lpm-port')
    if _lpm_port is None:
        pytest.fail("LPM port is required: pytest -sv <test.py> --lpm_port </dev/tty.usb-foo>")

    _lpm_tap = Lpm(_lpm_port)

    yield _lpm_tap


@pytest.fixture(scope="function")
def frontdoor_wlan(request, ip_address_wlan):
    """
    Frontdoor instance of the device connected to wlan.
    """
    LOGGER.info("frontDoorQueue")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    _frontdoor = FrontDoorQueue(ip_address_wlan)

    def teardown():
        if _frontdoor:
            _frontdoor.close()

    request.addfinalizer(teardown)

    return _frontdoor


@pytest.fixture(scope='function')
def device_playing_from_amazon(request, frontdoor_wlan):
    """
    This fixture will send playback request to device and verifies the right station or track is playing.
    Test steps:
    1. Play a station or a track by sending playback request to the device
    2. Verify the right station or track is playing by verifying 'nowPlaying' response
    """
    service_name = 'AMAZON'
    current_resource = 'STS_AMAZON_ACCOUNT'
    location = '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0'
    container_name = 'Classic Hits'
    track_location = '/v1/playback/type/playable/url/cHJpbWUvc3RhdGlvbnMvQTEwMlVLQzcxSTVEVTgvI3BsYXlhYmxl/trackIndex/0'
    content = dict(
        container_location=location,
        container_name=container_name,
        track_location=track_location)

    get_config = RESOURCES[current_resource]

    message_creator = MessageCreator(service_name)
    response_handler = ResponseHandler(service_name, get_config['name'])
    common_behavior_handler = CommonBehaviorHandler(frontdoor_wlan, response_handler, message_creator)

    LOGGER.info("Create passport account")
    passport_base_url = request.config.getoption('--passport-base-url')
    apikey = request.config.getoption('--api-key')
    LOGGER.info("Bose Person ID : %s ", frontdoor_wlan._bosepersonID)
    passport_user = PassportAPIUsers(frontdoor_wlan._bosepersonID, apikey, frontdoor_wlan._access_token, passport_base_url, logger=LOGGER)

    def delete_passport_user():
        """
        This function will delete passport user.
        """
        LOGGER.info("delete_passport_user")
        assert passport_user.delete_users(), "Fail to delete person id: %s" % boseperson_id
        common_behavior_handler.performCloudSync()
    request.addfinalizer(delete_passport_user)

    LOGGER.info("music_service_account")
    account_id = passport_user.add_service_account(service=get_config['provider'], accountID=get_config['name'],
                                                   account_name=get_config['provider_account_id'],
                                                   refresh_token=get_config['secret'])
    assert account_id and account_id != "", "Fail to add music service account."

    def remove_music_service():
        """
        This function will remove music service from account.
        """
        LOGGER.info("remove_music_service")
        # get account id for music service
        assert passport_user.remove_service_account(account_id),\
            "Fail to remove music account from passport account."
    request.addfinalizer(remove_music_service)

    common_behavior_handler.performCloudSync()

    LOGGER.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    LOGGER.debug("-- Start to play " + str(content['container_name']))
    playback_msg = message_creator.playback_msg(get_config['name'], content['container_location'],
                                                content['container_name'], content['track_location'])
    now_playing = common_behavior_handler.playContentItemAndVerifyPlayStatus(playback_msg)
    LOGGER.debug("Now Playing : " + str(now_playing))


@pytest.fixture(scope="function")
def rebooted_and_out_of_booting_state_device(deviceid, adb):
    """
    This fixture is used to reboot the device and wait until device come out of 'Booting' state.
    """
    adb.rebootDevice_adb()

    # Wait for CLI-Server to start and listens on 17000 port.
    status = None
    for _ in range(30):
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
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
        if device_state != 'Booting':
            break
        LOGGER.debug("Current device state : %s", device_state)
        time.sleep(1)


@pytest.fixture(scope="function")
def clear_wifi_profiles(deviceid):
    """
    This fixture is used to clear the WiFi profiles.
    """
    LOGGER.info("Clearing Wireless Profiles...")
    wifi_clear_profile_command = ' '.join(['network', 'wifi', 'profiles', 'clear'])
    adb_utils.adb_telnet_cmd(wifi_clear_profile_command, expect_after='Profiles Deleted',
                             device_id=deviceid)


@pytest.fixture(scope="function")
def set_lps_timeout(deviceid):
    """
    This fixture changes "NoAudioTimeout" and "NoNetworkConfiguredTimeout" params to test Low power state transition.
    Test steps:
    1. Change "NoAudioTimeout" to 1 minute and "NoNetworkConfiguredTimeout" to 2 minutes.
    2. Reboot and wait for CLI-Server to start and device state get out of Booting.
    3. At the end of test case revert "NoAudioTimeout" to 20 minutes and "NoNetworkConfiguredTimeout" to 120 minutes.
    4. Reboot and wait for CLI-Server to start and device state get out of Booting.
    """
    adb = rivieraCommunication.getCommunicationType('ADB')
    adb.setCommunicationDetail(deviceid)
    timer_file = '/opt/Bose/etc/InActivityTimer.json'
    # 1. Change "NoAudioTimeout" to 1 minute and "NoNetworkConfiguredTimeout" to 2 minutes.
    adb.executeCommand("/opt/Bose/bin/rw")
    adb.executeCommand("sed 's/\\\"NoNetworkConfiguredTimeout\\\": 120,/\\\"NoNetworkConfiguredTimeout\\\": 2,/;"
                       "s/\\\"NoAudioTimeout\\\": 20,/\\\"NoAudioTimeout\\\": 1,/' -i {}".format(timer_file))
    adb.executeCommand('sync')
    # 2. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(deviceid, adb)

    yield

    # 3. At the end of test case revert "NoAudioTimeout" to 20 minutes and "NoNetworkConfiguredTimeout" to 120 minutes.
    adb.executeCommand("/opt/Bose/bin/rw")
    adb.executeCommand("sed 's/\\\"NoNetworkConfiguredTimeout\\\": 2,/\\\"NoNetworkConfiguredTimeout\\\": 120,/;"
                       "s/\\\"NoAudioTimeout\\\": 1,/\\\"NoAudioTimeout\\\": 20,/' -i {}".format(timer_file))
    adb.executeCommand('sync')
    # 4. Reboot and wait for CLI-Server to start and device state get out of Booting.
    rebooted_and_out_of_booting_state_device(deviceid, adb)
