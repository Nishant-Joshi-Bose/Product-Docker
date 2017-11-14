import pytest
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from .demo_utils import DemoUtils
logger = get_logger(__name__)

@pytest.fixture(scope='function')
def save_speaker_log(request, device_ip):
    """
    This fixture collects the device log and save to given location.
    """
    logger.info("save_speaker_log")
    from CastleTestUtils.LoggerUtils.logreadLogger import LogreadLogger
    logreadlogger = LogreadLogger(device_ip)
    try:
        logreadlogger.start_log_collection(testName=request.function.__name__ ,path="./SpeakerLogs", saveperiodically=True)
    except:
        logger.info("Error while start: The log from the speaker will not be saved.")

    def teardown():
        logger.info("teardown")
        # stop speaker log collection
        try:
            logreadlogger.stop_log_collection()
        except:
            logger.info("Error while stop: The log from the speaker will not be saved.")

    request.addfinalizer(teardown)

@pytest.fixture(scope='class')
def device_ip(request):
    """
    This fixture gets the device ip from eth0
    :return: device ip
    """
    logger.info("device_ip")
    if request.config.getoption("--target").lower() == 'device':
        networkbaseObj = NetworkBase(None)
        device_ip = networkbaseObj.check_inf_presence('eth0').group(2)
    return device_ip

@pytest.fixture(scope="class")
def frontDoor(request, device_ip):
    """
    Get FrontDoorAPI instance.
    """
    logger.info("frontDoor")
    if device_ip is None:
        pytest.fail("No valid device IP")
    frontdoorObj = FrontDoorAPI(device_ip)
    return frontdoorObj

@pytest.fixture(scope='class')
def demoUtils(frontDoor):
    """
    Get DemoUtils instance.
    """
    logger.info("demoUtils")
    return DemoUtils(frontDoor)

@pytest.fixture(scope='function', autouse=True)
def setDemoOff(request, frontDoor, demoUtils):
    """
    Set demoMode off
    """
    from CastleTestUtils.SoftwareUpdateUtils.RebootDevice import reboot_device as wait_for_reboot
    logger.info("setDemoOff")
    demoResponse = frontDoor.getDemoMode()
    logger.info("demoResponse " + demoResponse)
    if demoResponse == 'on':
        demoUtils.setDemoMode("off", request.config.getoption("--device-id"))

        demoResponse = frontDoor.getDemoMode()
        logger.info("demoResponse" + demoResponse)
        if demoResponse != 'off':
            assert False , "DemoMode is not off when expected. demoResponse: " + demoResponse

@pytest.fixture(scope='function', autouse=True)
def test_log_banner(request):
    """
    Log start and completed test banner in console output.
    """
    testName = request.function.__name__
    logger.info("\n%s\n----- Start test:    %s\n%s\n", "-" * 60, testName, "-" * 60)

    def teardown():
        logger.info("\n%s\n----- Completed test:    %s\n%s\n", "-" * 60, testName, "-" * 60)

    request.addfinalizer(teardown)
