# test_audio_bass.py
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
PyTest based validation tests for the "/audio/bass" API.
"""

import os
import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import device_utils, adb_utils

import eddie_helper

LOGGER = get_logger(os.path.basename(__file__))


@pytest.mark.usefixtures("request", "frontdoor_wlan", "device_id")
@pytest.mark.parametrize("device_state", ["ON", "OFF"])
def test_audio_bass_state(request, device_state, frontdoor_wlan, device_id):
    """
    Test for verifying get and put and notify request of audio bass api for
    device states.

    :param request: A request for a fixture from a test or fixture function
    :param device_state: state of device(ON, OFF)
    :param frontdoor_wlan: Fixture to get frontdoor instance
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing get audio bass information while device in %s state", device_state)

    # Change the device state
    device_utils.change_current_device_state(device_state, device_id)

    # Check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Verify get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Capture data for put api verification
    initial_audio_data = response["body"]

    # Verify put /audio/bass api and notification response
    eddie_helper.verify_audio_put_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API,
                                      initial_audio_data)

    # At last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor_wlan,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("request", "frontdoor_wlan", "device_id")
@pytest.mark.parametrize("device_source", ["AUX", "BLUETOOTH"])
def test_audio_bass_source(request, frontdoor_wlan, device_source, device_id):
    """
    Test for verifying get and put and notify request of audio bass api for
    aux and bluetooth source.

    :param request: A request for a fixture from a test or fixture function
    :param frontdoor_wlan: Instance of frontdoor queue API
    :param device_source: source of device(AUX, BLUETOOTH)
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing get audio bass data while device in %s source", device_source)

    # Change the device state
    adb_utils.adb_telnet_tap(device_id=device_id)
    device_utils.set_device_source(device_source, device_id)

    # Check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Verify get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Capture data for put request verification
    initial_audio_data = response["body"]

    # Verify put /audio/bass api and notification response
    eddie_helper.verify_audio_put_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API,
                                      initial_audio_data)

    # At last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor_wlan,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("device_playing_from_amazon")
def test_audio_bass_playing_amazon(device_playing_from_amazon):
    """
    Test for verifying get and put  and notify request of audio bass api when
    music is playing.

    :param device_playing_from_amazon: Fixture to play music on device
    :return: None
    """
    LOGGER.info("Testing get audio bass data while music is playing")

    # Get frontdoor object
    frontdoor_wlan, _, _, _ = device_playing_from_amazon

    # Check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Verify get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Capture data for put request verification
    initial_audio_data = response["body"]

    # Verify put /audio/bass api and notification response
    eddie_helper.verify_audio_put_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API,
                                      initial_audio_data)

    # At last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor_wlan,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("request", "frontdoor_wlan", "device_id")
@pytest.mark.parametrize("device_state", ["ON", "OFF"])
def test_audio_bass_invalid_value(request, device_state, frontdoor_wlan, device_id):
    """
    Test for verifying error values of upper and lower boundary of audio bass api
    for device states

    :param request: A request for a fixture from a test or fixture function
    :param device_state: state of device(ON, OFF)
    :param frontdoor_wlan: Fixture to get frontdoor object
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing put audio bass information for invalid values when device is in %s state",
                device_state)

    # Change the device state
    device_utils.change_current_device_state(device_state, device_id)

    # Check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Execute get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Capture data for put request of invalid values after max and min values
    initial_audio_data = response["body"]

    # Verify put api for values after lower and upper boundary
    eddie_helper.verify_audio_invalid_args(frontdoor_wlan, initial_audio_data,
                                           eddie_helper.AUDIO_BASS_API)

    # At last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor_wlan,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("request", "frontdoor_wlan", "device_id")
@pytest.mark.parametrize("device_state", ["ON", "OFF"])
def test_audio_bass_keyerror(request, device_state, frontdoor_wlan, device_id):
    """
    Test for verifying keyerror of audio bass api for device states

    :param request: A request for a fixture from a test or fixture function
    :param device_state: state of device(ON, OFF)
    :param frontdoor_wlan: Fixture to get frontdoor instance
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing put audio bass information for invalid keys when device is in %s state",
                device_state)

    # Change the device state
    device_utils.change_current_device_state(device_state, device_id)

    # Check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)
    LOGGER.info("Testing put audio bass information for invalid key")

    # Execute get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor_wlan, eddie_helper.AUDIO_BASS_API)

    # Capture response to set the values back
    initial_audio_data = response["body"]

    # Validate error message by sending invalid key.
    LOGGER.info("Testing of audio bass api with invalid key")
    data_invalid_key = dict()
    data_invalid_key["invalid_key"] = "100"

    # Verify put /audio/bass api for invalid key
    eddie_helper.verify_audio_keyerror(frontdoor_wlan, data_invalid_key, eddie_helper.AUDIO_BASS_API)

    # Validate error message by sending extra key and value.
    LOGGER.info("Testing of audio bass api with extra key and value")
    data_invalid = dict()
    data_invalid["value"] = 10
    data_invalid["persistence"] = "Invalid"
    data_invalid["extra_key"] = "Invalid"

    # Verify put /audio/bass api for extra key
    eddie_helper.verify_audio_keyerror(frontdoor_wlan, data_invalid, eddie_helper.AUDIO_BASS_API)

    # At last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor_wlan,
                                          eddie_helper.AUDIO_BASS_API)
