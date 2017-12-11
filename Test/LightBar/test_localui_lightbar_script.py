import csv
import json
from time import sleep
from configure import conf
import serial.tools.list_ports as serialport
from CastleTestUtils.LoggerUtils.BSerialLogger import SerialLoggingUtil
from CastleTestUtils.LoggerUtils.log_setup import get_logger
logger = get_logger(__file__)

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
        Gets the active animation pattern currently being played on lightbar
        param: Frontdoor Instance

        """
        logger.info("Running Get Active Animation Test")
        animation_state = get_FrontdoorInstance.getActiveAnimation()
        logger.debug("The state is %s" % animation_state)
        currentData = animation_state["current"]
        if "value" in currentData.keys():
            received_animation = currentData["value"]
            logger.debug("The animation currently playing is :-- %s" % received_animation)
        else:
            logger.error("No animation is currently playing")
        assert "error" not in animation_state.keys(), "Error occurred in get animation"

    def test_deleteActiveAnimation(self, get_FrontdoorInstance):
        """
        Delete the animation pattern currently being played
        param: Frontdoor Instance

        """
        logger.info("Stopping the  active animation")
        animation_state = get_FrontdoorInstance.getActiveAnimation()
        logger.debug("The active animation is %s" % animation_state)
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
        Plays the negative scenarios of lightbar animation pattern from csv file
        param: Frontdoor Instance

        """
        # For negative testing values
        passScenarios = []
        failScenarios = []
        logger.info("Playing animation for negative tests")
        with open("ei_Bose_LightBar_Test_Scenarios.csv") as ScenarioFile:
            # skipping the labels from the csv file
            next(ScenarioFile)
            reader = csv.reader(ScenarioFile)
            # create the request to play animation from the scenarios
            for row in reader:
                # sleep before playing next animation 
                sleep(2)
                data = {"next":{"value":row[0], "transition":row[1], "repeat":row[2]}}
                if row[1] == "queue":
                    continue
                json_data = json.dumps(data)
                logger.info("data is %s " % json_data)
                animation_state = get_FrontdoorInstance.playLightBarAnimation(json_data)
                logger.debug("The received state is %s" % animation_state)
                if "error" in animation_state.keys():
                    if "description" in animation_state["error"].keys():
                        failScenarios.append(json_data)
                    else:
                        passScenarios.append(json_data)
                else:
                    passScenarios.append(json_data)
            logger.debug("PASS ARE %s" % passScenarios)
            logger.debug("FAIL ARE %s" % failScenarios)
            assert len(passScenarios) == 0, "Some negative scenarios passed {}".format(passScenarios)
            logger.debug("All the negative scenarios are:--%s\n" % failScenarios)

    def test_playValidAnimationScenarios(self, get_FrontdoorInstance):
        """
        Plays the positive scenarios of lightbar animation pattern from json file
        param: Frontdoor Instance

        """
        # For positive testing values
        passScenarios = []
        failScenarios = []
        SerialDetails = []
        logger.info("Playing positve tests for lightbar animation")
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
        with open("Animations_manifest.json") as filename:
            # convert json object to dictionary for parsing animation value
            json_to_dict = json.load(filename)
        for animation_name in json_to_dict["animations"]:
            animations.append(str(animation_name["name"]))
        logger.info("List of supported animations:\n")
        for iterator in animations:
            logger.debug(iterator)
        transitions = ['smooth', 'immediate']
        repeat_states = ['true', 'false']
        for animation in animations:
            for transition in transitions:
                for repeat_state in repeat_states:
                    # sleep before playing next animation
                    sleep(2)
                    animationData = {"next":{"value":animation, "transition":transition, "repeat":repeat_state}}
                    if transition == "queue":
                        continue
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
                        error = animation_state["error"]["description"]
                        if error == "animation not supported":
                            failScenarios.append(animationData)
                        else:
                            passScenarios.append(animationData)
                        if failScenarios:
                            logger.debug("\nSome scenarios are failed.They are :--\n %s" % failScenarios)
                            assert "False", "Failures found"
                    else:
                        logger.debug("Scenarios passed\n")
        logger.info("ALL POSITIVE SCENARIOS PASSED\n")
        logger.info("SerialDetails \n%s" % SerialDetails)
