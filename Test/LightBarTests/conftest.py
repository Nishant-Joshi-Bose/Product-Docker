import pytest
from FrontDoorAPI import FrontDoorAPI
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
def get_ipaddress():
    #getting the ipaddress of  device
    _ip_address = pytest.config.getoption('--ip-address')
    if _ip_address == None:
        print "The _ip_address is:", _ip_address
        pytest.fail("Do this: pytest -sv <test_example.py> --ip-address <ip>")
    return _ip_address

@pytest.fixture()
def get_FrontdoorInstance(get_ipaddress):
    # returning the instance of frontdoor api and ip address
    frontDoor = FrontDoorAPI(get_ipaddress)
    return frontDoor

