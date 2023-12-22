// #include "../camera-cart-esp32/FastDecelStepper.h"
#include "FastDecelStepper.h"
// #include "FastAccelStepper.h"

FastAccelStepperEngine engine = FastAccelStepperEngine();

const int ENCA = 14;
const int ENCB = 12;
// FastDecelStepper* stepper = null;
FastDecelStepper* stepper;

int32_t encoderClicks = 0;
void IRAM_ATTR readEncoder(){
  int b = digitalRead(ENCB);
  if (b ==HIGH){
    encoderClicks++;
  }
  else{
    encoderClicks--;
  }
  // stepper->readEncoder();
}

void setup(){
  Serial.begin(115200);
  pinMode(15,OUTPUT);
  digitalWrite(15,LOW);
  engine.init();
  stepper = new FastDecelStepper(engine, 18, 19, 27, 16);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);
  if(stepper){
    stepper->setAcceleration(20000);
    stepper->setMaxSpeed(20000);
    stepper->moveTo(50000);
  }
  
  digitalWrite(15,HIGH);
}

void loop(){
  // Serial.println(encoderClicks);
  stepper->runEncoder(encoderClicks);
}
