#include "DecelStepper.h"

void DecelStepper::goToSpeed(unsigned int speed){
  unsigned int currSpeed = this->speed();
  if ( currSpeed > speed){
    targetSpeed = speed;
    this->stop();
    decel = true;
    // Serial.print("startDecel");
    // Serial.println(millis());
  }
  else{
    this->setMaxSpeed(speed);
  }
}

void DecelStepper::goToSpeedAfterTime(unsigned int speed, unsigned long time){
  speedToBeSet = true;
  eventualTargetSpeed = speed;
  timeUntilSpeed = micros() + time;
}

bool DecelStepper::done(){
  return abs(this->distanceToGo()) <= padding;
}

void DecelStepper::moveTo(long position){
  targetPos = position;
  this->AccelStepper::moveTo(targetPos + padding);
}

void DecelStepper::move(long position){
  targetPos = this->AccelStepper::currentPosition() + position;
  // Serial.println(targetPos);
  this->AccelStepper::moveTo(targetPos + padding);
}

void DecelStepper::addPadding(long _padding){
  padding = _padding;
}

void DecelStepper::run(){
  //checking if we are currently decelerating we have reached our target speed
  if (decel && this->speed() <= targetSpeed){
    // endDecel = millis();
    
    this->setMaxSpeed(targetSpeed);
    this->moveTo(targetPos);
    decel = false;
    // Serial.print("endDecel");
    // Serial.println(millis());
  }
  //checking if the proper amount of time had elapsed since goToSpeedAfterTime was called
  if (speedToBeSet && micros() >= timeUntilSpeed){
    this->goToSpeed(eventualTargetSpeed);
    speedToBeSet = false;
    // Serial.print("start Accel");
    // Serial.println(millis());
  }
  this->AccelStepper::run();
}