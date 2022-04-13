from __future__ import print_function
from time import sleep, time
from xmlrpc.client import boolean
import serial
import re
import voicemeeter

ser = serial.Serial()
ser.baudrate = 9600
ser.port = "COM5"
ser.open()

timestamp = str(int(time() * 1000))
ser.write(timestamp.encode())
print(timestamp)

# A1 = True

kind = 'potato'

vmr = voicemeeter.remote(kind)
vmr.login()

def floatToDb(f):
    return f * 73 - 60

while True:
    serialln = str(ser.readline())
    print(serialln)

    slider, value = re.findall('\d+', serialln)
    
    print(f'slider: {slider}, value: {value}')

    #spotify.set_volume(float(value) / 100)

    if slider == "1":
        vmr.inputs[5].gain = floatToDb(float(value) / 100)
    elif slider == "2":
        vmr.inputs[6].gain = floatToDb(float(value) / 100)
    elif slider == "3":
        vmr.inputs[7].gain = floatToDb(float(value) / 100)
    elif slider == "4":
        vmr.inputs[1].gain = floatToDb(float(value) / 100)
    # elif slider == "5":
        # TODO