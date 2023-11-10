#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>



//mode tells what kind of movement we are doing
//  0: straight line, 1: turn clockwise ( + is clockwise), 2: stop
typedef struct instruction{
  int movement;
  int16_t time;
  int8_t mode;
} instruction;

const int buttonPin = 14;

int lastPressed = 0;

//the message to be sent to the cart
//currInstruction: a move that the cart needs to make in a certain amount of time 
//mode is what action we are performing:
//  0: play, 1: reset, 2: initialize instruction array, 3: append instruction, 4: clear instrutions, 
//  5: move forward, 6:move backward, 7: turn left, 8: turn right
typedef struct cart_message{
  instruction currInstruction;
  uint8_t mode;
} cart_message;

bool notSent = true;
instruction instructions[12] = {{500, 0, 0}, {90,0,1}, {-90,0,1}, {-500, 0, 0}};
instruction currInstruction;

// Create a struct_message called myData
cart_message myData;

enum { Idle, Receiving } state = Idle;
String incomingData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// address of camera cart
uint8_t cartAddress[] = {0xA0, 0xB7, 0x65, 0x61, 0xEF, 0x10};
StaticJsonDocument<10000> doc;
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, cartAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  while(Serial.available()){
    char c = Serial.read();
    if (state == Idle) {
      if (c == '{') {
        incomingData = c;
        state = Receiving;
      }
    } else if (state == Receiving) {
      incomingData += c;
      if (c == '}') {
        state = Idle;
        // Now, you have a complete JSON string in incomingData
        DeserializationError err = deserializeJson(doc, incomingData);
        if(err){
          Serial.println("ERROR: ");
          Serial.println(err.c_str());
          return;
        }
        if (!doc.isNull()) {
          //tells whether we are sending instructions, clearing instructions or driving
          int mode = doc["mode"];
          //sending over fully instructions
          if(mode == 0){
            int numInstructions = doc["numInstructions"];
            Serial.println(numInstructions);
            for(int i = 0; i < numInstructions; i++){
              Serial.println((int) doc["moveInstructions"][i]);
              myData.currInstruction.movement = doc["moveInstructions"][i];
              myData.currInstruction.time = doc["timeInstructions"][i];
              myData.currInstruction.mode = doc["modeInstructions"][i];
              if (doc["modeInstructions"][i] == 5 || doc["modeInstructions"][i] == 6){
                myData.mode = doc["modeInstructions"][i];
              }
              else{
                myData.mode = i == 0 ? 2 : 3;
              }
              
              esp_err_t result = esp_now_send(cartAddress, (uint8_t *) &myData, sizeof(myData));
            }  
          }
          else if (mode == 1){
            myData.mode = 4;
            esp_err_t result = esp_now_send(cartAddress, (uint8_t *) &myData, sizeof(myData));
          }
        }
        incomingData = ""; // Clear the buffer
      }
    }
  }
  
  //you can only press the button every second, as well as tells the cart to play instructions if button is pressed
  if (millis() - lastPressed > 1000 && digitalRead(buttonPin) == LOW){
    lastPressed = millis();
    myData.mode = 0;
    esp_err_t result = esp_now_send(cartAddress, (uint8_t *) &myData, sizeof(myData));
    
  }

}
