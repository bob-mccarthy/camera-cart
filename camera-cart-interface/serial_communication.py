import serial 
import serial.tools.list_ports
import json
import time
from utils import processPointsNoStops, processPointsWStops
import threading

class Communication:
  
    def __init__(self):
        self.stop = False


    def listAllPorts(self):
        ports = serial.tools.list_ports.comports()
        return [str(port) for port,_,_ in sorted(ports)]
    def connectToDevice(self, selectedPort, baudrate = 115200):
        self.connectedDevice = serial.Serial(port = selectedPort, baudrate=baudrate, timeout=.1) 
        
    def sendDirection(self, direction, magnitude):
        #converts directions to the sign that the movement should have as well as what mode the cart has to be in
        directionsToNums = {"Up": (1,8), "Down": (-1,8), "Left" : (-1,9), "Right": (1,9)}
        #  print(f'Sending {directionsToNums[direction]}')
        message = {
            "numInstructions" : 1,
            "moveInstructions" : [magnitude * directionsToNums[direction][0]],
            "timeInstructions" : [0],
            "modeInstructions" : [directionsToNums[direction][1]]
        }
        # print(message)
        self.sendDict(message)

    def sendDict(self, dictMessage):
        jsonMessage = json.dumps(dictMessage)
        #converts message from a string to a bytes to write to the serial monitor
        self.connectedDevice.write(bytes(jsonMessage + "\n", 'utf-8'))

    def sendPoints(self, xLst, yLst, scale):
        moveInstructions, modeInstructions = processPointsNoStops(xLst, yLst, scale)
        print(moveInstructions, modeInstructions)
        message = {
        "numInstructions" : len(moveInstructions),
        "moveInstructions": moveInstructions,
        "timeInstructions": [0 for _ in range(len(moveInstructions))],
        "modeInstructions": modeInstructions
        }
        self.sendDict(message)
        time.sleep(0.5)
    def serialMonitor(self, textbox):
        text = ""
        while True:
            while self.connectedDevice.in_waiting:
                text += str(self.connectedDevice.readline())
                textbox.setText(text)
                # print(self.connectedDevice.readline())


  


# hubCommunication = Communication()
# ports = hubCommunication.listAllPorts()
# print(ports)
# # # baudrate = 115200

# xLst = [0, 1, 2]
# yLst = [0, 0, 1]
# # # for i in range(len(comPorts)):
# # #         print("{}: {}".format(i, comPorts[i][0]))

# portNum = input("what device would you like to connect to? ")
# # # selectedPort = str(comPorts[int(portNum)][0])
# # # connectedDevice = 
# hubCommunication.connectToDevice(ports[int(portNum)])

# jsonMessage = json.dumps(message)
# # print(type(jsonMessage))
# connectedDevice.write(bytes(jsonMessage, 'utf-8')) 
# 

