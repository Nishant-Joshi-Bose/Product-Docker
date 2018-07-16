# test_factory_default.py
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
PyTest based tests for the Eddie factory default feature.
"""
import time
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.hardware.keys.keys import Keys
from CastleTestUtils.RivieraUtils.hardware.keys import keypress
from CastleTestUtils.RivieraUtils import adb_utils
import factory_utils
from ..ProductControllerAPI import eddie_helper

LOGGER = get_logger(__file__)

FACTORY_DEFAULT_KEYS = [Keys.AUX.value, Keys.VOLUME_DOWN.value]


@pytest.fixture(scope="module", params=[Keys.BLUETOOTH.value, Keys.VOLUME_UP.value, Keys.MULTIFUNCTION.value])
def cancel_button(request):
    """
    List of buttons used to cancel factory default buttonEvent countdown
    """
    param = request.param
    yield param


@pytest.fixture(scope="module", params=[2000, 4000, 6000, 8000])
def cancel_duration(request):
    """
    List of timeout durations of factory default buttonEvent
    """
    param = request.param
    yield param


@pytest.mark.usefixtures('frontdoor_wlan', 'tap', 'cancel_duration')
def test_factory_default_cancel_event(frontdoor_wlan, tap, cancel_duration):
    """
    Test to determine that factory default cancel event should be generated.
    Test Steps:
    1. Connect device to wlan using frontdoor_wlan fixture.
    2. Generate factoryDefault action using CLI Command for 'cancel_duration' timeout.
    3. Capture factoryDefault event notification.
    4. Verify factoryDefault event notification which should be 'CANCEL'.
    """
    # 1. Connect device to wlan using frontdoor_wlan fixture.

    # 2. Generate factoryDefault action using CLI Command for 'cancel_duration' timeout.
    # Press the Multi buttons [AUX & VolDown] for FactoryDefault for duration less than 1,0020ms.
    keypress.press_key(tap, FACTORY_DEFAULT_KEYS, cancel_duration)
    time.sleep(2)

    # 3. Capture factoryDefault event notification.
    state = factory_utils.get_factory_default_event_notification(frontdoor_wlan)
    # 4. Verify factoryDefault event notification which should be 'CANCEL'.
    assert state == factory_utils.EVENT_STATE_CANCEL, 'Got Invalid FactoryDefault event state : {}'.format(state)


@pytest.mark.usefixtures('frontdoor_wlan', 'tap', 'cancel_duration', 'cancel_button')
def test_factory_default_cancel_with_button_event(frontdoor_wlan, tap, cancel_duration, cancel_button):
    """
    Test to determine that factory default cancel event should be generated.
    Test Steps:
    1. Connect device to wlan using frontdoor_wlan fixture.
    2. Generate factoryDefault action using CLI Command.
    3. Generate Key press of 'cancel_button' after 'cancel_duration' timeout.
    4. Capture factoryDefault event notification.
    5. Release factoryDefault keys to complete action.
    6. Verify factoryDefault event notification which should be 'CANCEL'.
    """
    # 1. Connect device to wlan using frontdoor_wlan fixture.

    # Press the Multi buttons [AUX & VolDown] for FactoryDefault for minimum duration of 1,0020ms.
    # 2. Generate factoryDefault action using CLI Command.
    keypress.key_press_only(tap, FACTORY_DEFAULT_KEYS, async_response=True)
    # 3. Generate Key press of 'cancel_button' after 'cancel_duration' timeout.
    time.sleep((cancel_duration / 1000.0))
    keypress.press_key(tap, cancel_button, 500)

    # Wait for device to generate notification
    time.sleep(1)
    # 4. Capture factoryDefault event notification.
    state = factory_utils.get_factory_default_event_notification(frontdoor_wlan)
    # 5. Release factoryDefault keys to complete action.
    keypress.key_release_only(tap, FACTORY_DEFAULT_KEYS, async_response=True)
    # 6. Verify factoryDefault event notification which should be 'CANCEL'.
    assert state == factory_utils.EVENT_STATE_CANCEL, 'Got Invalid FactoryDefault event state : {}'.format(state)


@pytest.mark.usefixtures('device_id', 'adb', 'frontdoor_wlan', 'tap', 'router')
def test_factory_default_success(device_id, adb, frontdoor_wlan, tap, router):
    """
    Test to determine that factory default performs and reboots the device.
    Test Steps:
    1. Connect device to wlan using frontdoor_wlan fixture.
    2. Generate factoryDefault action using CLI Command.
    3. Wait for minimum 10 seconds to launch factoryDefault action.
    4. Verify COUNTDOWN events and COMPLETED state of event.
    5. Capture last system state which should be 'FactoryDefault'.
    6. Wait for factoryDefault action to complete.
    7. Wait for device to become online after reboot.
    8. Wait for CLI-Server to start and listens on 17000 port.
    9. Check for product state which should be 'SetupNetwork'.
    10. Check for NetworkProfiles.xml in product-persistence to verify success scenario.
    11. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    12. Configure device WiFi network.
    13. Check for product state which should be 'SetupOther'.
    14. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    """
    # 1. Connect device to wlan using frontdoor_wlan fixture.

    # Press the Multi buttons [AUX & VolDown] for FactoryDefault for minimum duration of 1,0020ms.
    # 2. Generate factoryDefault action using CLI Command.
    keypress.key_press_only(tap, FACTORY_DEFAULT_KEYS, async_response=True)
    # 3. Wait for minimum 10 seconds to launch factoryDefault action.
    time.sleep(11)
    # 4. Verify COUNTDOWN events and COMPLETED state of event.
    counts, state = factory_utils.get_event_notification_after_countdown(frontdoor_wlan,
                                                                         event=factory_utils.EVENT_FACTORY, counts=10)
    # Assert if COUNTDOWN buttonEvents are not received properly
    assert counts == 0, 'Got Invalid COUNTDOWN events, last count:{} and state:{}'.format(counts, state)
    # Assert if "COMPLETED" buttonEvent state is not received
    assert state == factory_utils.EVENT_STATE_OK, \
        'Button event state should be {}, received state : {}'.format(factory_utils.EVENT_STATE_OK, state)

    # Wait for device to generate notification for state change
    time.sleep(2)
    # 5. Capture last system state which should be 'FACTORY_DEFAULT'.
    state = factory_utils.get_last_system_state_notification(frontdoor_wlan)
    # Assert if "FACTORY_DEFAULT" system state is not received
    assert state == eddie_helper.FACTORY_DEFAULT, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.FACTORY_DEFAULT, state)

    # 6. Wait for factoryDefault action to complete.
    time.sleep(10)
    # 7. Wait for device to become online after reboot.
    adb.waitForRebootDevice()

    # 8. Wait for CLI-Server to start and listens on 17000 port.
    status = None
    for _ in range(30):
        status = adb.executeCommand("(netstat -tnl | grep -q 17000) && echo OK")
        if status and status.strip() == 'OK':
            break
        time.sleep(1)
    assert status, "CLIServer not started within 30s."
    assert status.strip() == 'OK', 'CLIServer is not stated even after 30 seconds'
    time.sleep(2)

    # 9. Check for product state which should be 'SetupNetwork'.
    device_state = None
    for _ in range(30):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        if device_state == eddie_helper.SETUPNETWORK:
            break
        LOGGER.debug('Got state: %s', device_state)
        time.sleep(1)
    assert device_state == eddie_helper.SETUPNETWORK, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.SETUPNETWORK, device_state)

    # 10. Check for NetworkProfiles.xml in product-persistence to verify success scenario.
    nw_file_status = adb.executeCommand("test -f /mnt/nv/product-persistence/NetworkProfiles.xml && echo FOUND")
    assert not nw_file_status, \
        '/mnt/nv/product-persistence/NetworkProfiles.xml should be removed after factory default.'

    # 11. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    file_status = adb.executeCommand("test -f /mnt/nv/product-persistence/FirstBootGreetingDone && echo FOUND")
    assert not file_status, \
        '/mnt/nv/product-persistence/FirstBootGreetingDone should be removed after factory default.'

    # 12. Configure device WiFi network.
    wifi_add_profiles_command = ' '.join(['network', 'wifi', 'profiles', 'add',
                                          router.ssid, router.security.upper(), router.password])
    LOGGER.debug("Sending telnet command to add wifi profile %s", wifi_add_profiles_command)

    adb_utils.adb_telnet_cmd(wifi_add_profiles_command, expect_after='->OK', expect_last='ADD_PROFILE_SUCCEEDED',
                             async_response=True, device_id=device_id)

    # 13. Check for product state which should be 'SetupOther'.
    device_state = None
    for _ in range(10):
        device_state = adb_utils.adb_telnet_cmd('getproductstate', expect_after='Current State: ', device_id=device_id)
        if device_state == eddie_helper.SETUPOTHER:
            break
        LOGGER.debug('Got state: %s', device_state)
        time.sleep(1)
    assert device_state == eddie_helper.SETUPOTHER, \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.SETUPOTHER, device_state)

    # 14. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    file_status = adb.executeCommand("test -f /mnt/nv/product-persistence/FirstBootGreetingDone && echo FOUND")
    assert file_status, \
        '/mnt/nv/product-persistence/FirstBootGreetingDone should be created in SetupOther.'


@pytest.mark.usefixtures('device_id', 'adb', 'frontdoor_wlan', 'tap', 'router')
@pytest.mark.parametrize("selected_source", [Keys.BLUETOOTH.value, Keys.AUX.value])
def test_factory_default_selected_state(device_id, adb, frontdoor_wlan, tap, selected_source, router):
    """
    Test to determine that factory default performs from SELECTED state and reboots the device.
    Test Steps:
    1. Connect device to wlan using frontdoor_wlan fixture.
    2. Change state to Aux for Selected state
    3. Generate factoryDefault action using CLI Command.
    4. Wait for minimum 10 seconds to launch factoryDefault action.
    5. Verify COUNTDOWN events and COMPLETED state of event.
    6. Capture last system state which should be 'FactoryDefault'.
    7. Wait for factoryDefault action to complete.
    8. Wait for device to become online after reboot.
    9. Wait for CLI-Server to start and listens on 17000 port.
    10. Check for product state which should be 'SetupNetwork'.
    11. Check for NetworkProfiles.xml in product-persistence to verify success scenario.
    12. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    13. Configure device WiFi network.
    14. Check for product state which should be 'SetupOther'.
    15. Check for FirstBootGreetingDone in product-persistence to verify success scenario.

    """
    # 1. Connect device to wlan using frontdoor_wlan fixture.

    # 2. Change state to Aux for Selected state.
    keypress.press_key(tap, selected_source, 500)
    time.sleep(2)
    state = frontdoor_wlan.getState()
    # Assert if "SELECTED" system state is not received
    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)

    # [3-11]. call test_factory_default_success
    test_factory_default_success(device_id, adb, frontdoor_wlan, tap, router)


@pytest.mark.usefixtures('device_id', 'adb', 'frontdoor_wlan', 'tap', 'router')
def test_factory_default_idle_state(device_id, adb, frontdoor_wlan, tap, router):
    """
    Test to determine that factory default performs from IDLE state and reboots the device.
    Test Steps:
    1. Connect device to wlan using frontdoor_wlan fixture.
    2. Change state to Idle by pressing MFB button for 2 seconds.
    3. Generate factoryDefault action using CLI Command.
    4. Wait for minimum 10 seconds to launch factoryDefault action.
    5. Verify COUNTDOWN events and COMPLETED state of event.
    6. Capture last system state which should be 'FactoryDefault'.
    7. Wait for factoryDefault action to complete.
    8. Wait for device to become online after reboot.
    9. Wait for CLI-Server to start and listens on 17000 port.
    10. Check for product state which should be 'SetupNetwork'.
    11. Check for NetworkProfiles.xml in product-persistence to verify success scenario.
    12. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    13. Configure device WiFi network.
    14. Check for product state which should be 'SetupOther'.
    15. Check for FirstBootGreetingDone in product-persistence to verify success scenario.

    """
    # 1. Connect device to wlan using frontdoor_wlan fixture.

    # 2. Change state to Idle by pressing MFB button for 2 seconds.
    keypress.press_key(tap, Keys.MULTIFUNCTION.value, 2000)
    time.sleep(2)
    state = frontdoor_wlan.getState()
    # Assert if "IDLE" system state is not received
    assert state == eddie_helper.IDLE, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.IDLE, state)

    # [3-11]. call test_factory_default_success
    test_factory_default_success(device_id, adb, frontdoor_wlan, tap, router)


@pytest.mark.usefixtures('device_id', 'adb', 'frontdoor_wlan', 'tap', 'device_playing_from_amazon', 'router')
def test_factory_default_sts_play_state(device_id, adb, frontdoor_wlan, tap, router):
    """
    Test to determine that factory default performs from SELECTED(play from amazon) state and reboots the device.
    Test Steps:
    1. Connect device to wlan using frontdoor_wlan fixture.
    2. Change state to SELECTED using fixture device_playing_from_amazon.
    3. Generate factoryDefault action using CLI Command.
    4. Wait for minimum 10 seconds to launch factoryDefault action.
    5. Verify COUNTDOWN events and COMPLETED state of event.
    6. Capture last system state which should be 'FactoryDefault'.
    7. Wait for factoryDefault action to complete.
    8. Wait for device to become online after reboot.
    9. Wait for CLI-Server to start and listens on 17000 port.
    10. Check for product state which should be 'SetupNetwork'.
    11. Check for NetworkProfiles.xml in product-persistence to verify success scenario.
    12. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    13. Configure device WiFi network.
    14. Check for product state which should be 'SetupOther'.
    15. Check for FirstBootGreetingDone in product-persistence to verify success scenario.
    """
    # 1. Connect device to wlan using frontdoor_wlan fixture.
    # 2. Change state to SELECTED using fixture device_playing_from_amazon.
    state = frontdoor_wlan.getState()
    # Assert if "SELECTED" system state is not received
    assert state == eddie_helper.SELECTED, \
        'Device should be in {} state. Current state : {}'.format(eddie_helper.SELECTED, state)

    # [3-11]. call test_factory_default_success
    test_factory_default_success(device_id, adb, frontdoor_wlan, tap, router)
