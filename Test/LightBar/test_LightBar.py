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
    transition = lb_state["properties"]["supportedTransitions"]
    values = lb_state["properties"]["supportedValues"]

def test_playAnimationInLoop():
    """
    Test: Play all animations in a loop
          Ensure they are started
    """
    lb_state = frontDoor.getActiveAnimation()
    print "The state is", lb_state
    transition = lb_state["properties"]["supportedTransitions"]
    values = lb_state["properties"]["supportedValues"]

    for value in values:
        time.sleep(3)
        print "value = ", value
        # create request to play animation in loop
        data = {"nextValue":{"value":value, "transition":"SMOOTH", "repeat":True}}
        data = json.dumps(data)
        lb_state = frontDoor.playLightBarAnimation(data)
        print "lb_state = ", lb_state
        next_val = lb_state["nextValue"]["value"]
        assert next_val == value
    lb_state = frontDoor.getActiveAnimation()
    current_val = lb_state["nextValue"]["value"]
    assert current_val == value
    lb_state = frontDoor.stopActiveAnimation(data)

def test_stopActiveAnimation():
    """
    Test: Start animation from list
          Stop animation
    """
    # DMR Adding a wait here b/c this test seemed to want to fail
    # when run directly after test_playAnimationInLoop
    time.sleep(3)
    lb_state = frontDoor.getActiveAnimation()
    values = lb_state["properties"]["supportedValues"]
    total_anims = len(values)
    selected_anim = random.randint(0, total_anims-1)
    anim_to_play = lb_state["properties"]["supportedValues"][selected_anim]
    data = {"nextValue":{"value":anim_to_play, "transition":"SMOOTH", "repeat":True}}
    data = json.dumps(data)
    print "Play animation - ", anim_to_play
    lb_state = frontDoor.playLightBarAnimation(data)
    time.sleep(2)
    data = {"nextValue":{"value":anim_to_play, "transition":"SMOOTH", "repeat":True}}
    data = json.dumps(data)
    lb_state = frontDoor.stopActiveAnimation(data)
    #TODO: Cleanup this logic
    if "currentValue" in lb_state:
        if "value" in lb_state["currentValue"]:
            current_key = lb_state["currentValue"]["value"]
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
    data = {"nextValue":{"value":value, "transition":"SMOOTH", "repeat":True}}
    data = json.dumps(data)
    lb_state = frontDoor.playLightBarAnimation(data)
    error = lb_state["error"]["description"]
    assert error == "animation not supported"
