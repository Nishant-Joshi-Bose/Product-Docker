import pytest
from CastleTestUtils.RivieraUtils  import rivieraCommunication
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from configure import conf

def pytest_addoption(parser):
    parser.addoption("--LPM_port", 
		      action="store", 
		      default=None,
                      help="serial port of the device")
@pytest.fixture()
def get_details():
    #getting the ipaddress and lpm port  of the device
    communication = rivieraCommunication.getCommunicationType("ADB")
    communication.setCommunicationDetail()
    _ip_address = communication.getIPAddress()
    conf["LPMport"] = pytest.config.getoption("--LPM_port")
    return _ip_address

@pytest.fixture()
def get_FrontdoorInstance(get_details):
    # returning the instance of frontdoor api and ip address
    frontDoor = FrontDoorAPI(get_details)
    return frontDoor
