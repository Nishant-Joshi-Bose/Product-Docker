from pyadb import ADB
import sys
import time
import os
import pexpect
import platform
import logging
import signal
import subprocess
    
class ADBComm():
    """
        This class provides the functionality to do communication with ADB shell.
    """
    
    __device = None
    adbObj = None
    package_path = "."
    fastboot_util = "./utils/lin/fastboot"
    fastboot_bin = "sudo ./utils/lin/fastboot"
    adb_bin = "sudo ./utils/lib/adb"
    adb_util = "./utils/lin/adb"
    is_windows = False
    device_of_interest = " device"
    def __init__(self, base_path = "."):
        """
            Initialize the adb path value
        """
        self.package_path = base_path
        self.init_binaries()
        logging.info("Using %s" %self.adb_bin)
        self.adbObj = ADB(self.adb_bin)
        #self.adbObj.restart_server()

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
            self.adb_bin = self.adb_util
            self.is_windows = True
        else:
            logging.debug("On Linux System....")
            self.fastboot_util = self.find_program("fastboot")            
            self.fastboot_bin = "sudo " + self.fastboot_util            
            self.adb_util = self.find_program("adb")
            self.adb_bin = "sudo " + self.adb_util            
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
        #(_, deviceList) = self.adbObj.get_devices()
        deviceList = self.getDevicesOverload()
        if ((deviceList == None) or (len(deviceList) == 0)): 
            logging.error("No devices found in adb devices")
            return False
        if self.__device:
            logging.debug("Do nothing");
            '''
            if not self.is_windows:
                if not self.adbObj.set_target_device(self.__device): 
                    logging.error("Device: {0} is not found in devices list".format(self.__device))
                    return False
                if self.adbObj.get_target_device() != self.__device: 
                    logging.error("Unable to set the target device: {0}".format(self.__device))
                    return False
            else:
                logging.debug("On windows assume setTargetDevice worked")
            '''
        else:
            print "4"
            if len(deviceList) > 1:
                logging.warning("There are multiple devices connected. and no device is selected !!!")
                return False
            else:
                print "5"
                self.__device = deviceList[0]
        logging.info("Connected to device: {0}".format(self.__device))
        return True

    def executeCommand(self, command, shell='shell', cwd=None):
        """
            Execute the command on ADB shell
            cwd (Optional): Perform any command on ADB shell at specific location. 
        """
        #if self.is_windows:
        if cwd:
            command = 'cd ' + cwd + ' && ' + self.adb_bin + ' ' + shell + ' ' + command
        else:
            command = self.adb_bin + ' ' + shell + ' ' + command
        logging.info("Executing %s" %command)
        stdOutStr = self.execute_cmd_on_host(command)
        '''
        else:
            if cwd:
                command = 'cd ' + cwd + '&& ' + command
            logging.info("Executing %s" %command)
            stdOutStr = self.adbObj.shell_command(command)
        '''
        return stdOutStr


    def execute_cmd_on_host(self, cmd):        
        out = None
        if self.is_windows:
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

    def putFile(self, local, remote):
        #if self.is_windows:
        command = local + ' ' + remote
        return self.executeCommand(command, 'push')
        #else:
        #    return self.adbObj.push_local_file(local, remote)

    def getFile(self, remote, local):
        #if self.is_windows:
        command = remote + ' ' + local
        return self.executeCommand(command, 'pull')
        #else:
        #    return self.adbObj.push_remote_file(remote, local)
    
    def restartServer(self):
        #if self.is_windows:
        self.executeCommand(' ', 'kill-server')
        self.executeCommand(' ', 'start-server')
        #else:
        #    self.adbObj.restart_server()

    def stopServer(self):
        #if self.is_windows:
        self.executeCommand(' ', 'kill-server')
        #else:
        #    self.adbObj.kill_server()

    def startServer(self):
        #if self.is_windows:
        self.executeCommand(' ', 'start-server')
        #else:
        #    self.adbObj.start_server()

    def getDevicesOverload(self):
        foundDeviceList = []
        #if self.is_windows:
        cmd = self.adb_bin + " devices -l"		
        result = self.execute_cmd_on_host(cmd)		
        if result is not None:		
            result_list = result.split("\n")		
            for line in result_list:
                if self.device_of_interest in line:
                    logging.debug("Found ADB dev: %s" %line.split(' ')[0])		
                    foundDeviceList.insert(-1, line.split(' ')[0])
        #else:
        #    foundDeviceList = self.adbObj.get_devices()[1]
        #    if foundDeviceList is None:
        #        foundDeviceList = []
        return foundDeviceList
        
    def getFirstDeviceAvailable(self):
        foundDeviceList = self.getDevicesOverload()
        if len(foundDeviceList) == 0:
            return None
        else:
            return foundDeviceList[0]
        
    def isDeviceAvailable(self, deviceToCheck=None):
        """
            Return True if device is found else False for the selected device.
        """
        deviceFound = False
        if deviceToCheck is None:
            deviceToCheck = self.__device
            
        #(_, deviceList) = self.adbObj.get_devices()
        deviceList = self.getDevicesOverload()
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
            #foundDeviceList = self.adbObj.get_devices()[1]
            foundDeviceList = self.getDevicesOverload()
            if len(foundDeviceList) == 0:
                logging.error("No device is connected")
                return result                
            if self.__device not in foundDeviceList:
                logging.error("Required device is not connected")
                return result
            # Reboot the device
            if toBootloader:
                logging.info("Rebooting the Device to Bootloader")
                self.executeCommand(' ', 'reboot bootloader')
            else:
                logging.info("Rebooting the Device to Normal")
                self.executeCommand(' ', 'reboot')
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
    
    
        logging.info("Waiting for device to reboot")
        #if self.is_windows:
        self.executeCommand(' ', 'wait-for-device')
        found = True
        '''
        else:
            signal.signal(signal.SIGALRM, handler)
            signal.alarm(timeoutSeconds)
            self.adbObj.wait_for_device()
            signal.alarm(0)
        '''    
        if not found:
            assert False, "Timeout!!! While waiting for device to reboot"
    
        #foundDeviceList = self.adbObj.get_devices()
        foundDeviceList = self.getDevicesOverload()
        if self.__device not in foundDeviceList:
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
    
