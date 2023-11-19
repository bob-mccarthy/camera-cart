from matplotlib import pyplot as plt, patches
import math
from utils import findArcInLines
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
  ax.axis('equal')
  ax.autoscale_view()
  ax.set_xlim([0, 50000])
  ax.set_ylim([-100000, 100000])
  plt.show()
xLst = [random.random() * 50000 for _ in range(3)]
yLst = [random.random() * 50000 for _ in range(3)]
print(xLst, yLst)
circleCenters = findArcInLines(xLst[0], yLst[0], xLst[1], yLst[1], xLst[2], yLst[2], 400 * mmToSteps)
print(circleCenters)
makePlot(xLst,yLst, circleCenters,400 * mmToSteps)