from pyadb import ADB
import sys
import time
import os
import pexpect
import platform
import logging
import signal
    
class ADBComm():
    """
        This class provides the functionality to do communication with ADB shell.
    """
    
    __device = None
    adbObj = None
    package_path = "."
    fastboot_util = "./utils/lin/fastboot"
    fastboot_bin = "sudo ./utils/lin/fastboot"
    adb_util = "./utils/lin/adb"
    is_windows = False
    def __init__(self, base_path = "."):
        """
            Initialize the adb path value
        """
        package_path = base_path
        self.init_binaries()
        logging.info("Using %s" %self.adb_util)
        self.adbObj = ADB(self.adb_util)
        self.adbObj.restart_server()

    def is_exe(self, fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)
    
    def find_program(self, program):
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if self.is_exe(exe_file):
                return exe_file            
        
    def init_binaries(self):
        if "Windows" in platform.system():
            logging.debug("On Windows System....")
            self.fastboot_util = self.package_path + "\utils\\win\\fastboot.exe"
            self.fastboot_bin = self.fastboot_util
            self.adb_util = self.package_path + "\utils\\win\\adb.exe"
            self.is_windows = True
        else:
            logging.debug("On Linux System....")
            self.fastboot_util = self.find_program("fastboot")            
            self.fastboot_bin = "sudo " + self.fastboot_util            
            self.adb_util = self.find_program("adb")
        return
        
    def get_fastboot_path(self):
        return self.fastboot_bin

    def setTargetDevice(self, device=None):
        """
            Set the required communication detail for ADB shell.
            
            device=None (Optional): Pass the deviceId to connect with specific device from the connected multiple devices.
                                    If device is None means there is only one device connected and it will fetch that one.
        """
        self.__device = device
        (_, deviceList) = self.adbObj.get_devices()
        if ((deviceList == None) or (len(deviceList) == 0)): 
            logging.error("No devices found in adb devices")
            return False
        if self.__device:
            if not self.adbObj.set_target_device(self.__device): 
                logging.error("Device: {0} is not found in devices list".format(self.__device))
                return False
            if self.adbObj.get_target_device() != self.__device: 
                logging.error("Unable to set the target device: {0}".format(self.__device))
                return False
        else:
            if len(deviceList) > 1:
                logging.warning("There are multiple devices connected. and no device is selected !!!")
                return False
            else:
                self.__device = deviceList[0]
        logging.info("Connected to device: {0}".format(self.__device))
        return True

    def executeCommand(self, command, cwd=None):
        """
            Execute the command on ADB shell
            cwd (Optional): Perform any command on ADB shell at specific location. 
        """
        logging.info("Executing %s" %command)
        
        if cwd:
            command = 'cd ' + cwd + '&& ' + command
            
        stdOutStr = self.adbObj.shell_command('"' + command + '"')
        return stdOutStr

    def putFile(self, local, remote):
        return self.adbObj.push_local_file(local, remote)

    def getFile(self, remote, local):
        return self.adbObj.push_remote_file(remote, local)
    
    def restartServer(self):
        self.adbObj.restart_server()

    def stopServer(self):
        self.adbObj.kill_server()

    def startServer(self):
        self.adbObj.start_server()

    def getFirstDeviceAvailable(self):
        foundDeviceList = self.adbObj.get_devices()[1]
        if len(foundDeviceList) == 0:
            return None
        return foundDeviceList[0]
        
    def isDeviceAvailable(self, deviceToCheck=None):
        """
            Return True if device is found else False for the selected device.
        """
        deviceFound = False
        if deviceToCheck is None:
            deviceToCheck = self.__device
            
        (_, deviceList) = self.adbObj.get_devices()
        if deviceList != None:
            if len(deviceList) != 0:
                if deviceToCheck == None: 
                    logging.error("No device is selected.Please choose the device")
                    deviceFound = False
                if deviceToCheck in deviceList:
                    deviceId = deviceToCheck
                    deviceFound = True
                else:
                    logging.info('Device: %s not found in adb devices list: %s' % (deviceToCheck, deviceList))
    
        if deviceFound:
            logging.info("Device found : %s " % deviceId)
        
        return deviceFound 

    def rebootDevice(self, toBootloader=False):
        """
        Reboots the connected device
        """
        result = False
        try:
            # Get connected device ID
            foundDeviceList = self.adbObj.get_devices()[1]
            if len(foundDeviceList) == 0:
                logging.error("No device is connected")
                return result                
            if self.__device not in foundDeviceList:
                logging.error("Required device is not connected")
                return result
            # Reboot the device
            if toBootloader:
                logging.info("Rebooting the Device to Bootloader")
                self.executeCommand("reboot bootloader")
            else:
                logging.info("Rebooting the Device to Normal")
                self.executeCommand("reboot")
            time.sleep(2)
            result = True
        except Exception, e:
            logging.error("rebootDevice: Exception occured")
            
        return result
    
    def waitForRebootDevice(self, timeoutSeconds=60):
        """
        Waits for the device after reboot
        """
        found = True
        def handler(signum, frame):
            global found
            found = False
            logging.error("Device not found after reboot")
    
        signal.signal(signal.SIGALRM, handler)
        signal.alarm(timeoutSeconds)
    
        logging.info("Waiting for device to reboot")
        self.adbObj.wait_for_device()
    
        signal.alarm(0)
            
        if not found:
            assert False, "Timeout!!! While waiting for device to reboot"
    
        foundDeviceList = self.adbObj.get_devices()
        if self.__device not in foundDeviceList[1]:
            logging.error("Device not found after reboot")
        logging.info("Device found : %s " % self.__device)
        return found
        
    
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

    adbC = ADBComm("")
    adbC.setTargetDevice()
    adbC.rebootDevice()
    adbC.waitForRebootDevice()
    adbC.isDeviceAvailable()
    
