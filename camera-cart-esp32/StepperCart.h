#ifndef STEPPERCART_H
#define STEPPERCART_H

#include "DecelStepper.h"

class StepperCart
{
  private:

    DecelStepper *left;
    DecelStepper *right;
    DecelStepper *slower; //slower motor when turning on an arc
    DecelStepper *faster; //faster motor when turning on an arc
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

    
  public:
    StepperCart(int dirPin1, int stepPin1, int dirPin2, int stepPin2, unsigned long _accel, unsigned long _speed, unsigned int _axleLength, unsigned int _wheelRadius);
    void moveLinear(unsigned long distance, int direction); //straight line movement of distance (Motor Stepper Steps), direction (backward: -1, forward: 1)
    void turnArc(unsigned int slowerSpeed, double accelTime, long slowerDist, long fasterDist, int direction); // faster implementation of turn arc
    void turnArc(unsigned long radius, unsigned int degrees, int direction); // turn arc of radius (millimeters), degrees (degrees), direction (left: -1, right: 1)
    void turn(unsigned int degrees, int direction); //turn in place defined by degrees in direction (-1:CCW, 1: CW)
    void run(); //steps the step motors if a step is due
    void setStop(bool _stop);//set if the motors will stop after this last instruction
    bool done();//returns if the motors have finsihed their previous instruction


};
#endif