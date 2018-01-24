import sys
import re
import time
import unittest
from telnetlib import Telnet
import datetime
from BLogSetup import get_logger
logger = get_logger("BTelnet")


class TelnetClass():

    def __init__(self, host, port=23, readSymbol=None):
        """
        Constructor
        You can pass in a readSymbol that the telnetRead function will look for
        as an indicator of command finished executing
        :param host: ip address of device to establish connection with
        :param port: port number of the telnet session
        :param readSymbol: indication of when to stop reading
        :return:
        """
        self.host = host
        self.port = port
        self.readSym = readSymbol
        self.telnetCon = None
        if self.readSym == None:
            if port == 23:
                self.readSym = "#"
            elif port == 17000:
                self.readSym = "->"

    #This will establish a telnet connection but not login
    #Timesout after 30 seconds if connection is not established
    def telnet_connect(self):
        """
        Establish a telnet connection
        It will try to connect to port 17000 and execute remote_services on
        if connection refused error occurs
        :return:
        """
        logger.info(["Telnet: Connecting to ", self.host, self.port])
        try:
            self.telnetCon = Telnet(self.host, self.port, 30)
        except Exception, e:
            if hasattr(e, 'strerror'):
                error = e.strerror
            else:
                error = e.message
            if error == "Connection refused":
                self._remote_services()
                self.login("root")
            else:
                raise Exception("Could not establish a telnet connection")
        logger.info(["Telnet: Connection Established", self.host, self.port])

    #Needs to be called if using real telnet (port 23)
    def login(self, username, password=None):
        """
        Login function if you need to provide credentials
        :param username: pass username
        :param password: pass password, default=None
        :return:
        """
        logger.info("Telnet: Logging in")
        output = self.telnetCon.read_until("login: ", timeout=5)
        self.telnetCon.write(username + "\n")
        if password != None:
            self.telnetCon.write(password + "\n")
        self.telnetCon.read_until("#", timeout=5)
        return

    def telnet_write(self, command):
        """
        Write to telnet
        Will check if connection is still up each time
        and try to open up a connection if it's down
        :param command: command to write
        :return:
        """
        if isinstance(self.telnetCon.sock, int):
            logger.debug("Telnet connection is closed, will attempt to reopen it")
            self.telnetCon.open(self.host, self.port)
            self.login("root")
        self.telnetCon.write(command + "\n")
        return

    #Read output from telnet
    def telnet_read(self, timeout=2):
        """
        Read telnet until symbol is detected or
        until the timeout is hit
        :param timeout: default timeout is 2 seconds
        :return: data that was read
        """
        (rc, data) =  self.telnet_read_response(timeout=timeout)
        return data

    def telnet_read_command_response(self, command, timeout=2):
        """
        Read response to a command
        :param command: command string
        :param timeout: default timeout is 2 seconds
        :return: only data, which was read
        """
        self.telnet_write(command)
        time.sleep(1)
        resp=self.telnet_read()[len(command)+1:]
        resp=resp.rstrip()
        resp=resp.lstrip()
        return resp

    def telnet_read_response(self, timeout=2):
        """
        Read telnet until symbol is detected or
        until the timeout is hit
        :param timeout: default timeout is 2 seconds
        :return: (bool) if connection is up or down, (string) response from telnet
        """
        success = True
        data = ''
        now = time.time()
        future = now + timeout
        #sock = self.telnetCon.sock_avail()
        if isinstance(self.telnetCon.sock, int):
            logger.debug("Telnet connection is closed, will attempt to reopen it")
            try:
                self.telnetCon.open(self.host, self.port)
                self.login("root")
            except Exception, e:
                if hasattr(e, 'strerror'):
                    error = e.strerror
                else:
                    error = e.message
                if error == "Connection refused":
                    self._remote_services()
                    self.login("root")
                else:
                    success = False
                    data = e.message
                    return success, data
        while data.find(self.readSym) == -1 and time.time() <= future:
            try:
                data += self.telnetCon.read_very_eager()
            except Exception, e:
                # In OSX the exception message from python's telnet library "connection closed" begins with a lower
                # case 'c'. In other OS it may be an uppercase'C', to be interoperable with different environments we
                # check for both.
                if e.message.find("Connection closed") != -1 or \
                        e.message.find("Connection reset by peer") != -1 or\
                        e.message.find("connection closed") != -1:
                    logger.info("Connection was closed by the peer during read")
                    logger.info("Attempting to re-establish connection")
                    self._remote_services()
                    self.login("root")
                else:
                    logger.exception(e)
                    success = False
                    data = str(e)

        if self.port == 23:
            #removes "root@spotty" from data
            data = re.sub(r'root@[a-zA-Z0-9]+:[a-zA-Z0-9-!$%^&*()_+|~=`{}\[\]:";]+#', '', data)
        else:
            #removes readSymbol from data
            data = re.sub(self.readSym, '', data)
        return success, data

    def close_telnet(self):
        """
        Closes telnet session
        :return:
        """
        logger.info("Telnet: Closing Telnet session")
        try:
            self.telnetCon.close()
        except Exception, e:
            logger.exception(["Exception thrown when closing Telnet:", e.message])
        return

    def open_telnet(self, port, host):
        logger.info(["Telnet: Open Telnet session", port, host])
        self.telnetCon.open(port, host)

    '''def __exit__(self, *err):
        print "died"
        self.telnetCon.close()'''

    def _remote_services(self):
        try:
            temp = Telnet(self.host, 17000, 30)
            temp.write("\n")
            temp.write("remote_services on\n")
            time.sleep(1)
            temp.read_very_eager()
            temp.close()
            self.telnetCon = Telnet(self.host, self.port, 30)
        except Exception, e:
            logger.debug("Failed to turn on remote services")


    def send_check_one_tap_command(self,command, expected_response="OK"):
        """
        This method could be use to send one tap command using telnet session through port 17000.
        It will establish telnet connection (connect to port 17000), send command, read and verify the response.
        Close the connection after command was send.
        :param command: command to send
        :type command: string
        :param expected_response: Response to expect whe send the command
        :type expected_response: string
        :return: True if the command was send successfully, False otherwise
        """
        self.port = 17000
        self.telnet_connect()
        self.telnet_write(command)
        time.sleep(0.5)
        response = self.telnet_read()
        if response.find(expected_response) != -1 :
            logger.debug("Tap command: " + command + " was send.")
            self.close_telnet()
            return True
        else:
            logger.error("ERROR. Couldn't send tap command: " + command)
            self.close_telnet()
            return False

class TesdTelnetCommands(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        pass

    def setUp(self):
        print "\n-----------------------------------------------------------------------"
        print "Test case name:    " + self._testMethodName
        print datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
        print "------------------------------------------------------------------------"
        self.testTeln = TelnetClass("192.168.1.150", 23)
        self.testTeln.telnet_connect()
        self.testTeln.login("root")

    def tearDown(self):
        self.testTeln.close_telnet()
        pass

#    @unittest.skip("")
    def test_telnet_write(self):
        self.testTeln.telnet_write("ps")
        res = self.testTeln.telnet_read()
        self.assertTrue(len(res) > 2)

#    @unittest.skip("")
    def test_telnet_write2(self):
        self.testTeln.telnet_write("ps")
        res = self.testTeln.telnet_read()
        self.assertTrue(len(res) > 2)


    def test_telnet_conn(self):
        try:
            self.testTeln.close_telnet()
            self.testTeln.telnet_read()
            self.testTeln.telnet_write("reboot")
            time.sleep(1)
            result = self.testTeln.telnet_read()
            result = self.testTeln.telnet_read()
            result = self.testTeln.telnet_read()
            result = self.testTeln.telnet_read()
            result = self.testTeln.telnet_read()
            print result
        except Exception, e:
            print e


if __name__ == '__main__':
    unittest.main()

