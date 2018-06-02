# eec_helper.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
import re
import logging

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

logger = get_logger(__name__, "eec_helper.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)

def lpm_aux_button_press(lpm_serial_client, adb):
    """
    Helper function for AUX key press through LPM serial
    param: lpm_serial_client: Generates an LPM Serial Client using the supplied port
    param: adb - gives adb instance
    """
    lpm_serial_client.button_tap(2, 100)
    # Check if AUX button is pressed
    aux_response = adb.executeCommand('echo getpdo system | nc 0 17000')
    logger.info(aux_response)
    result = re.search('AUX()', aux_response)
    if not result:
        assert False, "AUX Key is not pressed"

def lpm_bluetooth_button_press(lpm_serial_client, adb):
    """
    Helper function for Bluetooth key press through LPM serial
    param: lpm_serial_client: Generates an LPM Serial Client using the supplied port
    param: adb - gives adb instance
    """
    lpm_serial_client.button_tap(1, 100)
    # Check if BLUETOOTH button is pressed
    bluetooth_response = adb.executeCommand('echo getpdo system | nc 0 17000')
    logger.info(bluetooth_response)
    result = re.search('BLUETOOTH()', bluetooth_response)
    if not result:
        assert False, "BLUETOOTH Key is not pressed"
