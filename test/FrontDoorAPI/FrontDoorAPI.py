"""
Abstract: Utility to send websocket requests through FrontDoor

"""

import json
import sys
import thread
import threading
import time
import websocket

from FrontDoorAPIBase import FrontDoorAPIBase
from LoggerUtils.BLogSetup import get_logger

class FrontDoorAPI(FrontDoorAPIBase):
    """
    Class to communicate with target device via websocket connection
    """

    _jsonIn = []
    _isOpen = False
    _msgId = 0
    _msgCb = None

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
        self.logger = get_logger(self.__class__.__name__)
        self._ip_address = ip
        self.logger.debug(["The IP address of the target is", ip])
        self.open()

    def open(self, time_out=30):
        """
        Opens a WebSocket connection
        Note: There is a subprotocol, that needs to be passed known as 'Gabbo' protocol
        """
        self.logger.debug(["Opening Web Socket connection to the device with IP", self._ip_address])
        self._jsonIn[:] = []

        self._wss = websocket.WebSocketApp("ws://" + self._ip_address + ":8082/?product=Eddie",
                                           subprotocols=['gabbo'],
                                           on_message=self._onMessage,
                                           on_error=self._onError,
                                           on_close=self._onClose)
        self.jsonInEvent = threading.Event()
        self.jsonInEvent.clear()
        self._wss.on_open = self._onOpen
        self._run()
        start_time = time.time()
        self.logger.debug("Waiting for websocket connection to be established")
        while not self._wss.sock:
            if (time.time() - start_time) > time_out:
                raise websocket.WebSocketTimeoutException('Failed to acquire a socket after {}s'.format(time_out))

        self.logger.debug(["The sock object is at:", self._wss.sock])
        startTime = time.time()
        timeout = time.time() + self._timeout
        while not self._wss.sock.connected:
            sys.stdout.write('.')
            if time.time() > timeout:
                # It's taking too long,better close the connection
                self.logger.error("WebSocket Timeout - No Route to host")
                self._wss.close()
                break
            sys.stdout.flush()
            sys.stdout.write('\b')
        endTime = time.time()
        self.logger.debug(["The total time it takes", (endTime-startTime)])
        self.logger.info(["Is the websocket connection established?:", self._wss.sock.connected])

    def _run(self):
        """
        Creates thread and runs to open websocket
        """
        def runloop(*args):
            self._wss.run_forever()
        thread.start_new_thread(runloop, ())

    def close(self):
        """
        Closes the websocket connection
        """
        self.logger.debug("Closing WebSocket Connection")
        self._wss.close()
        self._isOpen = False

    def _onOpen(self, ws):
        self.logger.debug("WebSocket Connection Open")
        self._isOpen = True

    def _onClose(self, ws):
        self.logger.debug("WebSocket Connection Closed")
        self._wss.close()
        self._isOpen = False

    def isOpen(self):
        return self._isOpen

    def _onError(self, ws, error):
        self.logger.error(["Web Socket Error Received", error])

    def capture(self, cb=lambda msg, msgId: False):
        """
        Gets the response from the system that matches the Request ID of the current message
        Response type: String
        """
        timeout = time.time() + self._timeout
        while True:
            self._msgCb = cb
            response_id = self._request_id
            # To find the response where request ID matches with the expected ID
            for response in self._jsonIn:
                # Loading the JSON response into a python dictionary
                json_object = json.loads(response)

                # This dumps a python dict to a JSON object
                json_response = json.dumps(json_object, sort_keys=True, indent=4)

                if json_object["header"]["reqID"] == response_id:
                    self._jsonIn.remove(response)
                    # print "The Json Message received is:", json_response
                    return json_response

            if time.time() > timeout:
                # It's taking too long,better close the connection
                self.logger.error("WebSocket Timeout")
                self._wss.close()
                return "Response Timeout"

    def _onMessage(self, ws, message):
        # This function detects if the message is a notification and adds it to the last_notificaiton
        if '"method":"NOTIFY"' in message:
            self._last_notification = json.loads(message)
            self.logger.info("Last Notification received:"+ json.dumps(self._last_notification, indent=4, sort_keys=True))
        else:
            # print "This is not a notification & the message is:", message
            self._jsonIn.append(message)
            if self._msgCb(message, len(self._jsonIn) - 1):
                self.jsonInEvent.set()

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

        self.logger.debug("Getting status of the Websocket connection")
        self.logger.debug(["The value of isOpen is", self._isOpen])
        self.logger.info(["Sending ", request_type, api])
        try:
            # Encapsulating the JSON, with request type; header etc, into the message
            self.logger.debug("Enveloping Message in Process")
            env_message = self.envelope_message(request_type, api, data, self._request_id)

            self.logger.debug("Enveloped Message to be sent: \n" + env_message)
            self._wss.send(str(env_message))

            # Gets the response
            self.logger.debug(["Getting the response for:", api])
            self._response = self.capture()

            # Printing the response
            self.logger.debug("The returned response is:\n " + self._response)

            # Increment the counter for the next time
            self._request_id += 1

        except Exception as e:
            self._isOpen = False
            self.logger.error(["Exception in send() - ", str(e)])
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
        _header = {"header": {
            "resource": api,
            "version": '1',
            "token": "las9kdfjaslkjdbhgsdkKbldkfbvnl?adkfjnvlk",
            "method": request_type,
            "msgtype": "REQUEST",
            "device": "GUID",
            "reqID": request_id}}

        self.logger.debug(["The header Json string:", _header])
        self.logger.debug("The data received in Json string is: \n" + data)

        if request_type.lower() == "get":
            _message = json.dumps(_header, indent=4)
        elif request_type.lower() == "post" or "put":
            # Converting the header and into a Python Dict
            _header["body"] = json.loads(data)
            # Constructing the message to be sent
            _header = json.dumps(_header, indent=4)
            _message = _header
            self.logger.debug(["The value of the message is", _message])
        else:
            _message = "Non-supported type"
        return _message

    def last_notification(self):
        return self._last_notification

if __name__ == "__main__":
    castle_conn = FrontDoorAPI("192.168.1.148")
    castle_conn.getLanguage()
