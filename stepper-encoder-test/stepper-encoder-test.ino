const int ENCA = 7;
const int ENCB = 5;
long position = 0;
void IRAM_ATTR readEncoder(){
  int b = digitalRead(ENCB);
  if (b ==HIGH){
    position++;
  }
  else{
    position--;
  }
}

void setup(){
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);
}

void loop(){
  
}
