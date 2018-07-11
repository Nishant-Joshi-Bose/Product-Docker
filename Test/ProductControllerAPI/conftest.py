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
PyTest Configuration & Fixtures for the Product Controller frontdoor APIs.
"""
import time
import pytest
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.RivieraUtils import adb_utils
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.scripts.config_madrid import RESOURCES
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import eddie_helper

LOGGER = get_logger(__name__)


@pytest.fixture(scope="function")
def front_door_queue(request, ip_address_wlan):
    """
    Get FrontDoorQueue instance.
    """
    LOGGER.info("frontDoorQueue")
    if ip_address_wlan is None:
        pytest.fail("No valid device IP")
    galapagos_env = request.config.getoption("--galapagos-env")
    _frontdoor = FrontDoorQueue(ip_address_wlan, env=galapagos_env)

    if _frontdoor is None:
        pytest.fail("Not able to create socket connection to front_door_queue")

    def teardown():
        if _frontdoor:
            _frontdoor.close()

    request.addfinalizer(teardown)

    return _frontdoor


@pytest.fixture(scope='function')
def device_in_aux(device_id, frontdoor_wlan):
    """
    This fixture is used to change playing source to AUX and verifies the device state.
    Test steps:
    1. Change state to AUX play by pressing AUX button.
    2. Verify device state which should be "SELECTED".
    """
    # Change state to AUX play by pressing AUX button.
    tap = adb_utils.adb_telnet_tap(device_id)
    keypress.press_key(tap, Keys.AUX.value, 500)
    tap.close()
    time.sleep(2)

    # Verify device state which should be "SELECTED".
    state = frontdoor_wlan.getState()
    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)


@pytest.fixture(scope='function')
def device_playing_from_amazon(request, frontdoor_wlan, riviera, environment):
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
    common_behavior_handler = CommonBehaviorHandler(frontdoor_wlan, message_creator,
                                                    service_name, get_config['name'])

    LOGGER.info("Create passport account for music sources")
    passport_base_url = riviera.get_passport_url(environment)
    apikey = request.config.getoption('--api-key')
    LOGGER.info("Bose Person ID : %s ", frontdoor_wlan._bosepersonID)
    passport_user = PassportAPIUsers(frontdoor_wlan._bosepersonID, apikey, frontdoor_wlan._access_token,
                                     passport_base_url, logger=LOGGER)

    def delete_passport_user():
        """
        This function will delete passport user.
        """
        LOGGER.info("delete_passport_user")
        assert passport_user.delete_users(), "Fail to delete person id: {}".format(passport_user.bosePersonID)
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
        assert passport_user.remove_service_account(account_id), \
            "Fail to remove music account from passport account."

    request.addfinalizer(remove_music_service)

    common_behavior_handler.performCloudSync()

    LOGGER.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    LOGGER.debug("-- Start to play " + str(content['container_name']))
    playback_msg = message_creator.playback_msg(account_id, content['container_location'],
                                                content['container_name'], content['track_location'])

    # playing music
    play_response = frontdoor_wlan.sendPlaybackRequest(playback_msg)
    LOGGER.debug("Now Playing: %s", play_response)
    common_behavior_handler.verify_device_playback_response(play_response)
    common_behavior_handler.check_play_status(play_status='PLAY')
    common_behavior_handler.performCloudSync()


    # Verify device state which should be "SELECTED".
    state = frontdoor_wlan.getState()
    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)

    yield frontdoor_wlan, common_behavior_handler, service_name, get_config
