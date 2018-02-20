# test_clikey.py
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
Automated Test for CLI key based on key events.
"""
import pytest
import KeyEvent

from configure import conf
from clikey_data import positive_scenarios, negative_scenarios
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from pyadb import ADB

adb = ADB(conf["adb_path"])
logger = get_logger(__file__)

@pytest.mark.usefixture("save_speaker_log")
class Test_KeyEvent():
    """
     Test Class for CLI key events execution
    """
    def key_execution(self, scenario, serial_handler, pexpect_client):
        """
        This helper functions is used to read keyevent value from scenario
        and call the keyevent method according using getatttr.
        :param scenario : Dictionary  containing the key id and timeout value
                            (Ex: keydata={"origin": 0,
                                         "keyEvent": 2, "keyList": [1,2], "timeOutList": [500]})
        :param serial_handler : serial_handler fixture defined in conftest.py file
        :param pexpect_client : pexpect_client fixture defined in conftest.py file
        :return generated_intent : List of intent value generated  from serial logs

        Steps:
        1 Call the keyevent method from KeyEvent file
          depend on scenario["keyEvent"] value.
        2 Verify the intent generated from serial logs
          with the expected intent value
        """
        # Dictionary contain different keyevent methods name as value defined in KeyEvent file
        # and keyevent value  as key.
        event = {1:"key_single", 2:"key_multiple",
                 3:"key_repetitive", 4:"key_burst", 5:"key_release_always"}
        logger.debug("Test Scenarios: %s : " % scenario)

        # Call event method according to scenario["KeyEvent"] value
        event_method = event.get(scenario["keyEvent"])

        # Get the method of the keyevent based on event_method using getattr builtin function.
        serial_logs = getattr(KeyEvent, event_method)(scenario, pexpect_client, serial_handler)
        logger.info("----------------KEY EVENT LOG -----------------")
        for serial_log in serial_logs:
            logger.debug(serial_log)

        # Get the generated intents value from the serial logs
        generated_intent = KeyEvent.compute_intent(serial_logs)
        logger.debug("Generated Intents are %s " % generated_intent)
        return generated_intent

    @pytest.mark.parametrize("scenario", negative_scenarios)
    def test_negative_key_configuration(self, scenario, serial_handler, pexpect_client):
        """
        This test will check the negative scenarios for keyevents fron clikey_data file
        :param scenario : Negative scenario from clikey_data file

        Steps:
        1 Read the negative keyevent data and perform it through methods defined in KeyEvent file.
        2 Verify if the intent value generated with the keyevent data
           Intent value should not be generate in case of negative scenario.
        """
        assert isinstance(scenario, dict), "input scenario should be dictionary"
        logger.info('Negative Scenario: KeyEvent %s, KeyList %s, Timeout %s' \
                             % (scenario["keyEvent"], scenario["keyList"], scenario["timeOutList"]))
        # check if any intent generate incase of negative scenarios.
        # generated_intent must be empty.
        generated_intent = self.key_execution(scenario, serial_handler, pexpect_client)
        assert not generated_intent, "Scenario %s contain intent value %s" \
                                                                   % (scenario, generated_intent)

    @pytest.mark.parametrize("scenario", positive_scenarios)
    def test_positive_key_configuration(self, scenario, serial_handler, pexpect_client):
        """
        This test will check the positive scenarios for keyevents from clikey_data file.
        :param scenario : Positive scenario from clikey_data file

        Steps:
        1 Read the positive keyevent data and perform it
          through methods defined in KeyEvent file.
        2 Verify the intent value generated with the expected intent value
          through compute_intent method used in KeyEvent file.
        """
        assert isinstance(scenario, dict), "input scenario should be dictionary"
        logger.info('Positive Scenario: KeyEvent %s, KeyList %s, Timeout %s' \
                             % (scenario["keyEvent"], scenario["keyList"], scenario["timeOutList"]))
        expected_intent = scenario["action"]
        logger.debug("Expected operation:-- %s and intent value is %s" \
                                                           % (scenario["comment"], expected_intent))
        # check if any intent generate incase of positive scenarios.
        # generated_intent must contain intent value.
        generated_intent = self.key_execution(scenario, serial_handler, pexpect_client)
        assert scenario["action"] in generated_intent, \
                     "Scenario %s doesnot contain intent value %s" % (scenario, expected_intent)
