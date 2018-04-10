# conftest.py
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
Conftest.py for Diagnostics
"""
import pytest
import logging

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication

logger = get_logger(__name__, "DiagnosticsPage.log", level=logging.INFO, fileLoglevel=logging.DEBUG)

def pytest_addoption(parser):
    """ Command Line Parameters """ 
    parser.addoption("--network-iface", action="store", default="wlan0",
                     help="network interface to choose")

    parser.addoption("--device-id", default=None, help="Run the test scenario on passed device")

@pytest.fixture
def chrome_options(chrome_options):
    """
    This fixture would enable headless for all Chrome instances
    """
    chrome_options.add_argument('headless')
    return chrome_options

@pytest.fixture(scope='session')
def device(request):
    """
    Use request object to get device id from command line
    """
    device = request.config.getoption("--device-id")
    if device is None:
        pytest.fail("Provide valid Device Id")
    return device

@pytest.fixture(scope='session')
def device_ip(device):
    """
    Use device id to get device ip
    """
    adb = ADBCommunication(logger=logger)
    adb.setCommunicationDetail(device)
    return adb.getIPAddress()
