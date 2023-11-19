import math
class Line:
  def __init__(self, p1, p2):
    self.isVertical = p1[0] == p2[0]
    self.slope = (p2[1] - p1[1])/(p2[0] - p1[0]) if not self.isVertical else None
    self.yInt = p2[1] - self.slope*p2[0] if not self.isVertical else None
    self.p1 = p1
    self.p2 = p2
  
  def f(self, x):
    if self.isVertical:
      return None
    return x * self.slope + self.yInt
  
  def intersect(self, L):
    if self.isVertical:
      #if the other line is not vertical then the intersection
      #is the point on L where x equals the x value of the current line
      return None if L.isVertical else [self.p1[0], L.f(self.p1[0])]
    #if L is vertical but this line is not
    elif L.isVertical:
      return (L.p1[0], self.f(L.p1[0]))
    x = (L.yInt - self.yInt)/ (self.slope - L.slope)
    return [x, self.f(x)]
  
  def offset(self, dist):
    if self.isVertical:
      #it is a vertical line so just offset the x value 
      return Line([self.p1[0] + dist, self.p1[1]],[self.p2[0] + dist, self.p2[1]]) 
    u = self.perpendicularVector(dist)
    # print(u)
    p1 = (0, self.yInt)
    p2 = (1, self.f(1))
    # print([p1[0] + u[0], p1[1] + u[1]], [p2[0] + u[0], p2[1] + u[1]])
    #create a line with the two points from this line that have been shifted by u
    return Line([p1[0] + u[0], p1[1] + u[1]], [p2[0] + u[0], p2[1] + u[1]])
  
  def perpendicularVector(self, dist):
    if self.isVertical:
      return [dist, 0]
    if self.slope == 0:
      return [0, dist]
    x = 1
    y = -1/self.slope
    mag = math.sqrt(x**2 + y**2)
    return ((x/mag) * dist, (y/mag) * dist)
  

