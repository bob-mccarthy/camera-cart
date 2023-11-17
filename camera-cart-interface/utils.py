import math
import numpy as np

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
  turnRadius = 500
  moveInstructions = []
  modeInstructions = []
  lastX, lastY = xLst[0], yLst[0]
  for i in range(0, len(xLst) - 1):
    
    if i < len(xLst) - 2:
      # print(xLst[i], yLst[i],xLst[i+1], yLst[i+1],xLst[i+2], yLst[i+2])
      tanU, tanV, theta, isRight = findArcInLines(xLst[i], yLst[i],xLst[i+1], yLst[i+1],xLst[i+2], yLst[i+2], turnRadius)
      slowerSpeed, accelTime, slowerSteps, fasterSteps = calculateArcSpeed(turnRadius, axleLength, acceleration, baseSpeed, theta)
      # print(f'tanU: {tanU}')
      # print(f'tanV: {tanV}')
      #append an instruction from the line to the beginning of the arc
      moveInstructions.append(round(math.sqrt((tanU[0] - lastX) ** 2  + (tanU[1] - lastY) ** 2)))
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
      lastX = tanV[0]
      lastY = tanV[1]
      # #append the instruction for the straight line after the arc
      # moveInstructions.append(math.sqrt((xLst[i+2] - tanV[0]) ** 2  + (yLst[i+2] - tanV[1]) ** 2 ))
      # modeInstructions.append(0)
    else:
      moveInstructions.append(round(math.sqrt((xLst[i+1] - lastX) ** 2  + (yLst[i+1] - lastY) ** 2 )))
      modeInstructions.append(0)
    # else:
    #   #add straight line to move and mode instructions
    #   moveInstructions.append(math.sqrt((xLst[i+1] - xLst[i]) ** 2  + (yLst[i+1] - yLst[i]) ** 2 ) * scale)
    #   modeInstructions.append(0)

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
  

#given three points that represent two intersecting vectors and the radius a circle, in millimeters 
# it returns the two points on the line that are tangent to the
# circle of radius r, the degrees of the turn and whether 
# you need to go right or left
def findArcInLines(x1, y1, x2, y2, x3, y3, r):
  #convert radius to steps
  r *= mmToSteps

  #u and v are two vectors that share the point (x2, y2)
  # as the lines made by the 3 points
  # the calculations below assume the up is positive y
  # so multiply the y value by negative one because on the GUI
  # down in positive
  u = (x1-x2, (y1-y2))
  v = (x3-x2, (y3-y2))

  angle = find_acute_angle(u,v)
  s = r/(math.sin(angle/2))

  magU = math.sqrt(u[0] ** 2 + u[1] ** 2)

  #determine whether the vector needs to be rotated CW or CCW

  mConstant = 1
  #if u is not a vertical line than what we check is
  # check if vector v is below the line defined by u and multiply by -1
  # also check if the vector u is going backwards and if so mulitply by -1
  # the result number is the sign of the angle we should turn 
  if u[0] != 0:
    mConstant = (-1 if ((u[1]/u[0])*v[0] > v[1]) else 1) * (-1 if (u[0] > 0) else 1)
  #if u is a vertical line than what we check is
  # if u is to the right of v (v[0] > u[0])
  # if u is going downward (u[1] > 0]) then -1 
  else:
    mConstant = (-1 if (v[0] > u[0]) else 1) * (-1 if (u[1] > 0) else 1)


  #get vector along u that is of length s and rotate it by angle/2
  circleCenter = rotateVector([(v[0]/magU) * s, (v[1]/magU) * s], mConstant* math.degrees(angle/2))

  #vector perpendicular to u with magnitude r
  pU = [1, -u[0]/u[1]] if u[1] != 0 else [0, 1]
  magPU = math.sqrt(pU[0] ** 2 + pU[1] ** 2)
  pU = [(pU[0]/magPU) * r, (pU[1]/magPU) * r]

  #vector perpendicular to v with magnitude r
  pV = [1, -v[0]/v[1]] if v[1] != 0 else [0,1]
  magPV = math.sqrt(pV[0] ** 2 + pV[1] ** 2)
  pV = [(pV[0]/magPV) * r, (pV[1]/magPV) * r]



  #you go take the circle center and move along the vector that is perpendicular to u
  # one of these vectors should be line on u and be the point where the circle is tangent
  # to the line
  tmpU1 = [circleCenter[0] - pU[0] , circleCenter[1] - pU[1]]
  tmpU2 = [circleCenter[0] + pU[0] , circleCenter[1] + pU[1]]


  #same concept as above, but with v
  tmpV1 = [circleCenter[0] - pV[0] , circleCenter[1] - pV[1]]
  tmpV2 = [circleCenter[0] + pV[0] , circleCenter[1] + pV[1]]


  #find the point the two vectors that are colinear to u and v
  #the resulting points are where the circle is tangent to the lines 
  tanU = tmpU1 if isColinear(tmpU1, u) else tmpU2
  tanV = tmpV1 if isColinear(tmpV1, v) else tmpV2

  dist = math.sqrt((tanU[0] - tanV[0]) ** 2 + (tanU[1] - tanV[1]) ** 2)
  #find the angle, in degrees, between tanU and tanV using law of cosines
  theta = math.degrees(math.acos(1 - (dist**2/(2*r**2))))


  #find the angle of rotation need to get to point tanU and tanV
  # check if the point is directly above or below the center and if so it is a 90 or -90 degree turn
  thetaU = math.atan((tanU[1] - circleCenter[1])/(tanU[0] - circleCenter[0])) if tanU[0] != circleCenter[0] else (90 if tanU[1] > circleCenter[1] else -90)
  thetaV = math.atan((tanV[1] - circleCenter[1])/(tanV[0] - circleCenter[0])) if tanV[0] != circleCenter[0] else (90 if tanV[1] > circleCenter[1] else -90)
  # print(thetaU, thetaV)
  thetaU += 90
  thetaU += 90
  # #transform negative rotations to their first positve counterpart
  # if thetaU < 0:
  #   thetaU = -thetaU + 90
  # if thetaV < 0:
  #   thetaV = -thetaV + 90
  

  #90 degrees of rotation as the start and -90 degrees as the end
  # you take a right turn if tanV comes after tanU on the circle
  isRight = thetaU > thetaV
  

  #translate the tangent points back to their original positions
  # since we centered both vectors at 0
  tanU = [round(tanU[0] + x2, 5), round(tanU[1] + y2, 5)]
  tanV = [round(tanV[0] + x2, 5), round(tanV[1] + y2, 5)]
  # print(tanU, tanV)
  return (tanU, tanV, theta, isRight)
 

   
    
# calculateArcSpeed(250, 290, 2000, 1000, 90)
# calculateArcSpeed(500, 290, 2000, 2000, 90)
# calculateArcSpeed(500, 290, 2000, 2000, 90)
# calculateArcSpeed(1000, 290, 2000, 1000, 90)

# print(findArcInLines(-10000,10000,0,0, -100000,-100000, 500*mmToSteps))
# findArcInLines([-100,100], [-100,-100], 10)
# findArcInLines([100,-100], [100,100], 10)
# findArcInLines([100,0], [0,100], 10)

xLst = [0, 1000, 1000, 0]
yLst = [0, 0, 1000,1000]
# print(processPointsWStops(xLst, yLst, 100))
print(processPointsNoStops(xLst, yLst, 100))