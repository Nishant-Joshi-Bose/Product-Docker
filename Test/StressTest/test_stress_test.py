# test_stress_test.py
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
PyTest based stress tests for the Eddie.
"""
import os
import pytest
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.RivieraUtils import rivieraCommunication
import stress_utils
from ..ProductControllerAPI import eddie_helper

LOGGER = stress_utils.LOGGER


@pytest.mark.usefixtures("request", "device_id", "riviera", "__pytest_repeat_step_number", "add_wifi_at_end")
def test_stress_first_boot_greetings(request, device_id, riviera, __pytest_repeat_step_number):
    """
    Test to validate Welcome chimes and light_bar animation after factory default.
    Test Steps -
    1. Wait for device to exit "Booting" state otherwise "FactoryDefault" may misbehave.
    2. Perform Factory default on device.
    3. Check for NetworkProfiles.xml in product-persistence to verify factory default.
    4. Check for FirstBootGreetingDone in product-persistence to verify factory default.
    5. Check for product state which should be 'Booting'.
    6. Check for product state which should be 'FirstBootGreeting'.
    7. Verify chimes for 'FirstBootGreeting'.
    8. Get ip-address of network interface 'wlan1' which should be '192.0.2.1'.
    9. Verify all light_bar animations.
    """
    adb = riviera.communication
    assert isinstance(adb, rivieraCommunication.ADBCommunication)

    is_error_occurred = True

    def tear_down_get_logs():
        """ Get logs if any error occurred """
        if is_error_occurred:
            remote_file_name = "/tmp/logread.txt"
            local_file_name = os.getcwd() + "/logread_first_boot_greetings.txt"
            if request.config.getoption("--count") > 1:
                local_file_name = os.getcwd() + "/logread_first_boot_greetings_count_" \
                                  + str(__pytest_repeat_step_number + 1) + ".txt"

            LOGGER.info("Getting logs and saving to %s", local_file_name)
            command = "curl http://localhost/logread.txt > {}".format(remote_file_name)
            adb.executeCommand(command)
            adb.adbObj.get_remote_file(remote_file_name, local_file_name)

    request.addfinalizer(tear_down_get_logs)

    # 1. Wait for device to exit "Booting" state otherwise "FactoryDefault" may misbehave.
    device_state = stress_utils.wait_for_booting_state(adb, on_exit=True, timeout=60)
    if device_state == eddie_helper.BOOTING:
        response = adb.executeCommand("echo 'product boot_status' | nc 0 17000")
        LOGGER.error('product boot_status response first time: %s', response)
        # Try another time
        adb.rebootDevice_adb()

        device_state = stress_utils.wait_for_booting_state(adb, on_exit=True, timeout=60)
        response = adb.executeCommand("echo 'product boot_status' | nc 0 17000")
        LOGGER.info('product boot_status response after reboot: %s', response)
        assert (device_state != eddie_helper.BOOTING), 'Device is still in {} state.'.format(device_state)

    device_state = stress_utils.wait_for_product_state(adb, expected_state=eddie_helper.FIRSTBOOTGREETING, on_exit=True)
    assert (device_state != eddie_helper.FIRSTBOOTGREETING), 'Device is still in {} state.'.format(device_state)

    # 2. Perform Factory default on device.
    assert riviera.perform_factory_default(), 'Device was not rebooted after factory default within 30 seconds.'

    # 3. Check for NetworkProfiles.xml in product-persistence to verify factory default.
    nw_file_status = adb.executeCommand("test -f /mnt/nv/product-persistence/NetworkProfiles.xml && echo FOUND")
    assert not nw_file_status, \
        '/mnt/nv/product-persistence/NetworkProfiles.xml should be removed after factory default.'

    # 4. Check for FirstBootGreetingDone in product-persistence to verify factory default.
    file_status = adb.executeCommand("test -f /mnt/nv/product-persistence/FirstBootGreetingDone && echo FOUND")
    assert not file_status, \
        '/mnt/nv/product-persistence/FirstBootGreetingDone should be removed after factory default.'

    # 5. Check for product state which should be 'Booting'.
    device_state = stress_utils.wait_for_booting_state(adb)
    assert (device_state == eddie_helper.BOOTING), \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.BOOTING, device_state)

    # 6. Check for product state which should be 'FirstBootGreeting'.
    device_state = stress_utils.wait_for_product_state(adb, expected_state=eddie_helper.FIRSTBOOTGREETING, timeout=45)
    response = adb.executeCommand("echo 'product boot_status' | nc 0 17000")
    LOGGER.info('product boot_status response first time: %s', response)
    assert (device_state == eddie_helper.FIRSTBOOTGREETING), \
        'Device not in {} state. Current state: {}.'.format(eddie_helper.FIRSTBOOTGREETING, device_state)

    # 7. Verify chimes for 'FirstBootGreeting'.
    last_chime = stress_utils.get_last_chime(adb)
    assert 'T1_welcome_to_bose' in last_chime, 'Welcome chime is not played.'

    # 8. Get ip-address of network interface 'wlan1' which should be '192.0.2.1'.
    LOGGER.info("Checking ip address of network interface wlan1...")
    network_base = NetworkBase(None, device_id)
    ip_address = network_base.check_inf_presence('wlan1')
    assert ip_address == '192.0.2.1', "Setup AP mode not up after factory default."

    # 9. Verify all light_bar animations.
    LOGGER.info("Comparing light_bar animations...")
    lb_history = stress_utils.get_lb_history(adb)
    lb_expected = stress_utils.get_lb_expected_for_first_boot()
    assert stress_utils.compare_light_bar_animations(lb_expected, lb_history), \
        'light_bar animations are not played correctly. Expected:{}, Actual:{}'.format(lb_expected, lb_history)

    is_error_occurred = False
