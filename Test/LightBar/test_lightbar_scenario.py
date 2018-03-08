# test_lightbar_scenario.py
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
import random
import re
from time import sleep
import pytest
from configure import CONF
from common_exception import IPAddressError
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.SupportUtils.FileUtils.CsvFileSupport import read_rows_as_dictionary_list_from_csv_file
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI

LOGGER = get_logger(__file__)
INVALID_ANIMATION = "animation not supported"
ANIMATION_TIMEOUT = 10


def negative_animation_data():
    """
    This function will return the list of dictionary of data
    from the csv file.
    :return lightbar_data : List of negative animation data
    """
    LOGGER.info("Get the negative lightbar data")
    lightbar_data = read_rows_as_dictionary_list_from_csv_file(CONF['animation_filename'])[1]
    return lightbar_data


def pytest_generate_tests(metafunc):
    """
    This fixture could be used to dynamically parametrise the test with data.
    :param metafunc: Object that help to inspect a test function
    :return: None
    """
    if str(metafunc.function.__name__) == "test_play_valid_animation":

        if metafunc.config.getoption("--target").lower() == 'device':
            network_base = NetworkBase(None)
            iface = metafunc.config.getoption("--network-iface")
            device_ip = network_base.check_inf_presence(iface)
            if not device_ip:
                IPAddressError("No valid device IP")
            frontdoor = FrontDoorAPI(device_ip)
            lightbar_response = frontdoor.getActiveAnimation()
            LOGGER.debug("Lightbar response is %s", lightbar_response)
            animation_values = lightbar_response["properties"]["supportedValues"]
            metafunc.parametrize("animation_value", animation_values)


@pytest.mark.usefixtures("save_speaker_log")
class TestLightBarScenario(object):
    """
    Class that handles Lightbar Animation patterns
    """
    @pytest.mark.usefixtures('frontDoor', 'animation_list')
    def test_current_active_animation(self, frontDoor, animation_list):
        """
        Verify the active animation pattern on lightbar.

        Test Steps:
        1 Play some animation pattern over device
        2 Verify the current animation value from getActiveAnimation()

        :param frontDoor : fixture returning frontDoor instance
        :param animation_list : fixture returning supported animation values
        """
        expected_animation = random.choice(animation_list)
        LOGGER.debug("Expected animation value is %s", expected_animation)

        # play the animation value
        animation_data = json.dumps({"nextValue": {"value": expected_animation,
                                                   "transition": "SMOOTH", "repeat": "true"}})
        lightbar_play_response = frontDoor.playLightBarAnimation(animation_data)
        LOGGER.debug("lightbar play response is %s", lightbar_play_response)
        sleep(ANIMATION_TIMEOUT)

        # get the animation details
        lightbar_response = frontDoor.getActiveAnimation()
        LOGGER.debug("lightbar get response is %s", lightbar_response)
        current_animation = lightbar_response["currentValue"]["value"]
        assert current_animation == expected_animation, "Expected animation value as %s"\
            " mismatch with current animation value as %s" % (expected_animation, current_animation)

    @pytest.mark.usefixtures('frontDoor', 'animation_list')
    def test_stop_active_animation(self, frontDoor, animation_list):
        """
        Verify whether animation pattern on lightbar is stopped or not
        using 'frontDoor.stopActiveAnimation' api

        Test Steps:
        1 Play some animation pattern over device
        2 Stop the animation from stopActiveAnimation()
        3 Verify the current animation value from getActiveAnimation()

        :param frontDoor : fixture returning frontDoor instance
        :param animation_list : fixture returning supported animation values
        """
        expected_animation = random.choice(animation_list)
        LOGGER.debug("Expected animation value is %s", expected_animation)

        # play the animation value
        animation_data = json.dumps({"nextValue": {"value": expected_animation,
                                                   "transition": "SMOOTH", "repeat": "true"}})
        lightbar_play_response = frontDoor.playLightBarAnimation(animation_data)
        LOGGER.debug("lightbar play response is %s", lightbar_play_response)
        sleep(ANIMATION_TIMEOUT)
        # stop the active animation
        lightbar_stop_response = frontDoor.stopActiveAnimation(animation_data)
        LOGGER.debug("lightbar stop response is %s", lightbar_stop_response)

        # wait till the animation name delete from getActiveAnimation()
        sleep(ANIMATION_TIMEOUT)
        # get the animation details after stop the current animation
        lightbar_response = frontDoor.getActiveAnimation()
        current_animation = lightbar_response["currentValue"].get("value")
        assert not current_animation, "Expected animation value as %s"\
                                      " should be stopped" % (expected_animation)

    @pytest.mark.usefixtures('frontDoor', 'animation_list')
    @pytest.mark.parametrize("transition_value", ["SMOOTH", "IMMEDIATE"])
    def test_play_valid_animation(self, animation_value, frontDoor,
                                  transition_value, serial_handler):
        """
        Verify supported animation pattern on devices and lightbar status

        Test Steps:
        1 Play animation pattern over device
        2 Verify the current animation value from getActiveAnimation() with the animation value

        :param frontDoor : fixture returning frontDoor instance
        :param serial_handler: fixture returning serial com port instance
        :param animation_value: parameterize animation_value of supported animations
        :param transition_value: parameterize transition_value
        """

        LOGGER.debug("Playing animation value %s and transition %s",
                     (animation_value, transition_value))
        animation_data = json.dumps({"nextValue": {"value": animation_value,
                                                   "transition": transition_value,
                                                   "repeat": "true"}})

        # play the animation on device
        lightbar_play_response = frontDoor.playLightBarAnimation(animation_data)
        LOGGER.debug("lightbar play response is %s", lightbar_play_response)
        sleep(ANIMATION_TIMEOUT)

        # get the animation details after playing the current animation
        lightbar_response = frontDoor.getActiveAnimation()
        current_animation = lightbar_response["currentValue"].get("value")
        current_transition = lightbar_response["currentValue"].get("transition")
        assert current_animation == animation_value, "Expected animation value as '{0}' "\
            "mismatch with current animation value as '{1}'".format(animation_value,
                                                                    current_animation)
        assert current_transition == transition_value, "Expected transition value as '{0}' "\
            " mismatch with current transition value as '{1}'".format(transition_value,
                                                                      current_transition)

        # verify the animation value from LPM serial logs
        LOGGER.debug("Verifying from LPM serial logs")
        serial_outputs = serial_handler.execute_command('lb history')
        LOGGER.debug("Lightbar logs from LPM are: %s", serial_outputs)
        serial_animations = [serial_output for serial_output in serial_outputs
                             if 'Animation Name' in serial_output]
        serial_last_animation = re.search('Animation Name: (.+?), S',
                                          serial_animations[-1]).group(1)
        assert serial_last_animation == animation_value, "Expected animation value as '{0}' "\
            "mismatch with current animation value as '{1}'".format(animation_value,
                                                                    serial_last_animation)

    @pytest.mark.usefixtures('frontDoor')
    @pytest.mark.parametrize("animation_data", negative_animation_data())
    def test_play_invalid_animation(self, animation_data, frontDoor):
        """
        Verify invalid animation pattern on devices and lightbar status

        Test Steps:
        1 Play invalid animation pattern over device
        2 Verify the current animation value from getActiveAnimation() with the animation value

        :param frontDoor : fixture returning frontDoor instance
        :param animation_data : parameterize negative animation data
        """
        transition = animation_data["transition"]
        animation_value = animation_data["value"]
        LOGGER.debug("Playing invalid animation value %s and transition %s",
                     animation_value, transition)
        animation_data = json.dumps({"nextValue": {"value": animation_value,
                                                   "transition": transition, "repeat": "true"}})

        sleep(ANIMATION_TIMEOUT)
        # play the animation with invalid value on device
        lightbar_play_response = frontDoor.playLightBarAnimation(animation_data)
        LOGGER.debug("lightbar play response is %s", lightbar_play_response)

        current_animation_status = lightbar_play_response["error"].get("description")
        assert current_animation_status == INVALID_ANIMATION, "Invalid Animation "\
            "scenario played on device as %s" % animation_data
