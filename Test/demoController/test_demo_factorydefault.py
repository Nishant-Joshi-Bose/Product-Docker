#!/usr/bin/python2.7
"""
:Abstract:
This module contains factory default integrated test of the Demo Controller. It uses the FrontdoorAPI and adb
to validate responses.
"""
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
import logging
logger = get_logger(__name__, "DemoLog.log", level=logging.INFO, fileLoglevel=logging.DEBUG)

@pytest.mark.usefixtures("save_speaker_log")
class TestDemoFactoryDefault():
    """ Test Class for Demo factorydefault """
    #@pytest.mark.usefixtures("demoUtils", "deviceid", "device_ip", "request", "frontDoor_reboot") change
    #@pytest.mark.skip("")
    #def test_demoOffAfterTimeout(self, demoUtils, device_ip, request, deviceid, frontDoor_reboot): change
    @pytest.mark.usefixtures("demoUtils", "deviceid", "device_ip", "request", "front_door_email")
    #@pytest.mark.skip("")
    def test_demoOffAfterTimeout(self, demoUtils, device_ip, request, deviceid, front_door_email):
        """
        This test verifies demoMode is False after timeout
        Test steps:
        1. Set demoMode True
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv/product-persistance
        4. Verify device stays in demoMode True
        5. Verify device factory defaults within the specified time and demoMode is set to False
        """
        """
        logger.info("Start test_demoOffAfterTimeout")
        demoUtils.setDemoMode(True, deviceid, front_door_email, True, 3,
                              request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(True, front_door_email)
        status = demoUtils.setDemoTimeout(device_ip)
        assert status, "Error: Demo timeout not set"
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, "Demo timeout reported Exception {} " + responseTimeout
        demoUtils.verifyFactoryDefault(responseTimeout * 2, frontDoor_reboot, deviceid,
                                       request.config.getoption("--network-iface"))        
        """
        logger.info("Start test_demoOffAfterTimeout")
        demoUtils.setDemoMode(True, deviceid, front_door_email, True, 3,
                              request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(True, front_door_email)
        status = demoUtils.setDemoTimeout(device_ip)
        assert status, "Error: Demo timeout not set"
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, "Demo timeout reported Exception {} " + responseTimeout
        demoUtils.verifyFactoryDefault(responseTimeout * 2, front_door_email, deviceid,
                                       request.config.getoption("--network-iface"))
