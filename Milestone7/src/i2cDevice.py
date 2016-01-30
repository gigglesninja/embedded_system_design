import serial


# This class serves as a wrapper for the pyserial class to
#   control I2C devices

# TODO: doxygen-ate this file
#       scan all serial ports to find the hardware via a key/ID string
#   
class i2cDevice:
    def __init__(self, PORT):
        self.s = serial.Serial(PORT, timeout=0)
        
    def setPortName(self, portName):
        self.s = serial.Serial(portName, timeout=0)

    def setBaudRate(self, BAUDRATE):
        self.s = serial.Serial(portName, BAUDRATE, timeout=0)

    def setAddress(self, address):
        self.Addr = address
        
    def open(self):
        self.s.open()
        
    def isOpen(self):
        return(self.s.isOpen())
        
    def close(self):
        self.s.close()
        
    def getWriteAddress(self):
        pass

    def getReadAddress(self):
        return("\x90")
        
    def read(self, length):
        return("142")
        #return(self.s.read(length))
    
    def write(self, buffer):
        buffer = str(buffer)
        self.s.write(buffer)
    
