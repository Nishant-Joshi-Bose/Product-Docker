# graphite.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

"""
Utilities for posting data to a Graphite server.
"""
import json
import socket
import time
import urllib2

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

class GraphiteUtils(object):
    """
    Tool for interacting with the Memory Consumption Graphite data storage.
    """

    def __init__(self, carbonserver, carbonport, carbonhttpport, logger=None):
        self.logger = logger or get_logger(__name__)
        self.CARBON_SERVER = carbonserver
        self.CARBON_SERVER_PORT = carbonport
        self.CARBON_SERVER_HTTP_PORT = carbonhttpport

    def send_message(self, message):
        """
        Sends messages to the Graphite Database using a websocket.
        N.B. This will send the message to two servers.

        :param message: The message in string format that will be sent.
        :return: None
        """
        print('{0}: Sending Message:\n{1}'.format(time.strftime("%c"), message))
        sock = socket.socket()
        sock.connect((self.CARBON_SERVER, self.CARBON_SERVER_PORT))
        sock.sendall(message)

        print("Connected to Websocket at {}.".format(self.CARBON_SERVER))
        sock.close()

    def event_message(self, message):
        """
        Sends messages to the Graphite Database using a REST event.
        N.B. This will send the message to two servers.

        :param message: The message in string format that will be sent.
        :return: None
        """
        url = 'http://{}:{}/events'.format(self.CARBON_SERVER, self.CARBON_SERVER_HTTP_PORT)
        self.logger.info(message)
        req = urllib2.Request(url, data=json.dumps(message), headers={'Content-type': 'text/plain'})
        response = urllib2.urlopen(req)
        self.logger.info("event push : " + response.read())

    def graphite_push_data(self, message):
        """
        Wrapper for pushing data messages to this Graphite instance using a websocket.

        :param message: The message in string format that will be sent.
        :return:
        """
        try:
            self.send_message(message)
        except IOError as exception:
            self.logger.error("Graphite Push Data Unsuccessful - %s" % str(exception))
            assert False, "Graphite Push Data Unsuccessful - %s" % str(exception)

    def graphite_push_event(self, eventdata):
        """
        Wrapper for pushing data to this Graphite instance using an HTTP REST.

        :param eventdata: The event data in string format that will be posted to the database.
        :return:
        """
        try:
            self.event_message(eventdata)
        except IOError as exception:
            self.logger.error("Graphite Push Event Unsuccessful - %s" % str(exception))
            assert False, "Graphite Push Event Unsuccessful - %s" % str(exception)
