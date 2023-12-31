#include "FastDecelStepper.h"

FastDecelStepper::FastDecelStepper(FastAccelStepperEngine& engine, uint8_t  dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t microsteps){
  this->stepper = engine.stepperConnectToPin(stepPin);
  this->stepper->setDirectionPin(dirPin);
  this->stepper->setEnablePin(enablePin);
  this->stepper->setAutoEnable(false);
  clicksToSteps = (microsteps * 200) / 1000.0; // steps per revolution / encoder clicks for revolution
}


FastDecelStepper::FastDecelStepper(FastAccelStepperEngine& engine, uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t rxPin, uint8_t txPin, HardwareSerial& mySerial, uint16_t microsteps){

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

  clicksToSteps = (microsteps * 200) / 1000.0; // steps per revolution / encoder clicks for revolution
  
  this->stepper = engine.stepperConnectToPin(stepPin);
  this->stepper->setDirectionPin(dirPin);
  this->stepper->setEnablePin(enablePin);
  this->stepper->setAutoEnable(false);
}

void FastDecelStepper::setMaxSpeed(long maxSpeed){
  this->stepper->setSpeedInHz(maxSpeed);
}

void FastDecelStepper::setAcceleration(long acceleration){
  this->stepper->setAcceleration(acceleration);
}

void FastDecelStepper::setCurrentPosition(long position){
  // Serial.println("Resetting Position: ");
  // Serial.println(this->stepper->getCurrentPosition());
  this->stepper->setCurrentPosition(position);
  // Serial.println(this->stepper->getCurrentPosition());
}

void FastDecelStepper::goToSpeed(uint32_t speed){
  unsigned int currSpeed = this->stepper->getCurrentSpeedInMilliHz()/1000;
  if ( currSpeed > speed){
    this->targetSpeedMilliHz = speed * 1000;
    delay(5); // you need a slight delay, because if you call stopMove directly after calling one of the moveTo functions it won't stop
    this->stepper->stopMove();
    this->decel = true;
  }
  else{
    this->setMaxSpeed(speed);
    this->stepper->moveTo(this->targetPos + this->padding);
  }
}

void FastDecelStepper::goToSpeedAfterTime(uint32_t speed, unsigned long time){
  this->speedToBeSet = true;
  this->eventualTargetSpeed = speed;
  this->timeUntilSpeed = micros() + time;
}

bool FastDecelStepper::done(){
  return abs((this->targetPos + this->padding) - this->stepper->getCurrentPosition()) <= this->padding;
}

void FastDecelStepper::moveTo(long position){
  this->targetPos = position;
  this->stepper->moveTo(this->targetPos + this->padding, this->isBlocking);
  
}

void FastDecelStepper::move(long position){
  this->targetPos = this->stepper->getCurrentPosition() + position;
  this->stepper->moveTo(this->targetPos + this->padding, this->isBlocking);
  
}

void FastDecelStepper::moveTargetPos(long position){
  this->targetPos += position;
  this->stepper->moveTo(this->targetPos + this->padding, this->isBlocking);
  
}

void FastDecelStepper::setBlocking(bool _isBlocking){
  this->isBlocking = _isBlocking;
}



void FastDecelStepper::resetPos(){
  this->targetPos = 0;
  this->setCurrentPosition(0);
}

void FastDecelStepper::addPadding(long _padding){
  this->padding = _padding;
}

// void FastDecelStepper::attachEncoder(int8_t _pinA, int8_t _pinB){
  
//   attachInterrupt(digitalPinToInterrupt(this->pinA), std::bind(&this->readEncoder, this), RISING);
// }



void FastDecelStepper::run(){
  //checking if we are currently decelerating we have reached our target speed
  if (decel && (this->stepper->getCurrentSpeedInMilliHz()) <= this->targetSpeedMilliHz){  
    Serial.println(this->stepper->getCurrentSpeedInMilliHz()/1000);
    this->setMaxSpeed(this->targetSpeedMilliHz/1000);//convert speed back to steps per second
    this->moveTo(targetPos);
    this->decel = false;
  }
  //checking if the proper amount of time had elapsed since goToSpeedAfterTime was called
  if (speedToBeSet && micros() >= timeUntilSpeed){
    this->goToSpeed(this->eventualTargetSpeed);
    this->speedToBeSet = false;
  }

}

void FastDecelStepper::runEncoder(int32_t encoderClicks){
  //checking if we are currently decelerating we have reached our target speed
  if (decel && (this->stepper->getCurrentSpeedInMilliHz()) <= this->targetSpeedMilliHz){  
    Serial.println(this->stepper->getCurrentSpeedInMilliHz()/1000);
    this->setMaxSpeed(this->targetSpeedMilliHz/1000);//convert speed back to steps per second
    this->moveTo(targetPos);
    this->decel = false;
  }
  //checking if the proper amount of time had elapsed since goToSpeedAfterTime was called
  if (speedToBeSet && micros() >= timeUntilSpeed){
    this->goToSpeed(this->eventualTargetSpeed);
    this->speedToBeSet = false;
  }

  if((abs(this->stepper->getCurrentPosition() - (encoderClicks * this->clicksToSteps + this->accountedSteps)) > clicksToSteps)){
    uint32_t diff = this->stepper->getCurrentPosition() - (encoderClicks * this->clicksToSteps + this->accountedSteps);
    this->accountedSteps += diff;
    this->moveTargetPos(diff);
  }

}