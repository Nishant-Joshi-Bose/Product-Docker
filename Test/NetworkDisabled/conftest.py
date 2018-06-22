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
    common_behavior_handler = CommonBehaviorHandler(frontdoor_wlan, message_creator, service_name, get_config['name'])

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
        assert passport_user.delete_users(), "Fail to delete person id: %s" % passport_user.bosePersonID
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
def clear_wifi_profiles(device_id):
    """
    This fixture is used to clear the WiFi profiles.

    :param device_id: ADB Device ID of the device under test
    """
    LOGGER.info("Clearing Wireless Profiles...")
    wifi_clear_profile_command = ' '.join(['network', 'wifi', 'profiles', 'clear'])
    adb_utils.adb_telnet_cmd(wifi_clear_profile_command, expect_after='Profiles Deleted',
                             device_id=device_id)
