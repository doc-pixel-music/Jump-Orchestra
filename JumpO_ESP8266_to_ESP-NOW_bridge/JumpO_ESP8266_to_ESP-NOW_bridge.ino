/*
  JUMP ORCHESTRA using ESP-NOW transport for MIDI, you must need the ESP-NOW to USB-MIDI bridge 
  You'll find it on my Github page (https://github.com/doc-pixel-music/ESP-NOW-to-MIDI-bridge)
  Many thanks to Rui Santos (https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/)
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  DOC PIXEL Corp. (https://doc-pixel.com/)
*/

#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
// ESP32 S3 : 44:b1:76:cd:61:a0
uint8_t broadcastAddress[] = {0x44, 0xB1, 0x76, 0xCD, 0x61, 0xA0};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  byte channel; //midi channel to use
  byte type;  // 1 : Note ; 2 : Controle Change; 3 : Program
  byte address; // Address of note, cc or program change (0-127)
  byte value; // Velocity on type note or Controle Change value
  bool keypress; // Note ON or OFF (only for Type 1)
} struct_message;

// Create a struct_message called myData
struct_message myData;

const int buttonPin = 16;  // the number of the pushbutton pin
const int ledPin = 5;    // the number of the LED pin
int buttonState = 0;  // variable for reading the pushbutton status
int previousButtonState = 0;  // variable for storing the pushbutton status

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {

  // Init Serial Monitor
  Serial.begin(115200);

  // INIT PIN
  pinMode(ledPin, OUTPUT); // initialize the LED pin as an output:
  pinMode(buttonPin, INPUT); // initialize the pushbutton pin as an input:

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {

  myData.channel = 16; 
  myData.type = 1; //Send note
  myData.address = 59; //B3
  myData.value = 127; //max velocity

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState != previousButtonState) {
    previousButtonState = buttonState;

    if (buttonState == HIGH){
      digitalWrite(ledPin, HIGH); // turn LED on:
      myData.keypress = 1;
      // Send message via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      //Serial.println("Tile is ON");
    } else {
      // turn LED off:
      digitalWrite(ledPin, LOW);
      myData.keypress = 0;
      // Send message via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      //Serial.println("nobody on the tile");
    }
  }

  delay(20); //litle debouncing

}