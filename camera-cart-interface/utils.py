import math
import numpy as np
from line import Line

#steps in a rotation
stepsPerRot = 3200
wheelCircumference = (80*math.pi)
#times number of steps in a rotation divided by the circumference of the wheel
mmToSteps = stepsPerRot/wheelCircumference
#length from middle of one wheel to the other, in millimeters
axleLength = 290
acceleration = 2000
baseSpeed = 2000

def find_acute_angle(u, v):
  
    
  #angle between two vectors
  # acos(dot_prod(u,v)/ ||u||*||v||)
  magU = math.sqrt(u[0] ** 2 + u[1] ** 2)
  magV = math.sqrt(v[0] ** 2 + v[1] ** 2)

  dot_prod = u[0]*v[0] + u[1]*v[1]
  # Calculate the angle between the two vector in radians
  angle = math.acos(dot_prod/(magU*magV))
  return angle


# find the angle the robot has to turn to go from the line defined by the first two points,
# to get to the line defined by the last two points
def find_angle(x1, y1, x2, y2,x3,y3):
  #u and v are two vectors that share the point (x2, y2)
  # as the lines made by the 3 points
  # the calculations below assume the up is positive y
  # so multiply the y value by negative one because on the GUI
  # down in positive
  u = (x1-x2, (y1-y2) * -1)
  v = (x3-x2, (y3-y2) * -1)

  #finds the angle between the two vectors
  angle = find_acute_angle(u,v)

  #after finding the angle between the two vectors we then need
  #to find out what angle the robot need to turn 


  #checking if we need to CW or CCW
  #by checking if v is over the line defined by u (if so multiply by -1 otherwise 1)
  # and also checking if the cart is moving backwards (backwards means that u[0] > 0) 
  #   as that means the x component of the first vector u pointed in the negative direction
  # and if so multiply by -1
  #then multiply those numbers to get to figure out where to turn the robot CW or CCW
  mConstant = 1
  if u[0] != 0:
    mConstant = (1 if ((u[1]/u[0])*v[0] > v[1]) else -1) * (-1 if (u[0] > 0) else 1)
  #if u is vertical turn CW if x component of v is greater than x component of u
  # however that is flipped if vector v is going downwards 
  # (downwards means (y component of u is positive) u[1] > 0)
  else:
      mConstant = (1 if (v[0] > u[0]) else -1) * (-1 if (u[1] > 0) else 1)

  return (180 - round(math.degrees(angle),2)) * mConstant



#process points normally into straight line movements and turns about the robot origin
def processPointsWStops(xLst, yLst, scale):
  moveInstructions = []
  modeInstructions = []
  for i in range(len(xLst) - 1):
    #add straight line to move and mode instructions
    moveInstructions.append(math.sqrt((xLst[i+1] - xLst[i]) ** 2  + (yLst[i+1] - yLst[i]) ** 2 ) * scale)
    modeInstructions.append(0)
    if i < len(xLst) - 2:
      #add turn to move and mode instructions
      moveInstructions.append(find_angle(xLst[i], yLst[i],xLst[i+1], yLst[i+1],xLst[i+2], yLst[i+2]))   
      modeInstructions.append(1)
  return (moveInstructions, modeInstructions)   



#processing points but turns all of the straight line turns into curves
def processPointsNoStops(xLst, yLst, scale):
  xLst = [x * scale for x in xLst ]
  yLst = [y * scale * -1 for y in yLst]
  # print(f'points x, y: {xLst, yLst}')
  turnRadius = 300
  moveInstructions = []
  modeInstructions = []
  lastX, lastY = xLst[0], yLst[0]
  for i in range(0, len(xLst) - 1):
    
    if i < len(xLst) - 2:
      tan1, tan2, theta, isRight = findArc(xLst[i], yLst[i],xLst[i+1], yLst[i+1],xLst[i+2], yLst[i+2], turnRadius)
      slowerSpeed, accelTime, slowerSteps, fasterSteps = calculateArcSpeed(turnRadius, axleLength, acceleration, baseSpeed, theta)

      #append an instruction from the line to the beginning of the arc
      moveInstructions.append(round(math.sqrt((tan1[0] - lastX) ** 2  + (tan1[1] - lastY) ** 2)))
      modeInstructions.append(0)

      #append the arc instructions
      #first append the instruction for arc speed
      moveInstructions.append(round(slowerSpeed))
      modeInstructions.append(3 if isRight else 4)
      #append the instruction for the time at the slower speed for the slower motor
      moveInstructions.append(round(accelTime))
      modeInstructions.append(5)
      #append the instruction for the steps of the slower motor
      moveInstructions.append(round(slowerSteps))
      modeInstructions.append(6)
      #append the instruction for the steps of the slower motor
      moveInstructions.append(round(fasterSteps))
      modeInstructions.append(7)
      
      # print(tanU, tanV)
      lastX = tan2[0]
      lastY = tan2[1]
    else:
      moveInstructions.append(round(math.sqrt((xLst[i+1] - lastX) ** 2  + (yLst[i+1] - lastY) ** 2 )))
      modeInstructions.append(0)

  return (moveInstructions, modeInstructions)  


def rotateVector(u, angle):
  x, y = u
  angle_radians = math.radians(angle)
  x_new = x * math.cos(angle_radians) - y * math.sin(angle_radians)
  y_new = x * math.sin(angle_radians) + y * math.cos(angle_radians)
  mag = (math.sqrt(x_new*x_new + y_new*y_new))
  return (x_new, y_new)

#giving a triangle centered around the origin
#it rotates it and then translates its center to cX, cY
def rotateTriangle(A, B, C, angleDegrees, cX, cY):
  # Convert the angle from degrees to radians
  angleRadians = np.radians(angleDegrees)

  # Create the 2D rotation matrix
  rotationMatrix = np.array([[np.cos(angleRadians), -np.sin(angleRadians)],
                              [np.sin(angleRadians), np.cos(angleRadians)]])
  # Rotate the translated triangle
  rotatedTriangle = np.dot(np.array([A,B,C]), rotationMatrix)
  translatedArray = rotatedTriangle + np.array([cX, cY])

  return translatedArray.astype(int).tolist()

#returns the speed of the slower for the motor, the time the slower motor needs to be at the slower speed (in microseconds),
#the steps of bool motors in order turn in an arc 
# given the speed of the radius of the turn, the axle length of the cart,
# the acceleration of the cart, the speed of the faster motor, degrees of the turn
def calculateArcSpeed(radius, axleLength, accel, baseSpeed, degrees):
  C = (2 * radius)/axleLength
  ratio = ((C-1)/(C+1))
  fasterSteps = ((2*math.pi*(radius+axleLength/2))) * (degrees/360) * mmToSteps
  totalTime = fasterSteps/baseSpeed
  L = min(np.roots([accel/totalTime, -accel, baseSpeed - baseSpeed*ratio]))
  accelTime = totalTime - L
  slowerSpeed = baseSpeed - (accel* L)
  return (slowerSpeed, int(accelTime * 1000000), int(ratio*baseSpeed*totalTime), int(fasterSteps))

def isColinear(u, v):
  if u[0] != 0:
    return round((v[0]/u[0]) * u[1],5) == v[1]
  elif u[1] != 0:
    return round((v[1]/u[1]) * u[0], 5) == v[0]
  else:
    return True 
  

#find the arc which is tangent to the two line segments defined by 
#the 3 points input into the function with a radius of r
def findArc(x1, y1, x2, y2, x3, y3, r):
  r *= mmToSteps

  l1 = Line([x1, y1],[x2, y2])
  l2 = Line([x2, y2], [x3, y3])

  l1Offset1 = l1.offset(r)
  l1Offset2 = l1.offset(-r)
  
  #offset line 1 by radius units, and we pick the line which is closer to l2
  l1Offset = None
  if l1.isVertical:
    #if l1 is vertical we pick the line whose x value is closer the x value of 
    #point 2 on l2 (which is the point that l1 and l2 do not have in common)
    l1Offset = l1Offset2 if (abs(l2.p2[0] - l1Offset2.p1[0]) < 
                            abs(l2.p2[0] - l1Offset1.p1[0])) else l1Offset1
  else:
    #if l1 is a function than we plug the x value of point 2 for line 2 into
    #each of the potential offset lines and we pick the line whose y value is closer to
    #line 2 point 2's y value
    l1Offset = l1Offset2 if (abs(l1Offset2.f(l2.p2[0]) - l2.p2[1]) < 
                            abs(l1Offset1.f(l2.p2[0]) - l2.p2[1])) else l1Offset1
  
  l2Offset1 = l2.offset(r)
  l2Offset2 = l2.offset(-r)

  #offset line 2 by radius units, and we pick the line which is closer to l1
  l2Offset = None
  if l2.isVertical:
    #if l2 is vertical we pick the line whose x value is closer the x value of 
    #point 1 on l2 (which is the point that l1 and l2 do not have in common)
    l2Offset = l2Offset2 if (abs(l1.p1[0] - l2Offset2.p1[0]) < 
                            abs(l1.p1[0] - l2Offset1.p1[0])) else l2Offset1
  else:
    #if l2 is a function than we plug the x value of point 1 for line 1 into
    #each of the potential offset lines and we pick the line whose y value is closer to
    #line 1 point 1's y value
    l2Offset = l2Offset2 if (abs((l2Offset2.f(l1.p1[0]) - l1.p1[1])) < 
                            abs((l2Offset1.f(l1.p1[0]) - l1.p1[1]))) else l2Offset1

  center = l1Offset.intersect(l2Offset)
  
  l1v1 = l1.perpendicularVector(r)
  l1v2 = l1.perpendicularVector(-r) 

  SIG_FIGS = 3
  #the point on l1 that is tangent to the circle is the point that lies on l1 since both points are 
  #radius distance from the center of the circle 
  tan1 = ([center[0] + l1v1[0], center[1] + l1v1[1]] 
          if ((l1.isVertical and round(l1.p1[0],SIG_FIGS) == round((center[0] + l1v1[0]), SIG_FIGS)) or 
              (not l1.isVertical and round(l1.f(l1v1[0] + center[0]), SIG_FIGS) == round(l1v1[1] + center[1], SIG_FIGS)))                                                       
          else [center[0] + l1v2[0], center[1] + l1v2[1]])

  l2v1 = l2.perpendicularVector(r)
  l2v2 = l2.perpendicularVector(-r) 

  #the point on l1 that is tangent to the circle is the point that lies on l2 since both points are 
  #radius distance from the center of the circle.
  tan2 = ([center[0] + l2v1[0], center[1] + l2v1[1]]  
          if ((l2.isVertical and round(l2.p1[0],SIG_FIGS) == round((l2v1[0] + center[0]), SIG_FIGS)) or 
             (not l2.isVertical and round(l2.f(l2v1[0] + center[0]), SIG_FIGS) == round(l2v1[1] + center[1], SIG_FIGS)))
          else [center[0] + l2v2[0], center[1] + l2v2[1]])

  dist = math.sqrt((tan1[0] - tan2[0]) ** 2 + (tan1[1] - tan2[1]) ** 2)

  #find the angle, in degrees, between tan1 and tan2 using law of cosines
  #as the center, tan1, and tan2 form a triangle where 2 of the sides are length 2 and 
  # one is length dist
  theta = math.degrees(math.acos((2*(r**2) - dist**2)/(2*r**2)))
  

  #the turn we are talking is a right if the vector l1 has a component moving towards the right
  #and the point along the arc we are traveling to is also to the right and left otherwise
  #if l1 has a component moving left then those rules flip
  isRight = (tan2[0] > tan1[0]) ^ (l1.p2[0] - l1.p1[0] < 0)
  

  #if l1 is vertical then turn right if l2 to the right of l1 
  #but this is flipped if l1 is going in the negative direction
  if l1.isVertical:
    isRight = (tan2[0] > tan1[0]) ^ (l1.p2[1] - l1.p1[1] < 0)

  #However is the line where are turn to is vertical we turn right if the 
  #l2 is below the current line (which is flipped if we l1 is going to the left)
  if l2.isVertical:
    isRight = (tan1[1] > tan2[1]) ^ (l1.p2[0] - l1.p1[0] < 0)
  # return center
  return tan1, tan2, round(theta, SIG_FIGS), isRight
  


 
# print(calculateArcSpeed(150 * mmToSteps, axleLength, acceleration, baseSpeed, 5))
   
# xLst, yLst = (([20000, 29000, 29000.0, 22026.63711335779], [-20000, -20000, -31500.0, -30889.909800766418]))
# # xLst = [0, 1000, 1000, 0]
# # yLst = [0, 0, 1000,1000]
# # print(processPointsWStops(xLst, yLst, 100))
# print(processPointsNoStops(xLst, yLst, 100))