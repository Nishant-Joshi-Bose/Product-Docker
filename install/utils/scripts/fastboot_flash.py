#!/usr/bin/python
# fastboot_flash.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

import sys
import time
import os
import pexpect
import subprocess
import platform
import fnmatch
import logging
import shutil
import argparse
from collections import OrderedDict
from adb_communication import ADBComm
''' This class provides facility to perform update via fastboot utility for Qualcomm device.        
    It requires fastboot and adb utilities installed on system, and Micro-USB ADB port of device connected to any USB port of system.
'''
class FastbootUpdater:
    package_path = "."
    device_serial = None
    fastboot_device_list =  []
    is_initialzed = False
    fastboot_bin = ""
    adb_reboot_delay = 10
    fastboot_reboot_delay = 60
    is_intialized = False
    
    def __init__(self, package_path = ".", adb_obj = None, device = None):
        self.package_path = package_path
        if adb_obj is None:
            self.adbC = ADBComm(package_path)
        else:
            self.adbC = adb_obj
        self.fastboot_bin = self.adbC.get_fastboot_path()
        self.device_serial = device
        self.is_intialized = self.setDevice()
    
    def get_intiialized(self):
        return self.is_intiialized
         
    def execute_cmd_on_host(self, cmd):
        out = None
        if "Windows" in platform.system():
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
             
            ## Talk with command i.e. read data from stdout and stderr. Store this info in tuple 
            ## Read data from stdout and stderr, until end-of-file is reached. Wait for process to terminate. 
            (out, err) = p.communicate()
             
            ## Wait for command to terminate. Get return returncode ##
            p_status = p.wait()
            #print "Command output : ", output
            #print "Command exit status/return code : ", p_status		
            logging.debug("\t\t[%s]" %out)
        else:
            logging.info("\tExecuting: [%s]" %cmd)
            con = pexpect.spawn(cmd) 
            con.expect(pexpect.EOF, timeout=600)
            out = con.before
            con.close()
            logging.debug("\t\t[%s]" %out)
        return out

    def find_fastboot_devices(self):
        dev_list = []
        cmd = self.fastboot_bin + " devices"
        result = self.execute_cmd_on_host(cmd)
        if result is not None:
            result_list = result.split("\n")
            for line in result_list:
                if "fastboot" in line:
                    logging.info("Found FASTBOOT dev: %s" %line.split(' ')[0])
                    dev_list.insert(-1, line.split(' ')[0])
        return dev_list
    
    def boot_to_fastboot(self):
        ''' if adb devices give at least one device, 
                adb is active, boot to fastboot mode
            otherwise if fastboot devices give at least one device
                fastboot mode is active already
            otherwise
                provide error saying no valid device is found
        '''
        logging.info("Boot to Fastboot.")
        if self.adbC.isDeviceAvailable(self.device_serial):
            logging.info("Rebooting ADB Dev %s to Bootloader" %self.device_serial)
            self.adbC.setTargetDevice(self.device_serial)
            self.adbC.rebootDevice(True)
            count = 0
            while count < self.adb_reboot_delay:
                time.sleep(1)
                self.fastboot_device_list = self.find_fastboot_devices()
                if len(self.fastboot_device_list) > 0:
                    break
                sys.stdout.write('.')
                sys.stdout.flush()
                count = count + 1
            sys.stdout.write('\n')
        else:   # Try to see if device is already in fastboot mode
            self.fastboot_device_list = self.find_fastboot_devices()
            
        if len(self.fastboot_device_list) == 0:                
            logging.error("Not able to move / find any device to Fastboot mode.")
            return False
        else:
            if len(self.fastboot_device_list) > 1:
                logging.error("More then one device in fastboot mode.\nExit extra devices from Fastboot mode / unplug their USB to continue.")
                return False
        return True

    def reboot_from_fastboot(self):
        ''' if fastboot devices give at least one device, 
                fastboot is active, boot to adb mode by fastboot reboot
            otherwise
                provide error saying no valid device is found
        '''
        self.fastboot_device_list = self.find_fastboot_devices()
        if len(self.fastboot_device_list) > 0:
            self.execute_cmd_on_host(self.fastboot_bin + " reboot")
         
        logging.info("Waiting for device to boot for [%s] seconds." %str(self.fastboot_reboot_delay))
        count = 0
        while count < self.fastboot_reboot_delay:
            time.sleep(1)            
            if self.adbC.isDeviceAvailable(self.device_serial):
                break
            sys.stdout.write('.')
            sys.stdout.flush()
            count = count + 1
        sys.stdout.write('\n')
        if not self.adbC.isDeviceAvailable(self.device_serial):
            raise Exception("Not able to reboot device to Normal mode.")
        return
    
    def fastboot_erase(self, partition, reboot_back=False):
        cmd = self.fastboot_bin + " erase " + partition
        logging.debug("Erasing Partition: [%s]" %partition)
        out = self.execute_cmd_on_host(cmd)
        logging.debug(out)
        if reboot_back: 
            self.reboot_from_fastboot()
        return
        
    def fastboot_flash(self, partition, image_file, reboot_back=False):
        if not os.path.isfile(image_file):
            raise Exception("Partition image [%s] does not exist." %image_file)
        cmd = self.fastboot_bin + " flash " + partition + " " + image_file            
        logging.debug("Flashing partition: [%s]" %partition)
        out = self.execute_cmd_on_host(cmd)
        logging.debug(out)
        if reboot_back: 
            self.reboot_from_fastboot()
        return

    def setDevice(self):
        if self.device_serial is None:
            self.device_serial = self.adbC.getFirstDeviceAvailable()
             
        if self.device_serial is None:
            logging.warning("No ADB Device found. Finding Fastboot device ")
            dev_list = self.find_fastboot_devices()
            if len(dev_list) > 0:
                self.device_serial = dev_list[0]
                logging.info("Found Fastboot dev %s" %self.device_serial)
            else:
                logging.error("No Fastboot Device found.")
                return False
        else:
            logging.info("Using %s as device. Try to boot to fastboot" %self.device_serial)
            return self.boot_to_fastboot()
        return True

def get_fasboot_partition_files(package_path, full_update=False, 
                                all_partitions=False, userspace_only=False, 
                                erase_persist=False):
    # Map of all partitions vs. their files. We potentially can parse all labels from rawprogram*.xml
    partition_list = OrderedDict()
    partitions_to_flash = None
    if userspace_only:
        partition_list["bose"] = "orig_userpartition.ext4"
    elif (all_partitions) or (full_update):
        partition_list["partition"] = "*gpt_both0*.bin"
        partition_list["PrimaryGPT"] = "*gpt_main0*.bin"
        partition_list["BackupGPT"] = "*gpt_backup0*.bin"
        partition_list["modemst1"] = ""
        partition_list["modemst2"] = ""
        partition_list["fsg"] = ""
        partition_list["aboot"] =  "*emmc_appsboot*.mbn"
        partition_list["abootbak"] =  "*emmc_appsboot*.mbn"
        partition_list["boot"] = "*apq8017-boot*.img"
        partition_list["recovery"] = "*apq8017-boot*.img"
        partition_list["cache"] = "*apq8017-cache*.ext4"
        partition_list["dsp"] = "*adspso*.bin"
        partition_list["modem"] = "*NON-HLOS*.bin"
        partition_list["persist"] = "*apq8017-persist*.ext4"
        partition_list["recoveryfs"] =  "*apq8017-recovery*.ext4"
        partition_list["system"] =  "*apq8017-sysfs*.ext4"
        partition_list["sbl1"] = "*sbl1*.mbn"
        partition_list["sbl1bak"] = "*sbl1*.mbn"
        partition_list["tz"] = "*tz*.mbn"
        partition_list["tzbak"] = "*tz*.mbn"
        partition_list["devcfg"] = "*devcfg*.mbn"
        partition_list["devcfgbak"] = "*devcfg*.mbn"
        partition_list["keymaster"] = "*keymaster*.mbn"
        partition_list["keymasterbak"] = "*keymaster*.mbn"
        partition_list["cmnlib"] = "*cmnlib.mbn"
        partition_list["cmnlibbak"] = "*cmnlib.mbn"
        partition_list["cmnlib64"] = "*cmnlib64*.mbn"
        partition_list["cmnlib64bak"] = "*cmnlib64*.mbn"
        partition_list["rpm"] = "*rpm*.mbn"
        partition_list["rpmbak"] = "*rpm*.mbn"
        partition_list["userdata"] = "*apq8017-usrfs*.ext4"
        partition_list["bose"] = "orig_userpartition.ext4"
        partition_list["bose-persist"] = "*bose-persist*.ext4"
        #partition_list["fsc"] = ""
        #partition_list["ssd"] = ""
        #partition_list["DDR"] = ""
        #partition_list["sec"] = "*sec*.dat"
        #partition_list["splash"] = ""
        #partition_list["devinfo"] = ""
        #partition_list["misc"] = ""
        #partition_list["keystore"] =  ""
        #partition_list["config"] =  ""
        #partition_list["oem"] =  ""
        #partition_list["limits"] =  ""
        #partition_list["mota"] =  ""
        #partition_list["dip"] =  ""
        #partition_list["mdtp"] =  ""
        #partition_list["syscfg"] =  ""
        #partition_list["mcfg"] =  ""
        #partition_list["apdp"] = ""
        #partition_list["msadp"] =  ""
        #partition_list["dpo"] =  ""
        #partition_list["logdump"] =  "" 
        
    if erase_persist:
        partition_list["bose-persist"] = "*bose-persist*.ext4"
    if all_partitions:
        partition_list["bose-persist"] = ""
        partition_list["partition"] = ""
        partition_list["PrimaryGPT"] = ""
        partition_list["BackupGPT"] = ""
        partition_list["userdata"] = ""
        partition_list["persist"] = ""
        
    new_list = OrderedDict()   

    ''' Try to find files in package. If they are present, append path to actual file name.
        This ensures, at least those partitions for which no file is present, are not deleted.
        Later we can add version check, if we want to only update 
    '''
    for partition in partition_list:
        use_current = True
        if partition_list[partition] != "": 
            file_name = os.path.isfile(package_path + partition_list[partition])
            files = fnmatch.filter(os.listdir(package_path), partition_list[partition])
            # If more then one file match, warn user
            if len(files) > 1:
                logging.warning("Warn: Found more then one partition files matching [%s] at path [%s]" %(partition_list[partition], package_path))
            else:
                if len(files) > 0:
                    new_list[partition] = package_path + files[0]
                else:
                    if full_update:
                        logging.error("Err: Not able to find matching file for partition [%s] in full update mode. Exiting with error" %(partition_list[partition]))
                        sys.exit(-1)
                    #Else no file found, do not update this partition
        else:
            new_list[partition] = "" #Add this in list - will be ignored for all but full_update
            
    logging.debug("Partitions to Flash----")
    for partition in new_list:
        logging.debug("\t[%s]\t\t= [%s]" %(partition, new_list[partition]))        

    return new_list
    
def do_fastboot_flash(package_path, adb_obj, full_update=False,
                      all_partitions=False,  userspace_only=False, 
                      erase_persist=False):
    fastboot_updater = None
    try:
        logging.info("******************************************************************************************")
        logging.info("\tWARNING: Starting Fastboot Flash. Ensure the power and USB connections to the device are secured.")
        logging.info("\tWARNING: Power failure or any disruption in connection may damage the device completely.")
        logging.info("\tWARNING: In case of any such issues after update, QFIL update may be required to recover device.")
        logging.info("******************************************************************************************\n")

        logging.info("Fastboot Flash Start.")
        abs_package_path = os.path.normpath(os.path.abspath(package_path)) + os.sep
        if not os.path.isdir(abs_package_path):
            raise Exception("Package directory [%s] not present. Exiting." %abs_package_path)
            
        partitions = get_fasboot_partition_files(abs_package_path, full_update, all_partitions, userspace_only, erase_persist)
        fastboot_updater = FastbootUpdater(abs_package_path, adb_obj)
        if not fastboot_updater.get_intiialized:
            raise Exception("Cannot initialize device to Fastboot mode")        
        for partition in partitions:            
            logging.info("------------------------------------------------------------------------------------------")
            if (partitions[partition] != "") or (partitions[partition] == "" and full_update):
                logging.info("--- Erasing: [%s]" %partition)
                time.sleep(1)
                fastboot_updater.fastboot_erase(partition)
                if partitions[partition] != "":
                    logging.info("--- Flashing: [%s] with [%s]" %(partition, partitions[partition]))
                    time.sleep(1)
                    fastboot_updater.fastboot_flash(partition, partitions[partition])
            else:
                logging.info("--- Not Flashing: [%s] as filename is Blank" %(partition))
            logging.info("------------------------------------------------------------------------------------------")
        
        logging.info("******************************************************************************************")
        logging.info("Rebooting back to normal mode. This will take few seconds.")
        fastboot_updater.reboot_from_fastboot()
        logging.info("******************************************************************************************\n")
        
    except Exception as e:
        logging.error(type(e))
        logging.error('Error: %s %s' %(e.args,  sys.exc_info()[0]))
    except:
        logging.error('Unexpected Error: [%s]' %(sys.exc_info()[0]))
    finally:    
        logging.info("Fastboot Flash End.")
    return
    
if __name__ == '__main__':
    if "Windows" not in platform.system():
        if os.getuid() != 0:
            print("The flash script requires root priviledge. Please try and rerun in sudo mode.")
            sys.exit(-1)

    logging.basicConfig(filename="flash_util.log", format='%(asctime)s - %(levelname)s - %(message)s', filemode='w', level=logging.DEBUG)
    #define a new Handler to log to console as well
    console = logging.StreamHandler()
    # optional, set the logging level
    console.setLevel(logging.INFO)
    # set a format which is the same for console use
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    # tell the handler to use this format
    console.setFormatter(formatter)
    # add the handler to the root logger
    logging.getLogger('').addHandler(console)
            
    parser = argparse.ArgumentParser(description='Flash Product using Fastboot.')
    parser.add_argument('package', 
                        action='store', 
                        help="Full path of package Folder")
    subparsers = parser.add_subparsers(help='commands')
    
    fastboot_parser = subparsers.add_parser("fastboot", help="Perform fastboot update")
    fastboot_parser.add_argument('-f', '--full-update', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Erase All partitions including Partition table")       
    fastboot_parser.add_argument('-a', '--all', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Flash all partitions (except persist)")
    fastboot_parser.add_argument('-u', '--userspace-only', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Flash only userspace (bose) partition")
    fastboot_parser.add_argument('-e', '--erase-persist', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Erase persistent partition")    
    args = parser.parse_args()
    abs_package = os.path.abspath(args.package)
    
    full_update = False
    userspace_only = False
    all_partitions = False
    erase_persist = False
    update_fastboot = False
    if args.full_update:
        full_update = True
    else:
        # erase persist can coexist with other options
        if args.erase_persist:
            erase_persist = True            
        if ('userspace_only' in args.__dict__) and args.userspace_only:
            userspace_only = True
        elif ('all' in args.__dict__) and args.all:
            all_partitions = True
            
    if full_update or userspace_only or all_partitions or erase_persist:
        do_fastboot_flash(abs_package, None, full_update, all_partitions, userspace_only, erase_persist)
    else:
        print("No valid option to perform Fastboot operation. Exiting")
        sys.exit(-1)

    sys.exit(0)
    
