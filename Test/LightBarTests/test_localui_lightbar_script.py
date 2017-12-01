import csv
import json
from time import sleep
from configure import conf
import serial.tools.list_ports as serialport
from CastleTestUtils.LoggerUtils.BSerialLogger import SerialLoggingUtil
from CastleTestUtils.LoggerUtils.log_setup import get_logger
logger = get_logger(__file__)


class Test_lightbar():

    def get_portlist(self):
        # generate the list for serial port
        self.port_list = []
        for portName in serialport.comports():
            self.port_list.append(portName.device)
        return self.port_list

    def pytest_generate_tests(self, metafunc):
        conf["LPMport"] = metafunc.config.getoption("LPM_port")

    def test_getActiveAnimation(self, get_FrontdoorInstance):
        # getting the current animation value
        logger.info("Running Get Active Animation Test")
        animation_state = get_FrontdoorInstance.getActiveAnimation()
        logger.debug("The state is %s" % animation_state)
        transition = animation_state["properties"]["supported_transitions"]
        values = animation_state["properties"]["supported_values"]
        animation_keys = animation_state.keys()
        assert "current" in animation_keys, "Animation is not being played "
        current = animation_state["current"]
        if "value" in current.keys():
            received_animation = current["value"]
            logger.debug("The animation currently playing is :-- %s" % received_animation)
        else:
            logger.debug("No animation is currently playing")
        assert "error" not in animation_state.keys(), "Error occurred in get animation"

    def test_deleteActiveAnimation(self, get_FrontdoorInstance):
        # delete the playing animation
        logger.info("Stopping the  active animation")
        animation_state = get_FrontdoorInstance.getActiveAnimation()
        logger.debug("The active animation is %s" % animation_state)
        animation_keys = animation_state.keys()
        assert "current" in animation_keys, "Animation is not played correctly"
        currentData = animation_state["current"].keys()
        if "value" in currentData:
            animation_to_delete = animation_state["current"]["value"]
            value = animation_to_delete
            data = {"next":{"value":value, "transition":"smooth", "repeat":"true"}}
            data = json.dumps(data)
            animation_state = get_FrontdoorInstance.stopActiveAnimation(data)
            animation_keys = animation_state.keys()
            assert 'current' in animation_keys, "Animation is not playing perfect on the device"
            if "error" in animation_keys:
                assert "description" in (animation_keys["error"]).keys(), "All Animation is played successfully"
                assert False, animation_keys["error"]["description"]
            else:
                logger.debug("Last animation played on device is %s" % animation_to_delete)
        else:
            logger.debug("Play some animation using put request")

    def test_playInValidAnimationScenarios(self, get_FrontdoorInstance):
        # For negative testing values
        list_passScenarios = []
        list_failScenarios = []
        logger.info("Playing animation for negative tests")
        with open("ei_Bose_LightBar_Test_Scenarios.csv") as ScenarioFile:
            next(ScenarioFile)
            reader = csv.reader(ScenarioFile)
            # create the request to play animation from the scenarios
            for row in reader:
                sleep(2)
                data = {"next":{"value":row[0], "transition":row[1], "repeat":row[2]}}
                if row[1] == "queue":
                    continue
                data1 = json.dumps(data)
                print("data is ", data1)
                animation_state1 = get_FrontdoorInstance.playLightBarAnimation(data1)
                logger.debug("The received state is %s" % animation_state1)
                if "error" in animation_state1.keys():
                    if "description" in animation_state1["error"].keys():
                        list_failScenarios.append(data1)
                    else:
                        list_passScenarios.append(data1)
                else:
                    list_passScenarios.append(data1)
            logger.debug("PASS ARE %s" % list_passScenarios)
            logger.debug("FAIL ARE %s" % list_failScenarios)
            assert len(list_passScenarios) == 0, "Some negative scenarios passed {}".format(list_passScenarios)
            logger.debug("All the negative scenarios are:--%s\n" % list_failScenarios)

    def test_playValidAnimationScenarios(self, get_FrontdoorInstance):
        # For positive testing values
        list_passScenarios = []
        list_failScenarios = []
        SerialDetails = []
        logger.info("Playing positve tests for lightbar animation")
        logger.info("Playing positve tests for lightbar animation")
        checkValidPort = False
        list1 = self.get_portlist()
        logger.debug(list1)
        if conf["LPMport"] != None:
            if conf["LPMport"]  in self.get_portlist():
                checkValidPort = True
                logger.debug("Connected on Port %s" % conf["LPMport"])
            else:
                raise Exception("Please enter the valid port")
        # reading different animation scenarios from the json files
        list_of_animation = []
        with open("Animations_manifest.json") as filename:
            # convert json object to dictionary for parsing animation value
            json_to_dict1 = json.load(filename)
        for animation_name in json_to_dict1["animations"]:
            list_of_animation.append(str(animation_name["name"]))
        logger.info("List of supported animations:\n")
        for j in list_of_animation:
            logger.debug(j)
        list_transition = ['smooth', 'immediate']
        list_repeat = ['true', 'false']
        for name in list_of_animation:
            for a in list_transition:
                for b in list_repeat:
                    sleep(2)
                    data1 = {"next":{"value":name, "transition":a, "repeat":b}}
                    if a == "queue":
                        continue
                    data1 = json.dumps(data1)
                    animation_state1 = get_FrontdoorInstance.playLightBarAnimation(data1)
                    # verification for serial logs if connected with the device
                    if checkValidPort:
                        SerialLog = SerialLoggingUtil(None, conf["LPMport"])
                        SerialOutput = SerialLog.execute_command('lb history')
                        logger.info("\nLightbar logs from LPM are:\n %s" % SerialOutput)
                        SerialLastAnimation = SerialOutput[-2].split(",")
                        SerialLastAnimation = SerialLastAnimation[-2:]
                        SerialDetails.append(SerialLastAnimation)
                    if "error" in animation_state1.keys():
                        error = animation_state1["error"]["description"]
                        if error == "animation not supported":
                            list_failScenarios.append(data1)
                        else:
                            list_passScenarios.append(data1)
                        if list_failScenarios:
                            logger.debug("\nSome scenarios are failed.They are :--\n %s" % list_failScenarios)
                            assert "False", "Failures found"
                    else:
                        logger.debug("Scenarios passed\n")
                    break
        logger.info("ALL POSITIVE SCENARIOS PASSED\n")
        logger.info("SerialDetails \n%s" % SerialDetails)
