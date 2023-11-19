#ifndef DECELSTEPPER_H
#define DECELSTEPPER_H
#include <AccelStepper.h>

class DecelStepper: public AccelStepper{
  public:
    DecelStepper(uint8_t dirPin, uint8_t stepPin) : AccelStepper(AccelStepper::DRIVER, stepPin, dirPin){}
    void goToSpeed(unsigned int speed); //will accelerate or decelerate to given speed from whatever its current speed is
    void goToSpeedAfterTime(unsigned int speed, unsigned long time); //goes to certain speed after t microseconds
    void run();
    void moveTo(long position);
    void move(long position);
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
    


};

#endif