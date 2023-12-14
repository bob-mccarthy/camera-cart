#include "DecelStepper.h"

DecelStepper::DecelStepper(uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t rxPin, uint8_t txPin, HardwareSerial& mySerial, uint16_t microsteps): AccelStepper(AccelStepper::DRIVER, stepPin, dirPin){
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);

  TMC2209Stepper driver(&mySerial, R_SENSE, DRIVER_ADDRESS);
  mySerial.begin(115200, SERIAL_8N1, rxPin, txPin);
  driver.begin(); // Start all the UART communications functions behind the scenes
  driver.toff(4); //For operation with StealthChop, this parameter is not used, but it is required to enable the motor. In case of operation with StealthChop only, any setting is OK
  driver.blank_time(24); //Recommended blank time select value
  driver.I_scale_analog(false); // Disbaled to use the extrenal current sense resistors
  driver.internal_Rsense(false); // Use the external Current Sense Resistors. Do not use the internal resistor as it can't handle high current.
  driver.mstep_reg_select(true); //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
  driver.microsteps(microsteps); //Set the number of microsteps. Due to the "MicroPlyer" feature, all steps get converterd to 256 microsteps automatically. However, setting a higher step count allows you to more accurately more the motor exactly where you want.
  driver.TPWMTHRS(0); //DisableStealthChop PWM mode/ Page 25 of datasheet
  driver.semin(0); // Turn off smart current control, known as CoolStep. It's a neat feature but is more complex and messes with StallGuard.
  driver.shaft(true); // Set the shaft direction.
  driver.en_spreadCycle(false); // disable SpreadCycle.We want stealthchop becuase it is quieter. 
  driver.pdn_disable(true); // Enable UART control
  driver.VACTUAL(0); // Enable UART control
  driver.rms_current(SET_CURRENT);
  // mySerial.end();

  digitalWrite(enablePin, LOW);
}

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
  if (!needsFinish){
    this->AccelStepper::moveTo(targetPos + padding);
  }
  
}

void DecelStepper::move(long position){
  targetPos = this->AccelStepper::currentPosition() + position;
  if(!needsFinish){
    this->AccelStepper::moveTo(targetPos + padding);
  }
  
}

void DecelStepper::moveTargetPos(long position){
  targetPos += position;
  if (!needsFinish){
    this->AccelStepper::moveTo(targetPos + padding);
  }
  
}

void DecelStepper::finishExec(){
  needsFinish = true;
}
// void DecelStepper::moveTargetPosAfterRun(long position){
//   targetPos += position;
//   needsFinish = true;
//   // this->AccelStepper::moveTo(targetPos + padding);
// }

void DecelStepper::resetPos(){
  targetPos = 0;
  this->AccelStepper::setCurrentPosition(0);
}

void DecelStepper::addPadding(long _padding){
  padding = _padding;
}

void DecelStepper::run(){
  //checking if we are currently decelerating we have reached our target speed
  if (decel && this->speed() <= targetSpeed){    
    this->setMaxSpeed(targetSpeed);
    this->moveTo(targetPos);
    decel = false;
  }
  //checking if the proper amount of time had elapsed since goToSpeedAfterTime was called
  if (speedToBeSet && micros() >= timeUntilSpeed){
    this->goToSpeed(eventualTargetSpeed);
    speedToBeSet = false;

  }
  if (needsFinish && this->distanceToGo() == 0){
    needsFinish = false;
    this->moveTo(targetPos);
  }

  this->AccelStepper::run();
}