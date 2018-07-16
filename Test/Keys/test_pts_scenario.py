# test_pts_scenario.py
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
Automated Test for pts mode scenario.
"""
from time import sleep
import re
import pytest

from CastleTestUtils.RivieraUtils import device_utils, adb_utils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys.keypress import key_press_only, key_release_only
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys

LOGGER = get_logger(__file__)

# constants for pts scenario
RNDIS_INTERFACE = "rndis0"
PTS_INTERFACE = "rndis1"
MAKE_RNDIS_FILE_CMD = "touch /mnt/nv/force-rndis"
REMOVE_RNDIS_FILE_CMD = "rm -rf /mnt/nv/force-rndis"
PING_SUCCESS = 1

# timeout constants
TIMEOUT = 2
REBOOT_TIMEOUT = 10
PTS_TIMEOUT = 5.2


@pytest.mark.usefixtures("reboot_device_at_end", "adb", "device_id")
@pytest.mark.parametrize("device_source", ["AUX", "BLUETOOTH"])
def test_pts_mode_device_source(adb, device_source, device_id):
    """
    This test will press key for system info(pts) mode and verify ethernet IP.

    Test Steps:
    1. Go to device sources, get the available interfaces from adb
       and press system info key combo
    2. Get the interface of system info from apq port and ping its IP address
    3. Reboot the device to get back to normal mode from system info mode

    :param device_id: device id of product
    :param adb: fixture returning adb communication instance
    :param device_source: source of device(AUX, BLUETOOTH)
    :return: None
    """

    LOGGER.info("Performing the pts mode scenario execution while device is in %s source",
                device_source)

    # change the device source
    device_utils.set_device_source(device_source, device_id)

    # get network interfaces
    interfaces = get_network_interfaces(adb)

    # press the VOLUME_UP + VOLUME_DOWN key for pts mode
    input_keys = [Keys.VOLUME_UP.value, Keys.VOLUME_DOWN.value]
    LOGGER.debug("Pressing the keys %s", input_keys)
    pexpect_client = adb_utils.adb_telnet_tap(device_id)
    key_press_only(pexpect_client, input_keys, True)

    # duration to hold after key press
    LOGGER.debug("Switching device to pts mode")
    sleep(PTS_TIMEOUT)

    # Release the key VOLUME_UP and VOLUME_DOWN for pts mode
    key_release_only(pexpect_client, Keys.VOLUME_UP.value, True)
    key_release_only(pexpect_client, Keys.VOLUME_DOWN.value, True)

    # verify system info mode
    ping_status = verify_system_info_mode(interfaces, adb)

    # close pexpect client
    pexpect_client.close()

    # Remove rndis file
    adb.executeCommand(REMOVE_RNDIS_FILE_CMD)

    # verify ip address of pts mode by ping status
    assert ping_status == PING_SUCCESS,\
        "Not able to ping PTS mode IP address and ping status: {0}".format(ping_status)


@pytest.mark.usefixtures("reboot_device_at_end", "adb", "device_id")
@pytest.mark.parametrize("device_state", ["ON", "OFF", "SETUP"])
def test_pts_mode_device_state(adb, device_state, device_id):
    """
    This test will press key for system info(pts) mode and verify ethernet IP.

    Test Steps:
    1. Go to the device state, get the available interfaces from adb
       and press system info key combo
    2. Get the interface of system info from apq port and ping its IP address
    3. Reboot the device to get back to normal mode from system info mode

    :param adb: fixture returning adb communication instance
    :param device_state: state of device(ON, OFF, SETUP)
    :param device_id: Fixture to get device id from command line
    :return: None
    """

    LOGGER.info("Performing the pts mode scenario execution while device is in %s state",
                device_state)

    # change the device state
    device_utils.change_current_device_state(device_state, device_id)

    # get network interfaces
    interfaces = get_network_interfaces(adb)

    # press the VOLUME_UP + VOLUME_DOWN key for pts mode
    input_keys = [Keys.VOLUME_UP.value, Keys.VOLUME_DOWN.value]
    LOGGER.debug("Pressing the keys %s", input_keys)
    pexpect_client = adb_utils.adb_telnet_tap(device_id)
    key_press_only(pexpect_client, input_keys, True)

    # duration to hold after key press
    LOGGER.debug("Switching device to pts mode")
    sleep(PTS_TIMEOUT)

    # Release the key VOLUME_UP and VOLUME_DOWN for pts mode
    key_release_only(pexpect_client, Keys.VOLUME_UP.value, True)
    key_release_only(pexpect_client, Keys.VOLUME_DOWN.value, True)

    # verify system info mode
    ping_status = verify_system_info_mode(interfaces, adb)

    # close pexpect client
    pexpect_client.close()

    # Remove rndis file
    adb.executeCommand(REMOVE_RNDIS_FILE_CMD)

    # verify ip address of pts mode by ping status
    assert ping_status == PING_SUCCESS, \
        "Not able to ping PTS mode IP address and ping status: {0}".format(ping_status)


def get_network_interfaces(adb):
    """
    This function verifies the device is in system info (pts) mode.

    :param adb: fixture returning adb communication instance
    :return interfaces: network interfaces of the device
    """
    LOGGER.info("Getting available interfaces")
    interfaces = adb.getAvailableNetworkInterfaces()
    LOGGER.debug("Interfaces are: %s", interfaces)

    # Get rndis interface if not present
    if RNDIS_INTERFACE not in interfaces:
        adb.executeCommand(MAKE_RNDIS_FILE_CMD)
        LOGGER.debug("Rebooting device for rndis0 mode")
        adb.rebootDevice()
        for _ in range(REBOOT_TIMEOUT):
            if adb.isDeviceAvailable():
                break
            else:
                sleep(TIMEOUT)

    # Get available interfaces
    sleep(TIMEOUT)
    interfaces = adb.getAvailableNetworkInterfaces()
    LOGGER.debug("Interfaces before pts mode: %s", interfaces)

    return interfaces


def verify_system_info_mode(interfaces, adb):
    """
    This function verifies the device is in system info (pts) mode.

    :param interfaces: Available local interfaces for pts interface comparison
    :param adb: adb communication instance
    :return ping_status: ping status of pts interface
    """
    LOGGER.info("Device is in system info mode")

    new_interfaces = adb.getAvailableNetworkInterfaces()
    LOGGER.debug("Interfaces after pts mode: %s", new_interfaces)

    # get pts interface
    pts_interface = list(set(new_interfaces).difference(interfaces))
    assert PTS_INTERFACE in pts_interface,\
        "New interface is not created for pts mode and available interfaces: {0}".format(
            new_interfaces)
    LOGGER.debug("Pts interface: %s", pts_interface)

    # get ip address of pts interface
    command_output = adb.executeCommand('ifconfig ' + PTS_INTERFACE)
    LOGGER.debug("Interface details: %s", command_output)

    ip_address = re.search(r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}', command_output).group()
    LOGGER.debug("IP address of Interface of pts mode: %s", ip_address)

    # ping ip address of pts interface
    ping_status, _ = adb.pingCommand(ip_address)

    return ping_status
