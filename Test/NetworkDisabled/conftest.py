# NetworkDisabled/conftest.py
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
PyTest Configuration & Fixtures for the Eddie Network Disabled tests.
"""
import json
import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.CAPSUtils.TransportUtils.commonBehaviorHandler import CommonBehaviorHandler
from CastleTestUtils.CAPSUtils.TransportUtils.messageCreator import MessageCreator

from CastleTestUtils.scripts.config_madrid import RESOURCES

LOGGER = get_logger(__name__)


@pytest.fixture(scope='function')
def device_playing_from_amazon(frontdoor_wlan, passport_user_with_device):
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

    LOGGER.info("music_service_account")
    account_id = passport_user_with_device.add_service_account(service=get_config['provider'],
                                                               accountID=get_config['name'],
                                                               account_name=get_config['provider_account_id'],
                                                               refresh_token=get_config['secret'])
    assert account_id and account_id != "", "Fail to add music service account."

    response = frontdoor_wlan.cloudSync(json.dumps({"update": ["ALL"]}, indent=4))
    LOGGER.debug("CloudSync Response: %s", response)

    LOGGER.info("verify_device_source")
    common_behavior_handler.checkSourceStatus(service_name, get_config['name'])

    LOGGER.debug("Starting to play: %s", content['container_name'])
    playback_msg = message_creator.playback_msg(get_config['name'], content['container_location'],
                                                content['container_name'], content['track_location'])

    now_playing = common_behavior_handler.playContentItemAndVerifyPlayStatus(playback_msg)
    LOGGER.debug("Now Playing: %s", now_playing)

    yield

    # Remove music service from account.
    LOGGER.info("remove_music_service")
    # get account id for music service
    assert passport_user_with_device.remove_service_account(account_id), \
        "Fail to remove music account from passport account."
