const int ENCA = 14;
const int ENCB = 12;
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
  pinMode(15, OUTPUT);
  Serial.begin(115200);
  digitalWrite(15, HIGH);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);
}

void loop(){
  Serial.println(position);
}