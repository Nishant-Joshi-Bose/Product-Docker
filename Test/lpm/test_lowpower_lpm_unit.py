# test_lowpower_lpm_unit.py
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
Entering and exiting low power standby LPM unit test.
This puts the just the LPM into low power standby and then simulates a wake-up.
IPC messages are used to initiate the state changes and wait for key press events.
LPM tap is only used to verify entered state and simulate wake-up key press.

Note: This requires the RivieraLpmService's Python folder to be in the path.

Note: In order to use IPC communications you must follow the setup instructions
in the README from the Python folder in RivieraLpmService.
"""

import threading
import time

import pytest

import AutoLpmServiceMessages_pb2 as AutoIPCMessages
import lpm_client as LpmClient
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm

import lpmtestutils

logger = get_logger(__file__)


@pytest.mark.usefixtures('riviera_enabled_ipc', 'lpm_serial_client', 'lpm_ipc_client', 'ip_address_wlan')
def test_low_power_standby(lpm_ipc_client, lpm_serial_client, ip_address_wlan):
    """
    Enter standby state using IPC message, wait for key press, then resume.

    :param lpm_ipc_client: An Inter-Process Communication through IP connection
    :param lpm_serial_client: A serial connection to the LPM
    :param ip_address_wlan: The IP Address of the Device over Wireless
    :return: None
    """

    # Used to wait for asynchronus requests to come back.
    _thread_lock = threading.Event()

    # Used to verify that the state requested to enter.
    global verify_system_state_id
    verify_system_state_id = None

    # Waiting for key press to resume
    global waiting_for_key_press
    waiting_for_key_press = True

    # Reboot to put LPM in a good state, if necessary
    if not lpmtestutils.is_in_good_state_for_testing(lpm_serial_client):
        logger.info("LPM in incompatible state. Rebooting.")
        lpmtestutils.reboot_and_wait(lpm_serial_client)
        time.sleep(5)

    assert (lpm_serial_client.get_system_state() != Lpm.SystemState.LowPower), \
        "LPM already in LowPower state. Test aborted"

    ipc_client = LpmClient.LpmClient(ip_address_wlan)

    def wake_on_key_event_cb(resp):
        """
        Return to standby state. Normally the APQ would tell the LPM to change
        out of low power state. We play that role here.

        :param resp: Response from service in form of IpcLpmStateResponse_t.
        """
        global verify_system_state_id
        global waiting_for_key_press

        if waiting_for_key_press:
            waiting_for_key_press = False
            logger.info(resp)

            systemStateMsg = AutoIPCMessages.IpcLpmSystemStateSet_t()
            systemStateMsg.state = AutoIPCMessages.SYSTEM_STATE_STANDBY

            verify_system_state_id = systemStateMsg.state
            ipc_client.SetSystemState(systemStateMsg, set_system_state_cb)

    def set_system_state_cb(resp):
        """
        Callback for SetSystemState IPC transaction.

        :param resp: Response from service in form of IpcLpmStateResponse_t.
        """
        global verify_system_state_id
        logger.info("System State: {}".format(verify_system_state_id))
        if verify_system_state_id is not None:
            assert (verify_system_state_id == resp.sysState), \
                "System state {} does not match expected state {}".format(resp.sysState, verify_system_state_id)

        _thread_lock.set()

    # Pretend we are the
    lpm_ipc_client.RegisterEvent(AutoIPCMessages.IPC_KEY,
        AutoIPCMessages.IpcKeyInformation_t, wake_on_key_event_cb)

    # Enter low power system state.
    logger.info("Putting LPM in low power state.")
    systemStateMsg = AutoIPCMessages.IpcLpmSystemStateSet_t()
    systemStateMsg.state = AutoIPCMessages.SYSTEM_STATE_LOW_POWER

    _thread_lock.clear()
    # _verifySystemStateId = systemStateMsg.state
    verify_system_state_id = systemStateMsg.state

    lpm_ipc_client.SetSystemState(systemStateMsg, set_system_state_cb)
    _thread_lock.wait(10)

    # Give some time just in case.
    time.sleep(3)

    # Verify low power system state.
    assert (lpm_serial_client.wait_for_system_state([Lpm.SystemState.LowPower], 10)), \
        "Failed to enter LowPower system state."

    # Generate a fake key press. This will come in to us via IPC as if we are the APQ.
    logger.info("Waking LPM")
    lpm_serial_client.button_tap(5, 100)

    # Wait for and verify standby system state.
    assert(lpm_serial_client.wait_for_system_state([Lpm.SystemState.Standby], 10)), \
        "Failed to resume into Standby system state."
