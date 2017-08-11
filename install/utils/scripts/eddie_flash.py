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

def do_fastboot_update(package_path, all_partitions, userspace_only, partition_list_file, erase_persist):
    from fastboot_flash import FastbootUpdater
    from fastboot_flash import do_fastboot_flash
    do_fastboot_flash(package_path, all_partitions, userspace_only, partition_list_file, erase_persist)
    return

# This needs change after discussion on how to do LPM update
def do_lpm_update(package_path, lpm_serial_dev):
    from lpm_flash import LpmUpdater
    from lpm_flash import do_lpm_flash
    
    lpm_dir = package_path
    lpm_flash_files = fnmatch.filter(os.listdir(lpm_dir), "lpm*.bin")
    if len(lpm_flash_files)==0:
        raise Exception("Expected Extracted Package with any file matching [%s]. Not Found. Cannot Continue LPM Update." %(lpm_dir + "/lpm*.bin")) 
    
    if len(lpm_flash_files) > 1:
        print("Found multiple LPM Files at [%s]." %(lpm_dir))
    
    lpm_file = os.path.join(lpm_dir, lpm_flash_files[0])
    logging.info("Using [%s] for LPM Flash." %(lpm_file))
    do_lpm_flash(lpm_serial_dev, lpm_file)
    return

def do_qc_flash(package, update_fastboot, all_partitions, userspace_only, partition_list_file, erase_persist, update_lpm, lpm_serial_dev):
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
            do_fastboot_update(extracted_package, all_partitions, userspace_only, partition_list_file, erase_persist)
            logging.info("..... Fastboot update Finish .....")
            
        # Perform LPM update if enabled
        if update_lpm:
            logging.info("..... LPM update Start .....")              
            do_lpm_update(extracted_package, lpm_serial_dev)
            logging.info("..... LPM update Finish .....")
    except Exception as e:
        logging.error(type(e))
        logging.error('Error: %s %s' %(e.args,  sys.exc_info()[0]))
    except:
        logging.error('Unexpected Error: [%s]' %(sys.exc_info()[0]))
    return

# Required inputs: Package Path.
# Optional: update_lpm --> if passed update lpm, else update APQ8017 only 
#   if update_lpm is given, Required: <Serial device name>
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
            
    parser = argparse.ArgumentParser(description='Flash Eddie and LPM using Fastboot/Serial.')
    parser.add_argument('package', action='store', help="Full path of package TAR file")
    subparsers = parser.add_subparsers(help='commands')
    
    fastboot_parser = subparsers.add_parser("fastboot", help="Perform fastboot update")
    fastboot_parser.add_argument('-a', '--all', default=False, action='store_true', required=False, help="Flash all partitions (except persist)")
    fastboot_parser.add_argument('-u', '--userspace-only', default=False, action='store_true', required=False, help="Flash only userspace (bose) partition")
    fastboot_parser.add_argument('-e', '--erase-persist', default=False, action='store_true', required=False, help="Erase persistent partition")    
    fastboot_parser.add_argument('-l', '--partition-list', default="", action='store', required=False, help="Name of partition list file")    

    lpm_parser = subparsers.add_parser("lpm", help="Perform LPM update")
    lpm_parser.add_argument('-s', '--serial', default="", required=True, action='store', help="Serial port for LPM connection")

    args = parser.parse_args()
    abs_package = os.path.abspath(args.package)
    if os.path.exists(abs_package) and os.path.splitext(abs_package)[1] == ".tar":
        logging.info("Package to be used: [%s]" %abs_package)
    else:
         logging.info("Package Cannot be found / not TAR file: " %abs_package)
         sys.exit(-1)
    
    userspace_only = False
    all_partitions = False
    erase_persist = False
    partition_list_file = None
    update_lpm = False
    update_fastboot = False
    lpm_serial_dev = None
    if args.erase_persist:
        erase_persist = True

    if ('userspace_only' in args.__dict__) and args.userspace_only:
        userspace_only = True
    elif ('all' in args.__dict__) and args.all:
        all_partitions = True
    elif ('partition_list' in args.__dict__) and args.partition_list != "":
        partition_list_file = os.path.abspath(args.partition_list)
        if os.path.exists(partition_list_file):
            logging.info("Partitions to be flashed from: [%s]" %partition_list_file)
        else:
            logging.info("Partition list file invalid: [%s]" %partition_list_file)
            sys.exit(-1)
            
    if userspace_only or all_partitions or erase_persist or partition_list_file != "":
        update_fastboot = True
    
    if ('serial' in args.__dict__) and args.serial != "":
        update_lpm = True
        lpm_serial_dev = args.serial

    
    do_qc_flash(abs_package, update_fastboot, all_partitions, userspace_only, partition_list_file, erase_persist, update_lpm, lpm_serial_dev)
    sys.exit(0) 
    
