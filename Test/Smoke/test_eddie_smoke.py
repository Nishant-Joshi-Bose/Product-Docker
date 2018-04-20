"""
Organization: BOSE CORPORATION
Copyright: COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
This program may not be reproduced, in whole or in part in any
form or any means whatsoever without the written permission of:
BOSE CORPORATION
The Mountain,
Framingham, MA 01701-9168

This file is to test Eddie Smoke Test
"""

import time
import re
import logging
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.SupportUtils.StringUtils import BoseStringUtils
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from smoke_utility import SmokeUtils

logger = get_logger(__name__, "EddieSmokeTest.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)
logger.info("Starting Eddie Smoke Tests...")

smoke_util = SmokeUtils(logger)

class TestEddieSmoke(object):
    """
    Eddie Smoke Test Class
    """
    @pytest.mark.usefixtures("deviceid", "riviera", "adb")
    def test_factory_default(self, deviceid, riviera, adb):
        """
        :param deviceid: adb deviceid of Eddie
        :param riviera: Fixture to return RivieraUtils object
        :param adb: Fixture to return ADBCommunication object

        - Perform Factory default via TAP command
        - Validate Setup AP mode after Factory Default
        - Validate Product state should not be booting
        """
        riviera.perform_factory_default()
        ipaddress = smoke_util.get_deviceip(deviceid, 'wlan1')
        logger.debug("Device IP Address after factory default : %s ", ipaddress)
        assert ipaddress == '192.0.2.1', "Setup AP mode not up after factory default."

        product_state = adb.executeCommand('echo getproductstate | nc 0 17000')
        logger.debug("Eddie Product State : %s", product_state)
        if 'Booting' in product_state:
            assert False, "Eddie is in Booting State"

    @pytest.mark.usefixtures("ip_address_wlan")
    def test_wifi_profiles(self, ip_address_wlan):
        """
        :param ip_address_wlan: Fixture to get IP address of the device connected to WLAN

        - Add wifi profile to Eddie
        - Validate Device IP
        """
        logger.debug("Device IP Address : %s ", ip_address_wlan)
        assert BoseStringUtils.isIPValid(ip_address_wlan), "IP Address is not valid"

    @pytest.mark.usefixtures("adb")
    def test_galapagos_validation(self, adb):
        """
        :param adb: Fixture to return ADBCommunication object

        - Validate galapagos status
        - Validate server.crt file access mode
        """

        galapagos_status = adb.executeCommand('echo galapagos status | nc 0 17000')
        logger.debug("Galapagos Status : %s ", galapagos_status)
        if 'AUTHENTICATION_NO_ERROR' not in galapagos_status:
            assert False, "Galapogus status - Error in Authentication"

        if 'ACTIVATION_NO_ERROR' not in galapagos_status:
            assert False, "Galapogus status - Error in Activation"

        # It will take few seconds to appeared server.crt file after factory default
        start_time = time.time()
        timeout = 30
        while time.time() < timeout + start_time:
            file_access = adb.executeCommand('stat /mnt/nv/product-persistence/server.crt')
            if "No such file or directory" in file_access:
                logger.debug("server.crt file is not yet available... Retring again... Please wait....")
                continue
            else:
                result = re.search('Access: (\(\d{4}\/)', file_access)
                logger.debug("Server.crt file check : %s ", file_access)
                logger.debug("Server.crt file result : %s ", result)
                logger.debug("Server.crt file result group(0) : %s ", result.group(0))
                if '0444' not in result.group(1):
                    assert False, "Eddie server.crt file don't have read access"
                break
        product_status = adb.executeCommand('echo getproductstate | nc 0 17000')
        logger.debug("Eddie Product Status : %s ", product_status)
        if 'SetupOther' in product_status:
            oob_status = adb.executeCommand('echo setoobsetupcompleted | nc 0 17000')
            if 'SetOOBSetupCompleted is set' not in oob_status:
                assert False, "OOB status - Error in exit OOB"

    @pytest.mark.usefixtures("multiple_music_service_account", "verify_device_source", "music_sources", "adb", "frontDoor")
    def test_play_music(self, multiple_music_service_account, verify_device_source, music_sources, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object

        - Play and Validate Music Sources
        """
        logger.info("Bose Person ID : %s ", frontDoor._bosepersonID)

        for source_name in music_sources:
            logger.info("Music Sources : %s, ", source_name)
            smoke_util.validate_sources(source_name, adb)
            smoke_util.play_music(source_name, music_sources[source_name], frontDoor)
            smoke_util.stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("music_sources", "frontDoor")
    def test_switch_sources(self, music_sources, frontDoor):
        """
        :param music_sources: Fixture to return Sources List
        - Play and Validate Different Sources
        """
        # Below code will switch sources two times and play diffrent music sources
        for repeatcnt in range(0, 2):
            for source_name in music_sources:
                smoke_util.play_music(source_name, music_sources[source_name], frontDoor)

        smoke_util.stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("adb", "music_sources", "frontDoor")
    def test_store_preset(self, adb, music_sources, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object
        :param music_sources: Fixture to return Sources List

        - Store Preset on Eddie
        """

        preset_key_start = Keys.PRESET_1.value
        index = 0
        for source_name in music_sources:
            preset_key = preset_key_start + index
            smoke_util.play_music(source_name, music_sources[source_name], frontDoor)
            smoke_util.press_key(frontDoor._ip_address, preset_key, 5000)
            smoke_util.validate_preset(adb, (index+1), source_name)
            index += 1

        smoke_util.stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("music_sources", "frontDoor")
    def test_play_preset(self, music_sources, frontDoor):
        """
        :param music_sources: Fixture to return Sources List

        - Play stored Preset on Eddie
        """

        preset_key_start = Keys.PRESET_1.value
        index = 0
        for source_name in music_sources:
            preset_key = preset_key_start + index
            smoke_util.press_key(frontDoor._ip_address, preset_key, 750)
            smoke_util.validate_music_play(source_name, music_sources[source_name], frontDoor)
            index += 1

        smoke_util.stop_music(frontDoor._ip_address, frontDoor)

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_bluetooth_key(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object

        - Press Bluetooth Key and Validate
        """
        smoke_util.press_key(frontDoor._ip_address, Keys.BLUETOOTH.value, 750)
        bluetooth_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        result = re.search('BLUETOOTH()', bluetooth_response)
        if not result:
            assert False, "Bluetooth Key is not press"

    @pytest.mark.usefixtures("adb", "frontDoor")
    def test_aux_key(self, adb, frontDoor):
        """
        :param adb: Fixture to return ADBCommunication object

        - Press AUX Key and Validate
        """
        smoke_util.press_key(frontDoor._ip_address, Keys.AUX.value, 750)
        aux_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        result = re.search('AUX()', aux_response)
        if not result:
            assert False, "AUX Key is not press"

    @pytest.mark.usefixtures("frontDoor")
    def test_volume_updown_key(self, frontDoor):
        """
        :param frontDoor: Fixture to return frontDoor object

        - Press Volume Up and Down Key and Validate
        """
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

    @pytest.mark.usefixtures("frontDoor")
    def test_volume_updown_ramp_key(self, frontDoor):
        """
        :param frontDoor: Fixture to return frontDoor object

        - Press Volume Up and Down Ramp Key and Validate
        """
        volume_min = 100
        volume_max = 0

        volume_actual = smoke_util.get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)

        # Volume already set to MAX then need to do volume down before volume up key press
        if volume_actual == volume_max:
            smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 400)
            volume_actual = smoke_util.get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)

        smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 350)
        volume_up = smoke_util.get_volume(frontDoor)
        logger.debug("Volume Up : %s ", volume_up)
        assert volume_up > volume_actual, "Volume Up Key Press not happened"

        volume_actual = smoke_util.get_volume(frontDoor)
        logger.debug("Actual Volume : %s ", volume_actual)

        # Volume already set to MIN then need to do volume up before volume down key press
        if volume_actual == volume_min:
            smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_UP.value, 400)
            volume_actual = smoke_util.get_volume(frontDoor)
            logger.debug("Actual Volume : %s ", volume_actual)

        smoke_util.press_key(frontDoor._ip_address, Keys.VOLUME_DOWN.value, 350)
        volume_down = smoke_util.get_volume(frontDoor)
        logger.debug("Volume Down : %s ", volume_down)
        assert volume_down < volume_actual, "Volume Down Key Press not happened"
