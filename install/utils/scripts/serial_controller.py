#!/usr/bin/python
# serial_controller.py
#
# :Organization:  BOSE CORPORATION
#
# :Copyright:  COPYRIGHT 2015 BOSE CORPORATION ALL RIGHTS RESERVED.
#              This program may not be reproduced, in whole or in part in any
#              form or any means whatsoever without the written permission of:
#                  BOSE CORPORATION
#                  The Mountain,
#                  Framingham, MA 01701-9168

import serial
import sys
import time
import logging

class SerialController(object):
    """
    Serial Controller class that handles writing and reading to and from serial port
    """
    def __init__(self,
                 port=None,
                 baudrate=115200,
                 bytesize=8,
                 parity='N',
                 stopbits=1,
                 timeout=None,
                 xonxoff=False,
                 rtscts=False,
                 writeTimeout=None,
                 dsrdtr=False,
                 interCharTimeout=None,
                 ):
        """
        Constructor that contains default parameters
        :param port: (string) port will be opened if one is specified
        :param baudrate: (int) baudrate
        :param bytesize: (int) number of data bits
        :param parity:  (string) 'N' or 'Y', Parity checking
        :param stopbits: (int) number of stop bits
        :param timeout: (int) set a timeout value, None to wait forever
        :param xonxoff: (bool) Software flow control
        :param rtscts:  RTS/CTS flow control
        :param writeTimeout: set a timeout for writes
        :param dsrdtr: use rtscts setting, dsrdtr override if True or False
        :param interCharTimeout: Inter-character timeout, None to disable
        :return:
        """
        if port is None:
            return
        self.serial = serial.Serial( port=port, baudrate=baudrate,
                                     bytesize=bytesize, parity=parity,
                                     stopbits=stopbits, timeout=timeout,
                                     writeTimeout=writeTimeout,
                                     xonxoff=xonxoff, rtscts=rtscts, dsrdtr=dsrdtr,
                                     interCharTimeout=interCharTimeout )
    def isOpen(self):
        """
        Check if the serial port is still open.
        :return: True or False
        """
        # Some versions of Python interpreter contain different properties
        # of isOpen which is used to check whether serial connection is open or not
        if hasattr(self.serial, 'is_open'):
            return self.serial.is_open
        elif hasattr(self.serial, '_isOpen'):
            return self.serial._isOpen
        else:
            raise Exception("Serial: Serial object does not contain serial status property")

    def wrtPort(self, strings, no_print=False):
        """

        :param strings:
        :return:
        """
        if no_print==False:
            if strings == "\n" or strings == "\r\n":
                logging.info("\tSerial: Writing: '[New line]'")
            else:
                logging.info("\tSerial: Writing: '[%s]'" % strings)
        if self.isOpen():
            try:
                self.serial.write(strings)
                return True
            except Exception, e1:
                logging.error("\tSerial: Failed to write to serial port. [%s]" % str(e1) )
                print("\tSerial: Failed to write to serial port. [%s]" % str(e1) )
                return False
        else:
            self.serial.open()
            try:
                return True
            except Exception, e1:
                logging.error("\tSerial: Failed to write to serial port. [%s]" % str(e1) )
                print("\tSerial: Failed to write to serial port. [%s]" % str(e1) )
                return False
        return
        
    def wrtCharsWithDelay (self, cmd, dly_ms=0, no_print=False):
        """
        Write to the serial bus with a delay between each character.This is a work around for
        PAELLA-2284, an interrupt scheduling issue with the LPM when tap commands are sent too quickly.
        :param cmd: what you would like to send -> term_char added.
        :param dly_ms: delay in msec
        :return: None
        """
        status = False
        if self.isOpen() == False:
            self.serial.open()
            
        try:
            if no_print==False:
                if strings == "\n" or strings == "\r\n":
                    logging.info("\tSerial: Writing: '[New line]'")
                else:
                    logging.info("\tSerial: Writing: '[%s]'" % strings)
            if dly_ms == 0:
                self.serial.write(cmd)  # Send command!
            else:
                dly_ms *= .001  # convert to sec
                for ch in cmd:
                    self.serial.write(ch)  # Send command!
                    time.sleep(dly_ms)
            status = True
        except Exception, e1:
            print("\tSerial: Failed to write to serial port. [%s]" % str(e1) )
            logging.error("\tSerial: Failed to write to serial port. [%s]" % str(e1) )
        return status
        
    def readPort(self, bytes=None):
        """
        Read serial port until a given amount of bytes is received
        :param bytes: number of bytes to read, otherwise use inWaiting()
            Note: if you didn't specify a timeout, it will get stuck here untill
            a specified number of bytes is received
        :return: serial output
        """
        if bytes is None:
            bytes = self.serial.inWaiting()
        output = self.serial.read(bytes)
        return output


    def mergeLines (self, str):
        str = str.replace('\s', ' ')
        str = str.replace('\t', ' ')
        str = str.replace('\r', ' ')
        str = str.replace('\n', ' ')
        return str

    def wait_for_resp(self, exp_resp=None, timeout_s=3, no_print=False):
        """
        method only reads from the port until timeout_s or exp_resp is found
        :param exp_resp: expected response to read until it is found
        :param timeout_s: seconds to continue search
        :return: returns the response string or empty string
        """

        timeout = time.time() + timeout_s   # timeout in seconds from now
        response_string = ""

        if exp_resp is not None:
            outputCharacters = []
            while 1:
                if time.time() < timeout:
                    if self.serial.inWaiting():
                        outputCharacters += self.serial.read()
                        response_string = ''.join(outputCharacters)                        			            
                    response_string = self.mergeLines(response_string)
                    if exp_resp in response_string:
                        break
                else:
                    print("\tSerial: Timeout - Did not find response: %s in serial response: (%s) " % (exp_resp, response_string))
                    logging.warning("\tSerial: Timeout - Did not find response: %s in serial response: (%s) " % (exp_resp, response_string))
                    break
        else:
            print("\tSerial: exp_resp = None")
            logging.error("\tSerial: exp_resp = None")
            
        if no_print==False:
            logging.info("\tSerial: response: [%s] " % response_string)        
        return response_string

    def wait_for_any_resp(self, exp_resp=None, timeout_s=3, no_print=False):
        """
        method only reads from the port until timeout_s or exp_resp is found
        :param exp_resp: expected response to read until it is found
        :param timeout_s: seconds to continue search
        :return: returns the response string or empty string
        """
        timeout = time.time() + timeout_s   # timeout in seconds from now
        response_string = ""
        
        if exp_resp is not None:
            outputCharacters = []
            found = False;
            while 1:
                if time.time() < timeout:
                    if self.serial.inWaiting():
                        outputCharacters += self.serial.read()
                        response_string = ''.join(outputCharacters)
                    response_string = self.mergeLines(response_string)
                    for exp_resp_val in exp_resp:
                        #if exp_resp_val in response_string:
                        if response_string.find(exp_resp_val) >= 0:
                            found = True;
                            break
                    if found == True:
                        break                            
                else:
                    print("\tSerial: Timeout - Did not find response: %s in serial response: (%s) " % (exp_resp, response_string))
                    logging.warning("\tSerial: Timeout - Did not find response: %s in serial response: (%s) " % (exp_resp, response_string))
                    response_string = ""
                    break
        else:
            logging.error("\tSerial: exp_resp = None")
            print("\tSerial: exp_resp = None")
                
        if no_print==False:
            logging.info("\tSerial: response: [%s] " % response_string)
        return response_string

    def open(self):
        """
        Check if serial connection is already open and open it
        :return:
        """
        if self.isOpen():
            logging.info("\tSerial: Serial Port is already open" )            
        else:
            logging.info("\tSerial: Opening serial port" )
            self.serial.open()

    def close(self):
        """
        Close serial connection
        :return:
        """
        logging.info("\tSerial: Closing serial port" )
        self.serial.close()
        return

    def flush(self):
        """
        Flush out serial buffer
        :return:
        """
        self.serial.flushInput()
        self.serial.flushOutput()
        return


if __name__ == '__main__':
    test = SerialController('/dev/ttyUSB0')
    test.open()
    test.isOpen()
    test.close()

