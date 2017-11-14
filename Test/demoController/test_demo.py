#!/usr/bin/python2.7

import pytest
import time
from pyadb import ADB

from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.SoftwareUpdateUtils.RebootDevice import reboot_device as wait_for_reboot
logger = get_logger(__name__)
adb = ADB('/usr/bin/adb')

@pytest.mark.usefixtures("save_speaker_log")
class TestDemo():
    _timeout=180
    #@pytest.mark.skip(reason="")
    def test_demoOffAfterTimeout(self, demoUtils, frontDoor, request):
        """
        This test verifies demoMode goes 'off' after timeout
        Test steps:
        1. Set demoMode "on"
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv
        4. Verify device stays in demoMode "on" until timeout
        5. demoMode goes "off" after timeout
        """
        logger.info("Start test_demoOffAfterTimeout")
        demoUtils.setDemoMode("on", request.config.getoption("--device-id"))
        demoUtils.verifyDemoMode(self._timeout)
        time.sleep(10)
        wait_for_reboot.wait_for_reboot_device(request.config.getoption("--device-id"))
        demoResponse = frontDoor.getDemoMode()
        logger.info("demoResponse" + demoResponse)
        if demoResponse == 'on':
            assert False , "DemoMode is not off after timeout when expected" + demoResponse

    #@pytest.mark.skip(reason="")
    def test_demoOnAfterTimeout(self, frontDoor, demoUtils, request):
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
        demoUtils.setDemoMode("on", request.config.getoption("--device-id"))
        demoUtils.verifyDemoModeOn(self._timeout-40)
        msg = '{"demoMode":"on"}'
        frontDoor.setDemo(msg)
        time.sleep(2)
        demoResponse = frontDoor.getDemoMode()
        logger.info("demoResponse" + demoResponse)
        if demoResponse == 'on':
            assert False , "DemoMode is not on when set to on after reboot. demoResponse: " + demoResponse

    #@pytest.mark.skip(reason="")
    def test_demoOnFor30Min(self, frontDoor, demoUtils, request):
        """
        This test verifies demoMode stays 'on' for 30 minutes
        Test steps:
        1. Set demoMode 'on'
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv and demoMode is 'On'
        4. Set demoMode 'on' and verify demoMode stays 'on' for 30 min
        """
        logger.info("Start test_demo_on_after_timeout")
        demoUtils.setDemoMode("on", request.config.getoption("--device-id"))
        demoUtils.verifyDemoModeOn(self._timeout-40)
        msg = '{"demoMode":"on"}'
        frontDoor.setDemo(msg)
        time.sleep(2)
        demoUtils.verifyDemoModeOn(self._timeout*10)

    @pytest.mark.skip(reason="this test should work once the startPlayback is implemented. Skipping it for right now")
    def test_demoOnStandby(self, frontDoor, demoUtils, request):
        """
        This test after setting the demoMode to 'On' puts the device to standby and verify demoMode stays 'On' after coming out of standby.
        Test steps:
        1. Set demoMode 'oN'
        2. Wait for device to reboot
        3. Verify file 'demoModeOn' exists under /mnt/nv and verify demoMode is 'On'
        4. Set demoMode 'On' and verify
        5. StopPlayback
        6. StartPlayback
        7. Verify demoMode is 'oN'
        """
        logger.info("Start test_demo_on_after_timeout")
        demoUtils.setDemoMode("on", request.config.getoption("--device-id"))
        demoUtils.verifyDemoModeOn(self._timeout-40)

        msg = '{"demoMode":"on"}'
        frontDoor.setDemo(msg)
        time.sleep(10)
        demoResponse = frontDoor.getDemoMode()
        logger.info("demoResponse: " + demoResponse)
        if demoResponse != 'on':
            assert False , "DemoMode is not on when expected, demoMode: " + demoResponse
        stopPlaybackResponse = frontDoor.stopPlaybackRequest()
        assert stopPlaybackResponse["body"]["container"]["contentItem"]["status"] != "stop"
        startPlaybackResponse = frontDoor.startPlaybackRequest()
        assert startPlaybackResponse["body"]["container"]["contentItem"]["status"] != "play"

        demoResponse = frontDoor.getDemoMode()
        logger.info("demoResponse: " + demoResponse)
        if demoResponse != 'on':
            assert False , "DemoMode is not oN when expected, demoMode: " + demoResponse