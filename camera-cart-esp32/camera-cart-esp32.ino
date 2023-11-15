#include "StepperCart.h"
StepperCart cart(19, 18, 12, 14, 2000, 2000, 290, 40);

const int BASE_ACCEL = 2000;

const int BASE_SPEED = 2000;

//num steps in rotation/(circumference of wheel)
const double mmToSteps = 3200/(80*PI);

//units millimeters
const int AXLE_LENGTH = 290;

//units millimeters
const int TURN_RADIUS = 500; 

int v1;
int v2;
int a2;
int a1;
int instruction = 0;
unsigned long fasterSteps;
unsigned long slowerSteps;
unsigned int slowerSpeed;
double totalTime;
double accelTime;
const int extraDist = 10000;

void setup(){
  Serial.begin(115200);
}

void loop(){
  if (instruction == 0){
    cart.moveLinear(extraDist, 1);
    instruction += 1;
  }
  if (cart.done() && instruction == 1){
    cart.turnArc(1027, 5963726, 7100, 12900, -1);
    instruction+= 1;
  } 
  if (cart.done() && instruction == 2){

    cart.setStop(true);
    cart.moveLinear(extraDist, 1);
    instruction+= 1;
  } 
  if (cart.done() && instruction == 3){
    cart.moveLinear(extraDist + (500 * mmToSteps), -1);
    instruction+= 1;
  } 
  if (cart.done() && instruction == 4){
    cart.turn(90, 1);
    instruction+= 1;
  } 
  if (cart.done() && instruction == 5){
 
    cart.moveLinear(extraDist + (500 * mmToSteps), -1);
    instruction+= 1;
  } 
  cart.run();
}