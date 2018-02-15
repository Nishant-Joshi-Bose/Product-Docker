"""
Examples for various LPM tests that are initiated from APQ tap.
"""

import pexpect
import pytest
import time

from CastleTestUtils.LoggerUtils.log_setup import get_logger
from ApqTap import ApqTap

__logger = get_logger(__file__)

__ip_address = pytest.config.getoption('--ip-address')
if __ip_address is None:
    pytest.fail("IP address is required: pytest -sv <test.py> --ip-address <0.0.0.0>")


@pytest.mark.skip("Example/test functionality")
def test_backlight():
	"""
	Set the backlight to a few diferent levels.
	"""

	with ApqTap(__ip_address, 'adb') as tap:

		tap.send("backlight 100", "OK")
		time.sleep(3)

		tap.send("backlight 50", "OK")
		time.sleep(3)

		tap.send("backlight 10", "OK")

