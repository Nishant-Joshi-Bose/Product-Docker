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
import tarfile
import platform
import fnmatch
import shutil
import logging
import argparse
import subprocess

def run_cmd(command, print_output=True):
    """
    To run any command on host machine. (Wrap shell execute command in-between prints)
    """
    logging.info(command)
    output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
    if print_output:
		logging.debug(output)
    return output

def do_fastboot_update(package_path, full_update, all_partitions, userspace_only, partition_list_file, erase_persist, update_lpm):
    from fastboot_flash import FastbootUpdater
    from fastboot_flash import do_fastboot_flash
    do_fastboot_flash(package_path, full_update, all_partitions, userspace_only, partition_list_file, erase_persist)
    fbUpdater = FastbootUpdater(package_path)
    logging.info("******************************************************************************************");
    fs_version = run_cmd("%s shell cat /opt/Bose/etc/FS_VERSION" %fbUpdater.adb_util)
    logging.info("FS Version Installed: [%s]" %(fs_version))
    bose_version = run_cmd("%s shell cat /opt/Bose/etc/BoseVersion.json" %fbUpdater.adb_util)
    logging.info("Bose Version Installed: [%s]" %(bose_version))
    
    if update_lpm:
        from lpm_updater import LpmUpdater
        logging.info("..... LPM update Start .....")
        lpmUpdater = LpmUpdater(fbUpdater.adb_util)
        lpmUpdater.update(True)
        count = 0
        while count < lpmUpdater.total_update_timeout_sec:
            adb_device_list = fbUpdater.find_adb_devices()
            if len(adb_device_list) > 0:
                break
            sys.stdout.write('.')
            sys.stdout.flush()
            count = count + 1
            time.sleep(1)
        sys.stdout.write('\n')
        sys.stdout.flush()
        logging.info("Available LPM Version: [%s]" %lpmUpdater.current_version)
        logging.info("Actual LPM Version: [%s]" %lpmUpdater.target_version)
        if lpmUpdater.is_version_current:
            logging.info("LPM Firmware updated Succssfully")    
        else:
            logging.error("LPM Firmware not updated")
    return

def do_qc_flash(package, update_fastboot, full_update, all_partitions, userspace_only, partition_list_file, erase_persist, update_lpm):
    try:
        abs_package = os.path.abspath(package)
        logging.info("Using package: [%s] for update. Extracting..." %(abs_package)) 
        # Extract Tar
        tar = tarfile.open(package)
        tar.extractall()
        tar.close()
        # See if top directory, required scripts are available
        # At this point path_to_package should be valid path
        extracted_package = os.path.abspath("eddie")
        extracted_package = os.path.normpath(extracted_package) + os.sep
        if not os.path.exists(extracted_package):
            raise Exception("Expected Extracted Package [%s]. Not Found. Cannot Continue update." %(extracted_package))
        logging.info("Package extracted to [%s]." %(extracted_package))
        # Perform Fastboot update if enabled
        if update_fastboot:
            logging.info("..... Fastboot update Start .....")  
            do_fastboot_update(extracted_package, full_update, all_partitions, userspace_only, partition_list_file, erase_persist, update_lpm)
            logging.info("..... Fastboot update Finish .....")
    except Exception as e:
        logging.error(type(e))
        logging.error('Error: %s %s' %(e.args,  sys.exc_info()[0]))
    except:
        logging.error('Unexpected Error: [%s]' %(sys.exc_info()[0]))
    return

# Required inputs: Package Path.
if __name__ == '__main__':
    logging.basicConfig(filename="flash_util.log", format='%(asctime)s - %(levelname)s - %(message)s', filemode='w', level=logging.INFO)
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

    if "Windows" not in platform.system():
        if os.getuid() != 0:
            logging.info("The flash script requires root priviledge. Please try and rerun in sudo mode.")
            sys.exit(-1)
            
    parser = argparse.ArgumentParser(description='Flash Eddie and LPM using Fastboot Flash.')
    parser.add_argument('package', action='store', help="Full path of package TAR file")
    #parser.add_argument('serial', action='store', required=False, help="LPM Debug Serial port - e.g. /dev/ttyUSB1")
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
                                 help="Flash all partitions (except usrfs, persist, bose-persist & partition table)")
    fastboot_parser.add_argument('-u', '--userspace-only', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Flash only userspace (bose) partition")
    fastboot_parser.add_argument('-e', '--erase-persist', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Erase only bose-persist partition") 
    fastboot_parser.add_argument('-l', '--lpm-update', 
                                 default=False, 
                                 action='store_true', 
                                 required=False,
                                 help="Peform LPM update.") 
    
    args = parser.parse_args()
    abs_package = os.path.abspath(args.package)
    if os.path.exists(abs_package) and os.path.splitext(abs_package)[1] == ".tar":
        logging.info("Package to be used: [%s]" %abs_package)
    else:
         logging.info("Package Cannot be found / not TAR file: " %abs_package)
         sys.exit(-1)
    
    full_update = True
    userspace_only = False
    all_partitions = False
    erase_persist = False
    partition_list_file = None
    update_lpm = False
    update_fastboot = True
    #serial_port = None
    #if args.serial:
    #    serial_port = args.serial

    if ('lpm_update' in args.__dict__) and args.lpm_update:
        update_lpm = True

    # if user says full update, do so
    if ('full_update' in args.__dict__) and args.full_update:
        full_update = True
    else:
        # erase persist can coexist with other options
        if args.erase_persist:
            erase_persist = True            
        if ('userspace_only' in args.__dict__) and args.userspace_only:
            userspace_only = True
        elif ('all' in args.__dict__) and args.all:
            all_partitions = True

    if userspace_only or all_partitions or erase_persist:
        full_update = False

    do_qc_flash(abs_package, update_fastboot, full_update, all_partitions, userspace_only, partition_list_file, erase_persist, update_lpm)
    sys.exit(0) 
    
