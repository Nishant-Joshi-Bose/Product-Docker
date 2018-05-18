# test_lightBar.py
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
import time

import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

LOGGER = get_logger(__name__)


@pytest.mark.usefixtures('frontdoor_wlan')
def test_get_active_animation(frontdoor_wlan):
    """
    Asserts that we receive active Lightbar states and transitions through the
    Front Door connection.

    :param frontdoor_wlan: Fixture connection to the Front Door
    :return: None
    """
    lightbar_state = frontdoor_wlan.getActiveAnimation()
    LOGGER.debug("LightBar state: %s", lightbar_state)

    transitions = lightbar_state["properties"]["supportedTransitions"]
    assert len(transitions) > 1, \
        "We do not have multiple Lightbar animation transitions in {}. {} found.".format(transitions, len(transitions))

    supported_animations = lightbar_state["properties"]["supportedValues"]
    assert len(supported_animations) > 1, "Not able to find multiple animations in {}".format(supported_animations)


@pytest.mark.usefixtures('frontdoor_wlan')
def test_play_animation_in_loop(frontdoor_wlan):
    """
    Queues all animations and verifies queue through Front Door connection.

    :param frontdoor_wlan: Fixture connection to the Front Door
    :return: None
    """
    lightbar_state = frontdoor_wlan.getActiveAnimation()
    LOGGER.debug("LightBar state: %s", lightbar_state)
    animations = lightbar_state["properties"]["supportedValues"]
    # Cycle through Supported Lightbar Animations
    for animation in animations:
        time.sleep(3)
        LOGGER.debug("Current Animation: %s", animation)

        # create request to play animation in loop
        data = {"nextValue":
                    {"value": animation,
                     "transition": "SMOOTH",
                     "repeat": True}}
        lightbar_state = frontdoor_wlan.playLightBarAnimation(json.dumps(data))
        LOGGER.debug("LightBar State: %s", lightbar_state)
        next_animation_response = lightbar_state.get("nextValue")
        if next_animation_response:
            next_animation = next_animation_response.get("value")
            assert next_animation == animation, \
                "Our next running animation ({}) was not stored in {}".format(animation, lightbar_state)

    # Finally stop any running animations
    frontdoor_wlan.stopActiveAnimation(json.dumps(data))


@pytest.mark.usefixtures('frontdoor_wlan')
def test_stop_active_animation(frontdoor_wlan):
    """
    Attempts to stop a playing Animation after picking one from a random
    supported animation.

    :param frontdoor_wlan: Fixture connection to the Front Door
    :return: None
    """
    # Adding a slight wait here because this test will fail
    # when run directly after test_playAnimationInLoop
    time.sleep(3)
    lightbar_state = frontdoor_wlan.getActiveAnimation()
    supported_animations = lightbar_state["properties"]["supportedValues"]

    total_animations = len(supported_animations)
    selected_animation = random.randint(0, total_animations - 1)

    animation_to_play = lightbar_state["properties"]["supportedValues"][selected_animation]
    LOGGER.debug("Play animation - %s", animation_to_play)

    data = {"nextValue":
                {"value": animation_to_play,
                 "transition": "SMOOTH",
                 "repeat": True}}
    lightbar_state = frontdoor_wlan.playLightBarAnimation(json.dumps(data))

    time.sleep(2)

    data = {"nextValue":
                {"value": animation_to_play,
                 "transition": "SMOOTH",
                 "repeat": True}}
    lightbar_state = frontdoor_wlan.stopActiveAnimation(json.dumps(data))

    # Ensure we have the correct LightBar state
    if 'currentValue' in lightbar_state:
        if "value" in lightbar_state["currentValue"]:
            current_key = lightbar_state["currentValue"]["value"]
            assert current_key == animation_to_play, \
                "Current Key ({}) does not match requested animation ({}).".format(current_key, animation_to_play)
        else:
            assert False, "'value' not found in Current Value ({}).".format(lightbar_state['currentValue'])
    else:
        assert False, "Key [{}] not found in ({}).".format('currentValue', lightbar_state)

    if "error" in lightbar_state:
        if "description" in lightbar_state["error"]:
            error = lightbar_state["error"]["description"]
            assert error == "", "Error Message found: {}".format(error)


@pytest.mark.usefixtures('frontdoor_wlan')
def test_invalid_animation_value(frontdoor_wlan):
    """
    Test to ensure that an improper animation does not play.

    :param frontdoor_wlan: Fixture connection to the Front Door
    :return: None
    """
    value = "SOME_ANIMATION"
    data = {"nextValue":
                {"value": value,
                 "transition": "SMOOTH",
                 "repeat": True}}

    data = json.dumps(data)
    lightbar_state = frontdoor_wlan.playLightBarAnimation(data)

    error_anticipation = "animation not supported"
    error = lightbar_state["error"]["description"]
    assert error == error_anticipation, "Anticipated: '{}'; Received: {}".format(error_anticipation, error)
