from __future__ import print_function
from time import sleep, time
from xmlrpc.client import boolean
import serial
import re
from pycaw.pycaw import AudioUtilities
import os
import voicemeeter

class AudioController(object):
    def __init__(self, process_name):
        self.process_name = process_name
        self.volume = self.process_volume()

    def mute(self):
        sessions = AudioUtilities.GetAllSessions()
        for session in sessions:
            interface = session.SimpleAudioVolume
            if session.Process and session.Process.name() == self.process_name:
                interface.SetMute(1, None)
                print(self.process_name, 'has been muted.')  # debug

    def unmute(self):
        sessions = AudioUtilities.GetAllSessions()
        for session in sessions:
            interface = session.SimpleAudioVolume
            if session.Process and session.Process.name() == self.process_name:
                interface.SetMute(0, None)
                print(self.process_name, 'has been unmuted.')  # debug

    def process_volume(self):
        sessions = AudioUtilities.GetAllSessions()
        for session in sessions:
            interface = session.SimpleAudioVolume
            if session.Process and session.Process.name() == self.process_name:
                #print('Volume:', interface.GetMasterVolume())  # debug
                return interface.GetMasterVolume()

    def set_volume(self, decibels):
        sessions = AudioUtilities.GetAllSessions()
        for session in sessions:
            interface = session.SimpleAudioVolume
            if session.Process and session.Process.name() == self.process_name:
                # only set volume in the range 0.0 to 1.0
                self.volume = min(1.0, max(0.0, decibels))
                interface.SetMasterVolume(self.volume, None)

    def decrease_volume(self, decibels):
        sessions = AudioUtilities.GetAllSessions()
        for session in sessions:
            interface = session.SimpleAudioVolume
            if session.Process and session.Process.name() == self.process_name:
                # 0.0 is the min value, reduce by decibels
                self.volume = max(0.0, self.volume-decibels)
                interface.SetMasterVolume(self.volume, None)
                print('Volume reduced to', self.volume)  # debug

    def increase_volume(self, decibels):
        sessions = AudioUtilities.GetAllSessions()
        for session in sessions:
            interface = session.SimpleAudioVolume
            if session.Process and session.Process.name() == self.process_name:
                # 1.0 is the max value, raise by decibels
                self.volume = min(1.0, self.volume+decibels)
                interface.SetMasterVolume(self.volume, None)
                print('Volume raised to', self.volume)  # debug

ser = serial.Serial()
ser.baudrate = 9600
ser.port = "COM5"
ser.open()

timestamp = str(int(time() * 1000))
ser.write(timestamp.encode())
print(timestamp)

# A1 = True

# spotify = AudioController('Spotify.exe')
# chrome = AudioController('chrome.exe')
# discord = AudioController('Discord.exe')

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