import datetime
import time
import os
import serial

# Look for available COMPORTS
import serial.tools.list_ports
import threading

def serial_file_logger(modulename):
    log_format = '%(ascitime)s: %(message)s'
    datetimeformat = datetime.datetime.now().strftime("%Y_%m_%d-%H_%M_%S.%f%p")
    file_name = "./{0}_{1}.txt".format(modulename, datetimeformat)
    handle = open(file_name, "wb")
    return handle

class SerialLoggingUtil():
    def __init__(self, modulename, comportstr, baudrate=115200):
        self.stopflag = 0
        self.databuf = []
        self.datacaptureflag = 0
        self.modulename = modulename
        self.comportstr = comportstr
        self.baudrate = baudrate
	if self.modulename != None:
	        self.filehandle = serial_file_logger(self.modulename)
        self.comporthandle = connecttoCOMPort(self.comportstr, self.baudrate)


    def execute_command(self,command=""):
        print("IN EXECUTE COMMAND")
        console_output = []
        if self.comporthandle.isOpen():
            print("commands we got is",command)
            self.comporthandle.write(command + '\r\n')
            time.sleep(1)
            w = self.comporthandle.inWaiting()
            output = self.comporthandle.read(w)
            print output
            console_output = [ line for line in output.split('\r\n')]
        return console_output




class myThread(threading.Thread):
    def __init__(self, handle):
        threading.Thread.__init__(self)
        self.handle = handle
    def run(self):
        captureseriallog(self.handle)

class TestRecoveryTest():
    """Helper methods for Recovery Test."""

    BAUDRATES = (50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800,
                 9600, 19200, 38400, 57600, 115200)
             
    def getCOMPortList(self):
        """\
        Get the list of COM PORTS in the system
        """
        list = serial.tools.list_ports.comports()
        connected = []
        for element in list:
            connected.append(element.device)
        print("Connected COM ports: " + str(connected))
        return connected

def connecttoCOMPort(comport, baudrate):
        ser = serial.Serial(
                port=comport, \
                baudrate=baudrate, \
                parity=serial.PARITY_NONE, \
                stopbits=serial.STOPBITS_ONE, \
                bytesize=serial.EIGHTBITS, \
                    timeout=0)
        print "ConnecttoComPort - %d", ser
        return ser

def stopseriallog(handle):
        print "Stop Serial log capture"
        # global stopflag
        handle.stopflag = 1
        return handle.stopflag

def startdatacapture(handle):
        # global databuf
        handle.databuf = []
        # global datacaptureflag
        handle.datacaptureflag = 1

def stopdatacapture(handle):
        # global datacaptureflag
        handle.datacaptureflag = 0

def getdatabuf(handle):
        # global databuf
        return handle.databuf

def checkLPMReboot(handle, rebootstr='Eddie Bootloader'):
    returnValue = 0
    for items in handle.databuf:
        if rebootstr in items:
            print "LPM rebooted"
            returnValue = 1
    
    # Only for user notification
    if returnValue == 0:
        print "LPM did not reboot"

    return returnValue    
    
def checkAPQReboot(handle, rebootstr='Log Type: B - Since Boot(Power On Reset),  D - Delta,  S - Statistic'):
    returnValue = 0
    for items in handle.databuf:
        if rebootstr in items:
            print "APQ rebooted"
            returnValue = 1
    
    # Only for user notification
    if returnValue == 0:
        print "APQ did not reboot"

    return returnValue    



    
    



def captureseriallog(handle): 
        seq = []
        joined_seq = []
        handle.comporthandle.reset_input_buffer()
        handle.comporthandle.reset_output_buffer()
        
        # global stopflag
        handle.stopflag = 0        

        while True:
            for c in handle.comporthandle.read():
                if c == '\n':
                    handle.filehandle.write("%s" % joined_seq)
                    # handle.write("\r");
                    # global datacaptureflag
                    if handle.datacaptureflag == 1:
                        # global databuf
                        handle.databuf.append(joined_seq)
                    seq = []
                    break
                else:
                    seq.append(c)  # convert from ANSII
                    joined_seq = ''.join(str(v) for v in seq)  # Make a string from array

            # print stopflag
            if handle.stopflag == 1:
                print "Stop flag is 1"
                break
            
if __name__ == "__main__":
    # Summary variables
    ret = 0
    
    while True:
        comportstr = "/dev/ttyUSB0"
        APQhandle = SerialLoggingUtil('APQ', comportstr)
        startdatacapture(APQhandle)
    
        comportstr = "/dev/ttyUSB1"
        LPMhandle = SerialLoggingUtil('LPM', comportstr)
        startdatacapture(LPMhandle)
    
        p = myThread(APQhandle)
        p.start()
        q = myThread(LPMhandle)
        q.start()
    
        time.sleep(40)
        # time.sleep(5)
        stopdatacapture(APQhandle)
        stopdatacapture(LPMhandle)
    
        ret = stopseriallog(APQhandle)
        ret = stopseriallog(LPMhandle)
    
        p.join()
        q.join()
    
        APQhandle.comporthandle.close()
        LPMhandle.comporthandle.close()
    
        APQhandle.filehandle.close()
        LPMhandle.filehandle.close()
    
        data = getdatabuf(APQhandle)
        for item in data:
            print item
    
        data = getdatabuf(LPMhandle)
        for item in data:
            print item
    
        checkLPMReboot(LPMhandle)
        checkAPQReboot(APQhandle)
        break
    
