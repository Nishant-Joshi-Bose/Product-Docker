# -*- coding: utf-8 -*-
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:     COPYRIGHT 2015 BOSE CORPORATION ALL RIGHTS RESERVED.
#                 This program may not be reproduced, in whole or in part in any
#                 form or any means whatsoever without the written permission of:
#                     BOSE CORPORATION
#                     The Mountain,
#                     Framingham, MA 01701-9168
#
"""
:Abstract: This utility gathers RSS values of memory usage for a list of processes and DUTs.
           The output will be saved to a CSV file for each DUT.
"""
__author__ = 'dg71532'

import os
import csv
import time
import platform

from threading import Thread
from ..LoggerUtils.BTelnet import TelnetClass

class MemoryUsageCapture(object):
    """
    Example below:
    dutlist = ["192.168.1.205"]
    ProcessList = ["APServer", "BoseApp "]
    meml = MemoryUsageCapture(dutlist, ProcessList)
    meml.start_capturing()
    time.sleep(60)
    meml.stop_capturing()

    """
    def __init__(self, dutList, processList):
        """
        :param dutList: ex. ["192.168.1.205", "192.168.1.209"]
        :param processList: ex. ["APServer", "BoseApp "]
        :param captureInterval: interval of memory usage retrieval
        """
        self.dutList = dutList
        self.processList = processList
        if len(processList) == 0:
            raise Exception("Process list is empty!")
        elif len(dutList) == 0:
            raise Exception("Dut list is empty!")
        self.threadAlive = None
        self.threadPool = []
        self.filePath = None

    def start_capturing(self, path=None, captureInterval=10):
        """
        Start capturing memory RSS
        :param path: where to save the CSV files with results
        :param captureInterval: how often to capture memory RSS
        :return:
        """
        if path is None:
            path = os.path.join(os.path.dirname(os.path.realpath(__file__)), self.__class__.__name__)
            if not os.path.exists(path):
                os.makedirs(path)
        else:
            if not os.path.exists(path):
                os.makedirs(path)
        self.filePath = path

        print "Starting Memory Leak capture"
        self.threadAlive = True
        self.threadPool = []
        try:
            for dut in self.dutList:
                testTeln = TelnetClass(dut, 23)
                testTeln.telnet_connect()
                testTeln.login("root")
                testThread = Thread(target=self._get_top, args=(testTeln, dut, self.processList, captureInterval,))
                testThread.daemon = True
                testThread.start()
                self.threadPool.append(testThread)
        except (KeyboardInterrupt, SystemExit):
            self.stop_capturing()

    def stop_capturing(self):
        """
        Stop capturing memory rss values
        :return: None
        """
        print "Stopping Memory Leak capture"
        self.threadAlive = False
        for threads in self.threadPool:
            threads.join(10)

    def _get_top(self, testTeln, ip, processList, timeInterval=10,):
        """
        Get memory RSS using top command and save to CSV file
        :param testTeln: telnet object
        :param ip: ip address of the DUT
        :param processList: list of processes to capture memory usage for
        :param timeInterval: how often to capture
        :return: None
        """
        print "Get top started!"
        StatList = [[] for _ in range(len(processList))]
        DiffList = [[] for _ in range(len(processList))]
        TimeList = []
        timeCapture = ''
        result = {}
        if platform.system() == "Windows":
            time_format = "%Y-%m-%d-%H-%M-%S"
        else:
            time_format = "%F_%H-%M-%S"

        outputFile = os.path.join(self.filePath, 'memory_leak_'+ ip +'_'+time.strftime(time_format)+'.csv')

        with open(outputFile, 'wb') as csvfile:
            csv_writer = csv.writer(csvfile, delimiter=',')
            headerList = []
            headerList.append("Date")
            headerList.extend(processList)
            csv_writer.writerow(headerList)

        try:
            while self.threadAlive:
                searchString = "date; top -b -m -n 1 | grep "
                for process in processList:
                    searchString += "-e " + str(process) + " "
                testTeln.telnet_write(searchString)
                result = testTeln.telnet_read()

                parsedLine = result.split('\r\n')
                timeCapture = parsedLine[1]
                result = {}
                result['time'] = timeCapture
                for line in parsedLine:
                    if line.find("grep") >= 0:
                        continue
                    for x in range(len(processList)):
                        if processList[x] in line:
                            result[processList[x]] = {}
                            strippedString = line.split()
                            if strippedString[3].isdigit():
                                result[processList[x]]['mem'] = int(strippedString[3])
                                #StatList[x] = int(strippedString[3])

                for x in range(len(processList)):
                    print "------------ " + ip
                    print processList[x]
                    print result[processList[x]]['mem']
                with open(outputFile, 'ab') as csvfile:
                    csv_writer = csv.writer(csvfile)
                    rowList = [result['time']]
                    for x in range(len(processList)):
                        rowList.append(result[processList[x]]['mem'])
                    csv_writer.writerow(rowList)
                time.sleep(timeInterval)
        except Exception, e:
            print e
        finally:
            testTeln.close_telnet()
