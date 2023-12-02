from matplotlib import pyplot as plt, patches
import math
import random
from utils import findArc, mmToSteps
#steps in a rotation
stepsPerRot = 3200
wheelCircumference = (80*math.pi)
#times number of steps in a rotation divided by the circumference of the wheel
mmToSteps = stepsPerRot/wheelCircumference

def makePlot(xLst, yLst, circleCenters, radius, tanPoints):
  plt.rcParams["figure.figsize"] = [7.00, 7.00]
  plt.rcParams["figure.autolayout"] = True
  fig = plt.figure()
  ax = fig.add_subplot()
  for h, k in circleCenters:
    circle1 = patches.Circle((h, k), radius=radius, color='green', fill = False)
    ax.add_patch(circle1)
  plt.plot(xLst, yLst, 'bo', linestyle="--")
  for x,y in tanPoints:
    plt.plot(x,y, marker = "o", markersize = 5, markerfacecolor = "red")
  # plt.plot(10000, 2757.3593128807133, 'ro')
  # plt.plot(15121.320343559642, 4878.679656440356, 'ro')
  ax.axis('equal')
  # ax.autoscale_view()
  ax.set_xlim([0, 150000])
  ax.set_ylim([-80000, -40000])
  plt.show()


xLst, yLst = ([50000, 72500, 72500.0], [-50000, -50000, -66250.0])
# xLst = [10000, 10000, 20000]
# yLst = [0, 10000, 20000] 
# print(xLst, yLst)
circleCenters = []
circleCenters.append(findArc(xLst[0], yLst[0], xLst[1] ,yLst[1] , xLst[2], yLst[2], 200 ))
# circleCenters.append(findArc(xLst[1], yLst[1], xLst[2] ,yLst[2] , xLst[3], yLst[3], 300))
# print(circleCenters)
makePlot(xLst,yLst, circleCenters,200 * mmToSteps, [[69953.52091052968, -50000.0], [72500.0, -52546.479089470326]])

