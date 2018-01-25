#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:     COPYRIGHT 2016 BOSE CORPORATION ALL RIGHTS RESERVED.
#                 This program may not be reproduced, in whole or in part
#                 in any form or any means whatsoever without the written
#                 permission of:
#
#                     BOSE CORPORATION
#                     The Mountain,
#                     Framingham, MA 01701-9168
#
import paramiko
from scp import SCPClient
import os
import tarfile
import glob
import shutil
import time
import platform
import traceback
import threading
import argparse
from ..DiscoveryUtils.discover_soundtouch_devices import DeviceDiscovery

"""
Utility to download coredumps of a Shelby unit
Required external libraries: paramiko and scp

Ex:
core = GetCoredumps(1500)
if core.connect(["192.168.1.228", "192.168.1.205"], 22, 'root', ''):
    core.download_content("/mnt/nv/BoseLog", True)
"""
class GetCoredumps(object):

    def __init__(self, timeout=1200):
        self.disc = DeviceDiscovery()
        self.client = None
        self.ip = None
        self.timeout = timeout #seconds
        if platform.system() == "Windows":
            self.time_format = "%Y-%m-%d-%H-%M-%S"
        else:
            self.time_format = "%F_%H-%M-%S"

    def connect(self, serverIp, deviceID, port=22, user='root', password=''):
        """
        Establish connection with Shelby unit
        :param serverIp: (str) or (list) ip address of the unit or a list of units
        :param port: (int) ssh port number
        :param user: (str)
        :param password: (str)
        :return: True/False
        """
        print "Attempting to establish SSH connection"
        try:
            if deviceID:
                print "Using deviceID to get IP"
                serverIp = []
                if isinstance(deviceID, list):
                    for devID in deviceID:
                        device = self.disc.ssdp_find_device(deviceID=devID, timeout=10)
                        if device:
                            serverIp.append(device[1])
                else:
                    device = self.disc.ssdp_find_device(deviceID=deviceID, timeout=10)
                    if device:
                        serverIp.append(device[1])
            if isinstance(serverIp, list):
                self.client = {}
                for ip in serverIp:
                    client = paramiko.SSHClient()
                    client.load_system_host_keys()
                    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
                    client.connect(ip, port, user, password)
                    self.client[ip] = client
                    print "Connection established with %s" % ip
            else:
                self.ip = serverIp
                self.client = paramiko.SSHClient()
                self.client.load_system_host_keys()
                self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
                self.client.connect(serverIp, port, user, password)
                print "Connection established with %s" % serverIp
            return True
        except Exception, e:
            print "Failed to establish ssh connection"
            traceback.print_exc()
            return False

    def close(self):
        """
        close ssh connection(s)
        :return: None
        """
        if self.client:
            if isinstance(self.client, dict):
                for ip, client in self.client.iteritems():
                    print "Closing connection on %s" % ip
                    client.close()
            else:
                self.client.close()
            self.client = None


    def download_content(self, content="/mnt/nv/BoseLog", remove=False):
        """
        Download directory/file of choice
        :param content: directory or file (default: /mnt/nv/BoseLog)
        :param remove: keep the core files on the unit or now
        :return: None
        """
        threadlist = []
        if isinstance(self.client, dict):
            for ip, client in self.client.iteritems():
                try:
                    thread = threading.Thread(target=self._file_download_and_zip, args=(content, client, ip, remove))
                    thread.daemon = False
                    thread.start()
                    threadlist.append(thread)
                except Exception, e:
                    print "Failed to download coredump on", ip
                    print "Exception:", e
            for thread in threadlist:
                if thread:
                    thread.join(self.timeout)
        else:
            try:
                self._file_download_and_zip(content, self.client, self.ip)
            except Exception, e:
                print "Failed to download coredump on", self.ip
                traceback.print_exc()
        self.close()

    def _file_download_and_zip(self, content, client, ip, remove):
        """
        Download core files
        :param content: path file or folder
        :param client: ssh client
        :param ip: ip address of the device that contains coredumps
        :param remove: keep the core files on the unit or now
        :return: None
        """

        print "Downloading %s from %s" % (content, ip)
        localpath = os.path.join('.', '%s_CoreDump' % ip)
        scp = SCPClient(client.get_transport(), socket_timeout=self.timeout, progress=self._progress)
        scp.get(content, localpath, True)
        corefile = glob.glob(localpath+os.sep+'core*')
        if not corefile:
            print "No coredump found for %s" % ip
            shutil.rmtree(localpath)
        else:
            print "Found CoreDumps %s on %s" % (corefile, ip)
            self._make_tarfile("%s_CoreDump_%s%s" % (ip, time.strftime(self.time_format), ".tar.gz"), localpath)
            shutil.rmtree(localpath)
            if remove:
                print "Removing core files"
                client.exec_command("rm %s/core*" % content)


    def _progress(self, path, size, transferred):
        """
        Current download progress
        :param path: path of the file being downloaded
        :param size: size of the file
        :param transferred: bytes transferred
        :return: None
        """
        percent = int(round(float(transferred)/float(size) * 100))

        if (percent % 10) == 0:
            print "\nDownloading:", path
            print "Total Size:", size
            print "Transferred:", transferred
            print "Percent Complete: %d", percent
            #print "Percent Complete: %s%% " % "{0:.2f}".format(float(transferred)/float(size) * 100)

    def _make_tarfile(self, output_filename, source_dir):
        """
        Tar firectory with coredumps
        :param output_filename: *ipAddres*_CoreDump.tar.gz
        :param source_dir: location of folder or file
        :return:
        """
        with tarfile.open(output_filename, "w:gz") as tar:
            tar.add(source_dir, arcname=os.path.basename(source_dir))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--ip', help='<Required> IP addresses of all units used in this test suite',
                        action='store', nargs='+', dest='ipList')
    parser.add_argument('--deviceid', help='<Required> deviceID addresses of all units used in this test suite',
                        action='store', nargs='+', dest='deviceIdList')
    parser.add_argument('--timeout', help='Timeout for downloading coredumps',
                        action='store', dest='timeout', default=1200)
    parser.add_argument('--remove', help='Remove core files after download process',
                        action='store_true', dest='remove', default=False)
    results = parser.parse_args()
    if results.deviceIdList != None:
        core = GetCoredumps(int(results.timeout))
        if core.connect(None, deviceID=results.deviceIdList):
            core.download_content("/mnt/nv/BoseLog", results.remove)
    elif results.ipList != None:
        core = GetCoredumps(int(results.timeout))
        if core.connect(results.ipList, None, 22, 'root', ''):
            core.download_content("/mnt/nv/BoseLog", results.remove)
