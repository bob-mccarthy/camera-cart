from utils import rotateVector, rotateTriangle

class Roomba:
  def __init__(self, startX, startY, base, height):
    self.__unitVector = [1,0]
    self.__startX,self. __startY = startX, startY
    self.__cX, self.__cY = startX, startY
    self.__currRotation = 0
    #points that make up the roomba, with a center around 0, 0
    self.__D = height, 0
    self.__E = -height/2,  base/2
    self.__F = -height/2, -base/2

  #resets roomba to its original position and orientation
  def reset(self):
    self.__cX, self.__cY = self.__startX,self. __startY
    self.__currRotation = 0
    self.__unitVector = [1,0]

  #moves the center of the roomba by distance along the unit vector it is orientated in
  def move(self, distance):
    self.__cX += self.__unitVector[0] * distance 
    self.__cY += self.__unitVector[1] * distance * -1
    return self.__cX, self.__cY
  
  #turns the roomba angle degrees
  def turn(self, angle):
    self.__currRotation += angle
    self.__unitVector = rotateVector([1,0], self.__currRotation)

  #gets the points that make up the roomba
  def getRoombaCursor(self):
    return rotateTriangle(self.__D, self.__E, self.__F, self.__currRotation, self.__cX, self.__cY)
  
  def getRoombaCenter(self):
    return self.__cX, self.__cY
    