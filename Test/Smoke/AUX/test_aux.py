"""
Organization: BOSE CORPORATION
Copyright: COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
This program may not be reproduced, in whole or in part in any
form or any means whatsoever without the written permission of:
BOSE CORPORATION
The Mountain,
Framingham, MA 01701-9168

This file is to test AUX scenario
"""

import time
import logging
import pytest
import pexpect
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.RivieraUtils.hardware.keys import KeyUtils
from aux_utility import AUXUtils
from ..smoke_utility import SmokeUtils


logger = get_logger(__name__, "EddieAUXTest.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)
logger.info("Starting AUX Tests...")

smoke_util = SmokeUtils(logger)
aux_util = AUXUtils(smoke_util, logger)

class TestAUX(object):
    """
    Eddie Smoke Test Class
    """

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_aux_key(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object
        :param frontDoor: Fixture to return FrontDoor object

        - Press AUX Key and Validate
        - Press Bluetooth Key to switch source
        - Press AUX Key again and Validate
        """
        aux_res = aux_util.aux_key_press(adb, frontDoor)
        if not aux_res:
            assert False, "AUX Key is not press"

        bluetooth_res = aux_util.bluetooth_key_press(adb, frontDoor)
        if not bluetooth_res:
            assert False, "Bluetooth Key is not press"

        aux_res = aux_util.aux_key_press(adb, frontDoor)
        if not aux_res:
            assert False, "AUX Key is not press"

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_aux_volume(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object
        :param frontDoor: Fixture to return FrontDoor object

        - Press AUX Key and Validate
        - Press Volume Up and Down Key and validate volume
        """
        aux_res = aux_util.aux_key_press(adb, frontDoor)
        if not aux_res:
            assert False, "AUX Key is not press"

        volume_min = 100
        volume_max = 0

        volume_actual = smoke_util.get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)
        # Volume already set to MAX then need to do volume down before volume up key press
        if volume_actual == volume_max:
            smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 30)
            volume_actual = smoke_util.get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)

        smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 20)
        volume_up = smoke_util.get_volume(frontDoor)
        logger.debug("Volume Up : %s ", volume_up)
        assert volume_up > volume_actual, "Volume Up Key Press not happened"

        volume_actual = smoke_util.get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)

        # Volume already set to MIN then need to do volume up before volume down key press
        if volume_actual == volume_min:
            smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 30)
            volume_actual = smoke_util.get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)

        smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 20)
        volume_down = smoke_util.get_volume(frontDoor)
        logger.debug("Volume Down : %s ", volume_down)
        assert volume_down < volume_actual, "Volume Down Key Press not happened"

        aux_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        if 'AUX' not in aux_response:
            assert False, "AUX Key is not press"

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_aux_power_key(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object
        :param frontDoor: Fixture to return FrontDoor object

        - Press AUX Key and Validate
        - Press Power Key to put Eddie on Stand by
        - Press Power Key again to wake up Eddie and it should be on AUX mode
        """
        aux_res = aux_util.aux_key_press(adb, frontDoor)
        if not aux_res:
            assert False, "AUX Key is not press"

        tap = pexpect.spawn("telnet %s 17000" % frontDoor._ip_address)
        KeyUtils.power_key_press(tap, 3000)

        # Just to wait for 5 seconds before wake up Eddie again
        time.sleep(5)

        tap = pexpect.spawn("telnet %s 17000" % frontDoor._ip_address)
        KeyUtils.power_key_press(tap, 3000)

        aux_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        if 'AUX' not in aux_response:
            assert False, "AUX Key is not press"

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_aux_mute_key(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object
        :param frontDoor: Fixture to return FrontDoor object

        - Press AUX Key and Validate
        - Press Power Key to put Eddie on Mute and Validate
        - Press Power Key again to Unmute Eddie and Validate
        - Press Power Key to put Eddie on Mute and Validate
        - Press Bluetooth Key and Eddie Should automatically unmute
        - Press AUX Key and Eddie should unmuted
        """

        aux_res = aux_util.aux_key_press(adb, frontDoor)
        if not aux_res:
            assert False, "AUX Key is not press"

        muted = aux_util.aux_mute(frontDoor, 20)
        if not muted:
            assert False, "AUX mode not Muted"

        unmuted = aux_util.aux_unmute(frontDoor, 20)
        if not unmuted:
            assert False, "AUX mode not UnMuted"

        muted = aux_util.aux_mute(frontDoor, 20)
        if not muted:
            assert False, "AUX mode not Muted"

        bluetooth_res = aux_util.bluetooth_key_press(adb, frontDoor)
        if not bluetooth_res:
            assert False, "Bluetooth Key is not press"

        aux_res = aux_util.aux_key_press(adb, frontDoor)
        if not aux_res:
            assert False, "AUX Key is not press"

        response = frontDoor.getVolume()
        muted = response['body']['muted']

        if muted:
            assert False, "AUX mode not UnMuted"
