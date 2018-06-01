#!/usr/bin/python2.7
"""
:Abstract:
This module contains and integrated test of the Demo Controller. It uses the FrontdoorAPI and adb
to validate responses.
"""
import json
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import logging
logger = get_logger(__name__, "DemoLog.log", level=logging.INFO, fileLoglevel=logging.DEBUG)
import time

@pytest.mark.usefixtures("save_speaker_log")
class TestDemo():
    """ Test Class for Demo State """
    @pytest.mark.usefixtures("demoUtils", "deviceid", "device_ip", "request", "frontDoor_reboot")
    def test_demoOnAfterTimeout(self, demoUtils, device_ip, request, deviceid, frontDoor_reboot):
        """
        This test verifies the demoMode stays on after timeout
        Test steps:
        1. Set demoMode True
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv
        4. Verify demoMode is True
        5. Set demoMode True before timeout and verify demoMode stays True after timeout
        """
        logger.info("Start test_demo_on_after_timeout")
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, True, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(True, frontDoor_reboot)
        time.sleep(5)
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoModeOn(60, frontDoor_reboot)

    @pytest.mark.usefixtures("demoUtils", "deviceid", "device_ip", "request", "frontDoor_reboot")
    def test_demoOnFor30Min(self, demoUtils, device_ip, request, deviceid, frontDoor_reboot):
        """
        This test verifies demoMode stays True for 30 minutes
        Test steps:
        1. Set demoMode True
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv and demoMode is True
        4. Set demoMode True and verify demoMode stays True for 30 min
        """
        logger.info("Start test_demoOnFor30Min")
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, True, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(True, frontDoor_reboot)
        status = demoUtils.setDemoTimeout(device_ip)
        assert status, "Error: Demo timeout not set"
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, "Demo timeout reported Exception {} " + responseTimeout
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoModeOn(responseTimeout*30, frontDoor_reboot)

    @pytest.mark.usefixtures("request", "deviceid", "demoUtils", "device_ip", "frontDoor_reboot")
    def test_demoPlayPauseBehaviour(self, request, demoUtils, device_ip, deviceid, frontDoor_reboot):
        """
        This test verifies PlayPauseBehaviour while demoMode is True
        Test steps:
        1. Set demoMode True
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv and demoMode is True
        4. Set demoMode True and verify demoMode
        6. verifyPlayPauseBehaviour
        7. Verify demoMode is True
        """
        logger.info("Start test_demoOnStartStopPlayback")
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, True, 3, request.config.getoption("--network-iface"))
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, "Demo timeout reported Exception {} " + responseTimeout
        demoUtils.verifyDemoMode(True, frontDoor_reboot)
        time.sleep(5)
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoModeOn(20, frontDoor_reboot)
        demoUtils.verifyPlayPauseBehaviour(frontDoor_reboot)
        demoUtils.verifyDemoModeOn(20, frontDoor_reboot)

    @pytest.mark.usefixtures("demoUtils", "deviceid", "device_ip", "request", "get_config", "frontDoor_reboot")
    def test_demoKeyConfig(self, demoUtils, device_ip, request, get_config, deviceid, frontDoor_reboot):
        """
        This test add keyConfig. Set demoMode true and finally delete the keyConfig
        Test steps:
        1. With demoMode off verify no keyConfig exists
        2. Set keyConfig
        3. Set demoMode true and verify
        4. Verify keyConfig exists
        5. Delete the keyConfig and verify
        """
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot, "Error Reading configuration file")
        demoUtils.setKeyConfig(json.dumps(get_config), frontDoor_reboot)
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, True, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(True, frontDoor_reboot)
        time.sleep(5)
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoModeOn(10, frontDoor_reboot)
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot)
        demoUtils.deleteKeyConfig(frontDoor_reboot)
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot, "Error Reading configuration file")

    @pytest.mark.usefixtures("demoUtils", "deviceid", "device_ip", "request", "get_config", "frontDoor_reboot")
    def test_demoKeyControlWork(self, demoUtils, device_ip, request, get_config, deviceid, frontDoor_reboot):
        """
        This test verifies with demo set to True. Demo keyControl(volume) works when keyConfig is present
        Test steps:
        1. With demoMode off verify no keyConfig exists
        2. Add keyConfig and verify
        3. Set demoMode on and verify
        4. Play audio, verify
        5. Set, change volume and verify
        5. Delete the keyConfig and verify
        6. StopPlayback
        """
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot, "Error Reading configuration file")
        demoUtils.setKeyConfig(json.dumps(get_config), frontDoor_reboot)
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot)
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, True, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(True, frontDoor_reboot)
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoModeOn(20, frontDoor_reboot)
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot)
        demoUtils.playandValidateMusic(frontDoor_reboot)
        demoUtils.verifyVolumeKeyControl(device_ip, True, frontDoor_reboot)
        demoUtils.deleteKeyConfig(frontDoor_reboot)
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot, "Error Reading configuration file")
        demoUtils.stopPlayback(frontDoor_reboot)
