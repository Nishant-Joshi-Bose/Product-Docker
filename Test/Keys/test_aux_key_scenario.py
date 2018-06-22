# test_aux_key_scenario.py
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
Automated Test for AUX key related CLI key events.
"""
from time import sleep
import pytest

import cli_key_helper

from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from CastleTestUtils.RivieraUtils import device_utils, adb_utils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys.keypress import press_key, key_press_only, key_release_only
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys

LOGGER = get_logger(__file__)
FACTORY_TIMEOUT = 10
AUX_SOURCE = 'PRODUCT(AUX)'


@pytest.mark.usefixtures("request", "adb", "wifi_config")
@pytest.mark.parametrize("device_state", ['SETUP', 'ON', 'OFF'])
def test_factory_reset_cancel(request, device_state, wifi_config, device_id):
    """
    Cancel the factory reset countdown by releasing AUX or VOLUME DOWN key
    and validate the device state.

    Test Steps:
    1. Change the device state ('SETUP', 'ON', 'OFF').
    2. Change to AUX source.
    3. Start the factory reset countdown by
        pressing the KEY (AUX + VOLUME_DOWN).
    4. Cancel the factory reset countdown by releasing AUX key.
    5. Validate the device state.

    :param request: pytest request object
    :param device_state: state of device["SETUP", "ON", "OFF"].
    :param wifi_config: Fixture to get wifi configurations
    :param device_id: Fixture to get device id from command line
    """
    LOGGER.info("Performing factory reset cancel scenario while device is in %s state", device_state)
    interface = request.config.getoption("--network-iface")

    # change the device state
    device_utils.change_current_device_state(device_state, device_id)

    # change to aux source
    riviera_utils = RivieraUtils('ADB', device=device_id)
    device_utils.set_device_source('AUX', device_id)

    if device_state == "SETUP" and interface == "wlan0":
        cli_key_helper.get_network_ipaddr(request, wifi_config, device_id, interface)

    # press the AUX + VOLUME_DOWN key to start factory reset countdown
    input_keys = [Keys.AUX.value, Keys.VOLUME_DOWN.value]
    LOGGER.debug("Pressing the key %s", input_keys)
    pexpect_client = adb_utils.adb_telnet_tap(device_id)
    key_press_only(pexpect_client, input_keys, True)

    # duration to hold after key
    timeout = FACTORY_TIMEOUT / 2
    sleep(timeout)
    LOGGER.debug("Factory reset countdown started and releasing the AUX and VOLUME_DOWN key for cancel the countdown")

    # Release the key AUX and VOLUME_DOWN to cancel factory reset countdown
    key_release_only(pexpect_client, Keys.AUX.value, True)
    key_release_only(pexpect_client, Keys.VOLUME_DOWN.value, True)

    # verify the device status until factory_timeout seconds
    for _ in range(FACTORY_TIMEOUT):
        LOGGER.debug("Device available status: %s", riviera_utils.communication.isDeviceAvailable())

    LOGGER.debug("Factory reset cancelled")
    pexpect_client.close()
    current_source, current_state = riviera_utils.current_source_name_and_status(
        device_id)
    LOGGER.debug("After cancelling the factory reset, Source is %s State is %s", current_source, current_state)

    assert current_source == AUX_SOURCE, \
        "Did not cancel factory reset. Device source: {}, Device state: {}".format(current_source, current_state)

@pytest.mark.usefixtures("request", "wifi_config")
@pytest.mark.parametrize("device_state", ["SETUP", "ON", "OFF"])
def test_aux_key_release(request, device_state, wifi_config, device_id):
    """
    Press the AUX key and validate.

    Test Steps:-
    1. Change the device state ('SETUP', 'ON', 'OFF').
    2. Change to Bluetooth source.
    3. Press the AUX key and validate device state.

    :param request: pytest request object
    :param device_state: state of device["SETUP", "ON", "OFF"].
    :param wifi_config: Fixture to get wifi configurations
    :param device_id: Fixture to get device id from command line
    """
    LOGGER.info("Performing the Aux release while device is in %s state", device_state)
    interface = request.config.getoption("--network-iface")

    # change the device state
    device_utils.change_current_device_state(device_state, device_id)

    # change to bluetooth source
    riviera_utils = RivieraUtils('ADB', device=device_id)
    device_utils.set_device_source('BLUETOOTH', device_id)

    if device_state == "SETUP" and interface == "wlan0":
        cli_key_helper.get_network_ipaddr(request, wifi_config, device_id, interface)

    # press the AUX key
    input_key = [Keys.AUX.value]
    LOGGER.debug("Pressing the key %s", input_key)
    pexpect_client = adb_utils.adb_telnet_tap(device_id)
    press_key(pexpect_client, input_key, 0, True)

    # validate the device state
    current_source, current_state = riviera_utils.current_source_name_and_status(
        device_id)
    LOGGER.debug("After presssing AUX key, current source is %s and current state is %s", current_source, current_state)
    pexpect_client.close()

    assert current_source == AUX_SOURCE, \
        "Device did not factory reset cancel properly and current device source is {}".format(current_source)

@pytest.mark.usefixtures("request", "adb", "wifi_config")
@pytest.mark.parametrize("device_state", ["SETUP", "ON", "OFF"])
def test_factory_reset(request, device_state, wifi_config, device_id):
    """
    Press the AUX key and VOLUME DOWN for factory reset
    and validate the device state.

    Test Steps:-
    1. Change the device state["SETUP", "ON", "OFF"].
    2. Change to AUX source.
    3. Press AUX and VOLUME DOWN for factory reset
    4. Validate the device state after factory reset.

    :param request: pytest request object
    :param device_state: state of device["SETUP", "ON", "OFF"].
    :param wifi_config: Fixture to get wifi configurations
    :param device_id: Fixture to get device id from command line
    """
    LOGGER.info("Performing the factory reset while device is in %s state", device_state)
    reboot_flag = False
    interface = request.config.getoption("--network-iface")

    # change the device state
    device_utils.change_current_device_state(device_state, device_id)

    # change to aux source
    riviera_utils = RivieraUtils('ADB', device=device_id)
    device_utils.set_device_source('AUX', device_id)

    # press and hold AUX and VOLUME_DOWN key for factory reset
    input_keys = [Keys.AUX.value, Keys.VOLUME_DOWN.value]
    LOGGER.debug("Pressing the key %s", input_keys)
    pexpect_client = adb_utils.adb_telnet_tap(device_id)
    key_press_only(pexpect_client, input_keys, True)

    # duration to hold after key press
    sleep(FACTORY_TIMEOUT)

    # wait for device to reboot after factory reset
    for _ in range(device_utils.TIMEOUT):
        if not riviera_utils.communication.isDeviceAvailable():
            LOGGER.debug("Waiting for the device to reboot")
            reboot_flag = True
            break
        sleep(1)
    riviera_utils.communication.waitForRebootDevice()

    LOGGER.debug("Device reboot")
    assert reboot_flag, \
        "Device did not factory reset after countdown. Device available status: {}".format(
            riviera_utils.communication.isDeviceAvailable())

    # wait for telnet service to get start
    LOGGER.debug("Waiting for telnet service")
    sleep(device_utils.TIMEOUT)

    # Get the current source and current state from telnet logs
    # after factory reset
    current_source, current_state = riviera_utils.current_source_name_and_status(device_id)
    pexpect_client.close()
    LOGGER.debug("After Factory reset, Source: %s; State: %s", current_source, current_state)

    # restore the network after factory reset
    interface = request.config.getoption("--network-iface")
    if interface == "wlan0":
        cli_key_helper.get_network_ipaddr(request, wifi_config, device_id, interface)

    assert current_source in cli_key_helper.FACTORY_RESET_SOURCE, \
        "Device did not factory reset. Current status: {}".format(current_source)
