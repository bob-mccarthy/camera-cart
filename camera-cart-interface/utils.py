import math
import numpy as np

def find_angle(x1, y1, x2, y2,x3,y3):
      #u and v are two vectors that share the point (x2, y2)
      # as the lines made by the 3 points
      # the calculations below assume the up is positive y
      # so multiply the y value by negative one because on the GUI
      # down in positive
      u = (x1-x2, (y1-y2) * -1)
      v = (x3-x2, (y3-y2) * -1)
      
      #angle between two vectors
      # acos(dot_prod(u,v)/ ||u||*||v||)
      magU = math.sqrt(u[0] ** 2 + u[1] ** 2)
      magV = math.sqrt(v[0] ** 2 + v[1] ** 2)

      dot_prod = u[0]*v[0] + u[1]*v[1]
      print((x1, y1), (x2, y2), (x3,y3))
      print(u,v)
      # Calculate the angle between the two vector in radians
      angle = math.acos(dot_prod/(magU*magV))


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

def processPoints(xLst, yLst, scale):
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
   
  

xLst = [0, 1, 1,0]
yLst = [0, 0, 1,1]


# print(processPoints(xLst, yLst, 500))

# print(rotateVector([0,1], -90))

# print(rotateTriangle([20, 0], [-10, 5], [-10,-5], 90, 1, 2))

# print(find_angle(0, 0, 1, 0, 1, 1)) 
# print(find_angle(0, 1, 1, 1, 2, 0))