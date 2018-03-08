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

log = get_logger(__name__)


@pytest.mark.usefixtures('frontDoor')
def test_get_active_animation(frontDoor):
    """
    Asserts that we receive active Lightbar states and transitions through the
    Front Door connection.

    :param frontDoor: Fixture connection to the Front Door
    :return: None
    """
    lightbar_state = frontDoor.getActiveAnimation()
    log.debug("The state is %s", lightbar_state)

    transitions = lightbar_state["properties"]["supportedTransitions"]
    assert len(transitions) > 1, \
        "We do not have multiple Lightbar animation transitions in {}. {} found.".format(transitions, len(transitions))

    supported_animations = lightbar_state["properties"]["supportedValues"]
    assert len(supported_animations) > 1, "Not able to find multiple animations in {}".format(supported_animations)


@pytest.mark.usefixtures('frontDoor')
def test_play_animation_in_loop(frontDoor):
    """
    Queues all animations and verifies queue through Front Door connection.

    :param frontDoor: Fixture connection to the Front Door
    :return: None
    """
    lightbar_state = frontDoor.getActiveAnimation()
    log.debug("The state is %s", lightbar_state)
    animations = lightbar_state["properties"]["supportedValues"]

    # Cycle through Supported Lightbar Animations
    for animation in animations:
        time.sleep(3)
        log.debug("value = %s", animation)

        # create request to play animation in loop
        data = {"nextValue":
                    {"value": animation,
                     "transition": "SMOOTH",
                     "repeat": True}}
        lightbar_state = frontDoor.playLightBarAnimation(json.dumps(data))
        log.debug("lb_state = %s", lightbar_state)

        next_animation = lightbar_state["nextValue"]["value"]
        assert next_animation == animation, \
            "Our next running animation ({}) was not stored in {}".format(animation, lightbar_state)

    # Finally stop any running animations
    frontDoor.stopActiveAnimation(data)


@pytest.mark.usefixtures('frontDoor')
def test_stop_active_animation(frontDoor):
    """
    Attempts to stop a playing Animation after picking one from a random
    supported animation.

    :param frontDoor: Fixture connection to the Front Door
    :return: None
    """
    # Adding a wait here b/c this test seemed to want to fail
    # when run directly after test_playAnimationInLoop
    time.sleep(3)
    lightbar_state = frontDoor.getActiveAnimation()
    supported_animations = lightbar_state["properties"]["supportedValues"]

    total_animations = len(supported_animations)
    selected_animation = random.randint(0, total_animations - 1)

    animation_to_play = lightbar_state["properties"]["supportedValues"][selected_animation]
    log.debug("Play animation - %s", animation_to_play)

    data = {"nextValue":
                {"value": animation_to_play,
                 "transition": "SMOOTH",
                 "repeat": True}}
    lightbar_state = frontDoor.playLightBarAnimation(json.dumps(data))

    time.sleep(2)

    data = {"nextValue":
                {"value": animation_to_play,
                 "transition": "SMOOTH",
                 "repeat": True}}
    lightbar_state = frontDoor.stopActiveAnimation(json.dumps(data))

    # TODO: Cleanup this logic
    if "currentValue" in lightbar_state:
        if "value" in lightbar_state["currentValue"]:
            current_key = lightbar_state["currentValue"]["value"]
            assert current_key == animation_to_play
        else:
            assert True == False
    else:
        assert True == False

    if "error" in lightbar_state:
        if "description" in lightbar_state["error"]:
            error = lightbar_state["error"]["description"]
            assert error == ""


@pytest.mark.usefixtures('frontDoor')
def test_invalid_animation_value(frontDoor):
    """
    Test to ensure that an improper animation does not play.

    :param frontDoor: Fixture connection to the Front Door
    :return: None
    """
    value = "SOME_ANIMATION"
    data = {"nextValue":
                {"value": value,
                 "transition": "SMOOTH",
                 "repeat": True}}

    data = json.dumps(data)
    lightbar_state = frontDoor.playLightBarAnimation(data)

    error_anticipation = "animation not supported"
    error = lightbar_state["error"]["description"]
    assert error == error_anticipation, "Anticipated: '{}'; Received: {}".format(error_anticipation, error)
