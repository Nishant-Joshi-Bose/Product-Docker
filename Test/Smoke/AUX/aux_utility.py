"""
Organization: BOSE CORPORATION
Copyright: COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
This program may not be reproduced, in whole or in part in any
form or any means whatsoever without the written permission of:
BOSE CORPORATION
The Mountain,
Framingham, MA 01701-9168

Eddie AUX Test Utility
"""

import time
import pexpect
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys import KeyUtils
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys

class AUXUtils(object):
    """
    :Abstract: Common Helper class for AUX Test
    """

    def __init__(self, smoke_util, logger=None):
        """
        :param smoke_util: SmokeUtils object
        :param logger: Logger object
        
        init method to set Logger object
        """
        self.logger = logger or get_logger(__name__)
        self.smoke_util = smoke_util

    def aux_key_press(self, adb, frontDoor):
        """
        :param adb: ADBCommunication object
        :param frontDoor: FrontDoor object
        - AUX Key Press
        """
        self.smoke_util.press_key(frontDoor._ip_address, Keys.AUX.value, 750)
        aux_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        if 'AUX' not in aux_response:
            return False
        return True

    def bluetooth_key_press(self, adb, frontDoor):
        """
        :param adb: ADBCommunication object
        :param frontDoor: FrontDoor object
        - Bluetooth Key Press
        """
        self.smoke_util.press_key(frontDoor._ip_address, Keys.BLUETOOTH.value, 750)
        bluetooth_response = adb.executeCommand('echo getpdo system | nc 0 17000')
        if 'BLUETOOTH' not in bluetooth_response:
            return False
        return True

    def power_key(self, deviceip, presstime):
        """
        :param deviceip: Device IP of Eddie.
        :param presstime: time in ms.

        - Common function to press power key on Eddie.
        """
        tap = pexpect.spawn("telnet %s 17000" % deviceip)
        KeyUtils.power_key_press(tap, presstime)

    def aux_mute(self, frontDoor, presstime):
        """
        :param frontDoor: FrontDoor object.
        :param presstime: time in ms.

        - Common function to press power key on Eddie to Mute.
        """
        self.power_key(frontDoor._ip_address, presstime)

        #Give a second to mute volume
        time.sleep(1)

        response = frontDoor.getVolume()
        muted = response['body']['muted']

        if not muted:
            return False
        return True

    def aux_unmute(self, frontDoor, presstime):
        """
        :param frontDoor: FrontDoor object.
        :param presstime: time in ms.

        - Common function to press power key on Eddie to UnMute.
        """
        self.power_key(frontDoor._ip_address, presstime)

        #Give a second to mute volume
        time.sleep(1)

        response = frontDoor.getVolume()
        unmuted = response['body']['muted']

        if unmuted:
            return False
        return True
