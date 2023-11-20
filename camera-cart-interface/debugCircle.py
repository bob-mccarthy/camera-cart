from matplotlib import pyplot as plt, patches
import math
import random
from utils import findArc, mmToSteps
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
    circle1 = patches.Circle((h, k), radius=radius, color='green', fill = False)
    ax.add_patch(circle1)
  plt.plot(xLst, yLst, 'bo', linestyle="--")
  # plt.plot(10000, 2757.3593128807133, 'ro')
  # plt.plot(15121.320343559642, 4878.679656440356, 'ro')
  ax.axis('equal')
  ax.autoscale_view()
  ax.set_xlim([0, 40000])
  ax.set_ylim([-40000, 0])
  plt.show()


xLst, yLst = (([20000, 29000, 29000.0, 22026.63711335779], [-20000, -20000, -31500.0, -30889.909800766418]))
# xLst = [10000, 10000, 20000]
# yLst = [0, 10000, 20000] 
# print(xLst, yLst)
circleCenters = []
circleCenters.append(findArc(xLst[0], yLst[0], xLst[1] ,yLst[1] , xLst[2], yLst[2], 300 ))
circleCenters.append(findArc(xLst[1], yLst[1], xLst[2] ,yLst[2] , xLst[3], yLst[3], 300))
# print(circleCenters)
makePlot(xLst,yLst, circleCenters,300 * mmToSteps)

