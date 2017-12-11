import pytest
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from configure import conf

def pytest_addoption(parser):
    parser.addoption("--ip-address", 
		      action="store", 
		      default=None,
                      help="IP Address of Target under test")

    parser.addoption("--LPM_port", 
		      action="store", 
		      default=None,
                      help="serial port of the device")
@pytest.fixture()
def get_details():
    #getting the ipaddress and lpm port  of the device
    _ip_address = pytest.config.getoption('--ip-address')
    conf["LPMport"] = pytest.config.getoption("--LPM_port")
    if _ip_address == None:
        print "The _ip_address is:", _ip_address
        pytest.fail("Do this: pytest -sv <test_example.py> --ip-address <ip>")
    return _ip_address

@pytest.fixture()
def get_FrontdoorInstance(get_details):
    # returning the instance of frontdoor api and ip address
    frontDoor = FrontDoorAPI(get_details)
    return frontDoor
