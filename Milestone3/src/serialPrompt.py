<<<<<<< local
#IMPORTANT
#The logic behind the 5s timeout:
#Line 27 blocks until a "\n" character is received in the serial buffer. This would effectively allow us to print a status message (like "Nothing has happened...") every 5 seconds (or more or
#less) because once the timeout expires, it should break out of the loop, at least that is my understanding
#Alternatively, we can set the readline() to "non-blocking" by changing the timeout to 0


import serial

COM = 'COM7'

print("To do this:                                           Do this:\n")
print("Turn LED2 OFF.........................................<Nothing>")
print("Turn LED2 ON..........................................Move RPG at a SLOW speed")
print("Flash LED2 with a period of 500ms.....................Move RPG at a MEDIUM speed")
print("Flash LED2 with a period of 100ms.....................Move RPG at a FAST speed\n")

print("Have LED1 set by the state of SW1.....................Release SW3")
print("Have LED1 set by the state of SW2.....................Press SW3")
print("Flash LED1 rapidly 3 times............................Double-press SW1/SW2")

picSerial = serial.Serial(COM,baudrate=9600,timeout=5)          #Sets the serial port to COM, baudrate=9600, and timeout of 5s (it seems more like 2s though..)*
picSerial.close()
picSerial.open()
picSerial.flush()                                               #Clears the buffers
    
while picSerial.isOpen():
    serialInput = picSerial.readline()
    while not serialInput:
        serialInput = picSerial.readline()

    if "SLOW" in serialInput:                                  #"XXX" will be either be a keyword in the serial string or it will be the serial string itself
        print("RPG moved at a SLOW speed")

    elif "YYY" in serialInput:
        print("RPG moved at a MEDIUM speed")

    elif "ZZZ" in serialInput:
        print("RPG moved at a FAST speed")

    elif "AAA" in serialInput:
        print("LED1 now determined by state of SW1")
        print("Current state of SW1 is ")

    elif "BBB" in serialInput:
        print("LED1 now determined by state of SW2")
        print("Current state of SW2 is ")

    elif "CCC" in serialInput:
        print("SW1 was doubled pressed")

    elif "DDD" in serialInput:
        print("SW2 was doubled pressed")
        
    else:
        print("Nothing has happened in the last 5 seconds")
        print("Try pressing a button or turning the RPG!!\n")
        
        
    
=======
#IMPORTANT
#The logic behind the 5s timeout:
#Line 27 blocks until a "\n" character is received in the serial buffer. This would effectively allow us to print a status message (like "Nothing has happened...") every 5 seconds (or more or
#less) because once the timeout expires, it should break out of the loop, at least that is my understanding
#Alternatively, we can set the readline() to "non-blocking" by changing the timeout to 0


import serial

COM = 'COM3'

print("To do this:                                           Do this:\n")
print("Turn LED2 OFF.........................................<Nothing>")
print("Turn LED2 ON..........................................Move RPG at a SLOW speed")
print("Flash LED2 with a period of 500ms.....................Move RPG at a MEDIUM speed")
print("Flash LED2 with a period of 100ms.....................Move RPG at a FAST speed\n")

print("Have LED1 set by the state of SW1.....................Release SW3")
print("Have LED1 set by the state of SW2.....................Press SW3")
print("Flash LED1 rapidly 3 times............................Double-press SW1/SW2")

picSerial = serial.Serial(COM,baudrate=9600,timeout=5)          #Sets the serial port to COM, baudrate=9600, and timeout of 5s*
picSerial.open()
picSerial.flush()                                               #Clears the buffers
    
while picSerial.isOpen():
    serialInput = picSerial.readline()
    while not serialInput:
        serialInput = picSerial.readline()

    if "XXX" in serialInput:                                  #"XXX" will be either be a keyword in the serial string or it will be the serial string itself
        print("RPG moved at a SLOW speed")

    elif "YYY" in serialInput:
        print("RPG moved at a MEDIUM speed")

    elif "ZZZ" in serialInput:
        print("RPG moved at a FAST speed")

    elif "AAA" in serialInput:
        print("LED1 now determined by state of SW1")
        print("Current state of SW1 is ")

    elif "BBB" in serialInput:
        print("LED1 now determined by state of SW2")
        print("Current state of SW2 is ")

    elif "CCC" in serialInput:
        print("SW1 was doubled pressed")

    elif "DDD" in serialInput:
        print("SW2 was doubled pressed")

    else:
        print("Nothing has happened in the last 5 seconds")
        print("Try pressing a button or turning the RPG!!")
        
        
    
>>>>>>> other
