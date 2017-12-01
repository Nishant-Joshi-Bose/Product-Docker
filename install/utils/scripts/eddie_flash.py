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
from adb_communication import ADBComm

def do_fastboot_update(package_path, adb_obj, full_update, all_partitions, userspace_ipk_update, erase_persist):
    from fastboot_flash import do_fastboot_flash
    if full_update or all_partitions:
        userspace_only = False
    else:
        if userspace_ipk_update:
            userspace_only = True
        else:
            userspace_only = False
    do_fastboot_flash(package_path, adb_obj, full_update, all_partitions, userspace_only, erase_persist)
    return

def do_single_ipk_update(adbC, local_ipk_path, dest_ipk_path, opkg_cmd):
    logging.info("Updating using %s" %local_ipk_path)
    logging.info("%s" %(adbC.executeCommand("/opt/Bose/bin/stop")))
    logging.info("%s" %(adbC.executeCommand("mount -oremount,rw /opt/Bose")))
    adbC.putFile(local_ipk_path, dest_ipk_path)
    logging.info("%s" %(adbC.executeCommand("%s install  --force-reinstall --force-downgrade %s" %(opkg_cmd, dest_ipk_path))))
    logging.info("%s" %(adbC.executeCommand("rm %s" %dest_ipk_path)))
    return

def do_create_opkg_conf(adbC, opkg_conf):
    adbC.executeCommand('"mkdir -p /mnt/nv/update"')
    adbC.executeCommand('"echo \'dest root /opt/Bose/\' > %s"' %opkg_conf)
    adbC.executeCommand('"echo \'src/gz product https://invalid-bose-galapagos-softwareupdate-dev.s3.amazonaws.com/eddie\' >> %s"' %opkg_conf)
    adbC.executeCommand('"echo \'option cache_dir /dev/shm/update/cache/\' >> %s"' %opkg_conf)
    adbC.executeCommand('"echo \'option lists_dir /mnt/nv/update/update/list/\' >> %s"' %opkg_conf)
    adbC.executeCommand('"echo \'option lock_file /mnt/nv/update/update/opkg.lock\' >> %s"' %opkg_conf)
    adbC.executeCommand('"echo \'option info_dir /update/info/\' >> %s"' %opkg_conf)
    adbC.executeCommand('"echo \'option status_file /update/status\' >> %s"' %opkg_conf)

def do_ipk_update(package_path, adbC, userspace_ipk_update, lpm_ipk_update, hsp_ipk_updates):
    # For now, just use first available device
    device_id = adbC.getFirstDeviceAvailable()
    if device_id is None:
        logging.error("No ADB Device found to perform IPK update")
        return
    adbC.setTargetDevice(device_id)    
    hsp_ipk = package_path + "/hsp.ipk"
    lpm_ipk = package_path + "/lpm_updater.ipk"
    userspace_ipk = package_path + "/product.ipk"
    logging.info("******************************************************************************************");
    if userspace_ipk_update:
        if os.path.isfile(userspace_ipk):
            do_single_ipk_update(adbC, userspace_ipk, "/tmp/product.ipk", "opkg -d bose") 
            # Create /mnt/nv/update/opkg.conf
            do_create_opkg_conf( adbC, "/mnt/nv/update/opkg.conf")
            do_single_ipk_update(adbC, userspace_ipk, "/tmp/product.ipk", "export LD_LIBRARY_PATH=/opt/Bose/update/opkg/;/opt/Bose/update/opkg/opkg -f /mnt/nv/update//opkg.conf --add-arch armv7a-vfp-neon:100") 
            logging.info("Rebooting APQ")
            adbC.rebootDevice()
            time.sleep(2)
            adbC.waitForRebootDevice(60)
        else:
            logging.error("Product IPK not found %s" %userspace_ipk)
    if lpm_ipk_update:
        if os.path.isfile(lpm_ipk):
            #do_single_ipk_update(adbC, lpm_ipk, "/tmp/lpm.ipk", "opkg -d bose") 
            # Create /mnt/nv/update/opkg.conf
            do_create_opkg_conf(adbC, "/mnt/nv/update/opkg.conf")
            do_single_ipk_update(adbC, lpm_ipk, "/tmp/lpm.ipk", "export LD_LIBRARY_PATH=/opt/Bose/update/opkg/;/opt/Bose/update/opkg/opkg -f /mnt/nv/update//opkg.conf --add-arch armv7a-vfp-neon:100") 
            logging.info("Restarting LPM & APQ")
            adbC.executeCommand('\"/opt/Bose/bin/ResetUtil SYSTEM\"')
            time.sleep(2)
            adbC.waitForRebootDevice(60)
        else:
            logging.error("LPM IPK not found %s" %lpm_ipk)
    logging.info("******************************************************************************************");    
    return

def do_check_version(adb_obj):
    from fastboot_flash import FastbootUpdater
    # For now, just use first available device
    #device_id = adb_obj.getFirstDeviceAvailable()
    #if device_id is None:
    #    logging.error("No ADB Device found to perform version check")
    #    return
    #adb_obj.setTargetDevice(device_id)    
    logging.info("\n******************************************************************************************");
    riviera_version = adb_obj.executeCommand("cat /etc/riviera-version")
    logging.info("******* Riviera Version Installed:\n[%s]\n" %(riviera_version))
    fs_version = adb_obj.executeCommand("cat /opt/Bose/etc/FS_VERSION")
    logging.info("******* FS Version Installed:\n[%s]\n" %(fs_version))
    bose_version = adb_obj.executeCommand("cat /opt/Bose/etc/BoseVersion.json")
    logging.info("******* Bose Version Installed:\n[%s]\n" %(bose_version))
    logging.info("******************************************************************************************");
    return
    
def do_cleanup(adb_obj, package_path):
    adb_obj.stopServer()
    shutil.rmtree(package_path, ignore_errors=True)
    return
    
def do_qc_flash(package, update_fastboot, full_update, all_partitions, userspace_ipk_update, erase_persist, lpm_ipk_update, hsp_ipk_updates):
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
        adb_obj = ADBComm(extracted_package)
        
        # Perform Fastboot update if enabled
        if update_fastboot:
            logging.info("..... Fastboot update Start .....")  
            do_fastboot_update(extracted_package, adb_obj, full_update, all_partitions, userspace_ipk_update, erase_persist)
            logging.info("..... Fastboot update Finish .....")
        if userspace_ipk_update or lpm_ipk_update or hsp_ipk_updates:
            logging.info("..... IPK update Start .....")  
            do_ipk_update(extracted_package, adb_obj, userspace_ipk_update, lpm_ipk_update, hsp_ipk_updates)
            logging.info("..... IPK update Finish .....")
        do_check_version(adb_obj)
        do_cleanup(adb_obj, extracted_package)
        
    except Exception as e:
        logging.error(type(e))
        logging.error('Error: %s %s' %(e.args,  sys.exc_info()[0]))
    except:
        logging.error('Unexpected Error: [%s]' %(sys.exc_info()[0]))
    return

if __name__ == '__main__':
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
                                 help="Erase All partitions including Partition table and install all IPKs")       
    fastboot_parser.add_argument('-a', '--all', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Flash all partitions (except usrfs, persist, bose-persist & partition table) and install all IPKs")
    fastboot_parser.add_argument('-e', '--erase-persist', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Erase only bose-persist partition") 
    #fastboot_parser.add_argument('-r', '--riviera-update', 
    #                             default=False, 
    #                             action='store_true', 
    #                             required=False,
    #                             help="Peform HSP update using IPK.") 
    fastboot_parser.add_argument('-l', '--lpm-update', 
                                 default=False, 
                                 action='store_true', 
                                 required=False,
                                 help="Peform LPM update using IPK.") 
    fastboot_parser.add_argument('-u', '--userspace-update', 
                                 default=False, 
                                 action='store_true', 
                                 required=False, 
                                 help="Perform product update using IPK.")
    
    args = parser.parse_args()
    abs_package = os.path.abspath(args.package)
    if os.path.exists(abs_package) and os.path.splitext(abs_package)[1] == ".tar":
        logging.info("Package to be used: [%s]" %abs_package)
    else:
         logging.info("Package Cannot be found / not TAR file: " %abs_package)
         sys.exit(-1)
    
    full_update = False
    all_partitions = False
    erase_persist = False    
    userspace_ipk_update = False
    lpm_ipk_update = False
    hsp_ipk_update = False
    update_fastboot = False
    #serial_port = None
    #if args.serial:
    #    serial_port = args.serial

    if args.erase_persist:
        erase_persist = True            
    if ('userspace_update' in args.__dict__) and args.userspace_update:
        userspace_ipk_update = True
    if ('lpm_update' in args.__dict__) and args.lpm_update:
        lpm_ipk_update = True
    if ('riviera_update' in args.__dict__) and args.riviera_update:
        hsp_ipk_update = True
    if ('full_update' in args.__dict__) and args.full_update:
        full_update = True
        lpm_ipk_update = True
        hsp_ipk_update = True
        userspace_ipk_update = True
        erase_persist = True
    elif ('all' in args.__dict__) and args.all:
        all_partitions = True
        lpm_ipk_update = True
        hsp_ipk_update = True
        userspace_ipk_update = True

    if userspace_ipk_update or all_partitions or erase_persist or lpm_ipk_update or hsp_ipk_update or full_update:
        if all_partitions or erase_persist or full_update or userspace_ipk_update:
            update_fastboot = True
        do_qc_flash(abs_package, update_fastboot, full_update, all_partitions, userspace_ipk_update, erase_persist, lpm_ipk_update, hsp_ipk_update)
    else:
        logging.error("No option selected")
        
    sys.exit(0) 
    
