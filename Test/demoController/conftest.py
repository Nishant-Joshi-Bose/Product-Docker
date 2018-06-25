"""
Conftest.py for DemoController
"""
import pytest
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.DemoUtils.demoUtils import DemoUtils
from ..commonData import keyConfig
logger = get_logger(__name__)

@pytest.fixture(scope='class')
def demoUtils(adb):
    """
    Get DemoUtils instance.
    """
    logger.info("demoUtils")
    return DemoUtils(adb, logger)

@pytest.fixture(scope='function', autouse=True)
def setDemoOff(request, frontDoor_reboot, demoUtils, deviceid):
    """
    Set demoMode off and delete keyConfig
    """
    logger.info("setDemoOff")
    setDemo(request, frontDoor_reboot, demoUtils, deviceid)
    demoUtils.deleteKeyConfig(frontDoor_reboot)
    demoUtils.verifyDemoKeyConfig(frontDoor_reboot, "Error Reading configuration file")

def setDemo(request, frontDoor_reboot, demoUtils, deviceid):
    """
    Set demoMode False if True
    """
    demoResponse = frontDoor_reboot.getDemoMode()
    logger.info("demoResponse " + str(demoResponse))
    if demoResponse == True:
        demoUtils.setDemoMode(False, deviceid, frontDoor_reboot, True, 3,
                              request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(False, frontDoor_reboot)

@pytest.fixture(scope='session')
def get_config():
    """
    This fixture will return the keyConfig data from commonData.py
    :return: keyConfig
    """
    data = keyConfig
    return data

@pytest.fixture(scope='function')
def frontDoor_reboot(request, device_ip):
    frontDoorAPI = FrontDoorAPI(device_ip, email=request.config.getoption("--email"),
                                password=request.config.getoption("--password"))
    def tear():
        if frontDoorAPI:
            frontDoorAPI.close()
    request.addfinalizer(tear)
    return frontDoorAPI

@pytest.fixture(scope='function', autouse=True)
def resetDemo(request, frontDoor_reboot, demoUtils, device_id):
    """
    reset demoMode False if True
    :param request:
    :param frontDoor_reboot:
    :param demoUtils:
    :param device_id:
    :return:
    """
    def teardown():
        logger.info("set demoMode False towards the end of every test")
        setDemo(request, frontDoor_reboot, demoUtils, device_id)
        demoUtils.deleteKeyConfig(frontDoor_reboot)
        demoUtils.verifyDemoKeyConfig(frontDoor_reboot, "Error Reading configuration file")
    request.addfinalizer(teardown)
