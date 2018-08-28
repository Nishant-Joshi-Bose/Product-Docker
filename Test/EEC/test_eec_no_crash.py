# EEC/test_eec_no_crash.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
"""
Tests to ensure that Low Power is exited to proper running system.
"""
import time
import pytest

from CastleTestUtils.LpmUtils.Lpm import Lpm
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.rivieraCommunication import getCommunicationType

LOGGER = get_logger(__name__)

PROCESS_LIST = ["Telnet", "APServer", "BLESetup", "BLEToFrontDoorService", "BT", "BTSource",
                "BluetoothManager", "CLIServer", "DataCollectionService", "DemoController",
                "FrontDoor", "GalapagosClient", "HTTPProxy", "LPMService", "Monaco", "NetManager",
                "Passport", "SASSController", "Telemetry", "AVS", "Vfe", "SWUpdateWebServer",
                "SoftwareUpdateService", "Eddie", "avahi-daemon", "IoT", "UpnpSource", "CAPS",
                "STSCertified", "PtsServer"]

@pytest.mark.usefixtures('clear_wifi_profiles', 'set_lps_timeout', 'add_wifi_at_end')
def test_eec_no_crash(lpm_serial_client, device_id):
    """
    Test steps:
        1. Clear all wifi profiles
        2. Change NoNetworkConfiguredTimeout to 2 minutes and reboot
        3. Verify all processes are running ok
        4. Monitor for product state as low power or time exceeds 2 minutes
        5. Verify low power state
        6. Simulate MFB key to resume from low power state
        7. Verify all processes are still running ok as in step 3

    Teardown:
        1. Revert NoNetworkConfiguredTimeout to 20 minutes and reboot
        2. Add wifi profile back
    """
    LOGGER.info("test_all_processes_running")

    # Get ADB comm
    adb_comm = getCommunicationType('ADB')
    adb_comm.setCommunicationDetail(device_id)

    # Verify processes are running before LPS
    assert adb_comm.areProcessesRunning(PROCESS_LIST)

    # Wait for device to enter low power standby
    LOGGER.info("Check for the Low Power Standby state after 2 minutes")
    assert lpm_serial_client.wait_for_power_state([Lpm.PowerState.LowPower], 130), \
        "System did not go into Low Power state on time"

    # Make sure APQ completely shuts down
    time.sleep(30)

    # Simulate a key press to wake it up.
    LOGGER.info("Simulating a MFB key press.")
    lpm_serial_client.button_tap(4, 15)

    # Verify we are back in standby state.
    LOGGER.debug("Waiting 60s for power state to be %s or %s", Lpm.PowerState.Standby,
                 Lpm.PowerState.On)
    LOGGER.debug("Current Power State: %s", lpm_serial_client.get_power_state())
    assert lpm_serial_client.wait_for_power_state([Lpm.PowerState.Standby, Lpm.PowerState.On],
                                                  60), "Failed to resume into Standby system state."

    # Check processes again
    assert adb_comm.areProcessesRunning(PROCESS_LIST)
