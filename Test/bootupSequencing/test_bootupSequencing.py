import os
import re
import sys
import json
import time
import pytest
import logging 
import ConfigParser
from pyadb import ADB

from CastleTestUtils.FrontDoorAPI import FrontDoorAPI
from CastleTestUtils.NetworkUtils.network_base import NetworkBase
from CastleTestUtils.SoftwareUpdateUtils.RebootDevice import reboot_device as rd

cfg = ConfigParser.SafeConfigParser()
cfg.read("./Configs/conf_eddie.ini")
networkbase = NetworkBase(cfg)

adb = ADB('/usr/bin/adb')
timeout= 1000*10

def test_bootupSequencing():
    print "HEYAAAA"
    # TODO:Implement a notification for change from BOOTUP state
    # Note: Currently System goes into SETUP not STANDBY
    # Also, We need a way to catch BOOTING State - Logging Dependency
    startTime = time.time()	
    rd.reboot_device()
    endTime = time.time()
    print "Reboot of target completed and took..", (endTime-startTime)

    start = time.time()
    print "Checking if IP address has been acquired"
    for n in range(timeout):
        try:
	    ip = networkbase.check_inf_presence('wlan0')
	    if ip is not None:
	        print "IP Address has been acquired", ip
		break
	except:
	    print "Exception on IP Address"
	    continue
    end = time.time()

    print "Booting completed in", (end-start)
    print "Checking if system is in SETUP"
    frontDoor = FrontDoorAPI.FrontDoorAPI(ip.group(2))
    state = frontDoor.getState()
    print "The state is", state
    assert state == 'SETUP'
