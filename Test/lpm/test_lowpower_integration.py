# test_lowpower_integration.py
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
Entering and exiting low power standby integration test.
This puts the entire system into low power standby and then wakes it up.

This test requires connections to both LPM (via tap) and APQ (via adb).
"""

import time

import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm

import lpmtestutils
from ApqTap import ApqTap

logger = get_logger(__file__)


@pytest.mark.usefixtures('ip_address_wlan', 'lpm_serial_client')
def test_low_power_standby(ip_address_wlan, lpm_serial_client):
    """
    Enter system into low power standby state from APQ tap.
    """
    logger.info("Putting LPM into a compatible state. Rebooting.")
    lpmtestutils.reboot_and_wait(lpm_serial_client, wait_time=10)
    # Time to ensure Tap interface has come up post boot.
    time.sleep(20)

    logger.info("Entering Low Power Standby.")
    # Use APQ tap to put the system into low power.
    with ApqTap(ip_address_wlan) as tap:
        tap.send("lowpower suspend", "Success")

    # Ensure that we are in Low Power state
    assert lpm_serial_client.wait_for_system_state([Lpm.SystemState.LowPower], 20), \
        "Failed to enter LowPower system state."

    # Ensure a little time for key commands to be accepted
    time.sleep(20)
    # Simulate a key press to wake it up.
    logger.info("Simulating a MFB key press.")
    lpm_serial_client.button_tap(4, 250)

    # Verify we are back in standby state.
    logger.debug("Waiting 30s for power state to be {} or {}"
                 .format(Lpm.PowerState.Standby, Lpm.PowerState.On))
    logger.debug("Current Power State: {}".format(lpm_serial_client.get_power_state()))
    assert lpm_serial_client.wait_for_power_state([Lpm.PowerState.Standby, Lpm.PowerState.On], 30), \
        "Failed to resume into Standby system state."
