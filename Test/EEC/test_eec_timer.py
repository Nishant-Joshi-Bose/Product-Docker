# test_eec_timer.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
import logging
import time
import pexpect
import pytest

from config import CONFIG
from CastleTestUtils.LpmUtils.Lpm import Lpm
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils import adb_utils
from eec_helper import lpm_aux_button_press, lpm_bluetooth_button_press
from ..ProductControllerAPI import eddie_helper

logger = get_logger(__name__, "EnergyEfficiencyCompliance.log", level=logging.DEBUG, fileLoglevel=logging.DEBUG)

factory_default_keys = [Keys.AUX.value, Keys.VOLUME_DOWN.value]
network_interface_list_command = CONFIG["network_interfaces_list_cmd"]

# On to Standby timeouts in seconds
standby_timeout = CONFIG["network_standby_timeout"]

# Setup to LPS timeouts in seconds
lps_timeout = CONFIG["low_power_timeout"]


@pytest.mark.usefixtures('device_id', 'set_lps_timeout', 'tap', 'adb', 'lpm_serial_client')
def test_factory_default_low_power(device_id, adb, tap, lpm_serial_client):
    """
    1) Factory Default the system (out of box)
    2) Checks the system go to the 'SetupNetwork' state
    3) After 2 minutes, checks system go to the 'Low Power' state
    4) Wake up the system with multifunction key. Checks the 'SetupNetwork' state
    param: device_id - fixture to get device id
    param: adb - fixture to get adb object
    param: tap - fixture is used to get the pexpect client
    param: set_lps_timeout - This fixture changes "NoAudioTimeout" and "NoNetworkConfiguredTimeout" params
    to test Low power state transition
    param: lpm_serial_client: Generates an LPM Serial Client using the supplied port.
    """
    logger.info("test_factory_default_low_power")
    # Generate factoryDefault action using keypress
    keypress.key_press_only(tap, factory_default_keys, async_response=True)
    # Wait for minimum 11 seconds to launch factoryDefault action.
    time.sleep(11)
    # Wait for device to become online
    adb.waitForRebootDevice()
    # Wait for CLI-Server to start and listens on 17000 port.
    logger.info("Wait for CLI-Server to start and listens on 17000 port")
    status = None
    for _ in range(40):
        status = adb.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status and status.strip() == 'OK':
            break
        time.sleep(1)
    assert status, "CLIServer not started within 40s."
    assert status.strip() == 'OK', 'CLIServer is not stated even after 40 seconds'
    time.sleep(2)

    for _ in range(10):
        if adb.executeCommand("echo '?' | nc 0 17000 | grep 'getproductstate'"):
            break
        time.sleep(1)
    logger.debug("getproductstate command is registered to CLI-Server.")

    logger.info("Wait until system gets out from 'Booting' state.")
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        if device_state != 'Booting':
            logger.debug("Current device state : %s", device_state)
            break
        logger.debug("Current device state : %s", device_state)
        time.sleep(1)

    logger.info("Check for 'wlan1' interface is in the interface list")
    for _ in range(30):
        output = adb.executeCommand(network_interface_list_command)
        network_list = output.strip().replace('\r', '').split('\n')
        if 'wlan1' in network_list:
            logger.info(network_list)
            break
        logger.info(network_list)
        time.sleep(1)
    assert 'wlan1' in network_list, \
    "wlan1 interface is not in the list, system did not go through factory default"

    logger.info("Check for 'SetupNetwork' state")
    device_state = None
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        if device_state == eddie_helper.SETUPNETWORK:
            logger.debug('Got state: %s', device_state)
            break
        time.sleep(1)
    assert device_state == eddie_helper.SETUPNETWORK, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.SETUPNETWORK, device_state)

    logger.info("Check for the Low Power Standby state after 2 minutes")
    start_time = time.time()
    for _ in range(150):
        device_state = lpm_serial_client.get_power_state()
        logger.debug('Got state: %s', device_state)
        if "LowPower" in device_state:
            logger.debug('Got state: %s', device_state)
            break
        time.sleep(1)
    end_time = time.time()
    duration = end_time - start_time
    logger.debug("Total time took for the system to get into %s is %s(s)", device_state, duration)
    assert duration < lps_timeout, "System did not go into Low Power state on time"

    # Make sure APQ completely shuts down
    time.sleep(30)
    # Simulate a key press to wake it up.
    logger.info("Simulating a MFB key press.")
    lpm_serial_client.button_tap(4, 250)

    # Verify we are back in standby state.
    logger.debug("Waiting 60s for power state to be {} or {}"
                 .format(Lpm.PowerState.Standby, Lpm.PowerState.On))
    logger.debug("Current Power State: {}".format(lpm_serial_client.get_power_state()))
    assert lpm_serial_client.wait_for_power_state([Lpm.PowerState.Standby, Lpm.PowerState.On], 60), \
        "Failed to resume into Standby system state."

    # Check for SetupNetwork' state
    device_state = None
    for _ in range(10):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.SETUPNETWORK:
            break
    assert device_state == eddie_helper.SETUPNETWORK, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.SETUPNETWORK, device_state)


@pytest.mark.usefixtures('device_id', 'set_lps_timeout', 'adb', 'lpm_serial_client')
def test_aux_network_standby(device_id, lpm_serial_client, adb):
    """
    1) Press AUX Key to enter into AUX mode
    2) Check if the AUX key is pressed
    3) Check system is in "PlayingSelectedSilent" state
    4) After 1 min, Check the system goes to the 'NetworkNotConfigured' state
    5) Interrupt the transition to low power standby by aux key press
    6) Check if the system is in 'PlayingSelectedSilent' state
    param: device_id - fixture to get device id
    param: set_lps_timeout - fixture changes "NoAudioTimeout" and "NoNetworkConfiguredTimeout" params to test Low power state transition
    param: adb - fixture to get adb object
    param: lpm_serial_client - fixture to get LPM Serial Client connection
    """
    logger.info("test_aux_network_standby")
    # Generate AUX action using lpm client button tap
    lpm_aux_button_press(lpm_serial_client, adb)

    # Check for product state which should be 'PlayingSelectedSilent' state
    device_state = None
    for _ in range(5):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_SILENT:
            break
    assert device_state == eddie_helper.STATE_SILENT, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_SILENT, device_state)

    # Check for "NetworkNotConfigured" state after 1 minute
    start_time = time.time()
    for _ in range(60):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_NW_NOT_CONFIGURED:
            break
    end_time = time.time()
    duration = end_time - start_time
    logger.debug("Total time took for the system to get into %s is %s(s)", device_state, duration)
    assert duration < standby_timeout, "System did not go into NetworkNotConfigured state on time"

    # Interrupt the transition to 'LowPowerStandby'
    lpm_aux_button_press(lpm_serial_client, adb)

    # Check for PlayingSelectedSilent' state
    device_state = None
    for _ in range(10):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_SILENT:
            break
    assert device_state == eddie_helper.STATE_SILENT, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_SILENT, device_state)


@pytest.mark.usefixtures('device_id', 'set_lps_timeout', 'adb', 'lpm_serial_client')
def test_bluetooth_network_standby(device_id, lpm_serial_client, adb):
    """
    1) Press BLUETOOTH Key to enter into BLUETOOTH mode
    2) Check if the BLUETOOTH key is pressed
    3) Check system is in "PlayingSelectedSilent" state
    4) After 1 min, Checks the system goes to the 'NetworkNotConfigured' state
    5) Interrupt the transition to low power standby by BLUETOOTH key press
    6) Check if the system is in 'PlayingSelectedSilent' state
    param: device_id - fixture to get device id
    param: set_lps_timeout - fixture changes "NoAudioTimeout" and "NoNetworkConfiguredTimeout" params to test Low power state transition
    param: adb - fixture to get adb object
    param: lpm_serial_client - fixture to get LPM Serial Client connection
    """
    logger.info("test_bluetooth_network_standby")
    # Generate BLUETOOTH action using lpm client button tap
    lpm_bluetooth_button_press(lpm_serial_client, adb)

    # Check for product state which should be 'PlayingSelectedSilent' state
    device_state = None
    for _ in range(10):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_SILENT:
            break
    assert device_state == eddie_helper.STATE_SILENT, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_SILENT, device_state)

    # Check for "NetworkNotConfigured" state after 1 minute
    start_time = time.time()
    for _ in range(60):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_NW_NOT_CONFIGURED:
            break
    assert eddie_helper.STATE_NW_NOT_CONFIGURED in device_state, "Device is not in the 'NetworkNotConfigured' state"
    end_time = time.time()
    duration = end_time - start_time
    logger.debug("Total time took for the system to get into %s is %s(s)", device_state, duration)
    assert duration < standby_timeout, "System did not go into NetworkNotConfigured state on time"

    # Interrupt the transition to 'LowPowerStandby'
    lpm_bluetooth_button_press(lpm_serial_client, adb)

    # Check for PlayingSelectedSilent' state
    device_state = None
    for _ in range(10):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_SILENT:
            break
    assert device_state == eddie_helper.STATE_SILENT, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_SILENT, device_state)


@pytest.mark.usefixtures('device_id', 'set_lps_timeout', 'adb', 'lpm_serial_client')
def test_aux_network_standby_low_power(device_id, lpm_serial_client, adb):
    """
    1) Press AUX Key to enter into AUX mode
    2) Check if the AUX key is pressed
    3) Check system is in "PlayingSelectedSilent" state
    4) After 1 min, Checks the system goes to the 'NetworkNotConfigured' state
    5) After 2 minutes, Checks the 'Low Power' state
    6) Wake up the system with multifunction key. Checks the 'NetworkNotConfigured' state
    param: device_id - fixture to get device id
    param: set_lps_timeout - fixture changes "NoAudioTimeout" and "NoNetworkConfiguredTimeout" params to test Low power state transition
    param: adb - fixture to get adb object
    param: lpm_serial_client - fixture to get LPM Serial Client connection
    """
    logger.info("test_aux_network_standby_low_power")
    # Generate AUX action using lpm client button tap
    lpm_aux_button_press(lpm_serial_client, adb)

    # Check for product state which should be 'PlayingSelectedSilent' state
    device_state = None
    for _ in range(5):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_SILENT:
            break
    assert device_state == eddie_helper.STATE_SILENT, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_SILENT, device_state)

    # Check for "NetworkNotConfigured" state after 1 minute
    start_time = time.time()
    for _ in range(60):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_NW_NOT_CONFIGURED:
            break
    end_time = time.time()
    duration = end_time - start_time
    logger.debug("Total time took for the system to get into %s is %s(s)", device_state, duration)
    assert duration < standby_timeout, "System did not go into NetworkNotConfigured state on time"

    # Check for the "Low Power Standby" state after 2 minutes
    logger.info("Check for the Low Power Standby state after 2 minutes")
    start_time = time.time()
    for _ in range(150):
        device_state = lpm_serial_client.get_power_state()
        logger.debug('Got state: %s', device_state)
        if "LowPower" in device_state:
            logger.debug('Got state: %s', device_state)
            break
        time.sleep(1)
    end_time = time.time()
    duration = end_time - start_time
    logger.debug("Total time took for the system to get into %s is %s(s)", device_state, duration)
    assert duration < lps_timeout, "System did not go into Low Power state on time"

    # Make sure APQ completely shuts down
    time.sleep(30)
    # Simulate a key press to wake it up.
    logger.info("Simulating a MFB key press.")
    lpm_serial_client.button_tap(4, 250)

    # Verify we are back in standby state.
    logger.debug("Waiting 60s for power state to be {} or {}"
                 .format(Lpm.PowerState.Standby, Lpm.PowerState.On))
    logger.debug("Current Power State: {}".format(lpm_serial_client.get_power_state()))
    assert lpm_serial_client.wait_for_power_state([Lpm.PowerState.Standby, Lpm.PowerState.On], 60), \
        "Failed to resume into Standby system state."

    # Check for NetworkNotConfigured' state
    device_state = None
    for _ in range(10):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_NW_NOT_CONFIGURED:
            break
    assert device_state == eddie_helper.STATE_NW_NOT_CONFIGURED, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_NW_NOT_CONFIGURED, device_state)


@pytest.mark.usefixtures('add_wifi_at_end', 'device_id', 'set_lps_timeout', 'adb', 'lpm_serial_client')
def test_bluetooth_network_standby_low_power(device_id, lpm_serial_client, adb):
    """
    1) Press BLUETOOTH Key to enter into BLUETOOTH mode
    2) Check if the BLUETOOTH key is pressed
    3) Check system is in "PlayingSelectedSilent" state
    4) After 1 min, Checks the system goes to the 'NetworkNotConfigured' state
    5) After 2 minutes, Checks the 'Low Power' state
    6) Wake up the system with multifunction key. Checks the 'NetworkNotConfigured' state
    param: device_id - fixture to get device id
    param: set_lps_timeout - fixture changes "NoAudioTimeout" and "NoNetworkConfiguredTimeout" params to test Low power state transition
    param: adb - fixture to get adb object
    param: riviera - fixture to get riviera object
    param: lpm_serial_client - fixture to get LPM Serial Client connection
    """
    logger.info("test_bluetooth_network_standby_low_power")
    # Generate BLUETOOTH action using lpm client button tap
    lpm_bluetooth_button_press(lpm_serial_client, adb)

    # Check for product state which should be 'PlayingSelectedSilent' state
    device_state = None
    for _ in range(5):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_SILENT:
            break
    assert device_state == eddie_helper.STATE_SILENT, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_SILENT, device_state)

    # Check for "NetworkNotConfigured" state after 1 minute
    start_time = time.time()
    for _ in range(60):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_NW_NOT_CONFIGURED:
            break
    end_time = time.time()
    duration = end_time - start_time
    logger.debug("Total time took for the system to get into %s is %s(s)", device_state, duration)
    assert duration < standby_timeout, "System did not go into NetworkNotConfigured state on time"

    # Check for the "Low Power Standby" state after 2 minutes
    logger.info("Check for the Low Power Standby state after 2 minutes")
    start_time = time.time()
    for _ in range(150):
        device_state = lpm_serial_client.get_power_state()
        logger.debug('Got state: %s', device_state)
        if "LowPower" in device_state:
            logger.debug('Got state: %s', device_state)
            break
        time.sleep(1)
    end_time = time.time()
    duration = end_time - start_time
    logger.debug("Total time took for the system to get into %s is %s(s)", device_state, duration)
    assert duration < lps_timeout, "System did not go into Low Power state on time"

    # Make sure APQ completely shuts down
    time.sleep(30)
    # Simulate a key press to wake it up.
    logger.info("Simulating a MFB key press.")
    lpm_serial_client.button_tap(4, 250)

    # Verify we are back in standby state.
    logger.debug("Waiting 60s for power state to be {} or {}" \
               .format(Lpm.PowerState.Standby, Lpm.PowerState.On))
    logger.debug("Current Power State: {}".format(lpm_serial_client.get_power_state()))
    assert lpm_serial_client.wait_for_power_state([Lpm.PowerState.Standby, Lpm.PowerState.On], 60), \
        "Failed to resume into Standby system state."

    # Check for NetworkNotConfigured' state
    device_state = None
    for _ in range(10):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        logger.debug('Got state: %s', device_state)
        if device_state == eddie_helper.STATE_NW_NOT_CONFIGURED:
            break
    assert device_state == eddie_helper.STATE_NW_NOT_CONFIGURED, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.STATE_NW_NOT_CONFIGURED, device_state)
