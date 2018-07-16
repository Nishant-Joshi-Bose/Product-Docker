import pytest

from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.RivieraUtils import adb_utils, rivieraCommunication, rivieraUtils
from CastleTestUtils.LoggerUtils.CastleLogger import get_logger
from CastleTestUtils.NetworkUtils.network_base import NetworkBase

LOGGER = get_logger(__name__)

'''
NOTE CURRENTLY PASSWORD IS HARD CODED IN FRONTDOORAPI FILE
'''

def pytest_addoption(parser):
    """
    Command line options for the pytest tests in this module.
    :param parser: Parser used for method.
    :return: None
    """
    parser.addoption("--device-id",
                     action="store",
                     default=None,
                     help="device-id: Device Id")

    parser.addoption("--target",
                     action="store",
                     default="native",
                     help="target: [native/device], \
                            Specify whether the tests need to be executed on native or on device")

    parser.addoption("--network-iface",
                     action="store",
                     default="wlan0",
                     help="network interface to choose")

    parser.addoption("--ip-address",
                     action="store",
                     default=None,
                     help="IP Address of Target under test")

    parser.addoption("--email",
                     action="store",
                     default=True,
                     help="email to SSH")

    parser.addoption("--password",
                     action="store",
                     default=True,
                     help="Password to SSH")

@pytest.fixture(scope='session')
def device_ip(request, device_id):
    """
    This fixture gets the device IP
    :return: device ip
    """
    LOGGER.info("Trying to retrieve the IP-Address of the device")
    if request.config.getoption("--target").lower() == 'device':
        network_base = NetworkBase(None, device_id)
        interface = request.config.getoption("--network-iface")
        device_ip = network_base.check_inf_presence(interface)
        return device_ip

@pytest.fixture(scope='session')
def riviera(device_id):
    """
    Get RivieraUtil instance.
    """
    return rivieraUtils.RivieraUtils('ADB', device=device_id)

@pytest.fixture(scope='session')
def device_id(request):
    """
    Acquires the Command line Device ID.

    :param request: PyTest command line request options
    :return: String Device ID used when starting the test
    """
    return request.config.getoption('--device-id')

@pytest.fixture(scope="session")
def frontDoor_conn(device_ip, password, email, request):
    """
    Get FrontDoorAPI instance.
    """
    LOGGER.info("Spawning a front door object")
    if device_ip is None:
        pytest.fail("No valid device IP")
    front_door = FrontDoorAPI(device_ip, 8082, 'latest', None, None, email, password)

    def tear():
        if front_door:
            front_door.close()
    request.addfinalizer(tear)

    return front_door

@pytest.fixture(scope='session')
def email(request):
    """
    Use request object to get email from command line
    return: email
    """
    email = request.config.getoption("--email")
    if email is None:
        pytest.fail("Provide valid --email")
    return email


@pytest.fixture(scope='session')
def password(request):
    """
    Use request object to get password from command line
    return: password
    """
    password = request.config.getoption("--password")
    if password is None:
        pytest.fail("Provide valid --password")
    return password