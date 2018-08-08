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
Class and utilities used for interacting with the Graphite database used with Grafana
"""
import json
import socket
import time
import urllib2

from CastleTestUtils.LoggerUtils.CastleLogger import get_logger

class Graphite(object):
    """
    Graphite connection for sending messages and events.
    """

    _open = False
    _socket = socket.socket()
    headers = {'Content-type': 'text/plain'}

    def __init__(self, address, socket_port, rest_port, logger=None):
        self.address = address
        self.socket_port = socket_port
        self.rest_port = rest_port
        self.url = 'http://{}:{}/events'.format(self.address, self.rest_port)
        self.logger = logger or get_logger(__name__)
        self.logger.debug(self.__str__())

    def send_message(self, data):
        """
        Send all of the message data to the socket.

        :param data: Message data to be sent.
        :return: None
        """
        if not self._open:
            self.open()
        self.logger.debug('Sending message: {}'.format(data))
        self._socket.sendall(data)
        self.logger.debug('Message sent.')

    def send_event(self, data):
        """
        Sends all of the event data to the URL/events.

        :param data: Event data to be sent.
        :return:
        """
        self.logger.debug('Sending event: {}'.format(data))
        request = urllib2.Request(self.url, data=json.dumps(data), headers=self.headers)
        response = urllib2.urlopen(request)
        self.logger.debug('Event response: {}'.format(response.read()))

    def open(self):
        """
        Open the socket connection.

        :return: None
        """
        if not self._open:
            self._socket.connect((self.address, self.socket_port))
            self._open = True

    def close(self):
        """
        Close the socket connection.

        :return: None
        """
        if self._open:
            self._socket.close()
            self._open = False

    def __enter__(self):
        self.open()

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return exc_type is None

    def __str__(self):
        return "Graphite Connection ({}). Socket {}, HTTP {}"\
            .format(self.address, self.socket_port, self.rest_port)

    def __repr__(self):
        return "<Graphite address:{} socket_port:{} rest_port:{}>"\
            .format(self.address, self.socket_port, self.rest_port)


def generate_time_message(data, table, sub_table=None):
    """
    This function will convert a dictionary of states and times into a
    format that will be used by graphite.

    :param data: The data in dictionary format with service,
        <sub-service>, and time.
    :param table: The main table name used to store the data.
    :param sub_table: Optional sub table name below table where
        data is stored in Graphite

    :return: List of strings in Graphite format
    """
    table_name = '.'.join([table, sub_table]) if sub_table else table
    rows = []
    now = time.time()
    for key in data.keys():
        if key == 'state':
            # Special case for States. We need an added sub-table for each state
            #   and the timing is based upon when the collection started not when
            #   reboot started.
            rows = rows + generate_state_time_message(data['state'], table_name, now)
        else:
            run_time = data[key]['end'] - data[key]['start']
            row = '{table} {val} {ts}'.format(table='.'.join([table_name, key]),
                                              val=run_time, ts=now)
            rows.append(row)

    return rows


def generate_state_time_message(data, table, now):
    """
    A special Graphite row generator for State information.
    Collection of States has been started for collection start, not from reboot, or network.

    :param data: The dictionary formatted, state-parseable information.
    :param table: The name of the base table used for
    :param now: The epoch time when data is written

    :return: List of strings in Graphite format
    """
    start_time = data['start']
    rows = []
    table_name = '.'.join([table, 'state'])
    states = [key for key in data.keys() if key not in ['start', 'end']]
    for state in states:
        run_time = data[state]['first_seen'] - start_time
        row = '{table} {val} {ts}'.format(table='.'.join([table_name, state]),
                                          val=run_time, ts=now)
        rows.append(row)

    return rows
