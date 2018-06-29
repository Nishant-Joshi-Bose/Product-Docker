# test_preset_playback.py
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
PyTest based tests for interacting with the Eddie Preset storage playback.
"""
import time
from logging import WARNING
import pexpect
import pytest
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from pytest_testrail.plugin import pytestrail
from musicplayer import music_player_factory
from services import MusicService
from conftest import STORAGE_DURATION, PLAYBACK_DURATION, DELAY_AFTER_PRESET_SET

LOGGER = get_logger(__file__, level=WARNING)

@pytestrail.case('C657651')
@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'frontDoor_without_user', 'tap_wifi')
def test_single_stored_preset_playback(eddie_with_user, riviera_no_presets, frontDoor_without_user, tap_wifi):
    """
    This test will ensure that we can store a single music stream from
        a single provider on all presets then playback each of the
        presets without issue.

    :param riviera: A Riviera Device fixture
    :param device_ip: The IP Address of the Device Under Test
    :param tap: A PExpect Telnet connection to the Device
    :return: None
    """
    riviera, device_ip = eddie_with_user

    rmp = music_player_factory(device_ip, MusicService.RANDOM, frontDoor_without_user)
    station_response = rmp.play_next_station()
    response_location = station_response['body']['container']['contentItem']['location']
    LOGGER.debug("Running against this playback information: %s", response_location)

    # Press all presets sequentially
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, STORAGE_DURATION)

    time.sleep(DELAY_AFTER_PRESET_SET)

    current_product_settings = riviera.get_product_settings()
    presets = riviera.get_preset_stored(current_product_settings)
    assert len(presets) == 6, \
        "Incorrect number of presets in {}. Anticipated 6, found {}.".format(current_product_settings, len(presets))

    # Stop Music playback
    stop_playback_response = rmp.stop_playback()
    LOGGER.debug("Initial Stop Playback Response: %s", stop_playback_response)

    # Start playing presets
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, PLAYBACK_DURATION)
        time.sleep(10)

        current_now_playing = rmp.front_door.getNowPlaying()
        current_now_playing_location = current_now_playing['body']['container']['contentItem']['location']

        assert current_now_playing_location == response_location, \
            "We were not able to confirm that now playing ({}) is what is stored correctly on the preset ({})."\
                .format(current_now_playing_location, response_location)

        stop_playback_response = rmp.front_door.stopPlaybackRequest()
        LOGGER.debug("Stop Playback Response for Preset %s: %s", preset_number, stop_playback_response)


@pytestrail.case('C657652')
@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'frontDoor_without_user', 'tap_wifi')
def test_single_provider_multiple_stations_playback(eddie_with_user, riviera_no_presets, frontDoor_without_user, tap_wifi):
    """
    This test will ensure that we can playback multiple station streams
        from a single provider from preset.

    :param riviera: A Riviera Device fixture
    :param device_ip: The IP Address of the Device Under Test
    :param tap: A PExpect Telnet connection to the Device
    :return: None
    """

    riviera, device_ip = eddie_with_user

    playback_responses = {}
    music_player = music_player_factory(device_ip, MusicService.TUNEIN, frontDoor_without_user)

    # Press all presets sequentially
    for preset_number in range(1, 7, 1):
        # Start the first playback
        playback_responses[preset_number] = music_player.play_next_station()
        LOGGER.debug("Preset #%s received the playback response: %s", preset_number, playback_responses[preset_number])

        # Ensure that playback is going.
        time.sleep(10)

        # Store the playback
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, STORAGE_DURATION)

        # We want to ensure the Key Press was received and stored in local file store.
        time.sleep(DELAY_AFTER_PRESET_SET)

        stop_playback_response = music_player.stop_playback()
        LOGGER.debug("Preset #%s stop playback response: %s", preset_number, stop_playback_response)

    # Check to ensure that we have stored the correct number of presets
    post_product_settings = riviera.get_product_settings()
    assert len(riviera.get_preset_stored(post_product_settings)) == 6, \
        "We did not store all 6 presets in Product Settings: {}".format(post_product_settings)

    # Assert that what we stored matches what we told the system to play
    for preset_playback_response_number in playback_responses:
        preset_stored_content = riviera.get_preset_content(post_product_settings, preset_playback_response_number)
        preset_stored_content_location = preset_stored_content['actions'][0]['payload']['contentItem']['location']

        playback_content = playback_responses[preset_playback_response_number]
        playback_content_location = playback_content['body']['container']['contentItem']['location']

        assert playback_content_location == preset_stored_content_location, \
            "Preset #{} content ({}) did not match what my response was ({})".format(preset_playback_response_number,
                                                                                     preset_stored_content,
                                                                                     playback_content)

    # Check playback
    for preset_number in range(1, 7, 1):

        # Start Playback on the system
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, PLAYBACK_DURATION)

        #Ensure that playback is going.
        time.sleep(10)

        now_playing = music_player.front_door.getNowPlaying()
        now_playing_location = now_playing['body']['container']['contentItem']['location']

        # Check against Stored Preset Value
        stored_content = riviera.get_preset_content(post_product_settings, preset_number)
        stored_content_location = stored_content['actions'][0]['payload']['contentItem']['location']

        assert now_playing_location == stored_content_location, \
            "Now playing ({}) did not match Stored Content ({}) for Preset #{}."\
                .format(now_playing, stored_content, preset_number)

        # Check against Original Playback Value
        original_content = playback_responses[preset_number]
        original_content_location = original_content['body']['container']['contentItem']['location']

        assert now_playing_location == original_content_location, \
            "Now playing ({}) did not match Original Content Request ({}) for Preset #{}." \
                .format(now_playing, original_content, preset_number)

    music_player.stop_playback()


@pytestrail.case('C657654')
@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'passport_user_details', 'request', 'device_id')
def test_play_preset_after_power_cycle(eddie_with_user, riviera_no_presets, passport_user_details, request, device_id):
    """
    This test will ensure that we can store a single music stream from
        a single provider on all presets then playback each of the
        presets without issue after a system reboot.

    :param riviera: A Riviera Device fixture
    :param tap: A PExpect Telnet connection to the Device
    :param device_ip: The IP Address of the Device Under Test
    :return: None
    """
    riviera, device_ip = eddie_with_user

    #Need to create front_door and telnet object locally due to system reboot
    front_door = FrontDoorAPI(device_ip, email=passport_user_details["email"], password=passport_user_details["password"])
    tap = pexpect.spawn('telnet {} {}'.format(device_ip, request.config.getoption('--telnet-port')))

    music_player = music_player_factory(device_ip, MusicService.RANDOM, front_door)
    music_player.play_next_station()
    # Acquire Now Playing information
    now_playing = music_player.front_door.getNowPlaying()
    now_playing_location = now_playing['body']['container']['contentItem']['location']
    LOGGER.debug("Running against this playback information: %s", now_playing_location)

    # Press all presets sequentially
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap, preset_key, STORAGE_DURATION)

    #We want to ensure the Key Press was received and stored in local file store.
    time.sleep(DELAY_AFTER_PRESET_SET)

    current_product_settings = riviera.get_product_settings()
    presets = riviera.get_preset_stored(current_product_settings)
    assert len(presets) == 6, \
        "Incorrect number of presets in {}. Anticipated 6, found {}.".format(current_product_settings, len(presets))

    # Stop Music playback
    stop_playback_response = music_player.front_door.stopPlaybackRequest()
    LOGGER.debug("Initial Stop Playback Response: %s", stop_playback_response)

    # Close FrontDoor and Telnet connection before reboot
    front_door.close()
    tap.close()

    # Reboot the device
    riviera.communication.rebootDevice_adb()
    network_base = NetworkBase(None, device=device_id, logger=LOGGER)
    interface = request.config.getoption("--network-iface")
    riviera_ip_address = network_base.check_inf_presence(interface)
    time.sleep(10)

    # Check to ensure network is up
    assert device_ip == riviera_ip_address, "Input IP Address ({}) does not match retrieved ({}) after reboot."\
        .format(device_ip, riviera_ip_address)

    # Need to open FrontDoor and Telnet connection again
    front_door = FrontDoorAPI(device_ip, email=passport_user_details["email"], password=passport_user_details["password"])
    tap = pexpect.spawn('telnet {} {}'.format(device_ip, request.config.getoption('--telnet-port')))

    # Duration to start a preset.
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap, preset_key, PLAYBACK_DURATION)
        time.sleep(10)

        current_now_playing = front_door.getNowPlaying()
        current_now_playing_location = current_now_playing['body']['container']['contentItem']['location']

        assert current_now_playing_location == now_playing_location, \
            "We were not able to confirm that now playing ({}) is what is stored correctly on the preset ({})."\
                .format(current_now_playing_location, now_playing_location)

        stop_playback_response = front_door.stopPlaybackRequest()
        LOGGER.debug("Stop Playback Response for Preset %s: %s", preset_number, stop_playback_response)


@pytestrail.case('C657655')
@pytest.mark.usefixtures('riviera_no_presets', 'frontDoor_without_user', 'tap_wifi')
def test_play_empty_preset_with_nothing_playing(riviera_no_presets, frontDoor_without_user, tap_wifi):
    """
    This test will assure that when you press an empty preset with nothing
    playing, no music will start playback.

    :param riviera_no_presets: A Riviera connection that does not
        contain presets
    :param ip_address_wlan: The IP Address of the Device Under Test
    :param tap_wifi: A PExpect Telnet connection to the Device
    :return: None
    """
    # Get current playing information
    original_now_playing = frontDoor_without_user.getNowPlaying()
    original_now_playing_container = original_now_playing['body']['container']

    # Duration to start a preset.
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, PLAYBACK_DURATION)
        # Time delay to ensure music has been started
        time.sleep(10)

        current_now_playing = frontDoor_without_user.getNowPlaying()
        current_now_playing_container =  current_now_playing['body']['container']

        assert original_now_playing_container == current_now_playing_container, \
            "We were not able to confirm that now playing ({}) is what is stored correctly on the preset ({})." \
                .format(current_now_playing_container, original_now_playing_container)

        stop_playback_response = frontDoor_without_user.stopPlaybackRequest()
        LOGGER.debug("Stop Playback Response for Preset %s: %s", preset_number, stop_playback_response)


@pytestrail.case('C657656')
@pytest.mark.usefixtures('eddie_with_user', 'riviera_no_presets', 'frontDoor_without_user', 'tap_wifi')
def test_play_empty_preset_with_music_playing(eddie_with_user, riviera_no_presets, frontDoor_without_user, tap_wifi):
    """
    This test will ensure that we do not play something random from the
    presets key press during normal playback when nothing is stored in the
    preset.

    :param riviera_no_presets_playing_music: A Riviera connection that does not
        contain presets and is currently playing music.
    :param tap_wifi: A PExpect Telnet connection to the Device
    :return: None
    """
    riviera, device_ip = eddie_with_user

    rmp = music_player_factory(device_ip, MusicService.RANDOM, frontDoor_without_user)
    station_response = rmp.play_next_station()
    original_now_playing_location = station_response['body']['container']['contentItem']['location']
    LOGGER.debug("Running against this playback information: %s", original_now_playing_location)

    # Duration to start a preset.
    for preset_number in range(1, 7, 1):
        preset_key = preset_number + 7
        keypress.press_key(tap_wifi, preset_key, PLAYBACK_DURATION)
        # Time delay to ensure music has been started
        time.sleep(10)

        current_now_playing = frontDoor_without_user.getNowPlaying()
        current_now_playing_location = current_now_playing['body']['container']['contentItem']['location']

        assert current_now_playing_location == original_now_playing_location, \
            "We were not able to confirm that now playing ({}) is what is stored correctly on the preset ({})." \
                .format(current_now_playing_location, original_now_playing_location)

    stop_playback_response = frontDoor_without_user.stopPlaybackRequest()
    LOGGER.debug("Stop Playback Response %s", stop_playback_response)
