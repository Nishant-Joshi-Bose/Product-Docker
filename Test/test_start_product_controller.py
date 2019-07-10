import json
import logging
import time

import pytest

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

pytest.mark.usefixtures("eddie_product_controller")
def test_start_controller(eddie_product_controller):
    assert(True)
