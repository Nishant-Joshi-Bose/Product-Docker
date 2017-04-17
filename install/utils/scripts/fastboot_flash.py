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
import serial
import subprocess
import platform
import fnmatch
import logging
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
        device_of_interest = "apq8017"
        adb_device_list = None
        adb_reboot_delay = 15
        fastboot_reboot_delay = 60
                
        def __init__(self, package_path = ".", port = "/dev/ttyUSB0", serial_controller = None):
            self.package_path = package_path
            self.init_binaries()
            if not os.path.exists(self.fastboot_util):
                raise Exception("Cannot find Fastboot binary [%s]. Exiting." %(self.fastboot_util))
            if not os.path.exists(self.adb_util):
                raise Exception("Cannot find ADB binary [%s]. Exiting." %(self.adb_util))
            self.port = port
            self.serial_controller = None
            self.restart_adb()

        def execute_cmd_on_host(self, cmd):
            out = None
            logging.info("\tExecuting: [%s]" %cmd)
            con = pexpect.spawn(cmd)
            con.expect(pexpect.EOF, timeout=600)
            out = con.before
            con.close()
            logging.debug("\t\t[%s]" %out)
            return out

        def init_binaries(self):
            if "Windows" in platform.system():
                logging.info("On Windows System....")
                print("On Windows System....")
                self.fastboot_util = "fastboot.exe"
                self.fastboot_bin = self.fastboot_util
                self.adb_util = "adb.exe"
                self.adb_bin = self.adb_util
                self.is_windows = True
            else:            
                logging.info("On Linux System....")
                print("On Linux System....")
                self.fastboot_util = self.package_path + "/utils/lin/fastboot"
                self.fastboot_bin = "sudo " + self.fastboot_util
                self.adb_util = self.package_path + "/utils/lin/adb"
                self.adb_bin = "sudo " + self.adb_util                
       
        def restart_adb(self):
            if not self.is_windows:
                cmd = self.adb_bin + " kill-server; " + self.adb_bin + " start-server"
                self.execute_cmd_on_host(cmd)
            #else: TODO
            return
                    
        def find_adb_devices(self):           
            dev_list = []
            cmd = self.adb_bin + " devices -l"
            result = self.execute_cmd_on_host(cmd)
            if result is not None:
                result_list = result.split("\n")
                for line in result_list:
                    if self.device_of_interest in line:
                        print("\tFound ADB dev: %s" %line.split(' ')[0])
                        logging.info("\tFound ADB dev: %s" %line.split(' ')[0])
                        dev_list.insert(-1, line.split(' ')[0])
            if len(dev_list) == 0:
                print("\tNo ADB device found.")
                logging.warning("\tNo ADB device found.")
            return dev_list
            
        def find_fastboot_devices(self):
            dev_list = []
            cmd = self.fastboot_bin + " devices"
            result = self.execute_cmd_on_host(cmd)
            if result is not None:
                result_list = result.split("\n")
                for line in result_list:
                    if "fastboot" in line:
                        print("\tFound FASTBOOT dev: %s" %line.split(' ')[0])
                        logging.info("\tFound FASTBOOT dev: %s" %line.split(' ')[0])
                        dev_list.insert(-1, line.split(' ')[0])
            if len(dev_list) == 0:
                print("\tNo Fastboot device found.")
                logging.warning("\tNo Fastboot device found.")
            return dev_list
            
        def boot_to_fastboot(self):
            ''' if adb devices give at least one device, 
                    adb is active, boot to fastboot mode
                otherwise if fastboot devices give at least one device
                    fastboot mode is active already
                otherwise
                    provide error saying no valid device is found
            '''
            print("Try putting device to Fastboot mode. Find ADB Devices.")
            logging.info("Try putting device to Fastboot mode. Find ADB Devices.")
            self.adb_device_list = self.find_adb_devices()
            if len(self.adb_device_list) > 0:
                print("Rebooting [%s] to fastboot mode" %self.adb_device_list[0])
                logging.info("Rebooting [%s] to fastboot mode" %self.adb_device_list[0])
                cmd = self.adb_bin + " -s " + self.adb_device_list[0] + " reboot bootloader"
                self.execute_cmd_on_host(cmd)
                print("Waiting for device to boot up for [%s] seconds." %str(self.adb_reboot_delay))
                logging.info("Waiting for device to boot up for [%s] seconds." %str(self.adb_reboot_delay))
                time.sleep(self.adb_reboot_delay)
                
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
            if len(self.fastboot_device_list) == 0:
                self.adb_device_list = self.find_adb_devices()
                if len(self.adb_device_list) == 0:
                    raise("No device found in fastboot mode or adb mode.")
                else:
                    print("Warn: No device found in fastboot mode. Found device in ADB mode.")
                    logging.warning("No device found in fastboot mode. Found device in ADB mode.")
            else:
                self.execute_cmd_on_host(self.fastboot_bin + " reboot")
                print("Waiting for device to boot up for [%s] seconds." %str(self.fastboot_reboot_delay))
                logging.info("Waiting for device to boot up for [%s] seconds." %str(self.fastboot_reboot_delay))
                time.sleep(self.fastboot_reboot_delay)
                
            self.adb_device_list = self.find_adb_devices()
            if len(self.adb_device_list) == 0:
                raise Exception("Not able to reboot device to ADB mode.")
            return
        
        def fastboot_erase(self, partition, assume_fastboot=False, reboot_back=False):
            if not assume_fastboot:
                self.boot_to_fastboot()
            cmd = self.fastboot_bin + " erase " + partition
            logging.debug("\tErasing Partition: [%s]" %partition)
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
            logging.debug("\tFlashing partition: [%s]" %partition)
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

        # Run fastboot script - This is risky. 
        def update_from_script(self, fastboot_script_path_name, reboot_back=False):
            if not os.path.exists(fastboot_script_path_name):
                raise Exception("Fastboot script [%s] does not exist." %fastboot_script_path_name)
            self.boot_to_fastboot()
            '''  
            if self.is_windows:
                # TODO
                out = self.execute_cmd_on_host(fastboot_script_path_name)
            else:
                out = self.execute_cmd_on_host("sudo sh " + fastboot_script_path_name)
            print(out)
            '''
            ''' Parse script file til
            '''
            
            if reboot_back:
                self.reboot_from_fastboot()
            return

        ''' TODO: Not tested for future
        def update_from_pkg(self, package_path_name):
            # use update.zip later on when build mechanism provides it
            if not os.path.exists(package_path_name):
                raise Exception("Fastboot script [%s] does not exist." %package_path_name)
            self.boot_to_fastboot()
            out = self.execute_cmd_on_host(fastboot_bin + " fastboot update " + package_path_name)
            print(out)
            self.reboot_from_fastboot()
            return
        '''

'''  
fastboot_script_filter = "fastboot*"

def get_fastboot_scriptlist(package_path):
    script_list = fnmatch.filter(os.listdir(package_path), fastboot_script_filter)
    if len(script_list) == 0:
        raise Exception("No fastboot files found for update at path [%s]" %package_path)
        
    new_list = []
    for script in script_list:
        new_list.insert(-1, package_path + script)
    return new_list
'''

def get_fasboot_partition_files(package_path, erase_persist=False):
    # Map of all partitions vs. their files. We potentially can parse all labels from rawprogram*.xml
    partition_list = OrderedDict()
    partition_list["PrimaryGPT"] = "*gpt_main0*.bin"
    partition_list["BackupGPT"] = "*gpt_backup0*.bin"
    partition_list["modem"] = "*NON-HLOS*.bin"
    partition_list["fsc"] = ""
    partition_list["ssd"] = ""
    partition_list["sbl1"] = "*sbl1*.mbn"
    partition_list["sbl1bak"] = "*sbl1*.mbn"
    partition_list["rpm"] = "*rpm*.mbn"
    partition_list["rpmbak"] = "*rpm*.mbn"
    partition_list["tz"] = "*tz*.mbn"
    partition_list["tzbak"] = "*tz*.mbn"
    partition_list["devcfg"] = "*devcfg*.mbn"
    partition_list["devcfgbak"] = "*devcfg*.mbn"
    partition_list["dsp"] = "*adspso*.bin"
    partition_list["modemst1"] = ""
    partition_list["modemst2"] = ""
    partition_list["DDR"] = ""
    partition_list["fsg"] = ""
    partition_list["sec"] = "*sec*.dat"
    partition_list["splash"] = ""
    partition_list["aboot"] =  "*emmc_appsboot*.mbn"
    partition_list["abootbak"] =  "*emmc_appsboot*.mbn"
    partition_list["boot"] = "*apq8017-boot*.img"
    partition_list["recovery"] = ""
    partition_list["devinfo"] = ""
    partition_list["system"] =  "*apq8017-sysfs*.ext4"
    partition_list["cache"] = "*apq8017-cache*.ext4"
    partition_list["persist"] = "*apq8017-persist*.ext4"
    partition_list["misc"] = ""
    partition_list["keystore"] =  ""
    partition_list["config"] =  ""
    partition_list["oem"] =  ""
    partition_list["limits"] =  ""
    partition_list["mota"] =  ""
    partition_list["dip"] =  ""
    partition_list["mdtp"] =  ""
    partition_list["syscfg"] =  ""
    partition_list["mcfg"] =  ""
    partition_list["cmnlib"] = "*cmnlib.mbn"
    partition_list["cmnlibbak"] = "*cmnlib.mbn"
    partition_list["cmnlib64"] = "*cmnlib64*.mbn"
    partition_list["cmnlib64bak"] = "*cmnlib64*.mbn"
    partition_list["keymaster"] = "*keymaster*.mbn"
    partition_list["keymasterbak"] = "*keymaster*.mbn"
    partition_list["apdp"] = ""
    partition_list["msadp"] =  ""
    partition_list["dpo"] =  ""
    partition_list["logdump"] =  "" 
    partition_list["userdata"] = "*apq8017-usrfs*.ext4"
    if erase_persist:
        partition_list["bose-persist"] = "*bose-persist*.ext4"
    partition_list["bose"] = "*bose.ext4"
    
    new_list = OrderedDict()   

    ''' Try to find files in package. If they are present, append path to actual file name.
        This ensures, at least those partitions for which no file is present, are not deleted.
        Later we can add version check, if we want to only update 
    '''
    for partition in partition_list:
        if partition_list[partition] != "": 
            file_name = os.path.isfile(package_path + partition_list[partition])
            files = fnmatch.filter(os.listdir(package_path), partition_list[partition])
            # If more then one file match, warn user
            if len(files) > 1:
                print("Warn: Found more then one partition files matching [%s] at path [%s]" %(partition_list[partition], package_path))
                logging.warning("Warn: Found more then one partition files matching [%s] at path [%s]" %(partition_list[partition], package_path))
            else:
                if len(files) > 0:
                    new_list[partition] = package_path + files[0]
                #Else no file found, do not update this partition
    logging.info("Partitions to Flash----")
    for partition in new_list:
        logging.info("\t[%s]\t\t= [%s]" %(partition, new_list[partition]))        

    return new_list
    
def do_fastboot_flash(package_path, erase_persist=False, serial_port = None):
    try:
        print("Fastboot Flash Start.")
        
        package_path = os.path.normpath(os.path.abspath(package_path)) + os.sep
        if not os.path.isdir(package_path):
            raise Exception("Package directory [%s] not present. Exiting." %package_path)
        controller = None
        if serial_port is not None:
            print("Opening serial port [%s]." % serial_port)
            logging.debug("Opening serial port [%s]." % serial_port)
            controller = serial_controller.SerialController(serial_port)
            controller.open()
            
        partitions = get_fasboot_partition_files(package_path, erase_persist)
        fastboot_updater = FastbootUpdater(package_path, serial_port, controller)
        print("\n\n******************************************************************************************")
        print("\tWARNING: Starting Fastboot Flash in '15 seconds'. Ensure the power and USB connections to the device are secured.")
        print("\tWARNING: Power failure or any disruption in connection may damage the device completely.")
        print("\tWARNING: In case of any such issues after update, QFIL update may be required to recover device.")
        print("******************************************************************************************\n\n")
        time.sleep(15)
        assume_fastboot = False
        for partition in partitions:            
            logging.info("------------------------------------------------------------------------------------------")
            print("--- Erasing: [%s]" %partition)
            logging.info("--- Erasing: [%s]" %partition)
            time.sleep(1)
            fastboot_updater.fastboot_erase(partition, assume_fastboot)
            logging.info("--- Flashing: [%s] with [%s]" %(partition, partitions[partition]))
            print("--- Flashing: [%s] with [%s]" %(partition, partitions[partition]))
            time.sleep(1)
            fastboot_updater.fastboot_flash(partition, partitions[partition], assume_fastboot)            
            logging.info("------------------------------------------------------------------------------------------")
            assume_fastboot = True
        '''
        script_list = get_fastboot_scriptlist(package_path)
        # From package, run all the fastboot scripts for update 
        for script in script_list:
            print("**************************************************")
            print("\t\t[%s]" %script)
            print("**************************************************")
            fastboot_updater.update(script, False)
        '''
        print("\n******************************************************************************************")
        logging.info("\n******************************************************************************************")
        print("Rebooting back to normal mode. This will take a few minutes.")
        logging.info("Rebooting back to normal mode. This will take a few minutes.")
        fastboot_updater.reboot_from_fastboot()
        print("******************************************************************************************\n")
        logging.info("******************************************************************************************\n")
        
        if controller is not None:
            controller.close()
    except Exception as e:
        print(type(e))
        print('Error: %s %s' %(e.args, sys.exc_info()[0]))
        logging.error(type(e))
        logging.error('Error: %s %s' %(e.args,  sys.exc_info()[0]))
    except:
        print('Unexpected Error: [%s]' %(sys.exc_info()[0]))
        logging.error('Unexpected Error: [%s]' %(sys.exc_info()[0]))
    finally:    
        if controller is not None:
            if controller.isOpen():
                controller.close()                
        print("Fastboot Flash End.")
    return
    
if __name__ == '__main__':
    if len(sys.argv) < 2:
        #print ("  Usage: sudo fastboot_flash.py <Full path of extracted package> <Optional: Serial device name> <Optional: erase_persistence>")
        print ("  Usage: sudo fastboot_flash.py <Full path of extracted package> <Optional: erase-persist>")
        #print ("    e.g. sudo fastboot_flash.py /scratch/fastboot_package/ /dev/ttyUSB0")
        print ("      OR sudo fastboot_flash.py /scratch/fastboot_package/" )
        print ("      OR sudo fastboot_flash.py /scratch/fastboot_package/ erase-persist" )
    else:        
        if "Windows" not in platform.system():
            if os.getuid() != 0:
                print("The Fastboot flash requires root priviledge. Please try and rerun in sudo mode")
                sys.exit(-1)
                
        logging.basicConfig(filename="fastboot_flash.log", filemode='w', level=logging.INFO)
        if len(sys.argv) == 2:
            do_fastboot_flash(sys.argv[1])
        else:
            if sys.argv[1] == "erase-persist" or sys.argv[2] == "erase-persist":
                do_fastboot_flash(sys.argv[1], erase_persist=True)
            else:
                do_fastboot_flash(sys.argv[1])


