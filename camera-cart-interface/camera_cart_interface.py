import sys
import typing
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QHBoxLayout
from PyQt5.QtCore import Qt
from serial_communication import Communication
import threading
import time
from roomba import Roomba



CANVAS_LENGTH = 500
CANVAS_HEIGHT = 500

GRID_COLS = 20
GRID_ROWS = 20

UNITS = 50

hubCommunication = Communication()
ports = hubCommunication.listAllPorts()
print(ports)
hubCommunication.connectToDevice(ports[3])



class MainWindow(QtWidgets.QMainWindow):

    def __init__(self):
        super().__init__()
        
        # Set the layout for your main window
        central_widget = QtWidgets.QWidget()

        self.label = QtWidgets.QLabel()
        self.canvas = QtGui.QPixmap(CANVAS_LENGTH, CANVAS_HEIGHT)
        self.canvas.fill(Qt.white)
        self.label.setPixmap(self.canvas)

        # self.setCentralWidget(self.label)
        # layout.addWidget(self.canvas)
        
        #start with one point which is where the robot starts (this point can be moved)
        self.last_x, self.last_y = CANVAS_LENGTH // 2, CANVAS_HEIGHT // 2
        self.xLst, self.yLst = [self.last_x],[self.last_y]


        self.clearButton = QtWidgets.QPushButton("Clear")
        self.sendButton = QtWidgets.QPushButton("Send")

        
        # Connect the button's clicked signal to a function
        self.clearButton.clicked.connect(self.clear_clicked)
        self.sendButton.clicked.connect(self.send_clicked)

        # Create a layout for everything
        layout = QtWidgets.QHBoxLayout()

        rightPanel = QtWidgets.QVBoxLayout()
        rightPanel.addWidget(self.clearButton)
        rightPanel.addWidget(self.sendButton)

        # Add the QLabel and the button to the layout
        layout.addWidget(self.label)
        layout.addLayout(rightPanel)

        
        central_widget.setLayout(layout)

        self.setCentralWidget(central_widget)

        #check the last time a key was pressed to move the robot
        self.lastPressed = 0

        self.roomba = Roomba(CANVAS_LENGTH // 2, CANVAS_HEIGHT // 2, 20, 20)

        self.prevRoombaPoints = None

        #after every turn of the roomba we need to start a new line
        self.startNewLine = True

        self.drawRoomba()



    def keyPressEvent(self, event):
        # if time.time() - self.lastPressed < 0.5:
        #    return
        self.lastPressed = time.time()
        #checking if the robot was previously turning
        # prevWasTurn = self.lastDirection in [Qt.Key_Left, Qt.Key_Right]
        self.lastDirection = event.key()
        
        #send the direction we are moving 
        # and the first movement we do in a new direction should have a larger magnitude then if we are normally in that position
        if event.key() == Qt.Key_Up:
          self.roomba.move(5)
          x, y = self.roomba.getRoombaCenter()
          if self.startNewLine:
            self.xLst.append(x)
            self.yLst.append(y)
          else:
            self.xLst[-1] = x
            self.yLst[-1] = y
          self.startNewLine = False
          hubCommunication.sendDirection('Up',5 * UNITS)
        elif event.key() == Qt.Key_Down:
          
          self.roomba.move(-5)
          x, y = self.roomba.getRoombaCenter()
          if self.startNewLine:
            self.xLst.append(x)
            self.yLst.append(y)
          else:
            self.xLst[-1] = x
            self.yLst[-1] = y
          self.startNewLine = False
          hubCommunication.sendDirection('Down', 5 * UNITS)
        elif event.key() == Qt.Key_Left:
          self.startNewLine = True
          self.roomba.turn(5)
          hubCommunication.sendDirection('Left', 5)
        elif event.key() == Qt.Key_Right:
          self.startNewLine = True
          self.roomba.turn(-5)
          hubCommunication.sendDirection('Right', 5)
        self.drawLines()
        self.drawRoomba()
    # When the clear button is clicked remove all lines and reset the camera start 
    def clear_clicked(self):
        self.last_x, self.last_y = CANVAS_LENGTH // 2, CANVAS_HEIGHT // 2
        self.xLst, self.yLst = [self.last_x],[self.last_y]
        self.label.setPixmap(self.canvas)
        # self.canvas.fill(Qt.white)
        self.update()

    def send_clicked(self):
        hubCommunication.sendPoints(self.xLst, self.yLst, UNITS)

    def snapPosition(self, x, y):
       x = x
       y  = y
      #  x = int((x / CANVAS_LENGTH) * GRID_ROWS) * (CANVAS_LENGTH / GRID_ROWS)
      #  y = int((y / CANVAS_HEIGHT) * GRID_COLS) * (CANVAS_LENGTH / GRID_COLS)
       return x, y

    def mouseMoveEvent(self, e):
        if self.xLst == []: # First event.
            return # Ignore the first time.
        if e.x() > 0 and e.y() > 0 and e.x()<= CANVAS_LENGTH and e.y() <= CANVAS_HEIGHT:
          x, y = self.snapPosition(e.x(), e.y())
          self.drawLines(x, y)
          # Update the origin for next time.
          self.last_x = x
          self.last_y = y
        else:
           self.drawLines()

        

    def mouseReleaseEvent(self, e):
        # if outside the canvas
        if not (e.x() > 0 and e.y() > 0 and e.x()<= CANVAS_LENGTH and e.y() <= CANVAS_HEIGHT):
           self.drawLines()
           return
        x, y = self.snapPosition(e.x(), e.y())
        if self.xLst != []: 
            self.drawLines(x, y)
        self.xLst.append(x)
        self.yLst.append(y)

    def drawRoomba(self):
      painter = QtGui.QPainter(self.label.pixmap())
      pen = QtGui.QPen()
      #if we have draw a roomba in the past draw over it in white
      if self.prevRoombaPoints:
        painter.setBrush(QtGui.QBrush(Qt.white, Qt.SolidPattern))
        pen.setWidth(2)
        pen.setColor(QtGui.QColor('white'))
        painter.setPen(pen)
        oldA, oldB, oldC = self.prevRoombaPoints
        oldPoints = QtGui.QPolygon([
          QtCore.QPoint(*oldA),
          QtCore.QPoint(*oldB),
          QtCore.QPoint(*oldC),
        ])
        painter.drawPolygon(oldPoints)
      painter.setBrush(QtGui.QBrush(Qt.black, Qt.SolidPattern))
      pen.setWidth(2)
      pen.setColor(QtGui.QColor('black'))
      painter.setPen(pen)
      A,B,C = self.roomba.getRoombaCursor()
      self.prevRoombaPoints = A,B,C
      points = QtGui.QPolygon([
        QtCore.QPoint(*A),
        QtCore.QPoint(*B),
        QtCore.QPoint(*C),
      ])
      painter.drawPolygon(points)
      self.update()
       
    #draws all of the lines defined in X and Y list up until currX and currY
    def drawLines(self, currX = None, currY = None):
      painter = QtGui.QPainter(self.label.pixmap())
      pen = QtGui.QPen()
      pen.setWidth(5)
      if self.last_x is not None:
        pen.setColor(QtGui.QColor('white'))
        painter.setPen(pen)
        painter.drawLine(self.xLst[-1],self.yLst[-1], self.last_x, self.last_y)
      pen.setColor(QtGui.QColor('black'))
      painter.setPen(pen)
      for i in range(len(self.xLst) - 1):
          painter.drawLine(self.xLst[i], self.yLst[i], self.xLst[i + 1],  self.yLst[i + 1])
          # painter.drawArc(self.xLst[i], self.yLst[i], 100, 100, 0 * 16, 360 * 16)
      if currX is not None:
        painter.drawLine(self.xLst[-1],self.yLst[-1], currX, currY)
      self.update()
        
        


app = QtWidgets.QApplication(sys.argv)
window = MainWindow()
window.show()
hubCommunication.serialMonitor()
app.exec_()

# while(True):
   