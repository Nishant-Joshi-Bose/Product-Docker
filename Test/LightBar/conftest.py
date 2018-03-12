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
from time import sleep
import pytest
from common_exception import SerialPortError
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LoggerUtils.BSerialLogger import SerialLoggingUtil, TestRecoveryTest

LOGGER = get_logger(__file__)

TIMEOUT = 5


@pytest.mark.usesfixtures("frontDoor")
@pytest.fixture()
def animation_list(frontDoor):
    """
    Getting the animation pattern list
    :param frontDoor : fixture returning frontDoor instance
    :return animation_values : supported animation values
    """
    sleep(TIMEOUT)
    lightbar_response = frontDoor.getActiveAnimation()
    LOGGER.debug("Lightbar response is %s", lightbar_response)
    animation_values = lightbar_response["properties"]["supportedValues"]
    return animation_values


@pytest.fixture()
def serial_handler(request):
    """
    This fixture is used to start the serial logs of device an stop the serial log at the end.
    """
    lpm_port = request.config.getoption("--lpm-port")
    comports = TestRecoveryTest()
    if lpm_port not in comports.getCOMPortList():
        raise SerialPortError("Please enter valid port")
    apq_handle = SerialLoggingUtil('LPM', lpm_port)
    yield apq_handle

    # stopping the APQ serial log thread.
    apq_handle.comporthandle.close()
    apq_handle.filehandle.close()
