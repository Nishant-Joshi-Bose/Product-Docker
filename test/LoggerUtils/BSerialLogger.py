#!/usr/bin/python2.7
# BUtils.py
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
This is a module used to capture Shelby logread logs
using a serial connection
"""
import os
import time
import serial
import unittest
import datetime
import traceback
import threading
import serial.tools.list_ports
from BLogSetup import get_logger
from ..SerialPortUtils.serial_controller import SerialController
from logreadBase import LogreadBase
logger = get_logger("BSerialLogger")


class BSerialLogger(LogreadBase):
    """
    Class that handles collection of logread information

    Ex:
    from BSerialLogger import BSerialLogger
    self.logreadlogger = BSerialLogger('/dev/ttyUSB0')

    # self._testMethodName is retrieved from unittest
    self.logreadlogger.start_log_collection(
                    testName=self._testMethodName,
                    path="./LOGS/"+os.path.basename(__file__).replace('.py',''),
                    grep="-i bluetooth")
    time.sleep(30)
    # Check for a specific string in logread
    assert (state self.logreadlogger.start_keyword_checking(keyword='Device Name:  Bose SoundTouch1') == True),
                                                                                 'Failed Name Change'
    #stop log collection
    self.logreadlogger.stop_log_collection()


    """
    def __init__(self, comport, serialObj=None, name=''):
        """
        :param comport: comport of the serial device ("/dev/ttyUSB0" or "COM4")
        :param serialObj: serial object that can be reused if serial connection has been already established
        :return:
        """
        super(BSerialLogger, self).__init__()
        self.zip = False
        self.keeplogging = False
        self.shouldStop = False
        self.rebootFlag = False
        self.comport = comport
        if serialObj:
            self.serialcon = serialObj
        else:
            self.serialcon = SerialController(comport, baudrate=115200, timeout=1)

    def pause_log(self):
        """
        This method will pause the serial logger from reading the serial port
        :return:
        """
        logger.info("Pausing SerialLogger")

        self.serialcon.wrtPort("\npause log echo "+time.strftime(self.time_format)+"\n")
        self.serialcon.wrtPort("\n")
        self.serialcon.wrtPort("\x03\r\n")
        time.sleep(1)
        self.keeplogging = False

    def resume_log(self):
        """
        This method will resume the serial logger
        :return:
        """
        logger.info("Resuming SerialLogger")
        self._enable_logging()
        self.serialcon.wrtPort("\nresume log echo "+time.strftime(self.time_format)+"\n")

    def get_logread(self, saveLog=True, logName=""):
        """
        Get logread captured from boot to current time
        :return: (string) logread
        """
        logger.info("Getting log of Lisa/Shelby unit")
        log = ""
        currentLen = 0
        previousLen = 1
        self._exit_cli()
        self._login_root()
        self.serialcon.wrtPort("logread\r\n")
        time.sleep(1)
        while currentLen != previousLen:
            previousLen = len(log)
            log += self.serialcon.readPort()
            currentLen = len(log)
            time.sleep(1)

        if saveLog:
            l = lambda: "LogReadFromUnit" if logName == "" else logName
            self.testname = l()
            self._create_log_dir(l())
            self._save_log(log)
        return log

    def run_shell_cmd(self, cmd):
        """
        Execute the shell command and return the result
        :return: (string)
        """
        res = ""
        currentLen = 0
        previousLen = 1
        self._exit_cli()
        self._login_root()
        self.serialcon.wrtPort(cmd + "\r\n")
        time.sleep(1)
        while currentLen != previousLen:
            previousLen = len(res)
            res += self.serialcon.readPort()
            currentLen = len(res)
            time.sleep(1)

        return res

    def _logread(self, grep, saveper):
        """
        Private method that handles collection of logs
        This method will be run in its own thread
        :param grep: (string) grep arguments
        :param saveper: (bool) save the log periodically to a log file
        :return:
        """
        try:
            self._enable_logging()
            logger.debug("#### Beginning to capture Logs ####")
            self.logreadstartEvent.set()
            response = ""
            datecom = "\necho start"+time.strftime(self.time_format)+"\n"
            self.serialcon.wrtPort(datecom)
            t = threading.currentThread()
            logger.info("#### Capturing Logs ####")
            while getattr(t, "do_run", True):
                if self.keeplogging:
                    response = self.serialcon.readPort()
                if response is not "":
                    self.log += response
                    if self._startLogCollection:
                        self._requestedlog += response
                if self.startKeywordChecking:
                    self.keywordlog += response
                if response.find("Booting from nand") != -1 or response.find("Starting kernel") != -1:
                    self.rebootFlag = True
                if (response.find("The system is going down for reboot NOW") != -1 \
                        or response.find("login:") != -1) and self.rebootFlag:
                    logger.info("#### System is rebooting ####")
                    startime = time.time()
                    future = startime + 18
                    while time.time() < future:
                        if self._login_root():
                            break
                    self.rebootFlag = False
                    self._reconnect_log()
                if saveper:
                    with open(self.filePath, 'ab') as fileop:
                        fileop.write(response)
                        self.log = ""
                time.sleep(0.2)
            datecom = "\necho end "+time.strftime(self.time_format)+"\n"
            self.serialcon.wrtPort(datecom)
            time.sleep(0.5)
            response = self.serialcon.readPort()
            self.log += response
            self.serialcon.close()
        except Exception, e:
            logger.exception("Exception thrown by logger!")
            logger.exception(e)
            traceback.print_exc()
        finally:
            if self.saveLog:
                try:
                    logger.debug("Writing logread log to a file")
                    fo = open(self.filePath, "ab")
                    fo.write(self.log)
                    fo.close()
                    if self.zip:
                        self._zip_log()
                except Exception, e:
                    logger.exception("Failed to save logread to a file!")
                    logger.exception(e)
            else:
                logger.debug("Not saving logread log!")
            self.logreadEndEvent.set()

    def _enable_logging(self):
        """
         Send a "logread -f" command via serial
        :return:
        """
        response = ""
        tries = 0
        logger.debug("#### Enabling logread ####")
        self._exit_cli()
        if self.grep is not None:
            while (tries < 10 and response.find("root@") == -1 and response.find("logread -f") == -1) \
                    or response.find("password:") != -1:
                res = self._login_root()
                if res:
                    self.serialcon.wrtPort("logread -f | grep "+self.grep+"\r\n")
                    time.sleep(1)
                    response = self.serialcon.readPort()
                    self.keeplogging = True
                    break
                else:
                    tries += 1
        else:
            while (tries < 10 and response.find("root@") == -1 and response.find("logread -f") == -1)\
                    or response.find("password:") != -1:
                res = self._login_root()
                if res:
                    self.serialcon.wrtPort("logread -f\r\n")
                    time.sleep(1)
                    response = self.serialcon.readPort()
                    self.keeplogging = True
                    break
                else:
                    tries += 1

    def _reconnect_log(self):
        """
        Reconnect with the system if serial connection is interrupted
        :return: True or False
        """
        response = False
        try:
            self.serialcon.close()
            time.sleep(1)
        except Exception, e:
            logger.exception("Couldn't close serial connection")
            logger.exception(e)
        try:
            self.serialcon.open()
            time.sleep(2)
            response = True
        except Exception, e:
            logger.exception("Couldn't open serial connection")
            logger.exception(e)
            response = False
        if response:
            return self._enable_logging()
        else:
            return response

    def _login_root(self):
        """
        Login to root
        :return: True or False
        """
        count = 0
        login = False
        self.serialcon.wrtPort("\n")
        self.serialcon.wrtPort("\x03\r\n")
        time.sleep(0.5)
        response = self.serialcon.readPort()
        while response.find("login:") != -1 or count < 5:
            self.serialcon.wrtPort("root\n")
            time.sleep(0.2)
            response = self.serialcon.readPort()
            if response.find("root@") != -1:
                logger.info("Serial logged in as root *****")
                login = True
                break
            else:
                logger.error("Couldn't login to root")
            count += 1
            time.sleep(0.5)
        return login

    def _exit_cli(self):
        """
        Check CLI and exit
        :return: True or False
        """
        count = 0
        exit = False
        self.serialcon.wrtPort("\n")
        time.sleep(0.5)
        response = self.serialcon.readPort()
        if response.find("->") != -1:
            self.serialcon.wrtPort("\n")
            time.sleep(0.200)
            self.serialcon.wrtPort("local_services on\n")
            time.sleep(0.200)
            self.serialcon.wrtPort("\x03")
            time.sleep(0.200)
            self.serialcon.wrtPort("e\n")
            time.sleep(2)
            response = self.serialcon.readPort()
            if response.find("root@") != -1 or response.find("login:") != -1:
                logger.debug("Exited from CLI")
                exit = True
            else:
                logger.error("Couldn't exit cli")
                exit = False
        return exit


class TestBSerialLogger(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.serialLogger = BSerialLogger('/dev/ttyUSB0')

    def setUp(self):
        print "\n-----------------------------------------------------------------------"
        print "Test case name:    " + self._testMethodName
        print datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
        print"------------------------------------------------------------------------"

    @classmethod
    def tearDownClass(self):
        pass

    def tearDown(self):
        self.serialLogger.stop_log_collection()

    @unittest.skip("")
    def test_logread(self):
        """
        Test serial logread logger functions
        :return:
        """
        self.assertEqual(self.serialLogger.start_log_collection(testName=self._testMethodName, testClass="b 0 b"),
                         True,
                         msg="Failed to start BSerialLogger")
        time.sleep(10)
        self.serialLogger.stop_log_collection()

    @unittest.skip("")
    def test_pause_resume(self):
        """
        Test pause resume functionality of BSerialLogger
        :return:
        """
        self.assertEqual(self.serialLogger.start_log_collection(testName=self._testMethodName, testClass="b 0 b"),
                         True,
                         msg="Failed to start BSerialLogger")
        time.sleep(10)
        self.serialLogger.pause_log()
        time.sleep(10)
        self.serialLogger.resume_log()
        time.sleep(10)
        self.serialLogger.stop_log_collection()

    @unittest.skip("")
    def test_keyword_checking(self):
        """
        Test keyword checking
        :return:
        """
        self.assertEqual(self.serialLogger.start_log_collection(testName=self._testMethodName, testClass="b 0 b"),
                         True,
                         msg="Failed to start BSerialLogger")
        self.assertEqual(self.serialLogger.start_keyword_checking("Bluetooth"),
                         True,
                         msg="Failed to find the give keyword")
        self.serialLogger.stop_log_collection()

    def test_capture_log_collection(self):
        self.assertEqual(self.serialLogger.start_log_collection(testName=self._testMethodName, testClass="LogCollection"),
                         True,
                         msg="Failed to start BSerialLogger")
        self.serialLogger.start_temp_log()
        time.sleep(3)
        log = self.serialLogger.get_temp_log()
        self.assertTrue(len(log) > 5, msg="Temp log was not generated within 3 seconds")
        self.serialLogger.stop_temp_log()
        time.sleep(3)
        self.assertTrue(len(self.serialLogger._requestedlog) == 0,
                        msg="Requester log should be empty after stop_temp_log")

def serial_file_logger(modulename):
    log_format = '%(ascitime)s: %(message)s'
    datetimeformat = datetime.datetime.now().strftime("%Y_%m_%d-%H_%M_%S.%f%p")
    file_name = os.path.join(".", "{0}_{1}.txt".format(modulename, datetimeformat))
    handle = open(file_name, "wb")
    return handle

class SerialLoggingUtil():
    def __init__(self, modulename, comportstr, baudrate=115200):
        self.stopflag = 0
        self.databuf = []
        self.datacaptureflag = 0
        self.modulename = modulename
        self.comportstr = comportstr
        self.baudrate = baudrate
        if self.modulename != None:
            self.filehandle = serial_file_logger(self.modulename)
        self.comporthandle = connecttoCOMPort(self.comportstr, self.baudrate)

    def execute_command(self, command):
        """
        To execute command on serial port

        :param command: command to be executed on serial port;Ex:'lb history'
        :return: string output of command will be returned
        """
        console_output = []
        if self.comporthandle.isOpen():
            self.comporthandle.write(command + '\r\n')
            time.sleep(1)
            w = self.comporthandle.inWaiting()
            output = self.comporthandle.read(w)
            console_output = [line for line in output.split('\r\n')]
        return console_output

class myThread(threading.Thread):
    def __init__(self, handle):
        threading.Thread.__init__(self)
        self.handle = handle
    def run(self):
        captureseriallog(self.handle)

class TestRecoveryTest():
    """Helper methods for Recovery Test."""

    BAUDRATES = (50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800,
                 9600, 19200, 38400, 57600, 115200)

    def getCOMPortList(self):
        """\
        Get the list of COM PORTS in the system
        """
        list = serial.tools.list_ports.comports()
        connected = []
        for element in list:
            connected.append(element.device)
        print "Connected COM ports: " + str(connected)
        return connected

def connecttoCOMPort(comport, baudrate):
    ser = serial.Serial(port=comport, baudrate=baudrate, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=0)
    print "ConnecttoComPort - %d", ser
    return ser

def stopseriallog(handle):
    print "Stop Serial log capture"
    handle.stopflag = 1
    return handle.stopflag

def startdatacapture(handle):
    handle.databuf = []
    handle.datacaptureflag = 1

def stopdatacapture(handle):
    handle.datacaptureflag = 0

def getdatabuf(handle):
    return handle.databuf

def checkLPMReboot(handle, rebootstr='Eddie Bootloader'):
    """
        Checks if the LPM reboot
        returns True if LPM reboot
        returns False if LPM did not reboot
    """
    returnValue = False
    for items in handle.databuf:
        if rebootstr in items:
            print "LPM rebooted"
            returnValue = True

    # Only for user notification
    if not returnValue:
        print "LPM did not reboot"

    return returnValue

def checkAPQReboot(handle, rebootstr='Log Type: B - Since Boot(Power On Reset),  D - Delta,  S - Statistic'):
    """
        Checks if the APQ reboot
        returns True if APQ reboot
        returns False if APQ did not reboot
    """
    returnValue = False
    for items in handle.databuf:
        if rebootstr in items:
            print "APQ rebooted"
            returnValue = True

    # Only for user notification
    if not returnValue:
        print "APQ did not reboot"

    return returnValue

def captureseriallog(handle):
    seq = []
    joined_seq = []
    handle.comporthandle.reset_input_buffer()
    handle.comporthandle.reset_output_buffer()

    handle.stopflag = 0

    while True:
        for c in handle.comporthandle.read():
            if c == '\n':
                handle.filehandle.write("%s" % joined_seq)
                if handle.datacaptureflag == 1:
                    handle.databuf.append(joined_seq)
                seq = []
                break
            else:
                seq.append(c)  # convert from ANSII
                joined_seq = ''.join(str(v) for v in seq)  # Make a string from array

        # print stopflag
        if handle.stopflag == 1:
            print "Stop flag is 1"
            break

if __name__ == '__main__':
    unittest.main()
