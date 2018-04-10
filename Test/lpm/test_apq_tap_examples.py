# test_apq_tap_examples.py
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
Examples for various LPM tests that are initiated from APQ tap.
"""

import time

import pytest

from ApqTap import ApqTap


@pytest.mark.usefixtures('ip_address_wlan')
def test_backlight(ip_address_wlan):
    """
    Set the backlight to a few different levels.
    """

    with ApqTap(ip_address_wlan, 'adb') as tap:

        tap.send('lpm pt "backlight 100"', "OK")
        time.sleep(3)

        tap.send('lpm pt "backlight 50"', "OK")
        time.sleep(3)

        tap.send('lpm pt "backlight 10"', "OK")
