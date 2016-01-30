import ds1631 as ds
import time

d = ds.ds1631()
d.setConfig("9051P")

while(True):
	temp = "HELLO"
	temp = d.getTempC()
	time.sleep(1)
	temp = d.getTempC()
	high = temp[:5]
	low = temp[7:]
	temp = high + low
	temp = temp[3:7]
	tempMSB = temp[:2]
	tempLSB = temp[2:]
	tempMSB = int(tempMSB, 16)
	tempLSB = int(tempLSB, 16)
	tempLSB = float(tempLSB)
	temp = float(tempMSB) + float(tempLSB/256)
	print(str(temp) + " C")
	print(str(temp * 1.8 + 32) + " F")