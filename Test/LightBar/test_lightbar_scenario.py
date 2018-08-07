# LightBar/test_lightbar_scenario.py
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
Automated Tests for LightBar Animations for Eddie
"""
import json
import os
import random
import time
import ConfigParser
from time import sleep

import pytest

import lightbar_helper

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils

LOGGER = get_logger(__file__)


def pytest_generate_tests(metafunc):
    """
    This fixture could be used to dynamically parametrise the test with data.
    :param metafunc: Object that help to inspect a test function
    :return: None
    """
    if str(metafunc.function.__name__) == 'test_play_valid_animation':
        device_id = metafunc.config.getoption("--device-id")
        environment = metafunc.config.getoption("--environment")

        network_base = NetworkBase(None, device_id)

        interface = metafunc.config.getoption("--network-iface")
        device_ip_address = network_base.check_inf_presence(interface, timeout=10)

        if not device_ip_address:
            riviera_utils = RivieraUtils('ADB', device=device_id, logger=LOGGER)

            # Read Wifi Configurations
            wifi_config = ConfigParser.SafeConfigParser()
            parent_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
            wifi_ini_file = '{}/Configs/conf_wifiProfiles.ini'.format(parent_path)
            wifi_config.read(wifi_ini_file)

            router = metafunc.config.getoption("--router")
            riviera_utils.add_wifi_profile(wifi_config.get(router, 'ssid'),
                                           wifi_config.get(router, 'password'),
                                           wifi_config.get(router, 'security'),
                                           device=device_id)

            device_ip_address = network_base.check_inf_presence(interface)

            if not device_ip_address:
                pytest.fail("Not able to gather IP Address on {}".format(device_id))

        animation_values = lightbar_helper.get_lightbar_animations(device_id, device_ip_address, environment)
        LOGGER.debug("Lightbar response is %s", animation_values)
        metafunc.parametrize("animation_value", animation_values)


@pytest.mark.usefixtures('frontdoor_wlan', 'animation_list', 'serial_handler')
def test_current_active_animation(frontdoor_wlan, animation_list, serial_handler):
    """
    Verify the active animation pattern on lightbar.

    Test Steps:
    1 Play some animation pattern on device
    2 Verify the current animation value from getActiveAnimation() API

    :param frontdoor_wlan : fixture returning frontDoorQueue instance
    :param animation_list : fixture returning supported animation values
    :param serial_handler : fixture to start and stop serial logs
    """
    expected_animation = random.choice(animation_list)
    LOGGER.debug("Expected animation value is %s", expected_animation)

    # play the animation value
    animation_data = json.dumps({"nextValue": {"value": expected_animation,
                                               "transition": "SMOOTH", "repeat": "true"}})

    lightbar_play_response = frontdoor_wlan.playLightBarAnimation(animation_data)
    LOGGER.debug("lightbar play response is %s", lightbar_play_response)

    # get animation value from LPM serial logs
    current_animation = lightbar_helper.get_animation_from_lpm(serial_handler, expected_animation)

    assert current_animation == expected_animation, \
        "Expected Animation ({}) not equal to API Animation ({}).".format(expected_animation, current_animation)

    # verify animation value from Lightbar API Notifications
    lb_notification_value = lightbar_helper.animation_in_notification(frontdoor_wlan, expected_animation)
    assert lb_notification_value == expected_animation, \
        "Expected Animation ({}) not equal to API Animation ({}).".format(expected_animation, lb_notification_value)


@pytest.mark.usefixtures('frontdoor_wlan', 'animation_list', 'serial_handler')
def test_stop_active_animation(frontdoor_wlan, animation_list, serial_handler):
    """
    Verify whether animation pattern on lightbar is stopped or not
    using 'frontDoor.stopActiveAnimation' api

    Test Steps:
    1 Play some animation pattern over device
    2 Stop the animation from stopActiveAnimation()
    3 Verify the current animation value from getActiveAnimation()

    :param frontdoor_wlan: fixture returning frontDoorQueue instance
    :param animation_list: fixture returning supported animation values
    :param serial_handler: fixture to start and stop serial logs
    """
    expected_animation = random.choice(animation_list)
    LOGGER.debug("Expected animation value: %s", expected_animation)

    # play the animation value
    animation_data = json.dumps({"nextValue": {"value": expected_animation,
                                               "transition": "SMOOTH", "repeat": "true"}})
    lightbar_play_response = frontdoor_wlan.playLightBarAnimation(animation_data)
    LOGGER.debug("lightbar play response: %s", lightbar_play_response)

    # get animation value from LPM serial logs
    current_animation = lightbar_helper.get_animation_from_lpm(serial_handler, expected_animation)

    # stop the active animation
    lightbar_stop_response = frontdoor_wlan.stopActiveAnimation(animation_data)
    LOGGER.debug("lightbar stop response: %s", lightbar_stop_response)

    # get the animation details after stopping the current animation
    for _ in range(lightbar_helper.PLAY_STOP_ANIMATION_TIMEOUT):
        lightbar_response = frontdoor_wlan.getActiveAnimation()
        current_animation = lightbar_response.get("currentValue")
        if current_animation is None:
            break
        sleep(1)

    assert not current_animation, "Animation ({}) should be stopped".format(current_animation)


@pytest.mark.usefixtures("frontdoor_wlan", "animation_list", "serial_handler")
@pytest.mark.parametrize("transition_value", ["SMOOTH", "IMMEDIATE"])
def test_play_valid_animation(frontdoor_wlan, animation_value, transition_value, serial_handler):
    """
    Verify supported animation pattern on devices and lightbar status

    Test Steps:
    1 Play animation pattern over device
    2 Verify the current animation value from getActiveAnimation()
        with the animation value

    :param animation_value: parameterize animation_value of supported animations
    :param frontdoor_wlan: fixture returning frontDoor instance
    :param transition_value: parameterize transition_value
    :param serial_handler: fixture to start and stop serial logs
    """

    LOGGER.debug("Playing animation value %s and transition %s", animation_value, transition_value)
    animation_data = json.dumps({"nextValue": {"value": animation_value,
                                               "transition": transition_value,
                                               "repeat": "true"}})

    # play the animation on device
    lightbar_play_response = frontdoor_wlan.playLightBarAnimation(animation_data)
    LOGGER.debug("lightbar play response is %s", lightbar_play_response)

    # verify the animation value from LPM serial logs
    serial_last_animation = lightbar_helper.get_animation_from_lpm(serial_handler, animation_value)

    assert serial_last_animation == animation_value, \
        "Expected animation value '{0}'; Current animation value '{1}'".format(animation_value, serial_last_animation)

    # verify animation value from API Notifications
    lb_notification_value = lightbar_helper.animation_in_notification(frontdoor_wlan, animation_value)
    assert lb_notification_value == animation_value, \
        "Expected Animation: {}; API Notification Animation: {}".format(animation_value, lb_notification_value)

    frontdoor_wlan.stopActiveAnimation(animation_data)


@pytest.mark.usefixtures('frontdoor_wlan')
@pytest.mark.parametrize("animation_data", lightbar_helper.negative_animation_data())
def test_play_invalid_animation(animation_data, frontdoor_wlan):
    """
    Verify invalid animation pattern on devices and lightbar status

    Test Steps:
    1 Play invalid animation pattern over device
    2 Verify the current animation value from getActiveAnimation() with the animation value

    :param frontdoor_wlan: fixture returning frontDoor instance
    :param animation_data: parameterize negative animation data
    """
    transition = animation_data["transition"]
    animation_value = animation_data["value"]
    LOGGER.debug("Playing invalid animation value %s and \
                 transition %s", animation_value, transition)
    animation_data = json.dumps({"nextValue": {"value": animation_value,
                                               "transition": transition, "repeat": "true"}})

    # play the animation with invalid value on device
    lightbar_play_response = frontdoor_wlan.playLightBarAnimation(animation_data)
    if "body" in lightbar_play_response:
        lightbar_play_response = lightbar_play_response["body"]
    LOGGER.debug("lightbar play response is %s", lightbar_play_response)

    for _ in range(lightbar_helper.PLAY_STOP_ANIMATION_TIMEOUT):
        current_animation_status = lightbar_play_response["error"].get("message")
        LOGGER.debug("Play response in invalid animations value is %s", current_animation_status)
        if current_animation_status:
            break

    assert current_animation_status, \
        "Invalid Animation scenario played on device as {}".format(animation_data)
