"""
:Abstract: Module is used to keep common methods.
           All the common behaviors verification testing can be used for the demo.
"""

import pytest
import time
from pyadb import ADB
from CastleTestUtils.SoftwareUpdateUtils.RebootDevice import reboot_device as wait_for_reboot
from CastleTestUtils.LoggerUtils.log_setup import get_logger
adb = ADB('/usr/bin/adb')
logger = get_logger(__name__)

class DemoUtils():
    def __init__(self, frontDoor):
        """
        Define DemoUtils with the following variables:
        :param : frontDoor
        :type source: frontdoor object
        """
        logger.info("DemoUtils")
        self.frontDoor = frontDoor

    def setDemoMode(self, demoMode, deviceid):
        """
        set demoMode
        :param demoMode: on, off
        :param deviceid
        """
        logger.info("setDemoMode")
        msg = '{"demoMode":"' + demoMode + '"}'
        self.frontDoor.setDemo(msg)
        time.sleep(10)
        wait_for_reboot.wait_for_reboot_device(deviceid)
        logger.info("Reboot of target completed")
        try:
            cmd = 'find /mnt/nv -name DemoModeOn'
            result = adb.shell_command(cmd)
            if demoMode == 'on' and len(result) <1 :
                assert False,"File DemoModeOn not present when demoMode is on"
            elif demoMode == 'off' and result != None:
                assert False,"File DemoModeOn is present when demoMode is off"
        except Exception, e:
            raise Exception(e)

    def verifyDemoModeOn(self, timeout):
        """
        verifyDemoMode is on every 20 secs until timeout
        :param timeout:
        """
        logger.info("verifyDemoModeOn")
        startTime = time.time()
        elapsed = 0
        while elapsed <= timeout:
            demoResponse = self.frontDoor.getDemoMode()
            logger.info("demoResponse" + demoResponse)
            if demoResponse != 'on':
                assert False , "DemoMode is not on when expected. demoResponse: " + demoResponse
                time.sleep(20)
            elapsed = time.time() - startTime