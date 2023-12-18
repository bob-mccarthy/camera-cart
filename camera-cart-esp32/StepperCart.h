#ifndef STEPPERCART_H
#define STEPPERCART_H

// #include "DecelStepper.h"
#include "FastDecelStepper.h"
#include <FastAccelStepper.h>
class StepperCart
{
  private:
    FastAccelStepperEngine engine = FastAccelStepperEngine();
    FastDecelStepper *left;
    FastDecelStepper *right;
    FastDecelStepper *slower; //slower motor when turning on an arc
    FastDecelStepper *faster; //faster motor when turning on an arc
    bool stop = false; //check whether the cart is coming to a stop
    bool startDecel = false;
    bool startAccel = false;
    unsigned long speed; //speed you want to motors to operate at
    unsigned long accel; //acceleration of both motors
    unsigned long topSpeed; //speed to accelerate the slower motor back to at the end of an arc
    unsigned long bottomSpeed; //speed to decelerate to for the slower motor in an arc 
    unsigned long slowerTargetPos; //target position for slower motor when turning in an arc
    unsigned long paddingDistance; //we need to pad all instructions that are not working
    unsigned int axleLength;
    unsigned int wheelRadius;
    double mmToSteps;

    
  public:
    //initialize stepper cart with a4988 motor drivers
    StepperCart(int dirPin1, int stepPin1, int enPin1, int pinA1, int pinB1, int dirPin2, int stepPin2, int enPin2, int pinA2, int pinB2, unsigned long _accel, unsigned long _speed, unsigned int _axleLength, unsigned int _wheelRadius, uint16_t microsteps);
    //initialize stepper cart with tmc motor drivers
    StepperCart(int dirPin1, int stepPin1, int enablePin1, int rxPin1, int txPin1, HardwareSerial& mySerial1, 
                int dirPin2, int stepPin2, int enablePin2, int rxPin2, int txPin2, HardwareSerial& mySerial2, 
                unsigned long _accel, unsigned long _speed, unsigned int _axleLength, unsigned int _wheelRadius,
                uint16_t microsteps);
    void moveLinear(unsigned long distance, int direction); //straight line movement of distance (Motor Stepper Steps), direction (backward: -1, forward: 1)
    void moveTargetPosLinear(unsigned long distance, int direction); //moves target position in a straight line by distance (Motor Stepper Steps), direction (backward: -1, forward: 1)
    void turnArc(unsigned int slowerSpeed, unsigned long accelTime, long slowerDist, long fasterDist, int direction); // faster implementation of turn arc
    void turn(unsigned int degrees, int direction); //turn in place defined by degrees in direction (-1:CCW, 1: CW)
    void moveTargetPosTurn(unsigned int degrees, int direction); // turn in place, but if we are currently turning it will add this turn onto the end of the previous one
    void run(); //steps the step motors if a step is due
    void setStop(bool _stop);//set if the motors will stop after this last instruction
    bool done();//returns if the motors have finsihed their previous instruction
    void reset(); //resets all the positions of the stepper motors 
    void setBlocking(bool isBlocking); // if isBlocking is true then the cart waits til each instruction finishes before executing the next one
    void handleRightMotorInterrupt();
    void handleLeftMotorInterrupt();


};
#endif