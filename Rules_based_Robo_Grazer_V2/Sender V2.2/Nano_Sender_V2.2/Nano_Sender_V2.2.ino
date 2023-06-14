/*

  Ardumower (www.ardumower.de)
  Copyright (c) 2013-2014 by Alexander Grau
  Copyright (c) 2013-2014 by Sven Gennat
  
  Private-use only! (you need to ask for a commercial-use)  
 
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
  Private-use only! (you need to ask for a commercial-use)  
*/



 // code version 
#define VER "Leaf Collector" 

#include "TimerOne.h"
#include "EEPROM.h"
#include "RunningMedian.h"
#include <Wire.h>
#include "config.h"


// ---- choose only one perimeter signal code ----
#define SIGCODE_1  // Ardumower default perimeter signal
//#define SIGCODE_2  // Ardumower alternative perimeter signal
//#define SIGCODE_3  // Ardumower alternative perimeter signal


// --- MC33926 motor driver ---
#define USE_DOUBLE_AMPLTIUDE    1         // 1: use +/- input voltage for amplitude (default), 
                                          // 0: use only +input/GND voltage for amplitude
                                         
#define pinIN1       9  // M1_IN1         (if using old L298N driver, connect this pin to L298N-IN1 P2)
#define pinIN2       2  // M1_IN2         (if using old L298N driver, connect this pin to L298N-IN2 P2)
#define pinIN3       10  // M1_IN1         (if using old L298N driver, connect this pin to L298N-IN3 P1)
#define pinIN4       3  // M1_IN2         (if using old L298N driver, connect this pin to L298N-IN4 P1)
//#define pinPWM       3  // M1_PWM / nD2  was 3 (if using old L298N driver, leave open)
#define pinEnableA    5  // EN     was 5        (connect to motor driver L298N-enableA P2)  
#define pinEnableB    6  // EN     was 5        (connect to motor driver L298N-enableB P1)               

// motor driver fault pin used by perimeter setup
//#define pinFault     4  // M1_nSF
//#define USE_PERI_FAULT        0     // use pinFault for driver fault detection? (set to '0' if not connected!)

// motor driver feedback pin (=perimeter open/close detection, used for status LED)
#define USE_PERI_CURRENT      1     // use pinFeedback for perimeter current measurements? (set to '0' if not connected!)
#define pinFeedback A7  // Current sense   M1_FB (was A0)
#define PERI_CURRENT_MIN    0.03     // minimum Ampere for perimeter-is-closed detection 

// ---- sender current control (via potentiometer) ----
// sender modulates signal (PWM), based on duty-cycle set via this potentiometer
//#define USE_POT      0  // use potentiometer for current control? (set to '0' if not connected!)
//#define pinPot      A3  // 100k potentiometer (current control)   

// ---- sender automatic standby (via current sensor for charger) ----
// sender detects robot via a charging current through the charging pins
#define USE_CHG_CURRENT       0     // use charging current sensor for robot detection? (set to '0' if not connected!)
#define pinChargeCurrent     A2    // ACS712-05 current sensor OUT (was A2)
#define CHG_CURRENT_MIN   0.3  // minimum Ampere for charging detection

#define ROBOT_OUT_OF_STATION_TIMEOUT_MINS 360  // timeout for perimeter switch-off if robot not in station (minutes)

// ---- sender status LED ----
#define  pinLED 13  // ON: perimeter closed, OFF: perimeter open, BLINK: robot is charging





// --------------------------------------

volatile int step = 0;
volatile boolean enableSender = false;


double duty = 1.0;    // 100 duty%
int dutyPWM = 0;
double chargeCurrent = 0;
double periCurrentAvg = 0; 
double periCurrentMax = 0; 
//int faults = 0;
boolean isCharging = false;
boolean stateLED = false;
unsigned int chargeADCZero = 0;
RunningMedian<unsigned int,16> periCurrentMeasurements;
RunningMedian<unsigned int,96> chargeCurrentMeasurements;

int timeSeconds = 0;

unsigned long nextTimeControl = 0;
unsigned long nextTimeInfo = 0;
unsigned long nextTimeToggleLED = 0;
unsigned long nextTimeSec = 0;
int robotOutOfStationTimeMins = 0;


// must be multiple of 2 !
// http://grauonline.de/alexwww/ardumower/filter/filter.html    
// "pseudonoise4_pw" signal (sender)

#if defined (SIGCODE_1)	
  int8_t sigcode[] = { 1, 1,-1,-1, 1,-1, 1,-1,-1,1, -1, 1, 1,-1,-1, 1,-1,-1, 1,-1,-1, 1, 1,-1 };
#elif defined (SIGCODE_2)   
  int8_t sigcode[] = { 1,-1, 1, 1,-1,-1, 1, 1,-1,-1, 1,-1, 1, 1,-1,-1, 1, 1,-1,-1, 1,-1, 1,-1 };
#elif defined (SIGCODE_3)   
  int8_t sigcode[] = { 1, 1,-1,-1, 1,-1, 1, 1,-1, 1, 1,-1,-1, 1, 1,-1, 1,-1,-1, 1,-1,-1, 1,-1 };
#endif


// using transparently 5v / 1.1v ref
int analogReadMillivolt(int pin){
  int millivolt;
  int adc;
  // first read using 5v ref 
  adc = analogRead(pin);
  millivolt = ((double)adc) / 1024.0 * 5000.0;  
  if (adc < 10){
    // read using 1.1v ref
    analogReference(INTERNAL);
    adc = analogRead(pin);
    millivolt = ((double)adc) / 1024.0 * 1100.0;
    analogReference(DEFAULT); 
  }
  return millivolt;
}


void timerCallback(){       
  if (enableSender){
    if (sigcode[step] == 1) {      
      if (active_Perimeter == 1) digitalWrite(pinIN1, LOW); else digitalWrite(pinIN3, LOW);                           
      #if USE_DOUBLE_AMPLTIUDE            
      if (active_Perimeter == 1) digitalWrite(pinIN2, HIGH); else digitalWrite(pinIN4, HIGH);                              
      #endif             
      if (active_Perimeter == 1) digitalWrite(pinEnableA, HIGH); else digitalWrite(pinEnableB, HIGH);
    } else if (sigcode[step] == -1) {
      if (active_Perimeter == 1) {             
        digitalWrite(pinIN1, HIGH);          
        digitalWrite(pinIN2, LOW);                                 
        digitalWrite(pinEnableA, HIGH);
      }
      else{
        digitalWrite(pinIN3, HIGH);          
        digitalWrite(pinIN4, LOW);                                 
        digitalWrite(pinEnableB, HIGH);                          
      }
    } else {
      if (active_Perimeter == 1) digitalWrite(pinEnableA, LOW); else digitalWrite(pinEnableB, LOW); 
    } 
    step ++;    
    if (step == sizeof sigcode) {      
      step = 0; 
      //Serial.println("tim 0");      
    }    
  } else {
    //All off
    digitalWrite(pinEnableA, LOW);
    digitalWrite(pinIN1, LOW);
    digitalWrite(pinIN2, LOW);

    digitalWrite(pinEnableB, LOW);
    digitalWrite(pinIN3, LOW);
    digitalWrite(pinIN4, LOW);    
  }
}

void readEEPROM(){
  if (EEPROM.read(0) == 43){
    // EEPROM data available
    chargeADCZero = (EEPROM.read(1) << 8) | EEPROM.read(2);
  } else Serial.println("no EEPROM data found, using default calibration (INA169)");
  Serial.print("chargeADCZero=");
  Serial.println(chargeADCZero);  
}


void calibrateChargeCurrentSensor(){
  Serial.println("calibrateChargeCurrentSensor (note: robot must be outside charging station!)");
  RunningMedian<unsigned int,32> measurements;
  for (unsigned int i=0; i < measurements.getSize(); i++) {
    unsigned int m = analogRead(pinChargeCurrent);
    //Serial.println(m);
    measurements.add( m );
    delay(50);
  }
  float v;
  measurements.getAverage(v);  
  chargeADCZero = v;
  EEPROM.write(0, 43);
  EEPROM.write(1, chargeADCZero >> 8);
  EEPROM.write(2, chargeADCZero & 255);  
  Serial.println("calibration done");
  readEEPROM();
} 


  
void setup() {  
  pinMode(pinIN1, OUTPUT);    
  pinMode(pinIN2, OUTPUT);  
  pinMode(pinEnableA, OUTPUT);
  pinMode(pinIN3, OUTPUT);    
  pinMode(pinIN4, OUTPUT);  
  pinMode(pinEnableB, OUTPUT);

  //temp disable P2
  digitalWrite(pinEnableB, LOW);
  digitalWrite(pinIN3, LOW);
  digitalWrite(pinIN4, LOW);    
  
  //pinMode(pinPWM, OUTPUT);  
  pinMode(pinFeedback, INPUT);    
  //pinMode(pinFault, INPUT);      
  //pinMode(pinPot, INPUT);      
  pinMode(pinChargeCurrent, INPUT);
  
  // configure ADC reference
  // analogReference(DEFAULT); // ADC 5.0v ref    
  analogReference(INTERNAL); // ADC 1.1v ref       
    
  // sample rate 9615 Hz (19230,76923076923 / 2 => 9615.38)
  int T = 1000.0*1000.0/ 9615.38;
  Serial.begin(115200);
  
  Serial.print(VER);
  Serial.println(F("Nano_Sender V2.2"));
  
  #if defined (SIGCODE_1)	
		Serial.println(F("SIGCODE_1"));
	#elif defined (SIGCODE_2)   
		Serial.println(F("SIGCODE_2"));
	#elif defined (SIGCODE_3)   
		Serial.println(F("SIGCODE_3"));
	#endif
  
  //Serial.print("USE_PERI_FAULT=");
  //Serial.println(USE_PERI_FAULT);
  Serial.print(F("USE_PERI_CURRENT="));
  Serial.println(USE_PERI_CURRENT); 
  Serial.print(F("USE_CHG_CURRENT ="));
  Serial.println(USE_CHG_CURRENT ); 
  //Serial.println("press...");
  //Serial.println("  1  for current sensor calibration");  
  //Serial.println();
  
  readEEPROM();
  Serial.print(F("T="));
  Serial.println(T);    
  Serial.print(F("f="));
  Serial.println(1000.0*1000.0/T);    
  Timer1.initialize(T);         // initialize timer1, and set period
  //Timer1.pwm(pinPWM, 256);  
  Timer1.attachInterrupt(timerCallback);  
  //digitalWrite(pinIN1, HIGH);
  //digitalWrite(pinIN2, LOW);  
  //tone(pinPWM, 7800);

  // http://playground.arduino.cc/Main/TimerPWMCheatsheet
  // timer 2 pwm freq 31 khz  
  //cli();
  TCCR2B = TCCR2B & 0b11111000 | 0x01;
  //TIMSK2 |= (1 << OCIE2A);     // Enable Output Compare Match A Interrupt  
  //OCR2A = 255;                 // Set compared value
  //sei();

  I2C_setup();
}

void checkKey(){
  if (Serial.available() > 0) {
      char ch = (char)Serial.read();            
      Serial.print(F("received key="));
      Serial.println(ch);
      while (Serial.available()) Serial.read();
      switch (ch){
        case '1': 
          calibrateChargeCurrentSensor();           
          break;
      }
  }             
}

// Interrupt service run when Timer/Counter2 reaches OCR2A
//ISR(TIMER2_COMPA_vect) {
//  if (digitalRead(pinFault) == LOW) fault = true; 
  //if (digitalRead(pinPWM) == HIGH) fault = true; 
  //fault = true;

//void fault(){
  //Serial.println("MC_FAULT");
  //for (int i=0; i < 10; i++){
    //digitalWrite(pinLED, HIGH);
    //delay(50);
    //digitalWrite(pinLED, LOW);
    //delay(50);
  //}  
  //faults++;                          
//}


void loop(){  
/*
  if (millis() >= nextTimeControl){                    
    nextTimeControl = millis() + 100;
    dutyPWM = ((int)(duty * 255.0));
    if  ( (isCharging) || (robotOutOfStationTimeMins >= ROBOT_OUT_OF_STATION_TIMEOUT_MINS) ){
      // switch off perimeter
      Serial.println("Sender OFF"); 
      enableSender = false;
    } else {
      // switch on perimeter
      Serial.println("Sender ON");
      enableSender = true;
      
    }
    
  }

 //Out of station timer
  if (millis() >= nextTimeSec){
    nextTimeSec = millis() + 1000;
    timeSeconds++;
    if (timeSeconds >= 60){
      if (robotOutOfStationTimeMins < 1440) robotOutOfStationTimeMins++;
      timeSeconds=0;
    }
  }
*/

///=========================================
  if (millis() >= nextTimeInfo){                
    nextTimeInfo = millis() + 500;                
    checkKey();        

    //unsigned int v = 0;
    float v = 0;
    // determine charging current (Ampere)        
    if (USE_CHG_CURRENT) {                
      chargeCurrentMeasurements.getAverage(v);        
      chargeCurrent = ((double)(((int)v)  - ((int)chargeADCZero))) / 1023.0 * 1.1;  
      isCharging = (abs(chargeCurrent) >= CHG_CURRENT_MIN); 
      if (isCharging) robotOutOfStationTimeMins = 0; // reset timeout
    }  
    
    if (USE_PERI_CURRENT) {
      // determine perimeter current (Ampere)
      periCurrentMeasurements.getAverage(v);    
      //periCurrentAvg = ((double)v) / 1023.0 * 1.1 / 0.525;   // 525 mV per amp   
      periCurrentAvg = ((double)v) / (1023.0 * 0.652);   // 652 mV per amp    
      unsigned int h;
      periCurrentMeasurements.getHighest(h);    
      //periCurrentMax = ((double)h) / 1023.0 * 1.1 / 0.525;   // 525 mV per amp  
      periCurrentMax = ((double)h) / (1023.0 * 0.652);   // 652 mV per amp    
    }
        
    //Serial.print("time=");
    //Serial.print(millis()/1000);  
    /*  
    Serial.print("\tchgCurrent=");
    Serial.print(chargeCurrent, 3);
    Serial.print("\tchgCurrentADC=");
    chargeCurrentMeasurements.getAverage(v);        
    Serial.print( v );       
    Serial.print("\tisCharging=");
    Serial.print(isCharging);    
    Serial.print("\tperiCurrent avg=");
    Serial.print(periCurrentAvg);
    Serial.print("\tmax=");
    Serial.print(periCurrentMax);
    Serial.print("\tduty=");
    Serial.print(duty);
    Serial.print("\tdutyPWM=");        
    Serial.print(dutyPWM);        
    Serial.print("\tfaults=");
    Serial.print(faults); 
    Serial.print("\ttout=");    
    Serial.print(robotOutOfStationTimeMins);
    */
    //Serial.print(F("periCurrent avg = "));
    //Serial.print(periCurrentAvg);
    //Serial.println();

    
    //if (USE_POT){
      // read potentiometer
      //duty = max(0.01, ((float)map(analogRead(pinPot),  0,1023,   0,1000))  /1000.0 );
    //}              
  }
   
  if (USE_PERI_CURRENT) {
    periCurrentMeasurements.add( analogRead(pinFeedback) );    
  }

  if (USE_CHG_CURRENT){
    // determine charging current (Ampere)         
    chargeCurrentMeasurements.add( analogRead( pinChargeCurrent) );
  }
  
  // LED status 
  if (isCharging) {    
    // charging = flash
   if (millis() >= nextTimeToggleLED){
      nextTimeToggleLED = millis() + 500;
      stateLED = !stateLED;
    }
  } else {
    // not charging => indicate perimeter wire state (OFF=broken/perimeter turned off)
    stateLED = (periCurrentAvg >= PERI_CURRENT_MIN);
  }
  
  digitalWrite(pinLED, stateLED);   

  
   //Manage I2C
  if (HardwareTest){
    if (millis() > I2C_Req_time ){ //min time between requests
      ++test_counter;
      char command[32];
      char temp_Data[10]; //initial array
      strcpy(command,"99,S1 ,");
      dtostrf(test_counter, 6,0,  temp_Data); //float to char array
      strcat(command,temp_Data) ;
      strcat(command,",") ;
      request_I2C(command);  
      I2C_Req_time =  I2C_Req_timeout + millis();
    }
  }    
    if (digitalRead(I2C_1) == HIGH ) {
      if (millis() > I2C_start_time + I2C_timeout){
        //I2C request fail 
        Serial.println(F(" I2C Request Fail"));
        digitalWrite(I2C_1, LOW);//reset request
      }
    }

}
