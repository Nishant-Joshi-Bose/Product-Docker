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
from collections import OrderedDict

''' This class provides facility to perform update via fastboot utility for Qualcomm device.        
    It requires fastboot and adb utilities installed on system, and Micro-USB ADB port of device connected to any USB port of system.
'''
class FastbootUpdater:
        package_path = "."
        fastboot_util = "./utils/lin/fastboot"
        fastboot_bin = "sudo " + fastboot_util
        adb_util = "./utils/lin/adb"
        adb_bin = "sudo  " + adb_util
        is_windows = False
        device_of_interest = "device "
        adb_device_list =  []
        fastboot_device_list =  []
        adb_reboot_delay = 10
        fastboot_reboot_delay = 60
                
        def __init__(self, package_path = "."):
            self.package_path = package_path
            self.init_binaries()
            #if not os.path.exists(self.fastboot_util):
            #    raise Exception("Cannot find Fastboot binary [%s]. Exiting." %(self.fastboot_util))
            #if not os.path.exists(self.adb_util):
            #    raise Exception("Cannot find ADB binary [%s]. Exiting." %(self.adb_util))
            #self.copy_binaries()
            self.restart_adb()

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
                logging.debug("\tExecuting: [%s]" %cmd)
                con = pexpect.spawn(cmd) 
                con.expect(pexpect.EOF, timeout=600)
                out = con.before
                con.close()
                logging.debug("\t\t[%s]" %out)
            return out

        def copy_binaries(self):
            if "Windows" in platform.system():
                try:
                    destpath = os.path.abspath(os.path.join(self.package_path, '..'))
                    logging.debug("Trying to copy [%s] to [%s]" %(self.fastboot_util, destpath))
                    shutil.copy(self.fastboot_util, destpath)
                except:
                    # Ignore and continue, operation may fail going further
                    logging.debug("Ignoring copy error for fastboot EXE")
                self.fastboot_util = destpath + "\\fastboot.exe"
                self.fastboot_bin = self.fastboot_util

                try:
                    logging.debug("Trying to copy [%s] to [%s]" %(self.adb_util, destpath))
                    shutil.copy(self.adb_util, destpath)
                except:
                    # Ignore and continue, operation may fail going further
                    logging.debug("Ignoring copy error for ADB EXE")
                self.adb_util = self.package_path + "\\adb.exe"
                self.adb_bin = self.adb_util
            # else: nothing to do
            return

        def init_binaries(self):
            if "Windows" in platform.system():
                logging.debug("On Windows System....")
                self.fastboot_util = self.package_path + "\utils\\win\\fastboot.exe"
                self.fastboot_bin = self.fastboot_util
                self.adb_util = self.package_path + "\utils\\win\\adb.exe"
                self.adb_bin = self.adb_util
                self.is_windows = True
            else:            
                logging.debug("On Linux System....")
                #self.fastboot_util = self.package_path + "/utils/lin/fastboot"
                self.fastboot_util = "fastboot"
                self.fastboot_bin = "sudo " + self.fastboot_util
                #self.adb_util = self.package_path + "/utils/lin/adb"
                self.adb_util = "adb"
                self.adb_bin = "sudo " + self.adb_util                
            return
       
        def restart_adb(self):
            cmd = self.adb_bin + " kill-server; " + self.adb_bin + " start-server"
            self.execute_cmd_on_host(cmd)
            #else: TODO
            return
                    
        def stop_adb(self):
            cmd = self.adb_bin + " kill-server "
            self.execute_cmd_on_host(cmd)
            #else: TODO
            return

        def find_adb_devices(self, restartAdb=True):
            dev_list = []
            if restartAdb:
                self.restart_adb()
            cmd = self.adb_bin + " devices -l"
            result = self.execute_cmd_on_host(cmd)
            if result is not None:
                result_list = result.split("\n")
                for line in result_list:
                    if self.device_of_interest in line:
                        logging.info("Found ADB dev: %s" %line.split(' ')[0])
                        dev_list.insert(-1, line.split(' ')[0])
            return dev_list
            
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
            self.adb_device_list = self.find_adb_devices(False)
            if len(self.adb_device_list) > 0:
                cmd = self.adb_bin + " -s " + self.adb_device_list[0] + " reboot bootloader"
                self.execute_cmd_on_host(cmd)
                logging.info("Waiting for device to boot up for [%s] seconds." %str(self.adb_reboot_delay))
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
                raise Exception("Not able to move / find any device to Fastboot mode.")
            else:
                if len(self.fastboot_device_list) > 1:
                    raise Exception("More then one device in fastboot mode.\nExit extra devices from Fastboot mode / unplug their USB to continue.")
            return
            
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
                self.adb_device_list = self.find_adb_devices(True)
                if len(self.adb_device_list) > 0:
                    break
                sys.stdout.write('.')
                sys.stdout.flush()
                count = count + 1
            sys.stdout.write('\n')
            if len(self.adb_device_list) == 0:
                raise Exception("Not able to reboot device to Normal mode.")                    
            return
        
        def fastboot_erase(self, partition, assume_fastboot=False, reboot_back=False):
            if not assume_fastboot:
                self.boot_to_fastboot()
            cmd = self.fastboot_bin + " erase " + partition
            logging.debug("Erasing Partition: [%s]" %partition)
            out = self.execute_cmd_on_host(cmd)
            logging.debug(out)
            if reboot_back: 
                self.reboot_from_fastboot()
            return
            
        def fastboot_flash(self, partition, image_file, assume_fastboot=False, reboot_back=False):
            if not os.path.isfile(image_file):
                raise Exception("Partition image [%s] does not exist." %image_file)
                
            if not assume_fastboot:
                self.boot_to_fastboot()
            cmd = self.fastboot_bin + " flash " + partition + " " + image_file            
            logging.debug("Flashing partition: [%s]" %partition)
            out = self.execute_cmd_on_host(cmd)
            logging.debug(out)
            if reboot_back: 
                self.reboot_from_fastboot()
            return
        
        # Fetch file from device via adb to given dest
        def get_file(self, src, dest):
            return
    
        # Put file to device via adb from given src to device destination
        def put_file(self,src, dest):
            return

def get_fasboot_partition_files(package_path, full_update=False, 
                                all_partitions=False, userspace_only=False, 
                                partition_file=None, erase_persist=False):
    # Map of all partitions vs. their files. We potentially can parse all labels from rawprogram*.xml
    partition_list = OrderedDict()
    partitions_to_flash = None
    if userspace_only:
        partition_list["bose"] = "*bose.ext4"
    elif (all_partitions) or (partition_file is not None) or (full_update):
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
        partition_list["bose"] = "*bose.ext4"
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
    
def do_fastboot_flash(package_path, full_update=False,
                      all_partitions=False,  userspace_only=False, 
                      partition_file=None, erase_persist=False):
    try:
        logging.info("Fastboot Flash Start.")
        abs_package_path = os.path.normpath(os.path.abspath(package_path)) + os.sep
        if not os.path.isdir(abs_package_path):
            raise Exception("Package directory [%s] not present. Exiting." %abs_package_path)
        controller = None
        fastboot_updater = None
            
        partitions = get_fasboot_partition_files(abs_package_path, full_update, all_partitions, userspace_only, partition_file, erase_persist)
        fastboot_updater = FastbootUpdater(abs_package_path)
        logging.info("******************************************************************************************")
        logging.info("\tWARNING: Starting Fastboot Flash. Ensure the power and USB connections to the device are secured.")
        logging.info("\tWARNING: Power failure or any disruption in connection may damage the device completely.")
        logging.info("\tWARNING: In case of any such issues after update, QFIL update may be required to recover device.")
        logging.info("******************************************************************************************\n")
        assume_fastboot = False
        for partition in partitions:            
            logging.info("------------------------------------------------------------------------------------------")
            if (partitions[partition] != "") or (partitions[partition] == "" and full_update):
                logging.info("--- Erasing: [%s]" %partition)
                time.sleep(1)
                fastboot_updater.fastboot_erase(partition, assume_fastboot)
                assume_fastboot = True
                if partitions[partition] != "":
                    logging.info("--- Flashing: [%s] with [%s]" %(partition, partitions[partition]))
                    time.sleep(1)
                    fastboot_updater.fastboot_flash(partition, partitions[partition], assume_fastboot)
            else:
                logging.info("--- Not Flashing: [%s] as filename is Blank" %(partition))
            logging.info("------------------------------------------------------------------------------------------")
        
        logging.info("******************************************************************************************")
        logging.info("Rebooting back to normal mode. This will take few seconds.")
        fastboot_updater.reboot_from_fastboot()
        logging.info("******************************************************************************************\n")
        
        if controller is not None:
            controller.close()
    except Exception as e:
        logging.error(type(e))
        logging.error('Error: %s %s' %(e.args,  sys.exc_info()[0]))
    except:
        logging.error('Unexpected Error: [%s]' %(sys.exc_info()[0]))
    finally:    
        if controller is not None:
            if controller.isOpen():
                controller.close()                
        if fastboot_updater is not None:
            fastboot_updater.stop_adb()

        logging.info("Fastboot Flash End.")
    return
    
if __name__ == '__main__':
    if "Windows" not in platform.system():
        if os.getuid() != 0:
            print("The flash script requires root priviledge. Please try and rerun in sudo mode.")
            sys.exit(-1)
            
    parser = argparse.ArgumentParser(description='Flash Eddie and LPM using Fastboot.')
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
    '''fastboot_parser.add_argument('-l', '--partition-list', 
                                 default="", 
                                 action='store', 
                                 required=False, 
                                 help="Name of partition list file")    
    '''

    args = parser.parse_args()
    abs_package = os.path.abspath(args.package)
    
    full_update = False
    userspace_only = False
    all_partitions = False
    erase_persist = False
    partition_list_file = None
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
            
    if full_update or userspace_only or all_partitions or erase_persist or partition_list_file != "":
        logging.basicConfig(filename="fastboot_flash.log", format='%(asctime)s - %(levelname)s - %(message)s', filemode='w', level=logging.DEBUG)
        do_fastboot_flash(abs_package, full_update, all_partitions, userspace_only, partition_list_file, erase_persist)
    else:
        print("No valid option to perform Fastboot operation. Exiting")
        sys.exit(-1)

    sys.exit(0)
    
