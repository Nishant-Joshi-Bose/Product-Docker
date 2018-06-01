#!/usr/bin/python2.7
"""
:Abstract:
This module contains factory default integrated test of the Demo Controller. It uses the FrontdoorAPI and adb
to validate responses.
"""
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
import logging
logger = get_logger(__name__, "DemoLog.log", level=logging.INFO, fileLoglevel=logging.DEBUG)

@pytest.mark.usefixtures("save_speaker_log")
class TestDemoFactoryDefault():
    """ Test Class for Demo factorydefault """
    @pytest.mark.usefixtures("demoUtils", "deviceid", "device_ip", "request", "frontDoor_reboot")
    def test_demoOffAfterTimeout(self, demoUtils, device_ip, request, deviceid, frontDoor_reboot):
        """
        This test verifies demoMode is False after timeout
        Test steps:
        1. Set demoMode True
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv
        4. Verify device stays in demoMode True
        5. Verify device reboots the second time and demoMode is set to False within the specified time
        """
        logger.info("Start test_demoOffAfterTimeout")
        demoUtils.setDemoMode(True, deviceid, frontDoor_reboot, True, 3,
                              request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(True, frontDoor_reboot)
        status = demoUtils.setDemoTimeout(device_ip)
        assert status, "Error: Demo timeout not set"
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, "Demo timeout reported Exception {} " + responseTimeout
        demoUtils.verifyFactoryDefault(responseTimeout * 2, frontDoor_reboot, deviceid,
                                       request.config.getoption("--network-iface"))
