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
def demoUtils(adb):
    """
    Get DemoUtils instance.
    """
    logger.info("demoUtils")
    return DemoUtils(adb, logger)

@pytest.fixture(scope='function', autouse=True)
def setDemoOff(request, frontdoor_wlan, demoUtils, device_id):
    """
    Set demoMode off and delete keyConfig
    """
    logger.info("setDemoOff")
    setDemo(request, frontdoor_wlan, demoUtils, device_id)
    demoUtils.deleteKeyConfig(frontdoor_wlan)
    demoUtils.verifyDemoKeyConfig(frontdoor_wlan, "Error Reading configuration file")

def setDemo(request, frontdoor_wlan, demoUtils, device_id):
    """
    Set demoMode False if True
    """
    demoResponse = frontdoor_wlan.getDemoMode()
    logger.info("demoResponse " + str(demoResponse))
    if demoResponse == True:
        demoUtils.setDemoMode(False, device_id, frontdoor_wlan, True, 3,
                              request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(False, frontdoor_wlan)

@pytest.fixture(scope='session')
def get_config():
    """
    This fixture will return the keyConfig data from commonData.py
    :return: keyConfig
    """
    data = keyConfig
    return data

@pytest.fixture(scope='function', autouse=True)
def resetDemo(request, frontdoor_wlan, demoUtils, device_id):
    """
    reset demoMode False if True
    :param request:
    :param frontdoor_wlan:
    :param demoUtils:
    :param device_id:
    :return:
    """
    def teardown():
        if 'factorydefault' in request.keywords:
            print "factory default"
        else:
            print "no factory default"
            logger.info("set demoMode False towards the end of every test")
            setDemo(request, frontdoor_wlan, demoUtils, device_id)
    request.addfinalizer(teardown)
