import json
import pytest
from FrontDoorAPI import FrontDoorAPI

_ip_address = pytest.config.getoption('--ip-address')
if _ip_address == None:
    print "The _ip_address is:", _ip_address
    pytest.fail("Do this: pytest -sv <test_example.py> --ip-address <ip>")

frontDoor = FrontDoorAPI(_ip_address)

# GET REQUESTS
"""
The possible handlers are:
	1. CAPS
	2. FrontDoorSelfService
	3. NetworkServiceFrontDoorIF
	4. Eddie
"""

# Requests - where Eddie is the Handler

def test_getLanguage():
    print "Getting language list"
    language_list = frontDoor.send_api('/system/language')
    language_list = json.loads(language_list)
    validation_list = json.dumps({"supported_language_codes":["da","de","en","es","fr",\
		                                              "it","nl","sv","ja","zh",\
							      "ko","th","cs","fi","el",\
							      "no","pl","pt","ro","ru",\
							      "sl","tr","hu"]})
    validation_list = json.loads(validation_list)
    assert language_list["body"]["properties"]["supported_language_codes"] == validation_list["supported_language_codes"]

def test_setLanguage():
    print "\nSetting the language in the device"
    # Sending JSON data to FD Client
    data = json.dumps({"code": "da"}, indent=4)
    print "The data that is going to be sent\n", data
    language_list = frontDoor.send_api('/system/language',data)
    # Converting recieved JSON to python dict
    language_list = json.loads(language_list)
    assert language_list["header"]["resource"] == "/system/language"
    assert language_list["header"]["msgtype"] == "RESPONSE"
    assert language_list["header"]["method"] == "POST"
    assert language_list["body"]["code"] == "da"

def test_getState():
    """
    This is an endpoint to get the state
    the product is in ~ Product Controller State
    Previously in ECO1, we were getting it using,
    now_playing / getpdo system - TAP command
    """
    print "Getting systemstate status"
    stateStatus = frontDoor.send_api('/system/state')
    assert stateStatus

def test_getconfigurationStatus():
    """
    Getter for system Configuration status
    Product controller will cache the status information
    and use it during system startuo to drive which state to
    go to after BootUP
    """
    print "Getting the configuration status"
    configStatus = frontDoor.send_api('/system/configuration/status')
    assert configStatus

def test_getInfo():
    print "Getting info status"
    infoStatus = frontDoor.send_api('/system/info')
    infoStatus = json.loads(infoStatus)
    assert infoStatus["header"]["resource"] == "/system/info"
    assert infoStatus["header"]["msgtype"] == "RESPONSE"
    assert infoStatus["header"]["method"] == "GET"
    assert infoStatus["body"]["variant"] == "Professor"

# Requests - where Caps is the handler

def test_getCapsIntitalizationStatus():
    print "Getting the Caps Intitalization Status"
    capsStatus = frontDoor.send_api('/system/capsInitializationStatus')
    assert capsStatus

def test_getSources():
    print "Getting the source list"
    source_list = frontDoor.send_api('/system/sources')
    assert source_list 

def test_getnowPlaying():
    print "Getting the nowPlaying list"
    nowPlaying_list = frontDoor.send_api('/content/nowPlaying')
    assert nowPlaying_list

def test_playbackRequest():
    print "Getting the playback request"
    #playbackRequest = frontDoor.send_api('/content/playbackRequest')
    #assert playbackRequest

def test_setTransportControl():
    print "\nSet the transportControl"
    data = json.dumps({"state": "play"}, indent=4)
    print "The data that is going to be sent\n", data
    transportControl = frontDoor.send_api('/content/transportControl',data)
    transportControl = json.loads(transportControl)
    assert transportControl["body"]["state"] == "play"

# Requests - where FD is the handler

def test_getCapabilities():
    print "Getting the capabilities list"
    capabilities_list = frontDoor.send_api('/system/capabilities')
    assert capabilities_list 

def test_getPing():
    print "Getting ping status"
    pingStatus = frontDoor.send_api('/network/ping')
    assert pingStatus

# Requests - where NetworkServiceFrontDoorIF is the handler

def test_getnetworkStatus():
    print "\nGetting the network Status"
    netStatus = frontDoor.send_api('/network/status')
    assert netStatus

def test_getWiFiProfile():
    print "\nGetting the wifi profile"
    wifiProfile = frontDoor.send_api('/network/wifi/profile')
    assert wifiProfile

def test_getsiteScan():
    print "\nGetting the site-scan profiles"
    data = json.dumps({ }, indent=4)
    siteScan = frontDoor.send_api('/network/wifi/siteScan', data)
    assert siteScan

def test_getap(): 
    print "\nGetting the AP status"
    ap = frontDoor.send_api('/network/wifi/ap')
    assert ap

def test_getnetworkStatus():
    print "\nGetting the network Status"
    wifiStatus = frontDoor.send_api('/network/wifi/status')
    assert wifiStatus
