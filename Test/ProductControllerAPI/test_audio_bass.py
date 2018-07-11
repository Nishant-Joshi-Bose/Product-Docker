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


@pytest.mark.usefixtures("request", "wifi_config", "ip_address_wlan", "device_id")
@pytest.mark.parametrize("device_state", ["OFF", "ON", "SETUP"])
def test_audio_bass_state(request, device_state, wifi_config, device_id):
    """
    Test for verifying get and put and notify request of audio bass api for
    device states.

    :param request: A request for a fixture from a test or fixture function
    :param device_state: state of device(ON, OFF, SETUP)
    :param wifi_config: config parser instance of wifi profiles
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing get audio bass information while device in %s state", device_state)

    # change the device state
    device_utils.change_current_device_state(device_state, device_id)

    frontdoor = eddie_helper.get_frontdoor_instance(request, wifi_config)

    # check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor, eddie_helper.AUDIO_BASS_API)

    # verify get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor, eddie_helper.AUDIO_BASS_API)

    # capture data for put api verification
    initial_audio_data = response["body"]

    # verify put /audio/bass api and notification response
    eddie_helper.verify_audio_put_api(frontdoor, eddie_helper.AUDIO_BASS_API,
                                      initial_audio_data)

    # at last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("request", "front_door_queue", "device_id")
@pytest.mark.parametrize("device_source", ["AUX", "BLUETOOTH"])
def test_audio_bass_source(request, front_door_queue, device_source, device_id):
    """
    Test for verifying get and put and notify request of audio bass api for
    aux and bluetooth source.

    :param request: A request for a fixture from a test or fixture function
    :param front_door_queue: Instance of frontdoor queue API
    :param device_source: source of device(AUX, BLUETOOTH)
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing get audio bass data while device in %s source", device_source)

    # change the device state
    adb_utils.adb_telnet_tap(device_id=device_id)
    device_utils.set_device_source(device_source, device_id)

    # check if api exists
    eddie_helper.check_if_end_point_exists(front_door_queue, eddie_helper.AUDIO_BASS_API)

    # verify get /audio/bass api
    response = eddie_helper.verify_audio_get_api(front_door_queue, eddie_helper.AUDIO_BASS_API)

    # capture data for put request verification
    initial_audio_data = response["body"]

    # verify put /audio/bass api and notification response
    eddie_helper.verify_audio_put_api(front_door_queue, eddie_helper.AUDIO_BASS_API,
                                      initial_audio_data)

    # at last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, front_door_queue,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("front_door_queue", "device_playing_from_amazon")
def test_audio_bass_playing_amazon(front_door_queue):
    """
    Test for verifying get and put  and notify request of audio bass api when
    music is playing.

    :param front_door_queue: Instance of frontdoor queue API
    :return: None
    """
    LOGGER.info("Testing get audio bass data while music is playing")

    # check if api exists
    eddie_helper.check_if_end_point_exists(front_door_queue, eddie_helper.AUDIO_BASS_API)

    # verify get /audio/bass api
    response = eddie_helper.verify_audio_get_api(front_door_queue, eddie_helper.AUDIO_BASS_API)

    # capture data for put request verification
    initial_audio_data = response["body"]

    # verify put /audio/bass api and notification response
    eddie_helper.verify_audio_put_api(front_door_queue, eddie_helper.AUDIO_BASS_API,
                                      initial_audio_data)

    # at last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, front_door_queue,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("request", "wifi_config", "ip_address_wlan", "device_id")
@pytest.mark.parametrize("device_state", ["OFF", "ON", "SETUP"])
def test_audio_bass_invalid_value(request, device_state, wifi_config, device_id):
    """
    Test for verifying error values of upper and lower boundary of audio bass api
    for device states

    :param request: A request for a fixture from a test or fixture function
    :param device_state: state of device(ON, OFF, SETUP)
    :param wifi_config: config parser instance of wifi profiles
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing put audio bass information for invalid values when device is in %s state",
                device_state)

    # change the device state
    device_utils.change_current_device_state(device_state, device_id)

    frontdoor = eddie_helper.get_frontdoor_instance(request, wifi_config)

    # check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor, eddie_helper.AUDIO_BASS_API)

    # execute get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor, eddie_helper.AUDIO_BASS_API)

    # capture data for put request of invalid values after max and min values
    initial_audio_data = response["body"]

    # verify put api for values after lower and upper boundary
    eddie_helper.verify_audio_invalid_args(frontdoor, initial_audio_data,
                                           eddie_helper.AUDIO_BASS_API)

    # at last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor,
                                          eddie_helper.AUDIO_BASS_API)


@pytest.mark.usefixtures("request", "wifi_config", "ip_address_wlan", "device_id")
@pytest.mark.parametrize("device_state", ["OFF", "ON", "SETUP"])
def test_audio_bass_keyerror(request, device_state, wifi_config, device_id):
    """
    Test for verifying keyerror of audio bass api for device states

    :param request: A request for a fixture from a test or fixture function
    :param device_state: state of device(ON, OFF, SETUP)
    :param wifi_config: config parser instance of wifi profiles
    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Testing put audio bass information for invalid keys when device is in %s state",
                device_state)

    # change the device state
    device_utils.change_current_device_state(device_state, device_id)

    frontdoor = eddie_helper.get_frontdoor_instance(request, wifi_config)

    # check if api exists
    eddie_helper.check_if_end_point_exists(frontdoor, eddie_helper.AUDIO_BASS_API)
    LOGGER.info("Testing put audio bass information for invalid key")

    # execute get /audio/bass api
    response = eddie_helper.verify_audio_get_api(frontdoor, eddie_helper.AUDIO_BASS_API)

    # capture response to set the values back
    initial_audio_data = response["body"]

    # validate error message by sending invalid key.
    LOGGER.info("Testing of audio bass api with invalid key")
    data_invalid_key = dict()
    data_invalid_key["invalid_key"] = "100"

    # verify put /audio/bass api for invalid key
    eddie_helper.verify_audio_keyerror(frontdoor, data_invalid_key, eddie_helper.AUDIO_BASS_API)

    # validate error message by sending extra key and value.
    LOGGER.info("Testing of audio bass api with extra key and value")
    data_invalid = dict()
    data_invalid["value"] = 10
    data_invalid["persistence"] = "Invalid"
    data_invalid["extra_key"] = "Invalid"

    # verify put /audio/bass api for extra key
    eddie_helper.verify_audio_keyerror(frontdoor, data_invalid, eddie_helper.AUDIO_BASS_API)

    # at last save initially got values
    eddie_helper.set_initial_audio_values(initial_audio_data, frontdoor,
                                          eddie_helper.AUDIO_BASS_API)
