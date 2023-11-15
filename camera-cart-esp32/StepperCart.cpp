#include "StepperCart.h"

StepperCart::StepperCart(int dirPin1, int stepPin1, int dirPin2, int stepPin2, unsigned long _accel, unsigned long _speed, unsigned int _axleLength, unsigned int _wheelRadius){
  // DecelStepper _left(dirPin1, stepPin1);
  // DecelStepper _right(dirPin2, stepPin2);
  accel = _accel;
  speed = _speed;
  left = new DecelStepper(dirPin1, stepPin1);
  right = new DecelStepper(dirPin2, stepPin2);
  left->setAcceleration(accel);
  right->setAcceleration(accel);
  left->setMaxSpeed(speed);
  right->setMaxSpeed(speed);
  axleLength = _axleLength;
  paddingDistance = ((speed * speed) / (2 *(double)accel)) + 5000; 
  left->addPadding(paddingDistance);
  right->addPadding(paddingDistance);
  left->setCurrentPosition(0);
  right->setCurrentPosition(0);
  wheelRadius = _wheelRadius;
}

void StepperCart::moveLinear(unsigned long distance, int direction){
  // Serial.println("in moveLinear");
  left->move(distance*direction);
  right->move(distance*direction);
}

// void StepperCart::moveLinear(unsigned long distance, int direction){
//   left->move(distance*direction);
//   right->move(distance*direction);
// }

bool StepperCart::done(){
  return left->done() && right->done();
}

//Turns arc given the speed of the slower Motor, the time that it should accelerate, the amount of steps the slower and faster motors need to move, and the direction you want to turn
void StepperCart::turnArc(unsigned int slowerSpeed, double accelTime,  long slowerDist, long fasterDist, int direction){
  //if we are going left the slower more will be the left, and similarly with the right
  slower = direction == -1? left : right;
  faster = direction == -1? right : left;

  slower->move(slowerDist);
  faster->move(fasterDist);
  // faster->move(slowerDist);

  slower->goToSpeed(slowerSpeed);
  slower->goToSpeedAfterTime(speed, accelTime);

  // faster->goToSpeed(slowerSpeed);
  // faster->goToSpeedAfterTime(speed, accelTime);
  
}

void StepperCart::setStop(bool _stop){
  paddingDistance = _stop ? 0 : ((speed * speed) / (2 *(double)accel)) + 5000;
  left->addPadding(paddingDistance);
  right->addPadding(paddingDistance);
}


// StepperCart::turnArc(unsigned long radius, unsigned int degrees, int direction){
//   left->setCurrentPosition(0);
//   right->setCurrentPosition(0);

//   //the slower motor is in the direction we want to turn 
//   slower = direction == -1? left : right;
//   faster = direction == -1? right: left

  
//   //number of steps taken by the faster motor
//   int fasterTotalDistance = 0;

//   float C = (2.0*radius)/AXLE_LENGTH;
//   float avgRatio = ((C - 1)/ (C + 1)); //average ratio of the slower motor speed and higher motor speed (1/k) 
//   double T = fasterTotalDistance/speed; // total time of turn in seconds 

 

//   unsigned int quadSqrt = sqrt((accel * accel) + 4*(accel/T)*(speed*(1 - (1/k))))
//   double L = (accel - quadSqrt) / (2*(-accel/(double)T));//the length of acceleration in seconds
  

  
//   //if the robot is not coming to a stop at the end of the turn then add the padding distance;
//   if(!stop){
//     fasterTotalDistance += paddingDistance;
//   }
//   faster.moveTo
// }

void StepperCart::turn(unsigned int degrees, int direction){
  //amount of millimeters on the circumference needs to turn degrees amount of degrees
  long totalMM = 2*PI*axleLength*(degrees/360.0);
  //circumenfernce of wheel/ steps per rotation * totalMMs gives total number of steps
  long totalSteps = 3200/(2*PI*wheelRadius)*totalMM;
  left->move(totalSteps/2.0*direction);
  right->move(-1*totalSteps/2.0*direction);
}

void StepperCart::run(){
  left->run();
  right->run();
}