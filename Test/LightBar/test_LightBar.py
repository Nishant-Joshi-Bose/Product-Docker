"""
Automated Tests for LightBar Animations for Eddie
"""
import json
import time
import random

import pytest
from CastleTestUtils.FrontDoorAPI import FrontDoorAPI

_ip_address = pytest.config.getoption('--ip-address')
if _ip_address is None:
    print "The _ip_address is:", _ip_address
    pytest.fail("Do this: pytest -sv <test_example.py> --ip-address <ip>")

frontDoor = FrontDoorAPI.FrontDoorAPI(_ip_address)

def test_getActiveAnimation():
    """ Assert all active animations """
    lb_state = frontDoor.getActiveAnimation()
    print "The state is", lb_state
    transition = lb_state["properties"]["supported_transitions"]
    values = lb_state["properties"]["supported_values"]

def test_playAnimationInLoop():
    """
    Test: Play all animations in a loop
          Ensure they are started
    """
    lb_state = frontDoor.getActiveAnimation()
    print "The state is", lb_state
    transition = lb_state["properties"]["supported_transitions"]
    values = lb_state["properties"]["supported_values"]

    for value in values:
        time.sleep(5)
        print "value = ", value
        # create request to play animation in loop
        data = {"next":{"value":value, "transition":"smooth", "repeat":True}}
        data = json.dumps(data)
        lb_state = frontDoor.playLightBarAnimation(data)
        print "lb_state = ", lb_state
        next_val = lb_state["next"]["value"]
        assert next_val == value
    lb_state = frontDoor.getActiveAnimation()
    current_val = lb_state["next"]["value"]
    assert current_val == value
    lb_state = frontDoor.stopActiveAnimation(data)

def test_stopActiveAnimation():
    """
    Test: Start animation from list
          Stop animation
    """
    lb_state = frontDoor.getActiveAnimation()
    values = lb_state["properties"]["supported_values"]
    total_anims = len(values)
    selected_anim = random.randint(0, total_anims-1)
    anim_to_play = lb_state["properties"]["supported_values"][selected_anim]
    data = {"next":{"value":anim_to_play, "transition":"smooth", "repeat":True}}
    data = json.dumps(data)
    print "Play aniamtion - ", anim_to_play
    lb_state = frontDoor.playLightBarAnimation(data)
    time.sleep(2)
    data = {"next":{"value":anim_to_play, "transition":"smooth", "repeat":True}}
    data = json.dumps(data)
    lb_state = frontDoor.stopActiveAnimation(data)
    #TODO: Cleanup this logic
    if "current" in lb_state:
        if "value" in lb_state["current"]:
            current_key = lb_state["current"]["value"]
            assert current_key == anim_to_play
        else:
            assert True == False
    else:
        assert True == False
    if "error" in lb_state:
        if "description" in lb_state["error"]:
            error = lb_state["error"]["description"]
            assert error == ""

def test_invalidAnimationValue():
    """ Invalid animation value test """
    value = "SOME_ANIMATION"
    data = {"next":{"value":value, "transition":"smooth", "repeat":True}}
    data = json.dumps(data)
    lb_state = frontDoor.playLightBarAnimation(data)
    error = lb_state["error"]["description"]
    assert error == "animation not supported"
