#!/usr/bin/python2.7
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
"""
This is a module used to capture Castle logread logs
using an adb connection
This module requires adb client installation
"""

import traceback
import threading
import subprocess
import re
import json
from CastleTestUtils.FrontDoorAPI.FrontDoorAPI import FrontDoorAPI
from BLogSetup import get_logger
from logreadBase import LogreadBase
from CastleTestUtils.RivieraUtils.ADBUsbClient import ADBUsbClient
logger = get_logger("LogreadLogger")


class LogreadAdb(LogreadBase):
    """
    Class that handles collection of logread information via ADB
    """
    def __init__(self, hostAddr):
        """
        :param hostAddr: ip address of Castle device
        :return:
        """
        super(LogreadAdb, self).__init__(hostAddr)
        self.adb_client = ADBUsbClient()  # adb client object to send command to device
        self.proc = None  # instance of subprocess for adb shell command

    def get_logread(self, saveLog = True, logName=""):
        """
        Get logread captured from boot to current time
        :return: (string) logread log
        """
        logger.info("Getting logs from Device under Test")
        log = ""
        current_length = 0
        previous_length = 1
        self.proc = self.adb_client.get_shell_cmd_handle("logread")
        line_iter = iter(self.proc.stdout.readline, '')
        time.sleep(1)
        while current_length != previous_length:
            previous_length = len(log)
            try:
                line = next(line_iter)
            except StopIteration:
                print "Stopped"
                line = ""
            log += line
            current_length = len(log)
        if saveLog:
            l = lambda: "LogReadFromUnit" if logName == "" else logName
            self.testname = l()
            self._create_log_dir(l())
            self._save_log(log)
        self.proc.kill()
        return log

    def _logread(self, grep, saveper):
        """
        Private method that handles collection of logs
        This method will be run in its own thread
        :param grep_arguments: (string) grep argument
        :param save_periodically: (bool) save the log periodically to a log file
        :return:
        """
        line = ""
        try:
            line_iter = self._enable_logging()
            logger.debug("#### Beginning to capture Logs ####")
            self.logreadstartEvent.set()
            t = threading.currentThread()
            logger.info("#### Capturing Logs ####")
            try:
                line = next(line_iter)
            except StopIteration as e:
                logger.exception("#### Exception thrown by logger ####")
                logger.exception(e)
                traceback.print_exc()
            while getattr(t, "do_run", True):
                response = line
                if response:
                    self.log += response
                    if self._startLogCollection:
                        self._requestedlog += response
                else:
                    if not self.is_dut_ready():
                        raise LogreadAdbException("Device didn't reconnect to the previously connected network")
                    else:
                        self._reconnect_log()
                if self.startKeywordChecking:
                    self.keywordlog += response
                if not self._watch_adb_devices():
                    logger.info("System is rebooting")
                    time.sleep(30)
                    if not self.is_dut_ready():
                        raise LogreadAdbException("Device didn't reconnect to the previously connected network after reboot")
                    else:
                        line_iter = self._reconnect_log()
                if saveper:
                    with open(self.filePath, 'ab') as fileop:
                        fileop.write(response)
                        self.log = ""
                line = next(line_iter)
                time.sleep(0.2)

            self._disable_logging()
            self._clear_logread_buffer()

        except Exception as e:
            logger.exception("#### Exception thrown by logger ####")
            logger.exception(e)
            traceback.print_exc()
        finally:
            if self.saveLog:
                try:
                    logger.debug("Writing logread log to a file")
                    with open(self.filePath, 'ab') as open_file:
                        open_file.write(self.log)
                    if self.zip:
                        self._zip_log()
                except Exception as e:
                    logger.exception("Failed to save logread to a file!")
                    logger.exception(e)
            else:
                logger.debug("Not saving logread log!")
            self.logreadEndEvent.set()

    def _enable_logging(self):
        """
        Send a "logread -f" command to python adb.
        :return: iterator of each line of output
        """
        if not self.grep:
            self.proc = self.adb_client.get_shell_cmd_handle("logread -f")
            line_iter = iter(self.proc.stdout.readline, '')
            return line_iter
        else:
            self.proc = self.adb_client.get_shell_cmd_handle("logread -f | grep " + self.grep)
            line_iter = iter(self.proc.stdout.readline, '')
            return line_iter

    def _disable_logging(self):
        """
        Terminate subprocess of logread
        :return:
        """
        self.proc.kill()

    def _clear_logread_buffer(self):
        """
        Clear logread buffer
        :return:
        """
        status, resp = self.adb_client.send_shell_command("/etc/init.d/syslog restart")
        return status, resp

    def _reconnect_log(self):
        """
        Reconnect with the system if connection drops
        :return: True or False
        """
        self.adb_client = ADBUsbClient()
        line_iter = self._enable_logging()
        return line_iter

    def _watch_adb_devices(self):
        """
        watch if the adb device is up
        :return: True or False
        """
        try:
            response = subprocess.check_output("adb devices".split())
            logger.info("For command: {} response: {}".format("adb devices", response))
        except Exception as e:
            return False
        pattern = '.*\tdevice'
        match = re.search(pattern, response)
        if match:
            logger.info("ADB device is up")
            return True
        else:
            return False

    def is_dut_ready(self):
        """
        send a /system/info api to frontdoor to see if the DUT is ready
        :return: True if the device is up
        """
        status = self._watch_adb_devices()
        if status:
            frontdoor = FrontDoorAPI(self.host)
            system_info = frontdoor.send_api("/system/info")
            system_info = json.loads(system_info)
            try:
                variant = system_info["body"]["variant"]
                status = True
            except KeyError as e:
                status = False
        return status


class LogreadAdbException(Exception):
    pass

########################################################
# Unit tests to test the methods in logreadAdb class
########################################################
import unittest
import os
import time


class TestLoggerAdb(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.dev_logger = LogreadAdb("192.168.0.84")

    def test_get_log(self):
        """
        unittest to verify method- test start_log_collection(), stop_log_collection() methods

        :return:
        """
        self.dev_logger.start_log_collection(__name__, path="adb_logger")
        time.sleep(2)
        self.assertTrue(True)
        self.dev_logger.stop_log_collection(__name__, zip=False)

    def test_get_log_zip(self):
        """
        unittest to verify method- test zip variable (archive log) in top_log_collection() methods

        :return:
        """
        self.dev_logger.start_log_collection(__name__, path="adb_logger")
        time.sleep(2)
        self.assertTrue(True)
        self.dev_logger.stop_log_collection(__name__, zip=True)

    def test_get_log_grep(self):
        """
        unittest to verify method- test grep variable in start_log_collection() methods

        :return:
        """
        self.dev_logger.start_log_collection(__name__, grep="Ping", path="adb_logger")
        time.sleep(4)
        self.assertTrue(True)
        self.dev_logger.stop_log_collection(__name__, zip=False)

    def test_get_tem_log(self):
        """
        unittest to verify method- test start_tem_log(), get_tem_log(), stop_tem_log() methods

        :return:
        """
        self.dev_logger.start_log_collection(__name__, path="adb_logger")
        time.sleep(4)
        self.assertTrue(True)
        self.dev_logger.start_temp_log()
        time.sleep(1)
        log = self.dev_logger.get_temp_log()
        self.dev_logger.stop_temp_log()
        self.dev_logger.stop_log_collection(__name__, zip=False)

    def test_is_dut_ready(self):
        """
        unittest to verify method- test is_dut_ready() method

        :return:
        """
        resp = self.dev_logger.is_dut_ready()
        self.assertTrue(resp)

    def test_keyword_checking(self):
        """
        unittest to verify method- start_keyword_checking method

        :return:
        """
        self.dev_logger.start_log_collection(__name__, path="adb_logger")
        foundkeyword = self.dev_logger.start_keyword_checking(keyword="Ping")
        time.sleep(10)
        self.dev_logger.stop_log_collection(__name__, zip=False)
        self.assertTrue(foundkeyword)

    def test_get_logread(self):
        """
        unittest to verify method- test start_log_collection(), stop_log_collection() methods

        :return:
        """
        self.dev_logger.get_logread(saveLog=True, logName="")

    def test_reconnect_log(self):
        self.dev_logger.start_log_collection(__name__, path="adb_logger")
        time.sleep(5)
        os.system("killall adb")
        time.sleep(10)
        self.dev_logger.stop_log_collection(__name__, zip=False)

if __name__ == '__main__':
    unittest.main()
