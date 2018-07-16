# test_timezone.py
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
Automated Test to set and verify different timezones for Eddie
"""
import pytz
import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils import adb_utils

LOGGER = get_logger(__file__)

GET_TIMEZONE = "clock zone info get"
SET_TIMEZONE = "clock zone info set "
SYSTEM_INFO_FILE = "The System Time Zone Info file is " + '"/usr/share/zoneinfo/$timezone"'
NV_RAM_INFO_FILE = "The NV-RAM Time Zone Info file is " + '"/usr/share/zoneinfo/$timezone"'
SET_TIMEZONE_RESPONSE = "Setting the Clock Display Time Zone Info to " + '"$timezone"'

UNSUPPORTED_ZONES =  ['America/Punta_Arenas', 'Asia/Atyrau', 'Europe/Saratov']


def pytest_generate_tests(metafunc):
    """
    This fixture could be used to dynamically parametrise the test with data.
    :param metafunc: Object that help to inspect a test function
    :return: None
    """
    # get all timezones
    timezones = pytz.all_timezones
    LOGGER.debug("Timezones are \n %s", timezones)
    metafunc.parametrize("timezone", timezones)


@pytest.mark.usefixtures("device_id")
def test_set_timezone(device_id, timezone):
    """
    Set and verify different timezones on Eddie

    Test Steps:
    1 Set the timezone and verify its output
    2 Verify the timezone is set through get timezone command
    3 Verify system info and nvram files for timezone in get timezone command

    :param device_id: ADB Device ID of target system
    :param timezone: parameterize timezone value of all timezones
    """
    if timezone in UNSUPPORTED_ZONES:
        pytest.skip("{} is not supported on Riviera Linux OS".format(timezone))

    LOGGER.debug("Timezone to set for Eddie is %s", timezone)

    # execute set timezone command on telnet
    command = SET_TIMEZONE + timezone
    command_output = adb_utils.adb_telnet_cmd(command, async_response=True, device_id=device_id)
    LOGGER.debug("Set timezone command output is %s", command_output)

    # verify timezone is set
    timezone_response = SET_TIMEZONE_RESPONSE.replace('$timezone', timezone)
    LOGGER.debug("Timezone Response: %s", timezone_response)
    assert timezone_response in command_output, "timezone {0} not set for Eddie, result is "\
        "{1}".format(timezone, command_output)

    # verify same timezone is set using get timezone command
    output = adb_utils.adb_telnet_cmd(GET_TIMEZONE, async_response=True, device_id=device_id)
    LOGGER.debug("Get timezone command output is %s", output)

    # verify system info and nvram file in output of get command
    system_file = SYSTEM_INFO_FILE.replace('$timezone', timezone)
    nvram_file = NV_RAM_INFO_FILE.replace('$timezone', timezone)
    assert system_file in output, "timezone {0} not set for Eddie, result is {1}".format(
        timezone, output)
    assert nvram_file in output, "timezone {0} not set for Eddie, result is {1}".format(
        timezone, output)
