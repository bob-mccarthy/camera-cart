#include "StepperCart.h"

StepperCart::StepperCart(int dirPin1, int stepPin1, int enPin1, int dirPin2, int stepPin2,int enPin2, unsigned long _accel, 
                         unsigned long _speed, unsigned int _axleLength, unsigned int _wheelRadius, uint16_t microsteps){
  accel = _accel;
  speed = _speed;
  this->engine.init();
  left = new FastDecelStepper(engine, dirPin1, stepPin1, enPin1, microsteps);
  right = new FastDecelStepper(engine, dirPin2, stepPin2, enPin2, microsteps);
  left->setAcceleration(accel);
  right->setAcceleration(accel);
  left->setMaxSpeed(speed);
  right->setMaxSpeed(speed);
  axleLength = _axleLength;
  paddingDistance = 0;
  // paddingDistance = ((speed * speed) / (2 *(double)accel)) + 5000; 
  left->addPadding(paddingDistance);
  right->addPadding(paddingDistance);
  left->setCurrentPosition(0);
  right->setCurrentPosition(0);
  
  wheelRadius = _wheelRadius;
  //number of steps per revolution divided by the circumference of the wheel
  //  a4988 need to be in 16 microstep mode
  mmToSteps = 3200/(2*PI*wheelRadius);
}


StepperCart::StepperCart(int dirPin1, int stepPin1, int enablePin1, int rxPin1, int txPin1, HardwareSerial& mySerial1, 
                         int dirPin2, int stepPin2, int enablePin2, int rxPin2, int txPin2, HardwareSerial& mySerial2, 
                         unsigned long _accel, unsigned long _speed, unsigned int _axleLength, unsigned int _wheelRadius,
                         uint16_t microsteps){
  accel = _accel;
  speed = _speed;
  this->engine.init();
  left = new FastDecelStepper(engine, dirPin1, stepPin1, enablePin1, rxPin1, txPin1, mySerial1, microsteps);
  right = new FastDecelStepper(engine, dirPin2, stepPin2, enablePin2, rxPin2, txPin2, mySerial2, microsteps);
  left->setAcceleration(accel);
  right->setAcceleration(accel);
  left->setMaxSpeed(speed);
  right->setMaxSpeed(speed);
  axleLength = _axleLength;
  paddingDistance = 0;
  // paddingDistance = ((speed * speed) / (2 *(double)accel)) + 5000; 
  left->addPadding(paddingDistance);
  right->addPadding(paddingDistance);
  left->setCurrentPosition(0);
  right->setCurrentPosition(0);
  wheelRadius = _wheelRadius;
  mmToSteps = (200*microsteps)/(2*PI*wheelRadius);
}

void StepperCart::reset(){
  left->resetPos();
  right->resetPos();
}

void StepperCart::moveLinear(unsigned long distance, int direction){
  left->move(distance*direction);
  right->move(distance*direction);
}

void StepperCart::moveTargetPosLinear(unsigned long distance, int direction){
  left->moveTargetPos(distance*direction);
  right->moveTargetPos(distance*direction);
}

bool StepperCart::done(){
  return left->done() && right->done();
}

//Turns arc given the speed of the slower Motor, the time that it should accelerate (in microseconds), the amount of steps the slower and faster motors need to move, and the direction you want to turn
void StepperCart::turnArc(unsigned int slowerSpeed, unsigned long accelTime,  long slowerDist, long fasterDist, int direction){
  //if we are going left the slower more will be the left, and similarly with the right
  slower = direction == -1? left : right;
  faster = direction == -1? right : left;

  slower->move(slowerDist);
  faster->move(fasterDist);
  // faster->move(slowerDist);

  slower->goToSpeed(slowerSpeed);
  slower->goToSpeedAfterTime(speed, accelTime);

  
}

void StepperCart::setStop(bool _stop){
  paddingDistance = _stop ? 0 : ((speed * speed) / (2 *(double)accel)) + 5000;
  left->addPadding(paddingDistance);
  right->addPadding(paddingDistance);
}

void StepperCart::turn(unsigned int degrees, int direction){
  //amount of millimeters on the circumference needs to turn degrees amount of degrees
  double totalMM = 2*PI*axleLength*(degrees/360.0);
  //circumenfernce of wheel/ steps per rotation * totalMMs gives total number of steps
  long totalSteps = mmToSteps*totalMM;
  left->move(totalSteps/2.0*direction);
  right->move(-1*totalSteps/2.0*direction);
}


void StepperCart::moveTargetPosTurn(unsigned int degrees, int direction){
  //amount of millimeters on the circumference needs to turn degrees amount of degrees
  double totalMM = 2*PI*axleLength*(degrees/360.0);
  //circumfernce of wheel/ steps per rotation * totalMMs gives total number of steps
  long totalSteps = mmToSteps*totalMM;
  left->moveTargetPos(totalSteps/2.0*direction);
  right->moveTargetPos(-1*totalSteps/2.0*direction);
}

void StepperCart::setBlocking(bool isBlocking){
  left->setBlocking(isBlocking);
  right->setBlocking(isBlocking);
}

void StepperCart::runEncoder(int32_t leftEncoderClicks, int32_t rightEncoderClicks){
  left->runEncoder(leftEncoderClicks);
  right->runEncoder(rightEncoderClicks);
}

void StepperCart::run(){
  left->run();
  right->run();
}