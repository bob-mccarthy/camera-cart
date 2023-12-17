#include <esp_now.h>
#include <WiFi.h>
#include <AccelStepper.h>
#include "StepperCart.h"
#include <HardwareSerial.h>

//mode tells what kind of movement we are doing
//  0: straight line, 1: turn clockwise ( + is CW, - is CCW) 2: stop 
//  3: arc speed (if turning right), 4: arc speed (if turning left), 5: arc accel time,    
//  6: arc steps for the slower motor, 7: arc steps for faster motor 
typedef struct instruction{
  long movement;
  int16_t time;
  int8_t mode;
} instruction;


//the message to be sent to the cart
//currInstruction: a move that the cart needs to make in a certain amount of time 
//numInstructions: total number of instructions coming in 
//mode is what action we are performing:
//  0: play, 1: reset, 2: initialize instruction array, 3: append instruction, 4: clear instrutions, 
//  5: move, 6:turn
typedef struct cart_message{
  instruction currInstruction; 
  uint8_t mode;
} cart_message;

cart_message myData;
uint8_t numInstructions = 0;
const int MAX_LENGTH = 200;
instruction instructions[MAX_LENGTH];

const int BASE_ACCEL = 2000;

const int BASE_SPEED = 2000;

//length from the middle of each wheel
const int AXLE_LENGTH = 295;

const uint16_t MICROSTEPS = 16;

const int WHEEL_RADIUS = 40;

HardwareSerial SerialPort(1);
// StepperCart cart(19, 18, 12, 14, 2000, 2000, 290, 40);
StepperCart cart(14, 15,27, 16, 17, Serial2, 19, 18, 5, 4, 2, SerialPort, BASE_ACCEL, BASE_SPEED, AXLE_LENGTH, WHEEL_RADIUS, MICROSTEPS);

//used to check if we are playing preset instructions and which one we are on
bool playing = false;
int currInstruction = -1;

//if robot is turning while in driving mode
bool isTurning = false;
//true is robot is driving, false if it is reading prerecorded instructions
bool drivingMode = false;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  // Serial.println(myData.mode);
  switch(myData.mode){
    //playing the instruction
    case(0):
      playing = true;
      break;
    
    //stop
    case(1):
      break;

    //creating first instruction
    case(2):
      playing = false;
      cart.setStop(false);
      cart.reset();
      currInstruction = -1;
      instructions[0] = myData.currInstruction;
      numInstructions = 1;
      drivingMode = false;
      break;

    //appending instruction  
    case(3):
      if (numInstructions == MAX_LENGTH ){
        break;
      }
      instructions[numInstructions] = myData.currInstruction;
      numInstructions += 1;
      break;

    //reset instructions
    case(4):
      currInstruction = -1;
      playing = false;
      numInstructions = 0;
      break;

    //sending drive command
    case(5):
      // if (isTurning){
      //   cart.setBlocking(true);
      // }
      cart.setStop(true);
      cart.moveTargetPosLinear(abs(myData.currInstruction.movement), myData.currInstruction.movement > 0 ? 1: -1);
      isTurning = false;
      drivingMode = true;
      playing = false;
      break;

    //sending turn command
    case(6):
      // if (!isTurning){
      //   cart.setBlocking(true);
      // }
      cart.setStop(true);
      cart.moveTargetPosTurn(abs(myData.currInstruction.movement), myData.currInstruction.movement > 0 ? 1: -1);
      isTurning = true;
      drivingMode = true;
      playing = false;
      break;
  }
}

void setup(){
  // delay(4000);
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  cart.setStop(true);
  // cart.moveTargetPosLinear(500, -1);
}

void loop(){
  if (playing && cart.done() && currInstruction < numInstructions - 1){
    currInstruction++;
    if (currInstruction == numInstructions - 1){
      cart.setStop(true);
    }
    // Serial.println("instruction info");
    // Serial.println(instructions[currInstruction].mode);
    // Serial.println(instructions[currInstruction].movement);
    switch (instructions[currInstruction].mode){
      case(0):
        cart.moveLinear(abs(instructions[currInstruction].movement), instructions[currInstruction].movement > 0 ? 1: -1);
        break;
      case(1):
        cart.turn(abs(instructions[currInstruction].movement), instructions[currInstruction].movement > 0 ? 1: -1);
        break;
      case(2):
        break;
      case(3):
        cart.turnArc(
          abs(instructions[currInstruction].movement),
          instructions[currInstruction + 1].movement, 
          instructions[currInstruction + 2].movement, 
          instructions[currInstruction + 3].movement ,
          1
        );
        currInstruction += 3;
        break;
      case(4):
        cart.turnArc(
          abs(instructions[currInstruction].movement),
          instructions[currInstruction + 1].movement, 
          instructions[currInstruction + 2].movement, 
          instructions[currInstruction + 3].movement,
          -1
        );
        currInstruction += 3;
        break;
    }
  }
  if((playing && currInstruction != -1) || drivingMode){
    cart.run();
  }
  
}