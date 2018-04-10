# test_lpm_tap_examples.py
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
Examples of using Python hooks for executing LPM TAP commands on Eddie.
"""

import time

import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.LpmUtils.Lpm import Lpm

logger = get_logger(__file__)


@pytest.mark.usefixtures('lpm_serial_client')
def test_amp_fault_on(lpm_serial_client):
    """
    Manually set LPM into an amp fault state.
    """
    lpmVersion = lpm_serial_client.get_version()
    assert lpmVersion
    logger.info("LPM found at version %s", lpmVersion)

    lpm_serial_client.amp_fault_set_state(True)

    # Amp fault lasts until reboot
    lpm_serial_client.reboot()


@pytest.mark.skip(reason="Induced functionality removed.")
@pytest.mark.usefixtures('lpm_serial_client')
def test_amp_fault_induce(lpm_serial_client):
    """
    Induce an amp fault.
    """
    lpmVersion = lpm_serial_client.get_version()

    assert lpmVersion, "Missing/invalid LPM version response."
    logger.info("LPM found at version %s", lpmVersion)

    logger.info("Please wait. Inducing an amp fault takes about 15 seconds...")
    lpm_serial_client.amp_fault_induce()

    # amp fault will block until it is done, but just in case, let's wait a bit
    time.sleep(2)
    system_state = lpm_serial_client.get_system_state()
    logger.debug("Current System State: %s", system_state)
    assert (system_state == Lpm.SystemState.Error), \
        "Amp fault did not result in Error system state."

    # Amp fault lasts until reboot
    lpm_serial_client.reboot()


@pytest.mark.usefixtures('lpm_serial_client')
def test_some_keys(lpm_serial_client):
    """
    Test some key events.
    These route down to the PSoC so it tests the entire chain up from PSoC through SoC.
    """

    lpmVersion = lpm_serial_client.get_version()
    assert lpmVersion, "Missing/invalid LPM version response."
    logger.info("LPM found at version %s", lpmVersion)

    # Button examples

    # Volume down twice. 100 ms between down and up of the key.
    # Events with too small of an activation time will be ignored as noise.
    lpm_serial_client.button_tap(5, 100)
    lpm_serial_client.button_tap(5, 100)

    # Hold volume up
    lpm_serial_client.button_down(3)
    time.sleep(5)
    lpm_serial_client.button_up(3)

    # Slider examples
    # See https://wiki.bose.com/display/WSSW/Capsense+Strip+Input+Handling for mapping
    # of slider x position to presets on Eddie

    # Activate preset 2
    lpm_serial_client.slider_tap(15, 100)

    # Start on preset 2, drag to preset 3. Should do nothing.
    lpm_serial_client.slider_down(15)
    lpm_serial_client.slider_move(25)
    lpm_serial_client.slider_move(35)
    lpm_serial_client.slider_move(40)
    lpm_serial_client.slider_up(45)

    # Start on preset2, drag around in 2, hold to store preset
    lpm_serial_client.slider_down(15)
    time.sleep(0.5)
    lpm_serial_client.slider_move(21)
    time.sleep(0.5)
    lpm_serial_client.slider_move(23)
    time.sleep(1)
    lpm_serial_client.slider_move(30)
    time.sleep(1)
    lpm_serial_client.slider_up(28)
