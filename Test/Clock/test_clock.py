# test_clock.py
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
Automated Test to verify different clock scenarios for Eddie.
"""

import re
import datetime
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import adb_utils

LOGGER = get_logger(__file__)

# buffer seconds for utc time verification
BUFFER_SECS = 5

# Clock commands
GET_CLOCK_TIME = "clock time"
CLOCK_BRIGHTNESS_GET = "clock default bright get"
CLOCK_BRIGHTNESS_SET = "clock default bright set "

# Expected strings of clock commands
UTC_TIME = "UTC   time:"
GET_BRIGHTNESS_STRING = "The Clock Display default bright selection is "
SET_BRIGHTNESS_STRING = "Setting the Clock Display default bright selection to "
INVALID_BRIGHTNESS_STRING = "The Clock Display default bright selection is "\
                            "out of range: (0% through 100%)"

# Brightness values
DEFAULT_BRIGHTNESS = [0, 0.5, 10, 25, 50, 70, 85.5, 95, 100]
INVALID_BRIGHTNESS = [-1, -101, 101, 200, 1000]


@pytest.mark.usefixtures("device_id")
def test_utc_time(device_id):
    """
    Verify utc time on clock of Eddie matches system utc time

    Test Steps:
    1 Get the utc time from datetime and convert it into seconds
    2 Get the clock time from eddie and convert it into seconds
    3 Verify both the time matches

    :param device_id: device id of product
    :return: None
    """
    LOGGER.info("Verification of clock UTC time")
    # Get current utc time from datetime module
    utc_time = datetime.datetime.utcnow().strftime('%H:%M:%S')

    # Convert utc time into seconds
    hours, minutes, secs = re.split(':', utc_time)
    utc_sec = int(datetime.timedelta(hours=int(hours), minutes=int(minutes),
                                     seconds=int(secs)).total_seconds())
    LOGGER.debug("Current utc time: %s and utc time in seconds: %s ", utc_time, utc_sec)

    # Get clock utc time
    clock_utc = adb_utils.adb_telnet_cmd(GET_CLOCK_TIME, async_response=True, device_id=device_id,
                                         expect_after=UTC_TIME)
    # Convert clock utc time into seconds
    hours, minutes, secs = re.split(':', clock_utc)
    clock_utc_sec = int(datetime.timedelta(hours=int(hours), minutes=int(minutes),
                                           seconds=int(secs)).total_seconds())
    LOGGER.debug("Clock utc time: %s and utc time in seconds: %s ", clock_utc, clock_utc_sec)

    # Verify utc time and clock time
    assert utc_sec + BUFFER_SECS >= clock_utc_sec or utc_sec - BUFFER_SECS <= clock_utc_sec,\
        "Current utc {0} and clock utc {1} time does not match".format(utc_sec, clock_utc_sec)


@pytest.mark.usefixtures("device_id")
@pytest.mark.parametrize("brightness", DEFAULT_BRIGHTNESS)
def test_clock_brightness(device_id, brightness):
    """
    Verify that default clock brightness is set for Eddie

    Test Steps:
    1 Set the default brightness and verify the output
    2 Get the clock brightness and verify valid brightness is set

    :param device_id: device id of product
    :param brightness: parameterize value of brightness
    :return: None
    """
    LOGGER.info("Verify clock default brightness for value: %f", brightness)

    # Command to set clock default brightness
    command = CLOCK_BRIGHTNESS_SET + str(brightness)
    LOGGER.debug("Setting the clock brightness to %f", brightness)
    # Set the clock brightness
    clock_brightness = adb_utils.adb_telnet_cmd(command, async_response=True, device_id=device_id,
                                                expect_after=SET_BRIGHTNESS_STRING)

    # Verify clock brightness is set
    assert int(clock_brightness) == int(brightness), "Default brightness not set, "\
        "brightness is {}".format(clock_brightness)

    # Get the clock brightness
    clock_brightness = adb_utils.adb_telnet_cmd(CLOCK_BRIGHTNESS_GET, async_response=True,
                                                device_id=device_id,
                                                expect_after=GET_BRIGHTNESS_STRING)
    LOGGER.debug("Brightness after valid brightness set is %s", clock_brightness)

    # Verify the clock brightness
    assert int(clock_brightness) == int(brightness), "Not able to get the expected brightness, "\
        "brightness is {}".format(clock_brightness)


@pytest.mark.usefixtures("device_id")
@pytest.mark.parametrize("invalid_brightness", INVALID_BRIGHTNESS)
def test_invalid_brightness(device_id, invalid_brightness):
    """
    Verify that invalid clock brightness is not set on Eddie

    Test Steps:
    1 Get the default brightness before setting invalid brightness
    2 Set the invalid brightness
    2 Get the clock brightness again and verify valid brightness is set

    :param device_id: device id of product
    :param invalid_brightness: parameterize value of invalid brightness
    :return: None
    """
    LOGGER.info("Verify clock default brightness for invalid value: %d", invalid_brightness)

    # Get the clock brightness
    brightness = adb_utils.adb_telnet_cmd(CLOCK_BRIGHTNESS_GET, async_response=True,
                                          device_id=device_id,
                                          expect_after=GET_BRIGHTNESS_STRING)
    LOGGER.debug("Clock brightness before setting invalid brightness is %s", brightness)

    # Command to set clock default brightness
    command = CLOCK_BRIGHTNESS_SET + str(invalid_brightness)
    LOGGER.debug("Setting invalid clock brightness: %d", invalid_brightness)
    # Set the clock brightness
    clock_brightness = adb_utils.adb_telnet_cmd(command, async_response=True,
                                                device_id=device_id, expect_after=command)

    # Verify clock brightness is not set
    assert INVALID_BRIGHTNESS_STRING in clock_brightness, "Invalid brightness is set on clock, "\
        "brightness is {}".format(clock_brightness)

    # Get the clock brightness
    LOGGER.debug("Brightness after invalid brightness set is %s", clock_brightness)
    clock_brightness = adb_utils.adb_telnet_cmd(CLOCK_BRIGHTNESS_GET, async_response=True,
                                                device_id=device_id,
                                                expect_after=GET_BRIGHTNESS_STRING)

    # Verify the clock brightness at present is same as before setting invalid brightness
    assert int(clock_brightness) != invalid_brightness, "Invalid brightness is set on clock "\
        "brightness is {}".format(clock_brightness)
    assert brightness == clock_brightness, "Brightness is invalid, current brightness is "\
        "{}".format(clock_brightness)
