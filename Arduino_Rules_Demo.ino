//This is the V2 Library and Arduino Demo version of the Rules
//Demo V2 Developed by Karl Humphrey 03/06/2023

#include "Rules.h"

void setup() {
  //Start Serial
  Serial.begin(115200);
  while (!Serial);
  Serial.flush();
  rules_setup(); 
  Serial.println();
  Serial.println(F("Rules_Based_Demo"));

}

void loop() {
  readSerial();
  rules_refresh();
  command_execute();    
}


void command_execute(){
 uint32_t mycommand = rules_pop(); //get the next command from the stack
 
 //monitor commands and execute
 //modify this to interface to your peripherals, servos etc
 
 if (mycommand != -1){ 
  //0x0 Not used
  if (mycommand == 0x10) {Serial.println(F("CMD: LED OFF"));}
  if (mycommand == 0x11) {Serial.println(F("CMD: LED ON"));}
 }

  //Monitor flags and action
  if (system_current == Last_system_current){return;} //Only when a change occurs 
  
  //If you want to monitor Flags only you can do it here
  //Fan interface
  if ((bitRead(system_current,4) == 1) && (bitRead(Last_system_current,4) == 0) ){
    //Set Pin HIGH
    Serial.println(F("FAN ON"));
  } 
  if ((bitRead(system_current,4) == 0) && (bitRead(Last_system_current,4) == 1) ){
    //Set Pin LOW
    Serial.println(F("FAN OFF"));
  }
  
  Last_system_current = system_current;

}





















//
