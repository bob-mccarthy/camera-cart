#include <esp_now.h>
#include <WiFi.h>
#include <AccelStepper.h>

//mode tells what kind of movement we are doing
//  0: straight line, 1: turn clockwise ( + is clockwise) 2: stop
typedef struct instruction{
  int movement;
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
// bool instructionsSet = false;
// uint8_t numInstructions = 4;
// instruction instructions[12] = {{500, 0, 0}, {90,0,1}, {-90,0,1}, {-500, 0, 0}};
 
// Define some steppers and the pins the will use
//step pin, direction pin
AccelStepper stepper1(AccelStepper::DRIVER, 14, 12);// Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper2(AccelStepper::DRIVER, 18, 19);// Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5



bool playing = false;

//num steps in one cart revolution 
const int cartRev = 11600;

//keeps track of the first instruction
int currInstruction = -1;

long stepper1TargetPos = 0;
long stepper2TargetPos = 0;

bool isTurning = false;
bool drivingMode = false;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  switch(myData.mode){
    case(0):
      playing = true;
      break;
    case(1):
      break;
    case(2):
      playing = false;
      currInstruction = -1;
      instructions[0] = myData.currInstruction;
      numInstructions = 1;
      drivingMode = false;
      stepper1TargetPos = 0;
      stepper2TargetPos = 0;
      break;
    case(3):
      if (numInstructions == MAX_LENGTH ){
        break;
      }
      // Serial.println(myData.currInstruction.movement);
      instructions[numInstructions] = myData.currInstruction;
      numInstructions += 1;
      break;
    case(4):
      currInstruction = -1;
      playing = false;
      numInstructions = 0;
    case(5):
      
      //if the cart was just turning let it finish that motion before moving linearly it 
      while (isTurning && stepper1.distanceToGo() != 0 && stepper2.distanceToGo() != 0){
        stepper1.run();
        stepper2.run();
      }
      isTurning = false;
      stepper1.moveTo(stepper1TargetPos + myData.currInstruction.movement);
      stepper2.moveTo(stepper2TargetPos + myData.currInstruction.movement);
      Serial.println("move linear");
      Serial.println(stepper1TargetPos);
      Serial.println(stepper2TargetPos);
      stepper1TargetPos += myData.currInstruction.movement;
      stepper2TargetPos += myData.currInstruction.movement;
      drivingMode = true;
      playing = true;
      break;

    case(6):
      //if the cart was just moving linearly let it finish that motion before turning it 
      while (!isTurning && stepper1.distanceToGo() != 0 && stepper2.distanceToGo() != 0){
        stepper1.run();
        stepper2.run();
      }
      
      isTurning = true;
      stepper1.moveTo(stepper1TargetPos - degreesToSteps(myData.currInstruction.movement));
      stepper2.moveTo(stepper2TargetPos + degreesToSteps(myData.currInstruction.movement));
      Serial.println("turning");
      Serial.println(stepper1TargetPos);
      Serial.println(stepper2TargetPos);
      stepper1TargetPos += - degreesToSteps(myData.currInstruction.movement);
      stepper2TargetPos += degreesToSteps(myData.currInstruction.movement);
      drivingMode = true;
      playing = true;
      break;
  }
  

}

 
void setup()
{  
    Serial.begin(115200);
    
    stepper1.setMaxSpeed(1500.0);
    stepper1.setAcceleration(2000.0);

    stepper2.setMaxSpeed(1500.0);
    stepper2.setAcceleration(2000.0);

  
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
    
}
 
void loop()
{
    
    //check that user is playing the instructions, instructions have been set, the motors have reached their final position, and that this is not the last instruction
    if (playing && numInstructions > 0 && stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0 && currInstruction < numInstructions - 1){   
      
      currInstruction += 1;
      Serial.println(currInstruction);
      stepper1.setCurrentPosition(0);
      stepper2.setCurrentPosition(0);
      Serial.println(instructions[currInstruction].movement);
      Serial.println(instructions[currInstruction].mode);
      if (instructions[currInstruction].mode == 1){
        stepper1.moveTo(-degreesToSteps(instructions[currInstruction].movement));
        stepper2.moveTo(degreesToSteps(instructions[currInstruction].movement));
      }
      else{
        stepper1.moveTo(instructions[currInstruction].movement);
        stepper2.moveTo(instructions[currInstruction].movement);
      }
    }

    //check that we are either on an instruction, or we are in free drive mode, and that we are in playing mode
    if ((currInstruction != -1 || drivingMode) && playing){
      stepper1.run();
      stepper2.run();
    }
    

}

int degreesToSteps (int degrees){
  return (degrees / 360.0) * cartRev;
}