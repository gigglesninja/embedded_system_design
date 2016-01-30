from i2cDevice import i2cDevice
import serial.tools.list_ports

class ds1631(i2cDevice):
    def __init__(self):
        comports = (list(serial.tools.list_ports.comports()))
        for i in range(len(comports)):
            if 'FTDIBUS\\VID_0403+PID_6001+FTGNULUEA\\0000' in comports[i]:
                PORT = comports[i][0]
        self.i2c = i2cDevice(PORT)

    def getConfig(self):
        self.i2c.write(self.i2c.getReadAddress())    #Need to add a "0" to the LSB (address is only 7 bytes)
        return("1")
        #return(self.i2c.read(1))
        
    def setConfig(self,configData):
        self.i2c.write(configData)
        
    def getCounter(self):    
        pass
        
    def getTempC(self):
        pass
        
    def getTempF(self):
        TempC = self.getTempC()
        #Convert to F
        return(TempF)
        
    def softwareReset(self):
        self.i2c.write(54)
        
    # Returns the thermostat low value in Celsius.
    def getTH(self):
        self.i2c.write(self.i2c.getReadAddress())    #Need to add a "0" to the LSB (address is only 7 bytes)
        self.i2c.write(0xA1)
        data = self.i2c.read(2)
    
    # TH data must be in Celsius.
    def setTH(self,thData):
        pass
    
    # Returns the thermostat low value in Celsius.
    def getTL(self):
        self.i2c.write(self.i2c.getReadAddress())    #Need to add a "0" to the LSB (address is only 7 bytes)
        self.i2c.write(0xA2)
        data = self.i2c.read(2)
    
    # TL value must be in Celsius.
    def setTL(self, tlData):
        pass
        
    def startConvert(self):
        self.i2c.write("\x51")
        
    def stopConvert(self):
        self.i2c.write("\x22")
    
    # Resolution (res) should only be values from 9-12.
    def setResolution(self, res):
        if res == 9:
            config = int(self.getConfig()) & 0xF3
            self.setConfig(config)
        elif res == 10:
            config = int(self.getConfig()) | 0xF7
            config = config & 0x08
            self.setConfig(config)
        elif res == 11:
            config = int(self.getConfig()) | 0xFB
            config = config & 0x04
            self.setConfig(config)
        else:
            config = int(self.getConfig()) | 0x0C
            self.setConfig(config)
    
    # Should pass either True for active high or False for active low.
    # See DS1631 datasheet, pages 10-11.
    def setPolarity(self, pol):
        if pol:
            self.setConfig(int(self.getConfig()) | 0x02)
        else:
            self.setConfig(int(self.getConfig()) & 0xFD)
    
    # True for one shot enabled, False for continuous mode.
    def setOneShot(self, oneshot):
        if oneshot:
            self.i2c.write(0x01)
        else:
            self.i2c.write(0x00)
    
    # Returns True for flag set, False otherwise.
    def getTHFlag(self):
        return(int(self.getConfig()) & 0x40)
        
    # Returns True for flag set, False otherwise.
    def getTLFlag(self):
        return(int(self.getConfig()) & 0x20)
    
    def clearTHFlag(self):
        self.setConfig(int(self.getConfig()) & 0xBF)
        
    def clearTLFlag(self):
        self.setConfig(int(self.getConfig()) & 0xDF)
    
    # Returns True if conversion complete, false otherwise.
    def isConversionDone(self):
        self.i2c.write(self.i2c.getReadAddress())    #Need to add a "0" to the LSB (address is only 7 bytes)
        self.i2c.write(0xAC)
        data = self.i2c.read(1)
        data = int(data)
        if data >= 128:
            return True
        else:
            return False
        
    # Returns True if write is in progress, false otherwise.
    def isNonVolatileMemoryBusy(self):
        pass
    

