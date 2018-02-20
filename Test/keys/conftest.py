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

import pexpect
import pytest
from configure import conf
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.LoggerUtils.BSerialLogger import SerialLoggingUtil, myThread, stopseriallog, TestRecoveryTest
from pyadb import ADB
adb = ADB(conf["adb_path"])
logger = get_logger(__file__)

ASYNC_RESPONSE = 'ar'
TAP_RESPONSE = 'tap'

def pytest_generate_tests(metafunc):
    """
     This fixture could be used to parameterize the test with data.
    (i.e:--conf["ApqPort"] conf["ip_address"])
    : param metafunc : Object that help to inspect a test function and \
                     to generate tests according to test configuration.
    : return : none
    """
    conf["apqPort"] = metafunc.config.getoption("--apq-port")
    conf["ip_address"] = metafunc.config.getoption("--ip-address")
    comports = TestRecoveryTest()
    assert conf["apqPort"] != None, "Do this: pytest -sv <test_example.py> --apq-port <port>"
    assert conf["apqPort"] in comports.getCOMPortList(), "Please enter valid port"

@pytest.fixture()
def serial_handler():
    """
    This fixture is used to start the serial logs of device an stop the serial log at the end.
    """
    apq_handle = SerialLoggingUtil('APQ', conf["apqPort"])
    apq_thread = myThread(apq_handle)
    apq_thread.start()
    apq_handle.execute_command(TAP_RESPONSE + '\r\n')
    apq_handle.execute_command(ASYNC_RESPONSE + '\r\n')
    yield apq_handle
    #Stopping the APQ serial log thread.
    stopseriallog(apq_handle)
    apq_thread.join()
    apq_handle.comporthandle.close()
    apq_handle.filehandle.close()

@pytest.fixture()
def pexpect_client():
    """
    This fixture is used to get the pexpect client for performing tap commands of  CLI keys.
    """
    ip_address = conf["ip_address"]
    if not ip_address:
        found_device_list = adb.get_devices()[1]
        assert found_device_list, "No device is connected"
        client = pexpect.spawn('adb shell')
        client.expect('#')
        client.sendline('telnet localhost 17000')
    else:
        client = pexpect.spawn('telnet %s 17000' % ip_address)
    yield client
    client.close()
