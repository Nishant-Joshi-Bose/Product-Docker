# coding=utf-8
# test_network_disabled.py
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
PyTest based tests for the Eddie network disabled feature.
"""
import time

import pytest
import xmltodict

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.RivieraUtils import adb_utils
from CastleTestUtils.LpmUtils.Lpm import Lpm


LOGGER = get_logger(__file__)

NETWORK_DISABLED_KEYS = [Keys.BLUETOOTH.value, Keys.MULTIFUNCTION.value]
WIFI_STATE_CONNECTED = 'WIFI_STATION_CONNECTED'
WIFI_STATE_DISCONNECTED = 'WIFI_STATION_DISCONNECTED'
STATE_SILENT = 'PlayingSelectedSilent'
STATE_NOT_SILENT = 'PlayingSelectedNotSilent'
STATE_NW_CONFIGURED = 'NetworkConfigured'
STATE_NW_NOT_CONFIGURED = 'NetworkNotConfigured'
STATE_VC_CONFIGURED = 'VoiceConfigured'
STATE_VC_NOT_CONFIGURED = 'VoiceNotConfigured'
STATE_SETUP_OTHER = 'SetupOther'
STATE_SETUP_NW = 'SetupNetwork'


@pytest.mark.usefixtures('deviceid', 'ip_address_wlan')
def test_network_disabled_success(deviceid, ip_address_wlan):
    """
    Test to determine that network disabled feature works correctly.
    Test Steps:
    1. Device should be connected to wlan.
    2. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    3. IP Address should be allocated to “wlan0” network interface.
    4. Generate network disabled action using CLI Command.
    5. Get current 'WiFiStatus' which should be WIFI_STATE_DISCONNECTED.
    6. IP Address should not be allocated to “wlan0” network interface.
    7. Generate network disabled action using CLI Command.
    8. Get current 'WiFiStatus' for 10 times and stop once get WIFI_STATE_CONNECTED.
    9. IP Address should be allocated to “wlan0” network interface.
    """
    # 1. Device should be connected to wlan.
    LOGGER.debug('Device ip-address is %s', ip_address_wlan)

    tap = adb_utils.adb_telnet_tap(deviceid)
    network_base = NetworkBase(None, device=deviceid, logger=LOGGER)

    # 2. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert resp_dict['WiFiStatus'], 'Response should contain state information.'

    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED),\
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_CONNECTED, resp_dict['WiFiStatus']['@state'])

    # 3. IP Address should be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert ip_address, 'IP Address should be allocated to wlan0'

    # 4. Generate network disabled action using CLI Command.
    keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    # 5. Get current 'WiFiStatus' which should be WIFI_STATE_DISCONNECTED.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert resp_dict['WiFiStatus'], 'Response should contain state information.'
    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_DISCONNECTED),\
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_DISCONNECTED,
                                                              resp_dict['WiFiStatus']['@state'])

    # 6. IP Address should not be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert not ip_address, 'IP Address should not be allocated to wlan0'

    # 7. Generate network disabled action using CLI Command.
    keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    # 8. Get current 'WiFiStatus' for 10 times and stop once get WIFI_STATE_CONNECTED.
    for _ in range(10):
        resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
        resp_dict = xmltodict.parse(resp_xml)
        assert resp_dict['WiFiStatus'], 'Response should contain state information.'
        if resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED:
            break
        LOGGER.debug('Current state:%s', resp_dict['WiFiStatus']['@state'])
        time.sleep(2)

    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED),\
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_CONNECTED, resp_dict['WiFiStatus']['@state'])

    # 9. IP Address should be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert ip_address, 'IP Address should be allocated to wlan0'


@pytest.mark.usefixtures('deviceid', 'ip_address_wlan', 'device_playing_from_amazon')
def test_network_disabled_playing_state(deviceid, ip_address_wlan):
    """
    Test to determine that network disabled feature works correctly while playing from AMAZON MSP.
    Test Steps:
    1. Device should be connected to wlan.
    2. Check for product state which should be STATE_NOT_SILENT.
    3. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    4. IP Address should be allocated to “wlan0” network interface.
    5. Generate network disabled action using CLI Command.
    6. Get current 'WiFiStatus' which should be WIFI_STATE_DISCONNECTED.
    7. IP Address should not be allocated to “wlan0” network interface.
    8. Check for product state which should be STATE_SILENT.
    9. Generate network disabled action using CLI Command.
    10. Get current 'WiFiStatus' for 10 times and stop once get WIFI_STATE_CONNECTED.
    11. IP Address should be allocated to “wlan0” network interface.
    12. Check for product state which should be STATE_SILENT.
    """
    # 1. Device should be connected to wlan.
    LOGGER.debug('Device ip-address is %s', ip_address_wlan)

    tap = adb_utils.adb_telnet_tap(deviceid)
    network_base = NetworkBase(None, device=deviceid, logger=LOGGER)

    # 2. Check for product state which should be STATE_NOT_SILENT.
    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
    assert (device_state == STATE_NOT_SILENT), \
        'Device not in "{}" state. Current state: {}.'.format(STATE_NOT_SILENT, device_state)

    # 3. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert resp_dict['WiFiStatus'], 'Response should contain state information.'

    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED), \
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_CONNECTED, resp_dict['WiFiStatus']['@state'])

    # 4. IP Address should be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert ip_address, 'IP Address should be allocated to wlan0'

    # 5. Generate network disabled action using CLI Command.
    keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    # 6. Get current 'WiFiStatus' which should be WIFI_STATE_DISCONNECTED.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert resp_dict['WiFiStatus'], 'Response should contain state information.'
    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_DISCONNECTED), \
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_DISCONNECTED,
                                                              resp_dict['WiFiStatus']['@state'])

    time.sleep(5)

    # 7. IP Address should not be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert not ip_address, 'IP Address should not be allocated to wlan0'

    # 8. Check for product state which should be STATE_SILENT.
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
        if device_state == STATE_SILENT:
            break
        LOGGER.debug('Current device state : %s', device_state)
        time.sleep(1)

    assert (device_state == STATE_SILENT), \
        'Device not in "{}" state. Current state: {}.'.format(STATE_SILENT, device_state)

    # 9. Generate network disabled action using CLI Command.
    keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    # 10. Get current 'WiFiStatus' for 10 times and stop once get WIFI_STATE_CONNECTED.
    for _ in range(10):
        resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
        resp_dict = xmltodict.parse(resp_xml)
        assert resp_dict['WiFiStatus'], 'Response should contain state information.'
        if resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED:
            break
        LOGGER.debug('Current state : %s', resp_dict['WiFiStatus']['@state'])
        time.sleep(2)

    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED), \
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_CONNECTED, resp_dict['WiFiStatus']['@state'])

    # 11. IP Address should be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert ip_address, 'IP Address should be allocated to wlan0'

    # 12. Check for product state which should be STATE_SILENT.
    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
    assert (device_state == STATE_SILENT), \
        'Device not in "{}" state. Current state: {}.'.format(STATE_SILENT, device_state)


@pytest.mark.usefixtures('deviceid', 'lpm_tap', 'set_lps_timeout', 'ip_address_wlan')
@pytest.mark.parametrize("playable_state", ['Idle', 'NetworkStandby'])
def test_network_disabled_lps_from_playable_state(deviceid, lpm_tap, ip_address_wlan, playable_state):
    """
    Test to determine that network disabled feature works correctly from 'Idle' and 'NetworkStandby' state.
    Test Steps:
    1. Set NoNetworkConfiguredTimeout & NoAudioTimeout, reboot the device and configure Wlan using fixture.
    2. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    3. IP Address should be allocated to “wlan0” network interface.
    4. Change state to Idle by pressing MFB button for 2 seconds.
    5. Verify product state which should be STATE_VC_NOT_CONFIGURED or STATE_VC_CONFIGURED.
    6. Generate network disabled action using CLI Command.
    7. After 60 seconds, verify product state should be NetworkStandby(STATE_NW_CONFIGURED or STATE_NW_NOT_CONFIGURED).
    8. Wait for 2 minutes for state transition into LowPowerState.
    9. Check for LPM state which should be LowPower.
    10. Wake up LPM by generating a fake key press using LPM.
    11. Wait and verify for system state set to Standby or On.
    12. Check for product state which should be in any STATE_NW_NOT_CONFIGURED(NetworkStandby) state.
    13. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    14. IP Address should be allocated to “wlan0” network interface.
    15. Generate network disabled action using CLI Command.
    16. Get current 'WiFiStatus' which should be WIFI_STATE_DISCONNECTED.
    17. IP Address should not be allocated to “wlan0” network interface.
    18. Generate network disabled action using CLI Command.
    19. Get current 'WiFiStatus' for 10 times and stop once get WIFI_STATE_CONNECTED.
    20. IP Address should be allocated to “wlan0” network interface.
    """
    LOGGER.debug('Testing network disabled from "%s" state.', playable_state)
    # 1. Set NoNetworkConfiguredTimeout & NoAudioTimeout, reboot the device and configure Wlan using fixture.
    LOGGER.debug('Device ip-address is %s', ip_address_wlan)

    tap = adb_utils.adb_telnet_tap(deviceid)
    network_base = NetworkBase(None, device=deviceid, logger=LOGGER)

    # 2. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert resp_dict['WiFiStatus'], 'Response should contain state information.'

    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED),\
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_CONNECTED, resp_dict['WiFiStatus']['@state'])

    # 3. IP Address should be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert ip_address, 'IP Address should be allocated to wlan0'

    # 4. Change state to Idle by pressing MFB button for 2 seconds.
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)

    time.sleep(2)

    # 5. Verify product state which should be STATE_VC_NOT_CONFIGURED or STATE_VC_CONFIGURED.
    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
    assert (device_state == STATE_VC_NOT_CONFIGURED or device_state == STATE_VC_CONFIGURED), \
        'Device not in "IDLE" state. Current state: {}.'.format(device_state)

    if playable_state == 'Idle':
        # 6. Generate network disabled action using CLI Command.
        keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    time.sleep(60)

    # After 60 seconds, verify product state should be NetworkStandby(STATE_NW_CONFIGURED or STATE_NW_NOT_CONFIGURED).
    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
    assert (device_state == STATE_NW_NOT_CONFIGURED or device_state == STATE_NW_CONFIGURED), \
        'Device not in "Network Standby" state. Current state: {}.'.format(device_state)

    if playable_state == 'NetworkStandby':
        # Generate network disabled action using CLI Command.
        keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)
        time.sleep(5)

    # 8. Wait for 2 minutes for state transition into LowPowerState.
    time.sleep(120)

    # 9. Check for LPM state which should be LowPower.
    lpm_state = lpm_tap.get_system_state()
    assert (lpm_state == Lpm.SystemState.LowPower),\
        'LPM should be in LowPower state. Current state: {}.'.format(lpm_state)

    # 10. Wake up LPM by generating a fake key press using LPM.
    lpm_tap.button_tap(5, 100)

    LOGGER.debug('Wait and verify for system state set to Standby or On.')
    # 11. Wait and verify for system state set to Standby or On.
    assert (lpm_tap.wait_for_system_state([Lpm.SystemState.Standby, Lpm.SystemState.On], 15)), \
        "Failed to resume into Standby or On system state."

    LOGGER.debug('Waiting for APQ to resume')
    time.sleep(5)

    # 12. Check for product state which should be in STATE_SETUP_OTHER state.
    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
    assert (device_state == STATE_SETUP_OTHER), \
        'Device not in "{}" state. Current state: {}.'.format(STATE_SETUP_OTHER, device_state)

    # 13. Get current 'WiFiStatus' which should be WIFI_STATE_CONNECTED.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert resp_dict['WiFiStatus'], 'Response should contain state information.'

    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED),\
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_CONNECTED, resp_dict['WiFiStatus']['@state'])

    # 14. IP Address should be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert ip_address, 'IP Address should be allocated to wlan0'

    # 15. Generate network disabled action using CLI Command.
    keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    # 16. Get current 'WiFiStatus' which should be WIFI_STATE_DISCONNECTED.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert resp_dict['WiFiStatus'], 'Response should contain state information.'
    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_DISCONNECTED),\
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_DISCONNECTED,
                                                              resp_dict['WiFiStatus']['@state'])

    # 17. IP Address should not be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert not ip_address, 'IP Address should not be allocated to wlan0'

    # 18. Generate network disabled action using CLI Command.
    keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    # 19. Get current 'WiFiStatus' for 10 times and stop once get WIFI_STATE_CONNECTED.
    for _ in range(10):
        resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
        resp_dict = xmltodict.parse(resp_xml)
        assert resp_dict['WiFiStatus'], 'Response should contain state information.'
        if resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED:
            break
        LOGGER.debug('Current state : %s', resp_dict['WiFiStatus']['@state'])
        time.sleep(2)

    assert (resp_dict['WiFiStatus']['@state'] == WIFI_STATE_CONNECTED),\
        'WiFi should be in {} state. Current state:{}'.format(WIFI_STATE_CONNECTED, resp_dict['WiFiStatus']['@state'])

    # 20. IP Address should be allocated to “wlan0” network interface.
    ip_address = network_base.check_inf_presence("wlan0", timeout=1)
    assert ip_address, 'IP Address should be allocated to wlan0'


@pytest.mark.usefixtures('deviceid', 'lpm_tap', 'clear_wifi_profiles', 'set_lps_timeout')
def test_network_disabled_lps_from_setup_state(deviceid, lpm_tap):
    """
    Test to determine that network disabled feature works correctly from 'Setup' state.
    Test Steps:
    1. Clear Network profiles and set NoNetworkConfiguredTimeout & NoAudioTimeout and reboot the device using fixture.
    2. Check for product state which should be STATE_SETUP_NW.
    3. Get current 'WiFiStatus' All profiles should be cleared.
    4. Generate network disabled action using CLI Command.
    5. Check for product state which should be in any Setup state.
    6. Wait for 2 minutes for state transition into LowPowerState.
    7. Check for LPM state which should be LowPower.
    8. Wake up LPM by generating a fake key press using LPM.
    9. Wait and verify for system state set to Standby or On.
    10. Check for product state which should be in any STATE_NW_NOT_CONFIGURED(NetworkStandby) state.

    """
    # 1. Clear Network profiles and set NoNetworkConfiguredTimeout & NoAudioTimeout and reboot the device using fixture.

    # 2. Check for product state which should be STATE_SETUP_NW.
    device_state = None
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
        if device_state == STATE_SETUP_NW:
            break
        LOGGER.debug('Got state : %s', device_state)
        time.sleep(1)
    assert (device_state == STATE_SETUP_NW), \
        'Device not in "{}" state. Current state: {}.'.format(STATE_SETUP_NW, device_state)

    tap = adb_utils.adb_telnet_tap(deviceid)

    # 3. Get current 'WiFiStatus' All profiles should be cleared.
    resp_xml = adb_utils.adb_telnet_cmd('network wifi status', expect_after='\?>', device_id=deviceid)
    resp_dict = xmltodict.parse(resp_xml)
    assert not resp_dict['WiFiStatus'], 'WiFi profiles should be deleted.'

    # 4. Generate network disabled action using CLI Command.
    keypress.press_key(tap, NETWORK_DISABLED_KEYS, duration=5500, async_response=False)

    # 5. Check for product state which should be in any Setup state.
    device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
    assert ('Setup' in device_state), \
        'Device not in "Setup" state. Current state: {}.'.format(device_state)

    # 6. Wait for 2 minutes for state transition into LowPowerState.
    time.sleep(120)

    # 7. Check for LPM state which should be LowPower.
    lpm_state = lpm_tap.get_system_state()
    assert (lpm_state == Lpm.SystemState.LowPower),\
        'LPM should be in LowPower state. Current state: {}.'.format(lpm_state)

    # 8. Wake up LPM by generating a fake key press using LPM.
    lpm_tap.button_tap(5, 500)

    LOGGER.debug('Wait and verify for system state set to Standby or On.')
    # 9. Wait and verify for system state set to Standby or On.
    assert (lpm_tap.wait_for_system_state([Lpm.SystemState.Standby, Lpm.SystemState.On], 15)), \
        "Failed to resume into Standby or On system state."

    LOGGER.debug('Waiting for APQ to resume')
    time.sleep(5)

    # 10. Check for product state which should be in any STATE_NW_NOT_CONFIGURED(NetworkStandby) state.
    device_state = None
    for _ in range(20):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=deviceid)
        if device_state == STATE_NW_NOT_CONFIGURED:
            break
        LOGGER.debug('Got state: %s', device_state)
        time.sleep(1)
    assert (device_state == STATE_NW_NOT_CONFIGURED), \
        'Device not in "{}" state. Current state: {}.'.format(STATE_NW_NOT_CONFIGURED, device_state)
