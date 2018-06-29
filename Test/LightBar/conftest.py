# conftest.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
"""
conftest.py for Lightbar tests
"""
import glob
import os
from time import sleep

import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LoggerUtils.BSerialLogger import SerialLoggingUtil, TestRecoveryTest

LOGGER = get_logger(__file__)


@pytest.mark.usesfixtures("frontdoor_wlan")
@pytest.fixture()
def animation_list(frontdoor_wlan):
    """
    Getting the animation pattern list
    :param frontdoor_wlan: fixture returning frontDoor instance
    :return animation_values: supported animation values
    """
    sleep(5)
    lightbar_response = frontdoor_wlan.getActiveAnimation()
    animation_values = lightbar_response["properties"]["supportedValues"]
    LOGGER.debug("Animation Database Values: %s", animation_values)
    return animation_values


@pytest.fixture()
def serial_handler(request):
    """
    Start the serial logs of device an stop the serial log at the end.
    """
    lpm_port = request.config.getoption("--lpm-port")

    comports = TestRecoveryTest()
    if lpm_port not in comports.getCOMPortList():
        pytest.fail("Please enter valid LPM serial port.")

    module_name = 'LPM'
    lpm_handle = SerialLoggingUtil(module_name, lpm_port)

    yield lpm_handle

    # Stop the LPM serial log thread.
    lpm_handle.comporthandle.close()
    lpm_handle.filehandle.close()

    # Remove Created files
    pattern = '{}_*.txt'.format(module_name)
    for file_name in glob.glob(os.path.join('.', pattern)):
        os.remove(file_name)
