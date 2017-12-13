"""
Automated Tests for LightBar Animations for Eddie
"""
import re
import csv
import json
from time import sleep
from configure import conf
import pytest
import serial.tools.list_ports as serialport
from CastleTestUtils.LoggerUtils.BSerialLogger import SerialLoggingUtil
from CastleTestUtils.LoggerUtils.log_setup import get_logger
logger = get_logger(__file__)

@pytest.mark.usefixture("save_speaker_log")
class Test_lightbar():
    """
    Class that handles Lightbar Animation patterns

    """
    def get_portlist(self):
        """
        Generates the list for serial port

        """
        self.port_list = []
        for portName in serialport.comports():
            self.port_list.append(portName.device)
        return self.port_list

    def test_getActiveAnimation(self, get_FrontdoorInstance):
        """
        Get the active animation pattern currently being played on lightbar
        param: Frontdoor Instance

        """
        logger.info("Running Get Active Animation Test")
        animation_state = get_FrontdoorInstance.getActiveAnimation()
        logger.debug("The animation state is %s" % animation_state)
        currentData = animation_state["current"]
        if "value" in currentData.keys():
            received_animation = currentData["value"]
            logger.debug("The animation currently playing is :-- %s" % received_animation)
        else:
            logger.error("No animation is currently playing")

    def test_deleteActiveAnimation(self, get_FrontdoorInstance):
        """
        Delete the animation pattern currently being played
        param: Frontdoor Instance

        """
        logger.info("Running Delete Active Animation Test")
        animation_state = get_FrontdoorInstance.getActiveAnimation()
        logger.debug("The  animation state is %s" % animation_state)
        animation_keys = animation_state.keys()
        currentData = animation_state["current"].keys()
        if "value" in currentData:
            animation_value = animation_state["current"]["value"]
            data = {"next":{"value":animation_value, "transition":"smooth", "repeat":"true"}}
            json_data = json.dumps(data)
            animation_state = get_FrontdoorInstance.stopActiveAnimation(json_data)
            animation_keys = animation_state.keys()
            if "error" in animation_keys:
                assert False, animation_keys["error"]["description"]
            else:
                logger.debug("Last animation played on device is %s" % animation_value)
        else:
            logger.error("Play some animation using put request")

    def test_playInValidAnimationScenarios(self, get_FrontdoorInstance):
        """
        Play the negative scenarios of lightbar animation pattern from conf["csv_filename"] file
        param: Frontdoor Instance

        """
        # For negative testing values
        passNegScenarios = []
        failNegScenarios = []
        logger.info("Playing negative test of lightbar animation")
        with open(conf["csv_filename"]) as ScenarioFile:
            # skipping the labels from the csv file
            next(ScenarioFile)
            reader = csv.reader(ScenarioFile)
            # create the request to play animation from the scenarios
            # row will contain [animation_value,transition_value,repeat_status]
            for row in reader:
                # sleep before playing next animation 
                sleep(2)
                data = {"next":{"value":row[0], "transition":row[1], "repeat":row[2]}}
                if row[1] == "queue":
                    continue
                json_data = json.dumps(data)
                logger.info("Data is %s " % json_data)
                animation_state = get_FrontdoorInstance.playLightBarAnimation(json_data)
                logger.debug("The received state is %s" % animation_state)
                if "error" in animation_state.keys():
                    if "description" in animation_state["error"].keys():
                        passNegScenarios.append(json_data)
                    else:
                        failNegScenarios.append(json_data)
                else:
                    failNegScenarios.append(json_data)
            assert len(failNegScenarios) == 0, "Some negative scenarios failed %s " %(failNegScenarios)
            logger.debug("Passed Negative scenarios  are %s" % passNegScenarios)

    def test_playValidAnimationScenarios(self, get_FrontdoorInstance):
        """
        Plays the positive scenarios of lightbar animation pattern from conf["json_filename"] file
        param: Frontdoor Instance

        """
        # For positive testing values
        passPosScenarios = []
        failPosScenarios = []
        SerialDetails = []
        logger.info("Playing positive tests for lightbar animation")
        checkValidPort = False
        ports = self.get_portlist()
        logger.debug(ports)
        if conf["LPMport"]:
            if conf["LPMport"]  in self.get_portlist():
                checkValidPort = True
                logger.debug("Connected on Port %s" % conf["LPMport"])
            else:
                assert False, "Please enter the valid port"
        # reading different animation scenarios from the json files
        animations = []
        with open(conf["json_filename"]) as filename:
            # convert json object to dictionary for parsing animation value
            json_to_dict = json.load(filename)
        for animation_name in json_to_dict["animations"]:
            animations.append(str(animation_name["name"]))
        logger.info("List of supported animations:\n")
        for iterator in animations:
            logger.debug(iterator)
        # creating list of data for animation to play
        scenarios = [(animation, transition, repeat_state) for repeat_state in conf["repeat_states"] for transition in conf["transitions"] for animation in animations]
        for values in scenarios:
            # sleep before playing next animation
            sleep(2)
            animationData = {"next":{"value":values[0], "transition":values[1], "repeat":values[2]}}
            json_data = json.dumps(animationData)
            animation_state = get_FrontdoorInstance.playLightBarAnimation(json_data)
            # verification for serial logs if connected with the device
            if checkValidPort:
                SerialLog = SerialLoggingUtil(None, conf["LPMport"])
                SerialOutput = SerialLog.execute_command('lb history')
                logger.info("\nLightbar logs from LPM are:\n %s" % SerialOutput)
                SerialLastAnimation = SerialOutput[-2].split(",")[-2:]
                SerialDetails.append(SerialLastAnimation)
            if "error" in animation_state.keys():
                error_description = animation_state["error"]["description"]
                if error_description == "animation not supported":
                    failPosScenarios.append(animationData)
            else:
                passPosScenarios.append(animationData)
                logger.debug("Scenario passed\n")
        assert len(failPosScenarios) == 0,"Failed Positive scenarios are  %s " %(failPosScenarios)
        logger.info("All positive scenarios passed\n")
        logger.info("SerialDetails are %s" % SerialDetails)
