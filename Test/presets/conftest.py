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
PyTest Configuration & Fixtures for the Eddie Preset tests.
"""
import json
import time
from logging import WARNING

import pytest

from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

from musicplayer import music_player_factory

LOGGER = get_logger(__name__, level=20)

STORAGE_DURATION = 1750 * 1.25
PLAYBACK_DURATION = 250 * 1.25
DELAY_AFTER_PRESET_SET = 10


@pytest.mark.usefixtures('riviera', 'ip_address_wlan', 'passport_user', 'user_details')
@pytest.fixture(scope='function')
def riviera_no_presets(riviera, ip_address_wlan, passport_user, user_details):
    """
    Acquires a Riviera object that contains no preset information stored.

    N.B. We can't guarantee that we remove presets by removing
    ProductSettings.json data due to syncs by the Cloud

    :param riviera: CastleTestUtils Riviera device
    :param ip_address_wlan:
    :yield: CastleTestUtils Riviera device with no presets
    """
    front_door = FrontDoorAPI(ip_address_wlan, email=user_details["email"],
                              password=user_details["password"])
    device_guid = front_door.getInfo()["body"]["guid"]
    assert device_guid != None

    # Get the initial Riviera product settings
    initial_product_settings = riviera.get_product_settings()

    updated_product_settings = riviera.remove_all_presets(initial_product_settings, passport_user,
                                                          device_guid)

    sync_message = json.dumps({"update": ["ALL"]}, indent=4)
    front_door.cloudSync(sync_message)
    time.sleep(2)
    front_door.close()

    # Ensure Eddie rebooted
    riviera.communication.rebootDevice_adb()

    # We rebooted device, let's ensure we are still on the network
    riviera.waitUntilNetworkUp(ip_address_wlan)

    # Ensure we removed the presets were removed
    removed_product_settings = riviera.get_product_settings()
    assert not riviera.get_preset_stored(removed_product_settings), \
        'We could not remove presets on device: {}'.format(removed_product_settings)

    yield riviera


@pytest.mark.usefixtures('riviera_no_presets', 'ip_address_wlan')
@pytest.fixture(scope='function')
def riviera_no_presets_playing_music(riviera_no_presets, ip_address_wlan, frontdoor_wlan):
    """
    Sets up a Riviera device that will have no presets and be playing Amazon music.

    :param riviera_no_presets: Fixture of riviera device that has had its presets removed
    :param request: PyTest command line request option
    :yield: Tuple containing a Riviera Device and a FrontDoor WebSocket
    """

    random_music_player = music_player_factory(ip_address_wlan, 'random', frontdoor_wlan)
    random_music_player.front_door.logger.setLevel(WARNING)
    random_music_player.play_next_station()

    # Turn down the music playback volume
    volume_input = 10
    volume_data = {"defaultOn" : 30,
                   "max" : 100,
                   "min" : 0,
                   "muted" : False,
                   "value" : volume_input}
    volume_return = random_music_player.front_door.sendVolume(data=json.dumps(volume_data))
    LOGGER.debug('Sent Volume: %s; Received Volume: %s', volume_input, volume_return)

    # User should have the Riviera device, and, if necessary, a connection to the FrontDoorAPI
    yield riviera_no_presets, random_music_player.front_door

    # Stop the music being played
    random_music_player.stop_playback()


@pytest.fixture(scope='function')
@pytest.mark.usefixtures('music_service_account', 'ip_address_wlan', 'riviera',
                         'user_details')
def eddie_with_user(ip_address_wlan, riviera, user_details):
    """
    Initial stage of an Eddie object fixture that is on the network, has a
    passport account, and has multiple music services attached to it.

    :param passport_user_with_device_music:
    :param ip_address_wlan: The IP Address of the Device. Device will have
        been put online
    :param riviera: A Riviera connection object fixture
    :param tap_wifi: PExpect Telnet Client to the Riviera LPM
    :return: Tuple of a Front Door Connection, Tap Telnet Connection, A
        Riviera Connection, and the IP Address of the Device
    """
    front_door = FrontDoorAPI(ip_address_wlan, email=user_details["email"],
                              password=user_details["password"])
    sync_message = json.dumps({"update": ["ALL"]}, indent=4)
    front_door.cloudSync(sync_message)
    time.sleep(2)
    front_door.close()

    yield riviera, ip_address_wlan
