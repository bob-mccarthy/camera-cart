#ifndef FASTDECELSTEPPER_H
#define FASTDECELSTEPPER_H
#include <FastAccelStepper.h>
#include <HardwareSerial.h>
#include <TMCStepper.h>
#include <SPI.h>
#include <FunctionalInterrupt.h>

#define R_SENSE          0.10f      // R_SENSE for current calc.
#define DRIVER_ADDRESS   0b00       // TMC2209 Driver address according to MS1 and MS2
const int  SET_CURRENT = 600;




class FastDecelStepper{
  public:
    FastDecelStepper(FastAccelStepperEngine& engine, uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t microsteps);
    FastDecelStepper(FastAccelStepperEngine& engine, uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t microsteps, int8_t _pinA, int8_t _pinB);
    FastDecelStepper(FastAccelStepperEngine& engine, uint8_t dirPin, uint8_t stepPin, uint8_t enablePin, uint8_t rxPin, uint8_t txPin, HardwareSerial& mySerial, uint16_t microsteps); //initialize FastDecelStepper with TMC
    void setMaxSpeed(long maxSpeed);//sets the maxSpeed of the motor in steps per second 
    void setAcceleration(long acceleration); //sets the acceleration of the motor in steps per second per second
    void setCurrentPosition(long position);
    void goToSpeed(uint32_t speed); //will accelerate or decelerate to given speed from whatever its current speed is
    void goToSpeedAfterTime(uint32_t speed, unsigned long time); //goes to certain speed after t microseconds
    void run();
    void moveTo(long position);
    void move(long position);
    void setBlocking(bool _isBlocking);
    //set flag (needsFinish) which makes the stepper motor finsih it's execution before do another instruction 
    //(if either move command is called the changes to targetpos will take effect after the stepper motor has stopped)
    void moveTargetPos(long position); //moves target position by position steps
    void resetPos(); //set current and target position back to 0
    void addPadding(long _padding); // adds additional steps to all of your move instructions that is not reflected in the targetPos;
    bool done();
    // void attachEncoder(int8_t pinA, int8_t pinB);//initializes encoder and uses interrupt 
    void readEncoder();
    
  private:
    FastAccelStepper *stepper = NULL;
    int32_t encoderClicks = 0;
    int32_t clicksToSteps;
    int32_t accountedSteps = 0; //additional steps to get from 
    int8_t pinA, pinB;
    bool encoderAttached = false;
    uint32_t eventualTargetSpeed; // the speed will change to after timeUntilSpeed seconds have elapsed
    unsigned long timeUntilSpeed; //the time we are supposed to change our speed in microseconds
    bool speedToBeSet = false; // true if someone called goToSpeedAfterTime
    uint32_t targetSpeedMilliHz = 0; //save speed we want to decel to in steps/1000s (because the get speed function returns in steps per thousand seconds)
    long targetPos = 0; //keeps track of the targetPos disregarding padding
    long padding = 0;
    bool decel = false;
    bool isBlocking = false; //if blocking is true then positions will only be updated when the motors are done running
};

#endif