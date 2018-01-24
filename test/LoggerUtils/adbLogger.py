import os
import re
import time 
import tarfile 
import requests
import platform
import threading
import traceback

from pyadb import ADB
adb = ADB('/usr/bin/adb')

class CastleLogger(object):
    """
    Class that handles collection of logging information through ADB
    """
    def __init__(self, serial=""):
        """
        :param hostAddr: IP address of Shelby device
        :return:
        """
	_grep = None
	_filepath = None
	_response = ""

    def is_adb_running(self, timeout=30*5):
        """
	How are we checking if the target is ready?
	1. Check if adb-server is alive, up and running?
	2. If we are able to send a request, and recieve a response?
	   - We should still able to get logs, nevertheless.
	"""
	for n in range(timeout):
	    try:
	        deviceID = adb.get_devices()
		if not deviceID[1]:
		    print "Waiting for ADB-Server to start"
		    continue
	    	else:
	            print "The deviceID is", deviceID[1]
	            return deviceID
    	    except:
	        print "Exception, ADB-Server is not starting up"
	        adb.start_server()
	        continue
	
    	print "Target not in good state to collect logs"
	return False

    def send_logging(self):
        """
	Send the 'logread -f' command
	"""
	tries = 0 
	_response = ""
	print "Sending logread to the DUT"
	if self.is_adb_running() == False:
	    raise Exception("ADB-Server is down; Do check the connection status")
        while len(_response) < 1 and tries < 5 :
	    _response = adb.shell_command("logread -f")
	if tries >=4:
	    print "Failed to capturing logs"
	
if __name__ == "__main__":
    print "This is the Castle Logger"
    cl = CastleLogger()
    cl.is_adb_running()
    cl.send_logging()
