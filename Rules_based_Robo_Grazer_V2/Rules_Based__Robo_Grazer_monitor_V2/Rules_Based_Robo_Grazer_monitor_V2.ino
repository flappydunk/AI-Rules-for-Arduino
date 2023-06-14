


//Rules Cut down version for Nano
//experimental pond controller. Full functioning with facilty to be controlled by a ESP via I2C
//runtime version. No display options to reduce code footprint
//static arrays loaded into program memory to save space


#include <Wire.h>
#include <Servo.h>
#include <dht.h>
#include "config.h"
#include "Rules.h"
#include <avr/wdt.h>
dht DHT;

void setup(){
  //Start Serial
  Serial.begin(CONSOLE_BAUDRATE);
  while (!Serial);
  Serial.flush();

  Serial.println();
  Serial.println(F("Rules_Based_collector_monitor_V2"));
  //eeprom_init(); //
  //retrieve settings
  Sensors_setup();
  pinMode(LED_BUILTIN, OUTPUT);
  I2C_setup();
  Bat_setup();
  wdt_enable (WDTO_8S);
  rules_setup();
  display_current();
  New_Power_Servo_State = 1;
}

void loop() {
  readSerial();
  
  if (run_rules) {
    Read_Sensors();
    Bumper_Sense();
    Docked();
    Servo_loop();  
  }
  rules_refresh();
  wdt_reset();
  command_execute();
  
}


void command_execute(){
 uint32_t mycommand = rules_pop();
 int proc;
 //monitor commands
 if (mycommand != -1){ 
  //0x0 Not used
 // if (mycommand == 0x01) {Serial.println(F("Pump OFF")); action_servo(0,sServo_OFF);}
 }

  //Monitor flags 
  if ((bitRead(system_current,1) == 1)){digitalWrite(STOP,LOW);} else {digitalWrite(STOP,HIGH);}
  if ((bitRead(system_current,6) == 1)){
    //charge ON
    digitalWrite(pinChargeRelay, HIGH); 
  } 
  else {
    //Charge OFF
    digitalWrite(pinChargeRelay, LOW); 
  }
  //Fan
  if ((bitRead(system_current,8) == 1)){
    //Fan ON
    digitalWrite(IRF520, HIGH); 
  } 
  else {
    //Fan OFF
    digitalWrite(IRF520, LOW); 
  }

}





























//
