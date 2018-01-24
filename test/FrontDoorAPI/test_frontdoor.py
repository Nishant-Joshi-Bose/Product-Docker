import json
import pytest

from FrontDoorAPI import FrontDoorAPI

_ip_address = pytest.config.getoption('--ip-address')
if not _ip_address:
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

def test_getState():
    """
    This is an endpoint to get the state
    the product is in ~ Product Controller State
    Previously in ECO1, we were getting it using,
    now_playing / getpdo system - TAP command
    """
    print "\nGetting systemstate status"
    stateStatus = frontDoor.getState()
    assert stateStatus

def test_getconfigurationStatus():
    """
    Getter for system Configuration status
    Product controller will cache the status information
    and use it during system startuo to drive which state to
    go to after BootUP
    """
    print "\nGetting the configuration status"
    configStatus = frontDoor.getconfigurationStatus()
    assert configStatus

def test_getInfo():
    print "\nGetting info status"
    infoStatus = frontDoor.getInfo()
    assert infoStatus

def test_getDemo():
    print "\nGetting the demo mode"
    demoMode = frontDoor.getDemoMode()
    assert demoMode is False

# Requests - where Caps is the handler

def test_getCapsIntitalizationStatus():
    print "\nGetting the Caps Intitalization Status"
    capsStatus = frontDoor.getCapsIntializationStatus()
    assert capsStatus

def test_getSources():
    print "\nGetting the source list"
    source_list = frontDoor.getSources()
    assert source_list

def test_getnowPlaying():
    print "\nGetting the nowPlaying list"
    nowPlaying_list = frontDoor.getNowPlaying()
    assert nowPlaying_list

def test_playbackRequest():
    print "\nGetting the playback request"
    data = json.dumps({"source":"DEEZER",
                       "sourceAccount":"matthew_scanlan@bose.com",
                       "preset":{"type":"tracklisturl",
                                 "location":"/v1/playback/containerType/album/containerId/7776833",
                                 "name":"XSCAPE",
                                 "presetable":"true", "containerArt":""},
                       "playback":{"type":"tracklisturl",
                                   "location":"/v1/playback/containerType/album/containerId/7776833/track/78213533",
                                   "name":"XSCAPE", "presetable":"true"}}, indent=4)
    print "\nThe data going to be sent is\n", data
    playbackRequest = frontDoor.sendPlaybackRequest(data)
    assert playbackRequest

def test_getVolume():
    volume = frontDoor.getVolume()
    assert volume

def test_getDefaultVolume():
    defaultVolume = frontDoor.getDefaultVolume()
    assert defaultVolume

def test_sendVolume():
    data = json.dumps({"value": "10"}, indent=4)
    print "\nThe data that is going to be sent\n", data
    volume = frontDoor.sendVolume(data)
    assert volume

def test_setMinVolume():
    data = json.dumps({"min": "10"}, indent=4)
    print "\nThe data that is going to be sent\n", data
    minVolume = frontDoor.sendMinVolume(data)
    assert minVolume

def test_setMaxVolume():
    data = json.dumps({"max": "90"}, indent=4)
    print "\nThe data that is going to be sent\n", data
    maxVolume = frontDoor.sendMaxVolume(data)
    assert maxVolume

def test_volumeDecrement():
    data = json.dumps({"delta": "10"}, indent=4)
    print "\nThe data that is going to be sent\n", data
    decrementVolume = frontDoor.sendDecrementVolume(data)
    assert decrementVolume

def test_volumeIncrement():
    data = json.dumps({"delta": "10"}, indent=4)
    print "\nThe data that is going to be sent\n", data
    incrementVolume = frontDoor.sendIncrementVolume(data)
    assert incrementVolume

def test_setDefaultVolume():
    data = json.dumps({"defaultOn": "10"}, indent=4)
    print "\nThe data that is going to be sent\n", data
    defaultVolume = frontDoor.sendDefaultVolume(data)
    assert defaultVolume

def test_setTransportControl():
    print "\nSet the transportControl"
    data = json.dumps({"state": "play"}, indent=4)
    print "The data that is going to be sent\n", data
    transportControl = frontDoor.sendTransportControl(data)
    assert transportControl

def test_stopPlaybackRequest():
    print "\nStop/Deactivate the source"
    stopPlaybackResponse = frontDoor.stopPlaybackRequest()
    assert stopPlaybackResponse

def test_getZone():
    print "\nGet Zone Details"
    zoneDetails = frontDoor.getZone()
    assert zoneDetails


# Requests - where FD is the handler

def test_getCapabilities():
    print "\nGetting the capabilities list"
    capabilities_list = frontDoor.getCapabilities()
    assert capabilities_list

def test_getPing():
    print "\nGetting ping status"
    pingStatus = frontDoor.ping()
    assert pingStatus

# Requests - where NetworkServiceFrontDoorIF is the handler

def test_getnetworkStatus():
    print "\nGetting the network Status"
    netStatus = frontDoor.getNetworkStatus()
    assert netStatus

def test_getWiFiProfile():
    print "\nGetting the wifi profile"
    wifiProfile = frontDoor.getWifiProfile()
    assert wifiProfile

def test_getsiteScan():
    print "\nGetting the site-scan profiles"
    data = json.dumps({}, indent=4)
    siteScan = frontDoor.sendSiteScan(data)
    assert siteScan

def test_getap():
    print "\nGetting the AP status"
    ap = frontDoor.getAPModeStatus()
    assert ap

def test_getWifiStatus():
    print "\nGetting the network Status"
    wifiStatus = frontDoor.getNetworkWiFiStatus()
    assert wifiStatus

# Requests - where handler Lightbar Controller

def test_getActiveAnimation():
    print "Getting active animation"
    lightbar_status = frontDoor.getActiveAnimation()
    assert lightbar_status

def test_playLightBarAnimation():
    print "test play lightbar animation"
    data = {"next": {"value": "BT_DISCOVERABLE", "transition": "smooth", "repeat": True}}
    data = json.dumps(data)
    lightbar_status = frontDoor.playLightBarAnimation(data)
    assert lightbar_status

def test_stopAnimation():
    print "test play lightbar animation"
    data = {"next": {"value": "BT_DISCOVERABLE", "transition": "smooth", "repeat": False}}
    data = json.dumps(data)
    lightbar_status = frontDoor.stopActiveAnimation(data)
    assert lightbar_status
