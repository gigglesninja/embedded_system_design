from ds1631 import ds1631 as ds1631Import
import serial

class ds1631Serial:
    def __init__(self):
        self.ds1631 = ds1631Import()

    def stopConvert(self):
        self.ds1631.stopConvert()

    def setOneShot(self, oneshot):
        self.ds1631.setOneShot(oneshot)

    def isConversionDone(self):
        return(self.ds1631.isConversionDone())

    def getConfig(self):
        return(self.ds1631.getConfig())

    def setConfig(self, configData):
        self.ds1631.setConfig("154")
        #self.ds1631.setConfig(configData)
        
    def setPolarity(self, pol):
        self.ds1631.setPolarity(pol)
        
    def setResolution(self, res):
        self.ds1631.setResolution(res)
        
    def setTL(self, tlData):
        self.ds1631.setTL(tlData)

    def setTH(self, thData):
        self.ds1631.setTH(thData)

    def getTL(self):
        self.ds1631.getTL()

    def getTH(self):
        self.ds1631.getTH()

    def clearTLFlag(self):
        self.ds1631.clearTLFlag()

    def clearTHFlag(self):
        self.ds1631.clearTHFlag()

    def getTempC(self):
        self.ds1631.getTempC()

    def getTLFlag(self):
        self.ds1631.getTLFlag()
        
    def startConvert(self):
        self.ds1631.startConvert()

def ConvertHexToAscii(HexString):
    TempString = ""
    ReturnString = ""
    i = 0
    j = 0
    while i < len(HexString):
        while j < i+2:
            TempString += HexString[j]
            j += 1
        ReturnString += chr(int(TempString, 16))
        TempString = ""
        i += 2
    return(ReturnString)

def SendCmds(CmdsToSend):
    EchoString = ""
    for i in CmdsToSend:
        if "*" in i:
            EchoString += i[1:]
            CmdsToSend.remove(i)
        elif i == ":":
            CmdsToSend.remove(":")
            CmdsToSend.insert(0, ":")
        #Send to serial

        

CmdsArray = ['G', 'J14', 'L', 'N', 'P', 'R', 'S', 'T', 'W', ':']
CmdsToSend = []
TempCmd = ""
i = 0
Cmds = raw_input()
while Cmds[i] != 'P':
    
    if Cmds[i] == 'G':
        CmdsToSend.append(Cmds[i] + Cmds[i+1])
        i+=1
        
    elif Cmds[i] == 'J':
        if Cmds[i+1] == '1':
            if Cmds[i+2] == '4':
                TempCmd = Cmds[i] + Cmds[i+1] + Cmds[i+2] + Cmds[i+3] + Cmds[i+4]
                CmdsToSend.append(TempCmd)

                TempCmd = ""
                i += 4
                
    elif Cmds[i] == 'L':
        TempCmd = Cmds[i] + Cmds[i+1] + Cmds[i+2] + Cmds[i+3] + Cmds[i+4]
        CmdsToSend.append(TempCmd)
        TempCmd = ""
        i += 4
        
    elif Cmds[i] == 'N':
        CmdsToSend.append("N")
        i += 1
        
    elif Cmds[i] == 'P':
        CmdsToSend.append("P")
        i += 1
        
    elif Cmds[i] == 'R':
        CmdsToSend.append(Cmds[i] + Cmds[i+1] + Cmds[i+2])
        i += 3
        
    elif Cmds[i] == 'S':
        TempCmd += Cmds[i]
        i+=1
        while Cmds[i] not in CmdsArray:
            TempCmd += Cmds[i]
            i += 1
        CmdsToSend.append(TempCmd)
        TempCmd = ""
        
    elif Cmds[i] == 'T':
        i += 1
        while Cmds[i] not in CmdsArray:
            TempCmd += Cmds[i]
            i += 1
        CmdsToSend.append("*" + ConvertHexToAscii(TempCmd))
        TempCmd = ""
        
    elif Cmds[i] == 'W':
        TempCmd += Cmds[i]
        i+=1
        while Cmds[i] not in CmdsArray:
            TempCmd += Cmds[i]
            i += 1
        CmdsToSend.append(TempCmd)
        i += 2
        
    elif Cmds[i] == ":":
        CmdsToSend.append(Cmds[i])
        i += 1
    else:
        print("Error: "+ Cmds[i])
        print("\n")

    TempCmd = ""
    try:
        Cmds[i]
    except IndexError:
        print("!!BREAK!!")
        break

SendCmds(CmdsToSend)   

