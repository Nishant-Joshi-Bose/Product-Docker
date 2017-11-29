import os
import sys
import time
import json
import thread
import logging 
import requests
import threading
import websocket
import simplejson

from FrontDoorAPIBase import FrontDoorAPIBase

class FrontDoorAPI(FrontDoorAPIBase):

    _jsonIn = []
    _isOpen = False
    _msgId  = 0
    _msgCb  = None

    _timeout = 60 
    _request_id = 0 
    _deviceID = "123456"
    _ip_address = ""

    _last_notification = ""
    _raw_response = ""
    _response = ""

    def __init__(self, ip):
	"""
	Calls function to open a Web Socket connection.
	Takes in the IP Address as a paramter; Called implicitly, being a constructor 
	"""
        self._ip_address = ip
        print "\nThe IP address of the target is", ip
        self.open()

    def open(self):
        """
	Opens a WebSocket connection
        Note: There is a subprotocol, that needs to be passed known as 'Gabbo' protocol
        """
        print "Opening Web Socket connection to the device with IP", self._ip_address
        self._jsonIn[:] = []

        self._wss = websocket.WebSocketApp("ws://" + self._ip_address + ":8082",
			                            subprotocols=['gabbo'],
		                                    on_message = self._onMessage, 
						    on_error = self._onError, 
						    on_close = self._onClose)
	self.jsonInEvent = threading.Event()
	self.jsonInEvent.clear()
	self._wss.on_open = self._onOpen
        self._run()

	while self._wss.sock is None:
	    print "Waiting for websocket object to be created"
	print "\nThe sock object is at:", self._wss.sock

	print "\nWaiting for websocket connection to be established"
	startTime = time.time()
	while self._wss.sock.connected == False:
	    sys.stdout.write('.')
	    sys.stdout.flush()
	    sys.stdout.write('\b')
	endTime = time.time()
	print "\nThe total time it takes", (endTime-startTime)
	print "\nIs the websocket connection established?:",self._wss.sock.connected

    def _run(self):
	"""
	Creates thread and runs to open websocket
	"""
	def runloop(*args):
	    self._wss.run_forever()
	thread.start_new_thread(runloop, ())

    def close(self):
	print "\nClosing WebSocket Connection"
	self._wss.close()
	self._isOpen = False 
  
    def _onOpen(self, ws):
        print "\nWebSocket Connection Open"
        self._isOpen = True

    def _onClose(self, ws):
        print "\nWebSocket Connection Closed"
        self._wss.close()
	self._isOpen = False
     
    def isOpen(self):
        return self._isOpen
  
    def _onError(self, ws, error):
	print "\nWeb Socket Error Received", error

    def capture(self, cb= lambda msg, msgId: False):
        """
	Gets the response from the system that matches the Request ID of the current message
	Response type: String 
	"""
        print("###########################################")
	timeout = time.time() + self._timeout
	while True:
	    self._msgCb = cb
            response_id = self._request_id
            for response in self._jsonIn:
                # Loading the JSON response into a python dictionary
                json_object = json.loads(response)

		# This dumps a python dict to a JSON object
                json_response = json.dumps(json_object, sort_keys=True, indent=4)

                if json_object["header"]["reqID"] == response_id:
		    self._jsonIn.remove(response)
                    #print "The Json Message received is:", json_response
		    return json_response 
	        else:
	            return "Please parse through Json correctly"

	    if time.time() > timeout:
	        # It's taking too long,better close the connection
		print "\nWebSocket Timeout"
		self._wss.close()
		return "Response Timeout"
        	
    def _onMessage(self, ws, message):
	# This function detects if the message is a notification and adds it to the last_notificaiton
	if '"method":"NOTIFY"' in message:
	    self.last_notification = json.loads(message)
	    print "Last Notification recieved:", json.dumps(self.last_notification, indent=4, sort_keys=True)
	else:
	    #print "This is not a notification & the message is:", message
            self._jsonIn.append(message)
	    if self._msgCb(message, len(self._jsonIn) -1):
	        self.jsonInevent.set()
		
    def send(self, request_type, api, data=""):
        """
	Method that eventually gets called to send API to system and return the result
	param: GET / POST request
               API being called 
	       JSON data for POST API's 
	return JSON response from the system
	"""

	if not self._isOpen:
	    self.open()
	
	print "Getting status of the Websocket connection"
        print "The value of isOpen is", self._isOpen
	print "Sending -", api
	try:
	    # Encapsulating the JSON, with request type; header etc, into the message
	    print "Enveloping Message in Process"
	    env_message = self.envelope_message( request_type, 
			                         api, 
						 data, 
						 self._request_id)

	    print "Enveloped Message to be sent: \n", env_message
	    self._wss.send(str(env_message))

            # Gets the response
	    print "\nGetting the response for:", api
            self._response = self.capture()

            # Printing the response 
            print "\nThe returned response is:", self._response

	    # Incerement the counter for the next time
	    self._request_id +=1


	except Exception as e:
	    self._isOpen = False
	    print "Exception in send() - ", str(e)
	    return "WebSocket not connected"

        # Reset Counter if we get a lot of requests
        if self._request_id > 9999:
	    self._request_id = 0

        return self._response

    def envelope_message(self, request_type, api, data, request_id):
        """
	Function that handles the actual handling of Json responses
	Param: GET/POST request_type
	       api or 'resource'
	       data or 'body'
	       request_id

	"""    
	_header = json.dumps( { "header": {
	                               "resource": api,
		                       "version": '1',
	                               "token": "las9kdfjaslkjdbhgsdkKbldkfbvnl?adkfjnvlk",
	                               "method": request_type,
			               "msgtype":"REQUEST",
                                       "device": "GUID",
	                               "reqID": request_id 
			                  } 
				}, indent= 4 )

	print "The header Json string: \n", _header 
	print "The data recieved in Json string is: \n", data
		
	if request_type.lower() == "get":
	    _message = _header
        elif request_type.lower() == "post" or "put" or "delete":
	    # Converting the header and into a Python Dict 
	    _header = json.loads(_header)

   	    data = json.loads(data)
	    # Constructing the message to be sent
	    _header["body"] = data
	    _header = json.dumps(_header, indent=4)
	    _message = _header
	    print "The value of the message is", _message 
	else:
	    _message = "Non-supported type"
        return _message 

if __name__ == "__main__":
    castle_conn = FrontDoorAPI("192.168.1.148")
    castle_conn.get_language()
