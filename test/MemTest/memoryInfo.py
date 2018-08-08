# memoryInfo.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168
#
import re

from CastleTestUtils.RivieraUtils.rivieraUtils import RivieraUtils
from CastleTestUtils.RivieraUtils.rivieraCommunication import ADBCommunication

adb = ADBCommunication()
"""
:Abstract: This utility can be used to capture DiskUsage, Memory Usage, CPU Usage values with the help of Telnet or Serial Object.
"""

class MemoryInfo(object):
    """
    MemoryInfo Class
    """

    def __init__(self, device=None, module='Castle', trailingdelay=5):
        self.module = module
        self.trailingdelay = trailingdelay
        self.rivieraUtilsObj = RivieraUtils('ADB', device=device)
        self.adb = ADBCommunication()
        self.adb.setCommunicationDetail(device)

    def getDiskUsage(self):
        """
        Get Disk space information via df -h command and fetch CPU % via regular experssion.
        """
        try:
            resp = self.adb.executeCommand("df -h")
            print "Disk Usage :" + resp
            if resp:
                resp = re.sub(' +', ' ', resp)
                resp = re.findall(r'([^\s]+)', resp)
                print "Disk Usage rootfs Usage:" + resp[0]
                ds = []
                for v in range(7, len(resp), 6):
                    ds.append((resp[v] + " -> " + resp[v + 5], resp[v + 4]))
                return ds
        except Exception as e:
            print "getRootfsUsage: Get Root fs Unsuccessful - %s" % str(e)
            assert False, "getRootfsUsage: Get Root fs Unsuccessful - %s" % str(e)

    def getIOWrite(self):
        """

        :return:
        """
        try:
            resp = self.adb.executeCommand("iostat -d -k")
            print "iostat :" + resp
            if resp:
                resp = re.sub(' +', ' ', resp)
                resp = re.findall(r'([^\s]+)', resp)
                ds = []
                for v in range(13, len(resp), 6):
                    ds.append((resp[v], resp[v + 1], resp[v + 2], resp[v + 3], resp[v + 4], resp[v + 5]))
                return ds
        except Exception as e:
            print "getIOWrite: Get iostat Unsuccessful - %s" % str(e)
            assert False, "getIOWrite: Get iostat Unsuccessful - %s" % str(e)

    def getMemoryUsage(self):
        """
        Get Memory Free and Memory Used information from cat /proc/meminfo command
        """
        TotalmemFree = 0
        resp = self.adb.executeCommand("cat /proc/meminfo")
        print "MemoryFree_Capture Response : " + str(resp)
        if resp:
            resp = re.sub(' +', ' ', resp)
            memFree = re.search(r'MemFree:\s(\d+)', resp).group(1)
            print "Memory Free - MemFree : " + str(memFree)
            Inactiveanon = re.search(r'Inactive\(anon\):\s(\d+)', resp).group(1)
            print "Memory Free - Inactive(anon) : " + str(Inactiveanon)
            InactiveFile = re.search(r'Inactive\(file\):\s(\d+)', resp).group(1)
            print "Memory Free - Inactive(file) : " + str(InactiveFile)
            SReclaimable = re.search(r'SReclaimable:\s(\d+)', resp).group(1)
            print "Memory Free - SReclaimable : " + str(SReclaimable)
            TotalmemFree = int(memFree) + int(Inactiveanon) + int(InactiveFile) + int(SReclaimable)
            Totalmem = re.search(r'MemTotal:\s(\d+)', resp).group(1)
            TotalmemUsed = int(Totalmem) - int(TotalmemFree)
            print "Memory Free - Total : " + str(TotalmemFree)
            print "Memory Used - Total : " + str(TotalmemUsed)
            return TotalmemFree, TotalmemUsed
        print "Memory Free - Total : " + str(TotalmemFree)
        return TotalmemFree

    def getCPUUsage(self):
        """
        Get CPU Usage information via Top command with the use of Regular experssion
        """
        resp = self.adb.executeCommand("top -n 1")
        print "Memory Details :" + resp
        if resp:
            #Below we have used REGX to remove multiple space from response
            resp = re.sub(' +', ' ', resp)
            print "CPU Usage : " + str(re.search(r'CPU:\s((\d+\.\d+)|(\d+))', resp).group(1))
            return re.search(r'CPU:\s((\d+\.\d+)|(\d+))', resp).group(1)

    def getTop10Process(self):
        """
        Get Top 10 Process Information via ps -o command
        return: processdetails
        """
        global processdetails
        processdetails = {}
        loopcnt = 0
        loopcnt = self.getProcessDetails(r"ps -o comm,pid,ppid,time,rss | grep '[0-9]m' | sort -nk7 -r | head ", True, loopcnt)
        loopcnt = self.getProcessDetails(r"ps -o comm,pid,ppid,time,vsz,stat,rss | sort -nk7 -r | head ", False, loopcnt)
        return processdetails

    def getProcessDetails(self, command, IsMemMB, loopcnt):
        """
        Get Process Details of Top 10 process
        param: command - ps -o comm,pid,ppid,time,rss | grep '[0-9]m' | sort -nk7 -r | head
        param: IsMemMB - Convert MB units to KB
        param: Loopcount - Fetching top 10 process so loopcnt value is 10
        """
        try:
            global processdetails
            read = self.adb.executeCommand(command)
            splitted = read.split('\n')
            for k in range(1, len(splitted)-1):
                processline = splitted[k]
                processline = re.sub(' +', ' ', processline)
                processname = processline.split(' ')[0]
                memoryconsume = processline.split(' ')[len(processline.split(' '))-1]
                if IsMemMB is True:
                    memoryconsume = memoryconsume[:-1]
                if processname in processdetails:
                    counter = int(float(processdetails[processname].split(':')[1])) + 1
                    existmemory = processdetails[processname].split(':')[0]
                    if IsMemMB is True:
                        addmemory = ((int(float(memoryconsume[:-1])*1024)) + int(float(existmemory)))
                    else:
                        addmemory = ((int(float(memoryconsume))) + int(float(existmemory)))
                    processdetails[processname] = str(addmemory) +':'+ str(counter)
                else:
                    if IsMemMB is True:
                        processdetails[processname] = str((int(float(memoryconsume[:-1])*1024))).strip() +':'+ '1'
                    else:
                        processdetails[processname] = memoryconsume.strip() +':'+ '1'
                loopcnt = loopcnt + 1
                if loopcnt > 10:
                    break
            return loopcnt
        except Exception as e:
            print "getProcessDetails Unsuccessful - %s" % str(e)
            assert False, "getProcessDetails Unsuccessful - %s" % str(e)
