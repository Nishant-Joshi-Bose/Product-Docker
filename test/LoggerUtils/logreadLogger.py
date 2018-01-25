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
using a telnet connection
"""
import time
import traceback
import threading
from BTelnet import TelnetClass
from BLogSetup import get_logger
from logreadBase import LogreadBase
logger = get_logger("LogreadLogger")


class LogreadLogger(LogreadBase):
    """
    Class that handles collection of logread information via telnet
    """
    def __init__(self, hostAddr):
        """
        :param hostAddr: ip address of Shelby device
        :return:
        """
        super(LogreadLogger, self).__init__(hostAddr)
        self.log_telnetcon = None

    def get_logread(self, saveLog = True, logName=""):
        """
        Get logread captured from boot to current time
        :return: (string) logread
        """
        """
        Get logread captured from boot to current time
        :return: (string) logread
        """
        logger.info("Getting log of Lisa/Shelby unit")
        log = ""
        currentLen = 0
        previousLen = 1
        self.log_telnetcon.telnet_write("logread")
        time.sleep(1)
        while (currentLen != previousLen):
            previousLen = len(log)
            log += self.log_telnetcon.telnet_read()
            currentLen = len(log)
            time.sleep(1)

        if saveLog:
            l = lambda: "LogReadFromUnit" if logName == "" else logName
            self.testname = l()
            self._create_log_dir(l())
            self._save_log(log)
        return log

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
            response = ""
            datecom = "echo start "+time.strftime(self.time_format)
            self.log_telnetcon.telnet_write(datecom)
            self.logreadstartEvent.set()
            t = threading.currentThread()
            logger.info("#### Capturing Logs ####")
            while getattr(t, "do_run", True):
                success, response = self.log_telnetcon.telnet_read_response()
                if success == False:
                    logger.debug("Read failed, most likely connection is down. Need to wait for system to reconnect")
                    if self.is_dut_ready('http://'+self.host+':8090') == False:
                        raise Exception("Device didn't reconnect to the previously connected network")
                    else:
                        self._reconnect_log()
                if response is not "":
                    self.log += response
                    if self._startLogCollection:
                        self._requestedlog += response
                if self.startKeywordChecking:
                    self.keywordlog += response
                if response.find("The system is going down for reboot NOW") != -1:
                    logger.info("System is rebooting")
                    time.sleep(30)
                    if self.is_dut_ready('http://'+self.host+':8090') == False:
                        raise Exception("Device didn't reconnect to the previously connected network after reboot")
                    else:
                        self._reconnect_log()
                if saveper:
                    with open(self.filePath, 'ab') as fileop:
                        fileop.write(response)
                        self.log = ""
                time.sleep(0.2)

            datecom = "echo end "+time.strftime(self.time_format)
            self.log_telnetcon.telnet_write(datecom)
            time.sleep(0.5)
            response = self.log_telnetcon.telnet_read()
            self.log += response
            self.log_telnetcon.close_telnet()

        except Exception, e:
            logger.exception("#### Exception thrown by logger ####")
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
         Send a "logread -f" command via telnet
        :return:
        """
        response = ""
        tries = 0
        logger.debug("#### Enabling logread ####")
        self.log_telnetcon = TelnetClass(self.host, 23)
        self.log_telnetcon.telnet_connect()
        self.log_telnetcon.login("root")
        if self.grep is not None:
            while len(response) < 1 and tries <5 :
                self.log_telnetcon.telnet_write("logread -f | grep "+self.grep)
                time.sleep(1)
                response = self.log_telnetcon.telnet_read()
                tries += 1
        else:
            while len(response) < 1 and tries <5 :
                self.log_telnetcon.telnet_write("logread -f")
                time.sleep(1)
                response = self.log_telnetcon.telnet_read()
                tries += 1
        if tries >= 4:
            logger.error("Most likely failed to start logread capture!")

    def _reconnect_log(self):
        """
        Reconnect with the system if connection drops
        :return: True or False
        """
        response  = False
        try:
            self.log_telnetcon.close_telnet()
            time.sleep(1)
        except Exception, e:
            logger.exception("Couldn't close telnet connection")
        try:
            self.log_telnetcon._remote_services()
            time.sleep(2)
            response = True
        except Exception, e:
            logger.exception("Couldn't open telnet connection")
            response = False
        if response:
            self._enable_logging()

