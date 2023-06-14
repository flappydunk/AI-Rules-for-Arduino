//#include <SPI.h>
#include <Wire.h>
#include "config.h"

void setup(){
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Programme : ESP8266_Sender_Interface_V2.2");//This programme name
  pinMode(LED_BUILTIN, OUTPUT);
  I2C_setup();
  UDP_setup();
}



void loop(){

   //check console input
  if (Serial.available() > 0){
    char ch = (char)Serial.read();      
    if (ch == 'i') {
        SerialData_A = !SerialData_A; //toggle console messages
    }

  }   

  if (Polling_Next < millis()){
    //I2CScanner();
    Polling_Next = Polling_Duration + millis();
  }
  UDP_Rec_loop();
  
    
}
