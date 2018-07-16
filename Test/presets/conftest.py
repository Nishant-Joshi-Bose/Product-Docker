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
import random
import string
import time
from logging import WARNING

import pexpect
import pytest

from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.PassportUtils.passport_api import PassportAPIUsers
from CastleTestUtils.PassportUtils.passport_utils import create_passport_account
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication
from pyadb import ADB

from musicplayer import music_player_factory
from services import SERVICES

LOGGER = get_logger(__name__, level=20)

STORAGE_DURATION = 1750 * 1.25
PLAYBACK_DURATION = 250 * 1.25
DELAY_AFTER_PRESET_SET = 10


def pytest_addoption(parser):
    """
    Preset specific command line parameters

    :param parser: PyTest command line parser object.
    :return: None
    """
    parser.addoption('--communication', action='store', type=str, choices=['ADB', 'SSH'],
                     help='The communication type that will be used to connect to the Riviera device.')

    parser.addoption('--telnet-port', action='store', type=int, default=17000,
                     help='Port used to connect to the Riviera object through Telnet for LPM/Tap')


@pytest.mark.usefixtures('device_ip')
@pytest.fixture(scope='function', name='tap')
def telnet_client(request, device_ip):
    """
    A PExpect object connected to Riviera through telnet.

    :param request: PyTest command line request option
    :return: PExpect Telnet connection
    """
    port = request.config.getoption('--telnet-port')

    LOGGER.info('Spawning a Telnet connection to %s at %s', device_ip, port)
    child = pexpect.spawn('telnet {} {}'.format(device_ip, port))
    yield child

    # Finally close the Telnet connect through PExpect
    if child:
        child.close()


@pytest.mark.usefixtures('ip_address_wlan')
@pytest.fixture(scope='function', name='tap_wifi')
def telnet_client_wifi(request, ip_address_wlan):
    """
    A PExpect object connected to Riviera through telnet.

    :param request: PyTest command line request option
    :return: PExpect Telnet connection
    """
    port = request.config.getoption('--telnet-port')

    LOGGER.info('Spawning a Telnet connection to %s at %s', ip_address_wlan, port)
    child = pexpect.spawn('telnet {} {}'.format(ip_address_wlan, port))
    yield child

    # Finally close the Telnet connect through PExpect
    if child:
        child.close()

@pytest.fixture(scope='module')
def riviera(request):
    """
    Acquires a Riviera device using either ADB or SSH

    :param request: PyTest command line request option
    :return: CastleTestUtils Riviera device
    """
    device_communication = request.config.getoption('--communication') or 'ADB'

    if device_communication.upper() == 'ADB':
        device_id = request.config.getoption('--device-id')
        riviera_device = RivieraUtils(device_communication, device=device_id)
    elif device_communication.upper() == 'SSH':
        raise NotImplementedError('SSH is not implemented for Riviera Objects')
    else:
        raise RuntimeError('You should pick an approved communication type.')

    yield riviera_device

@pytest.mark.usefixtures('riviera', 'ip_address_wlan', 'passport_user', 'passport_user_details')
@pytest.fixture(scope='function')
def riviera_no_presets(riviera, ip_address_wlan, passport_user, passport_user_details):
    """
    Acquires a Riviera object that contains no preset information stored.

    N.B. We can't guarantee that we remove presets by removing
    ProductSettings.json data due to syncs by the Cloud

    :param riviera: CastleTestUtils Riviera device
    :param ip_address_wlan:
    :yield: CastleTestUtils Riviera device with no presets
    """

    # Ensure Eddie rebooted
    #riviera.communication.rebootDevice_adb()

    # Ensure our product is connected to the network
    #riviera.waitUntilNetworkUp(ip_address_wlan)

    front_door = FrontDoorAPI(ip_address_wlan, email=passport_user_details["email"], password=passport_user_details["password"])
    device_guid = front_door.getInfo()["body"]["guid"]
    assert device_guid != None

    # Get the initial Riviera product settings
    initial_product_settings = riviera.get_product_settings()

    updated_product_settings = riviera.remove_all_presets(initial_product_settings, passport_user, device_guid)

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
def riviera_no_presets_playing_music(riviera_no_presets, ip_address_wlan):
    """
    Sets up a Riviera device that will have no presets and be playing Amazon music.

    :param riviera_no_presets: Fixture of riviera device that has had its presets removed
    :param request: PyTest command line request option
    :yield: Tuple containing a Riviera Device and a FrontDoor WebSocket
    """

    random_music_player = music_player_factory(ip_address_wlan, 'random')
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
@pytest.mark.usefixtures('music_service_account', 'ip_address_wlan', 'riviera', 'passport_user_details')
def eddie_with_user(music_service_account, ip_address_wlan, riviera, passport_user_details):
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
    front_door = FrontDoorAPI(ip_address_wlan, email=passport_user_details["email"], password=passport_user_details["password"])
    sync_message = json.dumps({"update": ["ALL"]}, indent=4)
    front_door.cloudSync(sync_message)
    time.sleep(2)
    front_door.close()

    yield riviera, ip_address_wlan
