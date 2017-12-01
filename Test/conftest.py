import os
from CastleTestUtils.LoggerUtils.log_setup import get_logger
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.RivieraUtils import rivieraCommunication
import pytest
import datetime
_log = None
logger = get_logger(__name__)

def pytest_addoption(parser):
    parser.addoption("--device-id",
                     action="store",
                     default=None,
                     help="device-id: Device Id")

    parser.addoption("--target",
                     action="store",
                     default="native",
                     help="target: [native/device], Specify whether the tests need to be executed on native or on device")

    parser.addoption("--log-dir",
                     action="store",
                     default="SCMLogs",
                     help="Where to store logs.")

    parser.addoption("--log-type",
		     action="store",
                     default = "useSerial",
                     help="logging : [useSerial / ipBased ]")

    parser.addoption("--network-iface",
                     action="store",
                     default="wlan0",
                     help="network interface to choose")

    parser.addoption("--ip-address",
                    action="store",
                    default=None,
                    help="IP Address of Target under test")

def ping(ip):
    return 0 == os.system("ping -q -c 5 -i 0.2 -w 2 " + ip)

@pytest.fixture(scope='session')
def scm_ip(request):
    return request.config.getoption("--scm-ip")

################################################################
############          Starting Processes             ###########
################################################################
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
        iface = request.config.getoption("--network-iface")
        device_ip = networkbaseObj.check_inf_presence(iface).group(2)
        return device_ip

@pytest.fixture(scope="class")
def frontDoor(device_ip):
    """
    Get FrontDoorAPI instance.
    """
    logger.info("frontDoor")
    if device_ip is None:
        pytest.fail("No valid device IP")
    frontdoorObj = FrontDoorAPI(device_ip)
    return frontdoorObj

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

def create_log_dir(foldername):
    """
    Create a directory to store logs.
    :return: dir - name of the log directory.
    """
    if not os.path.exists(foldername):
        os.mkdir(foldername)
    subfolder = foldername +"/Logs-"+str(datetime.date.today())
    if not os.path.exists(subfolder):
        os.makedirs(subfolder)
    return subfolder

@pytest.fixture(scope='class')
def adb(request):
    """
    Get adb instance
    """
    adb = rivieraCommunication.getCommunicationType('ADB')
    adb.setCommunicationDetail(request.config.getoption("--device-id"))
    return adb