#!/usr/bin/python
# lpm_flash_tapload.py
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
import serial_controller
import logging

lpm_prompts = ["LPM>", "A4V>", "A4VBL>"]
lpm_bootloader_prompts = ["A4VBL>"]

''' This class provides facility to perform firmware update for LPM devices
        by doing Xfer of new firmware file via Serial.
'''
class LpmUpdater:
    upTypeDict = {"user":"0x8060000", "diags":"0x8020000", "blob":"0x00000000", "speaker":"0x8040000"}

    # so its global make default
    location = None
    current_prompt = None
    reboot_to_bootloader_cmd = "rb 2"
    reboot_to_usermode_cmd = "boot 0"
    reboot_to_diagmode_cmd = "boot 1"
    sec_to_enter_bootloader = 10
    sec_to_enter_usermode = 15
    sec_to_enter_diagmode = 15
    dev_newline_char = "\r\n"
    
    def __init__(self, port = "/dev/ttyUSB0", serial_controller = None, prompts = lpm_prompts, boot_prompts = lpm_bootloader_prompts, file_path_name = "", filetype="user"):
         # Open the comport to the A4V system.
        if serial_controller is None:
            print("Error: Serial controller not set.")
            logging.error("Serial controller not set.")
        else:
            self.controller = serial_controller
        self.port = port
        self.boot_prompts = boot_prompts
        self.prompts = prompts
        self.currentprompt = self.get_prompt()
        if self.currentprompt == None or len(self.currentprompt) == 0:
            raise Exception("Error: LPM Device not in valid state. Valid Prompt cannot be found on Serial connection.")

        try:
            self.location = self.upTypeDict[filetype]
        except Exception as e:            
            raise Exception("Error: Invalid update type: " + filetype)

        # Check to see if we got a path to where the file lives, otherwise use current directory.
        self.file_path = file_path_name

        if not os.path.exists(self.file_path):
            #print("Error file: {:s} \n!!Does not exist please double check the path or the filename".format(self.file_path))
            raise Exception("Error: file: {:s} \n!!Does not exist please double check the path or the filename".format(self.file_path))

        logging.info("\n\nFound update file and path: {:s}".format(self.file_path))
        print("\n\nFound update file and path: {:s}".format(self.file_path))


    # Sends newline command on serail, grabs output, and sees if it has any of lpm_prompts.
    # If match is found, sends matched entry of lpm_prompts array, else sends empty string (error)
    def get_prompt(self):
        self.controller.flush()
        found = False
        self.controller.wrtPort(self.dev_newline_char)
        self.controller.wrtPort(self.dev_newline_char)
        time.sleep(1)
        response_string = self.controller.wait_for_any_resp(self.prompts, 5)
        if len(response_string) != 0:
            # Received response, find which prompt it is
            for prompt in self.prompts:
                if prompt in response_string:
                    response_string = prompt
                    found = True
        if found == False:
            response_string = None
        return response_string

    # Returns true if string passed is part of one of the passed prompts
    def is_in_bootloader(self):
        self.currentprompt = self.get_prompt()
        if self.currentprompt == None:
            raise Exception("Error: LPM Device not in valid state. Valid Prompt cannot be found on Serial connection.")
    
        if any(self.currentprompt in s for s in self.boot_prompts):
            return True
        return False
        
    # Assume LPM is not in bootloader mode.
    # Send "rb" - -- waits for 60 seconds --> to receive prompt of boot loader
    def go_to_bootloader(self):
        logging.info("Going to Boot loader Mode.")
        status = True       
        if not self.is_in_bootloader():        
            self.controller.wrtPort(self.reboot_to_bootloader_cmd)
            time.sleep(5)
            self.controller.flush()
            self.controller.wrtPort(self.dev_newline_char)
            self.controller.wait_for_any_resp(self.prompts, self.sec_to_enter_bootloader, no_print=True)
            if not self.is_in_bootloader():
                status = False # Cannot boot to bootloader using usual command
            else:
                logging.info("LPM in Boot mode.")
        else:
            logging.info("LPM already in boot mode. No reboot required.")
        return status

    def go_to_usermode(self):
        status = True
        if self.is_in_bootloader():
            logging.info("Going to User mode. Sending [%s], and waiting for proper bootup." %self.reboot_to_usermode_cmd)
            self.controller.wrtPort(self.reboot_to_usermode_cmd)
            time.sleep(5)
            self.controller.flush()
            self.controller.wrtPort(self.dev_newline_char)
            self.controller.wait_for_any_resp(self.prompts, self.sec_to_enter_usermode, no_print=True)
            logging.info("Verifying if LPM came out of Boot mode or not.")
            if self.is_in_bootloader():
                status = False # Not ale to come out of bootloader
        return status

    def go_to_diagmode(self): #TODO
        status = True
        if self.is_in_bootloader():
            logging.info("Going to Diag mode. Sending [%s], and waiting for proper bootup.", self.reboot_to_diagmode_cmd)
            self.controller.wrtPort(self.reboot_to_diagmode_cmd) # TODO: This cannot be verified, as Diag image is not present - will fail
            time.sleep(5)
            self.controller.flush()
            self.controller.wrtPort(self.dev_newline_char)
            self.controller.wait_for_any_resp(self.prompts, self.sec_to_enter_diagmode, no_print=True)
            logging.info("Verifying if system came out of Boot mode or not.")
            if self.is_in_bootloader():
                status = False # Not ale to come out of bootloader
        else:
            status = False # TODO Do not know how to enter to diag mode from user mode
        return status
    
    def update(self):
        if self.location == None:
            print("Invalid locations.")
            logging.error("Invalid locations.")
            return False

        file_size = os.stat(self.file_path).st_size
        file_size_hex = hex(file_size)

        # Get into boot> mode:
        if not self.go_to_bootloader():
            print("Not able to switch to boot mode for update.")
            logging.error("Not able to switch to boot mode for update.")
            return False

        #send_cmd = "tapload " + self.location + ", " + file_size_hex + ", 0, 256"
        # Above command does not work on Riviera Breakboard. Use the one below
        send_cmd = "tapload " + self.location + " 20000 0 1"
        
        print("Sending [%s] to LPM, to start update." %send_cmd)
        logging.error("Sending [%s] to LPM, to start update." %send_cmd)
        self.controller.flush()
        self.controller.wrtPort(send_cmd)
        self.controller.wrtPort("\n") # Sending \r\n causes state of update to go bad
        resp = self.controller.wait_for_resp("Please send file...", 30)
        if len(resp) == 0:
            print("Not able to find proper string for update. Continuing with update.")
            logging.warning("Not able to find proper string for update. Continuing with update.")
        
        print("Sedning file [%s] to LPM." %self.file_path)
        logging.info("Sedning file [%s] to LPM." %self.file_path)
        with open(self.file_path, "rb") as f:
            byte = f.read(file_size)
            wrote = self.controller.serial.write(byte)
            #print("Bytes written [%s of %s]" %(str(wrote), str(file_size)))
            logging.info("Bytes written [%s of %s]" %(str(wrote), str(file_size)))
       
        time.sleep(5)
        self.controller.wrtPort(self.dev_newline_char)
        self.controller.wrtPort(self.dev_newline_char)
        resp = self.controller.wait_for_resp("Timeout while writing...", 30)
        if len(resp) == 0:
            print("Not able to find final string in update process. Continuing with update.")
            logging.warning("Not able to find final string in update process. Continuing with update.")

        '''    
        with open(self.file_path, "rb") as f:
            byte = f.read(256)
            while byte != b"":               
                # Send 256 Bytes Via Serial
                num_bytes_sent = str(len(byte))
                self.controller.serial.write(byte)
                time.sleep(0.1)
                resp = self.controller.wait_for_resp("...", 5)
                if ":" not in resp:
                    print "Error was not able to verify that the bootloader is receiving bytes"
                num_bytes_rec = resp.split(':')[1].strip()
                if num_bytes_sent != num_bytes_rec:
                    print ("Error sent " + num_bytes_sent + " bytes but received " + num_bytes_rec)
                byte = f.read(256)
        
        time.sleep(1)
        resp = resp + self.controller.wait_for_resp("...", 5)
        num_total_bytes_rec = resp.split()[1].split(':')[1]
        if file_size != int(num_total_bytes_rec):
            print (file_size, num_total_bytes_rec, "Error total sent bytes " + str(file_size) +
                          " does not match bytes received " + str(num_total_bytes_rec))
        
        '''
        # After sending bardeen___.bos you can send "up lpm" at the boot loader.
        # Finally boot to App
        time.sleep(5)
        status = self.go_to_usermode()
        if status == False:
            print("After update, unable to go back to user mode. Exiting update operation with Error.")
            logging.warning("After update, unable to go back to user mode. Exiting update operation with Error.")
        else:
            print("********** Update successfull. ********** ")
            logging.info("********** Update successfull. ********** ")
        return status

def do_lpm_flash(serial_port, lpm_flash_file):
    print("LPM Flash Start.")
    
    print("Opening serial port [%s]." % serial_port)
    logging.info("Opening serial port [%s]." % serial_port)
    controller = serial_controller.SerialController(serial_port)

    try:
        controller.open()
        # serial_controller = None, prompts = lpm_prompts, boot_prompts = lpm_bootloader_prompts, file_name = "", filetype="user"
        logging.info("Initializing utility.")
        lpm_updater = LpmUpdater(serial_port, controller, lpm_prompts, lpm_bootloader_prompts, lpm_flash_file)
        logging.info("Update operation Start")
        lpm_updater.update()
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
        if controller.isOpen():
            controller.close()
        print("LPM Flash End.")
    return
    
if __name__ == '__main__':
    if len(sys.argv) < 3:
        print ("  Usage: lpm_flash_tapload.py <Serial device name> <Full path and name of LPM flash file>")
        print ("    e.g. lpm_flash_tapload.py /dev/ttyUSB0 /scratch/lpm/lpm_0_0_1_128.bin")
    else:
        logging.basicConfig(filename="lpm_flash.log", filemode='w', level=logging.INFO)
        do_lpm_flash(sys.argv[1], sys.argv[2])

