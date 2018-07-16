# test_preset_storage.py
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
PyTest based tests for interacting with the Eddie Preset storage mechanism.
"""
import time
from random import sample
import pytest
import logging
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from pytest_testrail.plugin import pytestrail
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from musicplayer import music_player_factory
from ..services import MusicService
from conftest import STORAGE_DURATION, PLAYBACK_DURATION, DELAY_AFTER_PRESET_SET

LOGGER = get_logger(__name__, "Preset-Storage.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)

INVALID_SOURCE = 'INVALID_SOURCE'

@pytestrail.case('C618574', 'C618575')
@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'frontDoor_without_user', 'tap_wifi')
def test_store_same_all_presets(eddie_with_user, riviera_no_presets, frontDoor_without_user, tap_wifi):

    """
    Test that will ensure that we can store all preset locations with the same
    audio stream.

    :param riviera: A Riviera Device fixture
    :param tap: A PExpect client to the Riviera device
    :return: None
    """
    riviera, device_ip = eddie_with_user
    LOGGER.debug("Setting up a Music Player at {}".format(device_ip))
    rmp = music_player_factory(device_ip, MusicService.TUNEIN, frontDoor_without_user, log_level=10)
    station_response = rmp.play_next_station()
    response_location = station_response['body']['container']['contentItem']['location']

    # Press all presets sequentially
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, STORAGE_DURATION)

    # We want to ensure the Key Press was received and stored in local file store.
    time.sleep(DELAY_AFTER_PRESET_SET)

    current_product_settings = riviera.get_product_settings()
    presets = riviera.get_preset_stored(current_product_settings)
    assert len(presets) == 6, \
        'Incorrect number of presets in {}. Anticipated 6, found {}.'.format(current_product_settings, len(presets))

    # Randomly ensure that all stored presets are storing the same Now Playing information
    for preset in sample(presets, len(presets)):
        preset_location = riviera.get_preset_content(current_product_settings, int(preset))['actions'][0]['payload']['contentItem']['location']
        assert preset_location == response_location, \
            'Preset {} found to have location {}, should have location {}'\
                .format(preset, preset_location, response_location)

    # Finally stop the playback
    rmp.stop_playback()


@pytestrail.case('C657645', 'C657646', 'C657647')
@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'frontDoor_without_user', 'tap_wifi')
def test_store_random_station_on_all_keys(eddie_with_user, riviera_no_presets, frontDoor_without_user, tap_wifi):
    """
    Test will ensure that multiple different music sources and stations can be stored

    :param riviera: A Riviera Device fixture
    :param tap: A PExpect client to the Riviera device
    :return: None
    """

    riviera, device_ip = eddie_with_user

    preset_numbers = range(1, 7, 1)
    preset_keys = [preset_number + 7 for preset_number in preset_numbers]

    rmp = music_player_factory(device_ip, MusicService.RANDOM, frontDoor_without_user)
    for preset_number in preset_numbers:
        station_response = rmp.play_next_station()
        music_service = station_response['body']['container']['contentItem']['source']

        preset_key = preset_keys[preset_number - 1]

        LOGGER.debug("Attempting to store a %s station to Preset #%s", music_service, preset_number)

        # Store the currently played to the Preset
        keypress.press_key(tap_wifi, preset_key, STORAGE_DURATION)
        time.sleep(DELAY_AFTER_PRESET_SET)

        current_product_settings = riviera.get_product_settings()
        assert preset_number in riviera.get_preset_stored(current_product_settings), \
            "Not able to find Preset #{} in Current Settings: {}".format(preset_number, current_product_settings)

        # Check that the current preset is correct
        preset_location = riviera.get_preset_content(current_product_settings, preset_number)['actions'][0]['payload']['contentItem']['location']
        response_location = station_response['body']['container']['contentItem']['location']

        assert preset_location == response_location, \
            "Preset Location ({}) does not match Response Location ({}) for Preset #{}" \
                .format(preset_location, response_location, preset_number)

    # Finally stop the playback
    rmp.stop_playback()


@pytestrail.case('C657648')
@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'frontDoor_without_user', 'tap_wifi', 'device_id', 'request')
def test_storing_persists_after_reboot(eddie_with_user, riviera_no_presets, frontDoor_without_user, tap_wifi, device_id, request):
    """
    This test will ensure that random stations saved to the device persist
    after a reboot of the device.

    :param riviera: A Riviera Device fixture
    :param tap: A PExpect client to the Riviera device
    :return: None
    """

    riviera, device_ip = eddie_with_user

    preset_numbers = range(1, 7, 1)
    preset_keys = [preset_number + 7 for preset_number in preset_numbers]

    current_product_settings = riviera.get_product_settings()

    rmp = music_player_factory(device_ip, MusicService.RANDOM, frontDoor_without_user)
    for preset_number in preset_numbers:
        station_response = rmp.play_next_station()
        music_service = station_response['body']['container']['contentItem']['source']

        preset_key = preset_keys[preset_number - 1]

        LOGGER.debug("Attempting to store a %s station to Preset #%s", music_service, preset_number)

        # Store the currently played to the Preset
        keypress.press_key(tap_wifi, preset_key, STORAGE_DURATION)
        time.sleep(DELAY_AFTER_PRESET_SET)

        current_product_settings = riviera.get_product_settings()
        assert preset_number in riviera.get_preset_stored(current_product_settings), \
            "Not able to find Preset #{} in Current Settings: {}".format(preset_number, current_product_settings)

        # Check that the current preset is correct
        preset_location = riviera.get_preset_content(current_product_settings, preset_number)['actions'][0]['payload']['contentItem']['location']
        response_location = station_response['body']['container']['contentItem']['location']

        assert preset_location == response_location, \
            "Preset Location ({}) does not match Response Location ({}) for Preset #{}" \
                .format(preset_location, response_location, preset_number)

    rmp.stop_playback()

    # Reboot the device
    riviera.communication.rebootDevice_adb()
    network_base = NetworkBase(None, device_id)
    interface = request.config.getoption("--network-iface")
    device_ip = network_base.check_inf_presence(interface)
    time.sleep(10)

    rebooted_product_settings = riviera.get_product_settings()
    presets = riviera.get_preset_stored(current_product_settings)

    for preset in range(1, len(presets)+1):
        rebooted_preset_location = riviera.get_preset_content(rebooted_product_settings, preset)['actions'][0]['payload']['contentItem']['location']
        current_preset_location = riviera.get_preset_content(current_product_settings, preset)['actions'][0]['payload']['contentItem']['location']
        assert rebooted_preset_location == current_preset_location, \
            'Preset {} found to have location {}, should have location {}'\
                .format(preset, rebooted_preset_location, current_preset_location)


@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'frontDoor_without_user', 'tap_wifi')
def test_overwrite_existing_presets(eddie_with_user, riviera_no_presets, frontDoor_without_user, tap_wifi):
    """
    This test will ensure that device store (overwrite) new presets if
    there are existing presets stored already.

    :param riviera: A Riviera Device fixture
    :param tap: A PExpect client to the Riviera device
    :return: None
    """

    # First we will save all presets with AMAZON music
    riviera, device_ip = eddie_with_user
    LOGGER.debug("Setting up a Music Player at {}".format(device_ip))
    rmp = music_player_factory(device_ip, MusicService.AMAZON, frontDoor_without_user, log_level=10)
    station_response = rmp.play_next_station()
    response_location = station_response['body']['container']['contentItem']['location']

    # Press all presets sequentially
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, STORAGE_DURATION)

    # We want to ensure the Key Press was received and stored in local file store.
    time.sleep(DELAY_AFTER_PRESET_SET)

    current_product_settings = riviera.get_product_settings()
    presets = riviera.get_preset_stored(current_product_settings)
    assert len(presets) == 6, \
        'Incorrect number of presets in {}. Anticipated 6, found {}.'.format(current_product_settings, len(presets))

    # Randomly ensure that all stored presets are storing the same Now Playing information
    for preset in sample(presets, len(presets)):
        preset_location = riviera.get_preset_content(current_product_settings, int(preset))['actions'][0]['payload']['contentItem']['location']
        assert preset_location == response_location, \
            'Preset {} found to have location {}, should have location {}'\
                .format(preset, preset_location, response_location)

    # Finally stop the playback
    rmp.stop_playback()

    # Now we will try to save presets with SPOTIFY music to validate overwrite logic
    LOGGER.debug("Setting up a Music Player at {}".format(device_ip))
    rmp = music_player_factory(device_ip, MusicService.SPOTIFY, frontDoor_without_user, log_level=10)
    station_response = rmp.play_next_station()
    response_location = station_response['body']['container']['contentItem']['location']

    # Press all presets sequentially
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, STORAGE_DURATION)

    # We want to ensure the Key Press was received and stored in local file store.
    time.sleep(DELAY_AFTER_PRESET_SET)

    current_product_settings = riviera.get_product_settings()
    presets = riviera.get_preset_stored(current_product_settings)
    assert len(presets) == 6, \
        'Incorrect number of presets in {}. Anticipated 6, found {}.'.format(current_product_settings, len(presets))

    # Randomly ensure that all stored presets are storing the same Now Playing information
    for preset in sample(presets, len(presets)):
        preset_location = riviera.get_preset_content(current_product_settings, int(preset))['actions'][0]['payload']['contentItem']['location']
        assert preset_location == response_location, \
            'Preset {} found to have location {}, should have location {}'\
                .format(preset, preset_location, response_location)

    # Finally stop the playback
    rmp.stop_playback()
