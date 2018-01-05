#!/usr/bin/python2.7
"""
:Abstract:
This module contains and integrated test of the Demo Controller. It uses the FrontdoorAPI and adb
to validate responses.
"""
import pytest
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from ..keyUtils import cli_keys as key
logger = get_logger(__name__)

@pytest.mark.usefixtures("save_speaker_log")
class TestDemo():
    """ Test Class for Demo State """
    def test_demoOffAfterTimeout(self, demoUtils, device_ip, request):
        """
        This test verifies demoMode goes 'off' after timeout
        Test steps:
        1. Set demoMode "on"
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv
        4. Verify device stays in demoMode "on"
        5. Verify device reboots the second time and demoMode goes "off" within the specified time
        """
        logger.info("Start test_demoOffAfterTimeout")
        demoUtils.setDemoMode("on", True, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode("on")
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, responseTimeout
        demoUtils.verifySecondReboot(responseTimeout *2)

    def test_demoOnAfterTimeout(self, demoUtils, device_ip, request):
        """
        This test verifies the demoMode stays on after timeout
        Test steps:
        1. Set demoMode "on"
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv
        4. Verify demoMode is "on" until timeout-40
        5. Set demoMode "on" before timeout and verify demoMode is "on" after timeout
        """
        logger.info("Start test_demo_on_after_timeout")
        demoUtils.setDemoMode("on", True, 3, request.config.getoption("--network-iface"))
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, responseTimeout
        demoUtils.verifyDemoModeOn(responseTimeout-40)
        demoUtils.setDemoMode("on", False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoModeOn(60)

    def test_demoOnFor30Min(self, demoUtils, device_ip, request):
        """
        This test verifies demoMode stays 'on' for 30 minutes
        Test steps:
        1. Set demoMode 'on'
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv and demoMode is 'On'
        4. Set demoMode 'on' and verify demoMode stays 'on' for 30 min
        """
        logger.info("Start test_demoOnFor30Min")
        demoUtils.setDemoMode("on", True, 3, request.config.getoption("--network-iface"))
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, responseTimeout
        demoUtils.verifyDemoModeOn(responseTimeout-40)
        demoUtils.setDemoMode("on", False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoModeOn(responseTimeout*10)

    @pytest.mark.skip(reason="wip this test should work once the startPlayback is implemented. Skipping it for right now")
    def test_demoOnStandby(self, frontDoor, demoUtils, device_ip):
        """
        This test after setting the demoMode to 'On' puts the device to standby and verify demoMode stays 'On' after coming out of standby.
        Test steps:
        1. Set demoMode 'on'
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv and verify demoMode is 'On'
        4. Set demoMode 'On' and verify
        5. StopPlayback
        6. StartPlayback
        7. Verify demoMode is 'oN'
        """
        logger.info("Start test_demoOnStandby")
        demoUtils.setDemoMode("on", True)
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, responseTimeout
        demoUtils.verifyDemoModeOn(responseTimeout-40)
        demoUtils.setDemoMode("on", False)
        demoUtils.verifyDemoMode("on")
        stopPlaybackResponse = frontDoor.stopPlaybackRequest()
        assert stopPlaybackResponse["body"]["container"]["contentItem"]["status"] != "stop"
        startPlaybackResponse = frontDoor.startPlaybackRequest()
        assert startPlaybackResponse["body"]["container"]["contentItem"]["status"] != "play"
        demoUtils.verifyDemoMode("on")

    def test_demoKeyIntent(self, request, demoUtils, device_ip):
        """
        This test verifies keyIntent is not present when demoMode is 'Off' and is present when demoMode is 'On' and /opt/Bose/etc/KeyConfiguration-demo.json file exists
        Test steps:
        1. With demoMode off press mfb and verify keyIntent is not present
        2. Set demoMode 'On'
        3. Press mfb and verify keyIntent is present and /opt/Bose/etc/KeyConfiguration-demo.json file exists
        """
        key.mfb_playpause()
        demoUtils.verifyNotification()
        key.mfb_nw_standby()
        demoUtils.verifyNotification()
        key.mfb_lp_standby()
        demoUtils.verifyNotification()
        key.mfb_next_track()
        demoUtils.verifyNotification()
        key.mfb_prev_track()
        demoUtils.verifyNotification()
        logger.info("Start test_demoOnKeyIntent")
        demoUtils.setDemoMode("on", True, 3, request.config.getoption("--network-iface"))
        status, responseTimeout = demoUtils.getDemoTimeout(device_ip)
        assert status, responseTimeout
        demoUtils.verifyDemoModeOn(responseTimeout-60)
        demoUtils.setDemoMode("on", False, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode("on")
        key.mfb_playpause()
        demoUtils.verifyNotification()
        key.mfb_nw_standby()
        demoUtils.verifyNotification()
        key.mfb_lp_standby()
        demoUtils.verifyNotification()
        key.mfb_next_track()
        demoUtils.verifyNotification()
        key.mfb_prev_track()
        demoUtils.verifyNotification()