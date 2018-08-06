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
def setDemoOff(request, front_door_email, demoUtils, deviceid):
    """
    Set demoMode off and delete keyConfig
    """
    logger.info("setDemoOff")
    setDemo(request, front_door_email, demoUtils, deviceid)
    demoUtils.deleteKeyConfig(front_door_email)
    demoUtils.verifyDemoKeyConfig(front_door_email, "Error Reading configuration file")

def setDemo(request, front_door_email, demoUtils, deviceid):
    """
    Set demoMode False if True
    """
    demoResponse = front_door_email.getDemoMode()
    logger.info("demoResponse " + str(demoResponse))
    if demoResponse == True:
        demoUtils.setDemoMode(False, deviceid, front_door_email, True, 3,
                              request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode(False, front_door_email)

@pytest.fixture(scope='session')
def get_config():
    """
    This fixture will return the keyConfig data from commonData.py
    :return: keyConfig
    """
    data = keyConfig
    return data
"""
@pytest.fixture(scope='function')
def front_door_email(request, device_ip):
    frontDoorAPI = FrontDoorAPI(device_ip, email=request.config.getoption("--email"),
                                password=request.config.getoption("--password"))
    def tear():
        if frontDoorAPI:
            frontDoorAPI.close()
    request.addfinalizer(tear)
    return frontDoorAPI
"""
@pytest.fixture(scope='function', autouse=True)
def resetDemo(request, front_door_email, demoUtils, device_id):
    """
    reset demoMode False if True
    :param request:
    :param front_door_email:
    :param demoUtils:
    :param device_id:
    :return:
    """
    def teardown():
        logger.info("set demoMode False towards the end of every test")
        setDemo(request, front_door_email, demoUtils, device_id)
    request.addfinalizer(teardown)
