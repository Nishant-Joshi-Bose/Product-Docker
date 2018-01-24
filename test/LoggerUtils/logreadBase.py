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
This is a base module used to capture logread logs
"""
import os
import re
import time
import tarfile
import platform
import requests
import threading
from BLogSetup import get_logger
logger = get_logger("LogreadLogger")


class LogreadBase(object):
    """
    Base class that handles collection of logread information
    """
    def __init__(self, hostAddr=None):
        """
        :param hostAddr: ip address of Shelby device
        :return:
        """
        self._requestedlog = ""
        self._startLogCollection = False
        self.grep = None
        self.filePath = None
        self.testname = ""
        self.host = hostAddr
        self.log = ""
        self.keywordlog = ""
        self.saveLog = True
        self.zip = True
        self.startKeywordChecking = False
        self.keywordFound = False
        self.logreadstartEvent = threading.Event()
        self.logreadEndEvent = threading.Event()
        self._log_thread = None
        if platform.system() == "Windows":
            self.time_format = "%Y-%m-%d-%H-%M-%S"
        else:
            self.time_format = "%F_%H-%M-%S"

    def start_log_collection(self,
                             testName,
                             testClass=None,
                             path=None,
                             grep=None,
                             saveperiodically=False):
        """
        (MUST BE CALLED FIRST) The main method to be called to start logread logging
        :param testName: this will be used the name of the log file
        :param testClass: this will be the parent directory of the log file,
                otherwise it will be called "LogreadLogger" by default
        :param path: specific path where the logs will be saved
        :param grep: used to grep logread to get desired output
        :param saveperiodically: periodically write the logs to a file
        :return: True or False
        """
        if self._log_thread and self._log_thread.do_run:
            raise Exception('_log_thread already running!')
        try:
            self.logreadstartEvent.clear()
            self.log = ""
            self.testname = testName
            # If path to the log folder is not provided, one will be created based on testClass name if one is provided
            # and log file name based on testName
            if path is None:
                path = os.path.dirname(os.path.realpath(__file__))
                if testClass:
                    path = os.path.join(path, testClass)
                else:
                    path = os.path.join(path, self.__class__.__name__)
                if not os.path.exists(path):
                    os.makedirs(path)

                self.filePath = os.path.join(path, testName + "_" + time.strftime(self.time_format) + ".txt")
            else:
                if not os.path.exists(path):
                    os.makedirs(path)
                self.filePath = os.path.join(path, testName + "_"+time.strftime(self.time_format)+".txt")

            logger.debug("#### Starting logger ####")
            self.grep = grep
            if self._log_thread is None:
                pass
            else:
                if self._log_thread.isAlive():
                    self._log_thread.do_run = False
                    self._log_thread.join(30)
            self._log_thread = threading.Thread(target=self._logread, args=(grep, ),
                                                kwargs={'saveper': saveperiodically})
            self._log_thread.do_run = True
            self._log_thread.daemon = True
            self._log_thread.start()
            self.logreadstartEvent.wait(10)
            return True
        except Exception, e:
            logger.exception(e)
            return False

    def stop_log_collection(self, saveLog=True, zip=True):
        """
        (MUST BE CALLED LAST) Tells the logread logging thread to stop collecting logs
        :param saveLog: by default logs will be saved
                        but you can pass False to prevent the log from being saved
        :return:
        """
        if not self._log_thread and not self._log_thread.do_run:
            raise Exception("_log_thread is not running. stop_log_collection() must be called last!")
        logger.debug("#### Stopping Logger ####")
        self.saveLog = saveLog
        self.zip = zip
        self._log_thread.do_run = False
        self.stop_temp_log()
        self.logreadEndEvent.wait(10)
        self.logreadEndEvent.clear()
        if self._log_thread:
            self._log_thread.join(10)

    def start_keyword_checking(self, keyword=None, regularExpression=None, timeout=10):
        """
        This method tells the currently running logread logging thread to temporarily
        start outputting to self.keywordlog
        Check self.keywordlog if it contains the specified keyword
        :param keyword: a string that will be searched in the logread output
        :param regularExpression: a string that contains pattern to search in the logread output
        :param timeout: (int) in seconds
        :return: True or False to whether the keyword was found
        """
        logger.debug("#### Looking for Keyword #####")
        self.startKeywordChecking = True
        self.keywordFound = False
        reobject = None
        if keyword and not keyword.isspace():
            logger.debug("Using simple string")
            start_time = time.time()
            while time.time()-start_time < timeout:
                if self.keywordlog.find(keyword) != -1:
                    self.keywordFound = True
                    break
            return self.keywordFound

        elif regularExpression and not regularExpression.isspace():
            logger.debug("Using regular expression")
            start_time = time.time()
            while time.time()-start_time < timeout:
                reobject = re.search(regularExpression, self.keywordlog)
                if reobject:
                    reobject = reobject.group(0)
                    logger.debug(reobject)
                    self.keywordFound = True
                    break
        else:
            raise Exception('Check your keyword or regular expression when using start_keyword_checking() method')
        if not self.keywordFound:
            logger.debug(self.keywordlog)
        self.startKeywordChecking = False
        self.keywordlog = ""
        return self.keywordFound

    def start_temp_log(self):
        """
        This method will cause the logread to be saved to
        self_requestedlog.
        To retrieve the log you MUST call get_temp_log
        :return: None
        """
        self._startLogCollection = True

    def get_temp_log(self):
        """
        This will return all of the logs collected after start_log_collection
        method has been called.
        Call stop_temp_log when you're done with capturing log collection
        :return: (string) current logread
        """
        returnlog = self._requestedlog
        self._requestedlog = ""
        return returnlog

    def stop_temp_log(self):
        """
        Method used to stop log collection
        :return: None
        """
        self._startLogCollection = False
        self._requestedlog = ""

    def get_logread(self, saveLog=True, logName=""):
        """
        Get logread captured from boot to current time
        :return: (string) logread
        """

    def _save_log(self, log):
        try:
            logger.debug("Writing logread log to a file")
            fo = open(self.filePath, "ab")
            fo.write(log)
            fo.close()
            self._zip_log()
        except (OSError, IOError) as e:
            logger.exception("Failed to save logread to a file!")
            logger.exception(e)

    def _create_log_dir(self, logname, path=None):
        if not path:
            path = os.path.dirname(os.path.realpath(__file__))
            path = os.path.join(path, "Logread")
            if not os.path.exists(path):
                os.makedirs(path)
            self.filePath = os.path.join(path, logname+"_"+time.strftime(self.time_format)+".txt")

        else:
            # if path is specified, save logs into that directory
            if not os.path.exists(path):
                os.makedirs(path)
            self.filePath = os.path.join(path, logname+"_"+time.strftime(self.time_format)+".txt")

    def _logread(self, grep, saveper):
        """
        Private method that handles collection of logs
        This method will be run in its own thread
        :param grep: (string) grep arguments
        :param saveper: (bool) save the log periodically to a log file
        :return:
        """
        raise NotImplementedError("To be implemented")

    def _zip_log(self):
        """
        Zip the current log and delete the file afterwards
        :return:
        """
        prevdir = os.getcwd()
        try:
            self.filePath = os.path.abspath(self.filePath)
            dir = os.path.dirname(self.filePath)
            logger.debug(['Making Archive in:', dir])
            file = os.path.basename(self.filePath)
            tar = tarfile.open(os.path.join(dir, self.testname+"_"+time.strftime(self.time_format)+".tar.gz"), "w:gz")
            tar.add(self.filePath, arcname=file)
            tar.close()
            os.remove(self.filePath)
        except Exception, e:
            logger.exception("Exception during compression of log file!")
            logger.exception(e)

    def _enable_logging(self):
        """
         Send a "logread -f" command via telnet
        :return:
        """
        raise NotImplementedError("To be implemented")

    def _reconnect_log(self):
        """
        Reconnect with the system if connection drops
        :return: True or False
        """
        raise NotImplementedError("To be implemented")

    def is_dut_ready( self, url_str, timeout=30*5):
        """
        check if the device under test is ready by POSTing a request and
        reading a response.  if the DUT is not up, requests will throw a fit,
        and so we ignore it for a while and keep looping.  But if GET fails,
        we have a problem.

        :param url_str: URL string of the device, e.g.
                        http://foo/bar:8090
        :param timeout: time to wait for the system to reboot before giving up
        :return: True if the device is up
        """
        (rc, xml_str) = self.is_dut_ready_resp(url_str, timeout)
        return rc

    def is_dut_ready_resp(self, url_str, timeout=30*5):
        """
        check if the device under test is ready by POSTing a request and
        reading a response.  if the DUT is not up, requests will throw a fit,
        and so we ignore it for a while and keep looping.  But if GET fails,
        we have a problem.
        Also returns an /info response string:

        (rc, xml_str) = self.is_dut_ready_resp( url_str )

        :param url_str: URL string of the device, e.g.
                        http://foo/bar:8090
        :param timeout: time to wait for the system to reboot before giving up
        :return: True and a data string if OK, False and an empty string otherwise
        """
        ustr = url_str + "/info"
        logger.debug(ustr)
        # sm2 can have up to 4+ min delay if connected via USB and Ethernet
        for n in range(timeout):
            try:
                resp = requests.get(ustr, timeout=2.0)
                logger.debug("GET response: %s", resp.text)
                m = re.search(r'serialNumber', resp.text)
                if not m:
                    logger.debug("=(%d) no valid reponse to GET, continue.." % n)
                    time.sleep(2.0)
                    continue
                else:
                    # if we found the string, the system is ready
                    logger.debug("GET ok")
                    return True, resp.text
            except requests.exceptions.RequestException as e:
                # if the ustr is unreachable, we get this exception
                logger.debug("Exception on GET after POST.")
                time.sleep(2.0)
                continue

        logger.error("Failed to establish communication with " + url_str)
        return False, ""
