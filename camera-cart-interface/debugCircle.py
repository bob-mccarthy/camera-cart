from matplotlib import pyplot as plt, patches
import math
from line import findArc
import random
#steps in a rotation
stepsPerRot = 3200
wheelCircumference = (80*math.pi)
#times number of steps in a rotation divided by the circumference of the wheel
mmToSteps = stepsPerRot/wheelCircumference

def makePlot(xLst, yLst, circleCenters, radius):
  plt.rcParams["figure.figsize"] = [7.00, 7.00]
  plt.rcParams["figure.autolayout"] = True
  fig = plt.figure()
  ax = fig.add_subplot()
  for h, k in circleCenters:
    circle1 = patches.Circle((h, k), radius=radius, color='green')
    ax.add_patch(circle1)
  plt.plot(xLst, yLst, 'bo', linestyle="--")
  # plt.plot(10000, 2757.3593128807133, 'ro')
  # plt.plot(15121.320343559642, 4878.679656440356, 'ro')
  ax.axis('equal')
  ax.autoscale_view()
  ax.set_xlim([0, 30000])
  ax.set_ylim([-10000, 30000])
  plt.show()

# xLst = [10000, 10000, 20000]
# yLst = [0, 10000, 20000] 
# print(xLst, yLst)
# circleCenters = [findArc(xLst[0], yLst[0], xLst[1] ,yLst[1] , xLst[2], yLst[2], 3000)]
# print(circleCenters)
# makePlot(xLst,yLst, circleCenters,3000)