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

def do_fastboot_update(package_path, erase_persist):
    fastboot_flash_path = os.path.join(package_path, "utils/scripts/fastboot_flash.py")
    fastboot_flash_dstpath = os.path.join(package_path, "../fastboot_flash.py")
     
    if not os.path.exists(fastboot_flash_path):
        raise Exception("Expected Extracted Package with file [%s]. Not Found. Cannot Continue update." %(fastboot_flash_path))
        
    shutil.copy(fastboot_flash_path, fastboot_flash_dstpath)
    sys.path.append(os.path.abspath(fastboot_flash_path))  
    from fastboot_flash import FastbootUpdater
    from fastboot_flash import do_fastboot_flash
    do_fastboot_flash(package_path, erase_persist)
    
    if os.path.exists(fastboot_flash_dstpath): 
        os.remove(fastboot_flash_dstpath)
        fastboot_flash_dstpath = os.path.join(package_path, "../fastboot_flash.pyc")
        if os.path.exists(fastboot_flash_dstpath): 
            os.remove(fastboot_flash_dstpath)
    return

def do_lpm_update(package_path, lpm_serial_dev):
    lpm_flash_path = os.path.join(package_path, "utils/scripts/lpm_flash.py")
    lpm_flash_dstpath = os.path.join(package_path, "../lpm_flash.py")
    serial_path = os.path.join(package_path, "utils/scripts/serial_controller.py")
    serial_dstpath = os.path.join(package_path, "../serial_controller.py")    
    if not os.path.exists(lpm_flash_path):
        raise Exception("Expected Extracted Package with file [%s]. Not Found. Cannot Continue update." %(lpm_flash_path))
    
    shutil.copy(lpm_flash_path, lpm_flash_dstpath)
    shutil.copy(serial_path, serial_dstpath)
    sys.path.append(os.path.abspath(lpm_flash_path))
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
    print("Using [%s] for LPM Flash." %(lpm_file))
    do_lpm_flash(lpm_serial_dev, lpm_file)
    
    if os.path.exists(serial_dstpath): 
        os.remove(serial_dstpath)
        serial_dstpath = os.path.join(package_path, "../serial_controller.pyc")
        if os.path.exists(serial_dstpath): 
            os.remove(serial_dstpath)

    if os.path.exists(lpm_flash_dstpath): 
        os.remove(lpm_flash_dstpath)
        lpm_flash_dstpath = os.path.join(package_path, "../lpm_flash.pyc")
        if os.path.exists(lpm_flash_dstpath): 
            os.remove(lpm_flash_dstpath)
    return

def do_qc_flash(package, update_fastboot, erase_persist, update_lpm, lpm_serial_dev):
    try:
        abs_package = os.path.abspath(package)
        logging.info("Using package: [%s] for update. Extracting..." %(abs_package)) 
        print("Using package: [%s] for update. Extracting..." %(abs_package)) 
        # Extract Tar
        tar = tarfile.open(package)
        tar.extractall()
        tar.close()
        
        # See if top directory, required scripts are available
        # At this point path_to_package should be valid path
        extracted_package = os.path.abspath("eddie-package")
        extracted_package = os.path.normpath(extracted_package) + os.sep
        if not os.path.exists(extracted_package):
            raise Exception("Expected Extracted Package [%s]. Not Found. Cannot Continue update." %(extracted_package))
            
        print ("Package extracted to [%s]." %(extracted_package))
        # Perform Fastboot update if enabled
        if update_fastboot:
            print("\n\n..... Fastboot update Start .....")
            logging.info("..... Fastboot update Start .....")  
            do_fastboot_update(extracted_package, erase_persist)
            print("..... Fastboot update Finish .....")
            logging.info("..... Fastboot update Finish .....")
            
        # Perform LPM update if enabled
        if update_lpm:
            print("\n\n..... LPM update Start .....")
            logging.info("..... LPM update Start .....")              
            do_lpm_update(extracted_package, lpm_serial_dev)
            logging.info("..... LPM update Finish .....")
            print("..... LPM update Finish .....")
    except Exception as e:
        print(type(e))
        print('Error: %s %s' %(e.args, sys.exc_info()[0]))
        logging.error(type(e))
        logging.error('Error: %s %s' %(e.args,  sys.exc_info()[0]))
    except:
        print('Unexpected Error: [%s]' %(sys.exc_info()[0]))
        logging.error('Unexpected Error: [%s]' %(sys.exc_info()[0]))
    return
    
def get_package_name(args):
    for arg in args:
        if "tar.gz" in arg:
            abs_package = os.path.abspath(arg)
            if os.path.exists(abs_package) and os.path.splitext(abs_package)[1] == ".gz":
                return abs_package
    return None
    
def is_fastboot_enabled(args):
    for arg in args:
        if arg == "update_fastboot":
            return True
    return False

def is_erase_persist(args):
    for arg in args:
        if arg == "erase-persist":
            return True
    return False

def is_lpm_enabled(args):
    for arg in args:
        if arg == "update_lpm":
            return True
    return False

def get_lpm_serial(args):
    for arg in args:
        if "Windows" in platform.system():
            if "com" in arg.lower():
                return arg
        else:
            if "/dev/tty" in arg:
                return arg
    return None
    
def print_usage():
    print ("  Usage: qc_flash.py <Package TarBall Path> update_fastboot <Optional: erase-persist> update_lpm  <LPM Serial Device>")
    print ("  Usage: qc_flash.py <Package TarBall Path> update_lpm <LPM Serial Device>")
    print ("  Usage: qc_flash.py <Package TarBall Path> update_fastboot <Optional: erase-persist>")
    print ("    e.g. qc_flash.py /scratch/eddie_qc8017_32_Release.tar.gz update_fastboot")
    print ("      OR qc_flash.py /scratch/eddie_qc8017_32_Release.tar.gz update_lpm /dev/ttyUSB0")
    print ("      OR qc_flash.py /scratch/eddie_qc8017_32_Release.tar.gz update_fastboot update_lpm /dev/ttyUSB0")
    print ("      OR qc_flash.py /scratch/eddie_qc8017_32_Release.tar.gz update_fastboot erase-persist update_lpm /dev/ttyUSB0")
    
# Required inputs: Package Path.
# Optional: update_lpm --> if passed update lpm, else update APQ8017 only 
#   if update_lpm is given, Required: <Serial device name>
if __name__ == '__main__':
    if "Windows" not in platform.system():
        if os.getuid() != 0:
            print("The flash script requires root priviledge. Please try and rerun in sudo mode")
            sys.exit(-1)

    if len(sys.argv) < 3:
        print_usage()
        sys.exit(-1)
    
    package = get_package_name(sys.argv[1:])
    if package == None:
        print_usage()
        sys.exit(-1)
    
    update_fastboot = is_fastboot_enabled(sys.argv[1:])
    erase_persist = is_erase_persist(sys.argv[1:])
    
    update_lpm = is_lpm_enabled(sys.argv)
    lpm_serial_dev = None
    if update_lpm:
        lpm_serial_dev = get_lpm_serial(sys.argv[1:])
        if lpm_serial_dev==None:
            print_usage()
            sys.exit(-1)
        
    logging.basicConfig(filename="flash_util.log", filemode='w', level=logging.INFO)
    do_qc_flash(package, update_fastboot, erase_persist, update_lpm, lpm_serial_dev)
    sys.exit(0) 
    
