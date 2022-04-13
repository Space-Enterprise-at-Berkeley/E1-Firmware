import serial
ser = serial.Serial('/dev/cu.usbserial-AB0KTZ91', 57600)

def do(s):
    for i in s:
        if (i == ord('\n')): print("\\n")
        else: 
            if 20 < i < 127: print(chr(i), end="")
            else: 
                if i==0: print("0", end="")
                else: print("?", end="")
while True:
    s = ser.in_waiting
    if (s > 0):
        do(ser.read(s))


