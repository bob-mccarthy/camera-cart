#ifndef DECELSTEPPER_H
#define DECELSTEPPER_H
#include <AccelStepper.h>
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>

#define R_SENSE          0.10f      // R_SENSE for current calc.
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2
const int  SET_CURRENT = 600;


class DecelStepper: public AccelStepper{
  public:
    DecelStepper(uint8_t dirPin, uint8_t stepPin) : AccelStepper(AccelStepper::DRIVER, stepPin, dirPin){}
    DecelStepper(uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t rxPin, uint8_t txPin, HardwareSerial& mySerial, uint16_t microsteps); //initialize DecelStepper with TMC
    void goToSpeed(unsigned int speed); //will accelerate or decelerate to given speed from whatever its current speed is
    void goToSpeedAfterTime(unsigned int speed, unsigned long time); //goes to certain speed after t microseconds
    void run();
    void moveTo(long position);
    void move(long position);
    //set flag (needsFinish) which makes the stepper motor finsih it's execution before do another instruction 
    //(if either move command is called the changes to targetpos will take effect after the stepper motor has stopped)
    void finishExec();
    void moveTargetPos(long position); //moves target position by position steps
    void resetPos(); //set current and target position back to 0
    void addPadding(long _padding); // adds additional steps to all of your move instructions that is not reflected in the targetPos;
    bool done();
    
  private:
    unsigned int eventualTargetSpeed; // the speed will change to after 
    unsigned long timeUntilSpeed; //the time we are supposed to change our speed in microseconds
    bool speedToBeSet; // true if someone called goToSpeedAfterTime
    unsigned int targetSpeed = 0; //save speed we want to decel to
    long targetPos = 0; //keeps track of the targetPos disregarding padding
    long padding = 0;
    bool decel = false;
    bool needsFinish = false; // set if the current instructions need to finish before new target pos can be set
    


};

#endif