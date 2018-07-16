"""
Conftest.py for DemoController
"""
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.DemoUtils.demoUtils import DemoUtils
from ..commonData import keyConfig
logger = get_logger(__name__)

@pytest.fixture(scope='class', autouse=True)
def resetDemo(request, frontDoor, demoUtils, device_id):
    """
    reset demoMode False if True
    """
    def teardown():
        logger.info("set demoMode False towards the end of all the tests")
        setDemo(request, frontDoor, demoUtils, device_id)
    request.addfinalizer(teardown)

@pytest.fixture(scope='class')
def demoUtils(frontDoor, adb):
    """
    Get DemoUtils instance.
    """
    logger.info("demoUtils")
    return DemoUtils(frontDoor, adb, logger)

@pytest.fixture(scope='function', autouse=True)
def setDemoOff(request, frontDoor, demoUtils, device_id):
    """
    Set demoMode off and delete keyConfig
    """
    logger.info("setDemoOff")
    setDemo(request, frontDoor, demoUtils, device_id)
    demoUtils.deleteKeyConfig()
    demoUtils.verifyDemoKeyConfig("Error Reading configuration file")

def setDemo(request, frontDoor, demoUtils, device_id):
    """
    Set demoMode False if True
    """
    demoResponse = frontDoor.getDemoMode()
    logger.info("demoResponse " + str(demoResponse))
    if demoResponse == True:
        demoUtils.setDemoMode(False, device_id, True, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(False)

@pytest.fixture(scope='session')
def get_config():
    """
    This fixture will return the keyConfig data from commonData.py
    :return: keyConfig
    """
    data = keyConfig
    return data

