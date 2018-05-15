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
from CastleTestUtils.RivieraUtils import device_utils
from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from ..smoke_utility import SmokeUtils


logger = get_logger(__name__, "EddieAUXTest.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)
logger.info("Starting AUX Tests...")
smoke_util = SmokeUtils(logger)

class TestAUX(object):
    """
    Eddie Smoke Test Class
    """

    @pytest.mark.usefixtures("adb", "frontDoor", "device_id")
    def test_aux_key(self, adb, frontDoor, device_id):
        """
        :param adb: Fixture to return ADBCommunication object
        :param frontDoor: Fixture to return FrontDoor object

        - Press AUX Key and Validate
        - Press Bluetooth Key to switch source
        - Press AUX Key again and Validate
        """
        riviera = RivieraUtils('ADB', device=device_id)
        device_utils.set_device_source('AUX', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
            assert False, "AUX Key is not press"
        
        device_utils.set_device_source('BLUETOOTH', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'BLUETOOTH' not in current_source:
            assert False, "Bluetooth Key is not press"
        
        device_utils.set_device_source('AUX', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
            assert False, "AUX Key is not press"

    
    @pytest.mark.usefixtures("adb", "frontDoor", "device_id")
    def test_aux_volume(self, adb, frontDoor, device_id):
        """
        :param adb: Fixture to return ADBCommunication object
        :param frontDoor: Fixture to return FrontDoor object

        - Press AUX Key and Validate
        - Press Volume Up and Down Key and validate volume
        """
        riviera = RivieraUtils('ADB', device=device_id)
        device_utils.set_device_source('AUX', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
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

        current_source,current_state = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
            assert False, "AUX Key is not press"

    @pytest.mark.usefixtures("adb", "frontDoor", "device_id")
    def test_aux_power_key(self, adb, frontDoor, device_id):
        """
        :param adb: Fixture to return ADBCommunication object
        :param frontDoor: Fixture to return FrontDoor object

        - Press AUX Key and Validate
        - Press Power Key to put Eddie on Stand by
        - Press Power Key again to wake up Eddie and it should be on AUX mode
        """
        riviera = RivieraUtils('ADB', device=device_id)
        device_utils.set_device_source('AUX', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
            assert False, "AUX Key is not press"

        tap = pexpect.spawn("telnet %s 17000" % frontDoor._ip_address)
        KeyUtils.power_key_press(tap, 3000)

        # Just to wait for 5 seconds before wake up Eddie again
        time.sleep(5)

        tap = pexpect.spawn("telnet %s 17000" % frontDoor._ip_address)
        KeyUtils.power_key_press(tap, 3000)

        riviera = RivieraUtils('ADB', device=device_id)
        current_source,current_state = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
            assert False, "AUX Key is not press"

    @pytest.mark.usefixtures("adb", "frontDoor", "device_id")
    def test_aux_mute_key(self, adb, frontDoor, device_id):
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
        riviera = RivieraUtils('ADB', device=device_id)
        device_utils.set_device_source('AUX', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
            assert False, "AUX Key is not press"
        
        presstime = 20
        tap = pexpect.spawn("telnet %s 17000" % frontDoor._ip_address)

        KeyUtils.power_key_press(tap, presstime)
        #Give a second to mute volume
        time.sleep(1)
        response = frontDoor.getVolume()
        muted = response['body']['muted']
        if not muted:
            assert False, "AUX mode not Muted"

        KeyUtils.power_key_press(tap, presstime)
        #Give a second to mute volume
        time.sleep(1)
        response = frontDoor.getVolume()
        muted = response['body']['muted']
        if muted:
            assert False, "AUX mode not UnMuted"

        KeyUtils.power_key_press(tap, presstime)
        #Give a second to mute volume
        time.sleep(1)
        response = frontDoor.getVolume()
        muted = response['body']['muted']
        if not muted:
            assert False, "AUX mode not Muted"

        device_utils.set_device_source('BLUETOOTH', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'BLUETOOTH' not in current_source:
            assert False, "Bluetooth Key is not press"

        device_utils.set_device_source('AUX', device_id)
        current_source, current_status = riviera.current_source_name_and_status(device_id)
        if 'AUX' not in current_source:
            assert False, "AUX Key is not press"

        response = frontDoor.getVolume()
        muted = response['body']['muted']

        if muted:
            assert False, "AUX mode not UnMuted"
