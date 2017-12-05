import pytest
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.DemoUtils.demoUtils import DemoUtils
logger = get_logger(__name__)

@pytest.fixture(scope='class')
def demoUtils(frontDoor, adb):
    """
    Get DemoUtils instance.
    """
    logger.info("demoUtils")
    return DemoUtils(frontDoor, adb)

@pytest.fixture(scope='function', autouse=True)
def setDemoOff(request, frontDoor, demoUtils):
    """
    Set demoMode off
    """
    logger.info("setDemoOff")
    demoResponse = frontDoor.getDemoMode()
    logger.info("demoResponse " + demoResponse)
    if demoResponse == 'on':
        demoUtils.setDemoMode("off", True, 3, request.config.getoption("--network-iface"))
        demoUtils.verifyDemoMode("off")

