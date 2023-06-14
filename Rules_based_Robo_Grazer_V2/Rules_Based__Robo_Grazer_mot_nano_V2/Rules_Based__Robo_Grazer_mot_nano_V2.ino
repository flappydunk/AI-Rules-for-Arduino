//Rules Cut down version for Nano
//experimental pond controller. Full functioning with facilty to be controlled by a ESP via I2C
//runtime version. No display options to reduce code footprint
//static arrays loaded into program memory to save space

//Select motor driver
//#define L298N //12v driver
#define MCC //24v Driver

#include <Wire.h>
#include "config.h"
#include "Rules.h"
//#include <avr/wdt.h>




//peri
#include "adcman.h"
#include "perimeter.h"
Perimeter perimeter;
#define pinPerimeterLeft A6        // perimeter pin
#define pinPerimeterRight A7       // leave open
#include "PID.h"
PID perimeterPID ;             // perimeter PID controller 
PID motorLeftPID ;              // motor left wheel PID controller
PID motorRightPID;              // motor right wheel PID controller


void setup(){
  //Start Serial
  Serial.begin(Serial_BAUDRATE);
  while (!Serial);
  Serial.flush();

  Serial.println();
  Serial.println(F("Rules_Based_Robo_Grazer_mot__V2"));

  
  pinMode(LED_BUILTIN, OUTPUT);
  start_I2C_Slave(I2C_DEV_ADDR);
  Motor_setup();
  setMotormow(0);//mow off
  digitalWrite(pinMotorEnable, HIGH);
  //wdt_enable (WDTO_8S);
  rules_setup();
  display_current();
  Stop(); //stop motors
  Last_system_current = system_current;
  periStart();
}

void loop() {
  readSerial();
  readSensors();
  
  rules_refresh();
  command_execute();
  periLoop();
}



void command_execute(){
 uint32_t mycommand = rules_pop();
 //int proc;
 if (mycommand != -1){ //not empty
  
  //decode decide which proc to send command to
  
  if (mycommand == 0x11) {turndir = random(2);}
  if (mycommand == 0x21) {leftTicks = 0; rightTicks = 0;}
  if (mycommand == 0x41) {jamCount = 0;}
  if (mycommand == 0x31) {
    leftTicks = 0; 
    rightTicks = 0;
    if (jamCount == 0){
      //start timer
      jam_timeout = millis() + (jam_time * 1000);
    }
    jamCount++;
    if (jam_timeout < millis()){
        
        if (jamCount > jamCount_max){
            bitSet(system_current,20);          
        }
        else{
          jamCount = 0;
        }
    }   
    Serial.print("jamCount ");
    Serial.println(jamCount);  
  }

  //else if (mycommand == 0x11) {Serial.println(F("Pump ON")); action_servo(0,sServo_ON);}
  //else if (mycommand == 0x02) {Serial.println(F("Heat OFF")); action_servo(2,sServo_OFF);}
 // else if (mycommand == 0x12) {Serial.println(F("Heat ON")); action_servo(2,sServo_ON);}
 // else if (mycommand == 0x03) {Serial.println(F("Video OFF")); action_servo(1,sServo_OFF);}
  //else if (mycommand == 0x13) {Serial.println(F("Video ON")); action_servo(1,sServo_ON);}
  //else if (mycommand == 0x04) {Serial.println("Water OFF"); action_servo(3,sServo_OFF);}
  //else if (mycommand == 0x14) {Serial.println(F("Water ON")); action_servo(3,sServo_ON);}
 
  

 }
}



void readSensors(){
   //system monitor LED and sensor scan
   /*
  if ((millis() > LED_next_time)){//LED
    if ( digitalRead(LED_BUILTIN) == HIGH ) { //flashes if working
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
    }
    else {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
      //wdt_reset(); //reset watchdog
    } 
    
    LED_next_time =  LED_poll_time + millis();
  }
*/

    if(bitRead(system_current,22) == 1) {
      setMotormow(motorMowPWMMax);//speed for grass cutter
    }
    else{
      setMotormow(0);
    }
    
   //Emergency STOP Activated
  if (digitalRead(pinSTOP) == LOW ){
      bitSet(system_current,21);// Avoid
      //Serial.println("Stopped");
  }
  if(bitRead(system_current,16) == 1 || bitRead(system_current,8) == 1 || system_current == 0x0) {
    Stop();
    //Serial.println("ST");
    return;
  }
  
  //read flags and run only once
  //if(Last_system_current == system_current){
     //return;
  //}
  Last_system_current = system_current; 
  if(bitRead(system_current,3) == 1) {
    FWD_straight();
    //Serial.println("Fwd");
    return;
  }
  if(bitRead(system_current,4) == 1) {
    BWD_straight();
    //Serial.println("Bwd");
    return;
  }
  if(bitRead(system_current,6) == 1) {//turn random
    if (turndir > 0){
      FWD_Left();
      //Serial.println("TL");
    }
    else{
      FWD_Right();
      //Serial.println("TR");
    }
    return;
  }
   
  if(bitRead(system_current,13) == 1) {
    PeriTrk();
    //Serial.println("PT");
    return;
  }
  
  
  
  //Serial.print("ML : ");
  //Serial.print(Cur_ML);
  //Serial.print("\t MR : ");
  //Serial.println(Cur_MR);
 
}

void periStart(){
  
  ADCMan.init();
  perimeter.setPins(pinPerimeterLeft, pinPerimeterRight);          
  perimeter.useDifferentialPerimeterSignal = true;
  
  perimeter.speedTest();

  perimeterPID.Kp  = perimeterPIDKp;       // perimeter PID controller
  perimeterPID.Ki  = perimeterPIDKi;
  perimeterPID.Kd  = perimeterPIDKd;
}

void periLoop(){
   ADCMan.run();  

   //read perimeter data and smoothe result
  //my cascade smoother
  smoothMag_V1 = smoother(perimeter.getMagnitude(0), smoothMag_V1,2.5); //harder boundaries set. removes large swings
  smoothMag_V2 = smoother(smoothMag_V1,smoothMag_V2,2.5); //harder boundaries set. removes large swings
  smoothMag_V3 = smoother(smoothMag_V2,smoothMag_V3,2.5); //harder boundaries set. removes large swings
  KhsmoothMag = smoother(smoothMag_V3,KhsmoothMag,2.5); //harder boundaries set. removes large swings
 // ?????
  //perimeterMagMaxValue = perimeter.getSignalMax(0); //updates max signal
  //?????
  //Send current pos for mowing
  if (KhsmoothMag < 0) {
    bitSet(system_current,12);
    perimeterInside = true; //used for perimeter tracking
    //Serial.println("In");
  }
  else {
    perimeterInside = false;
    bitClear(system_current,12);
    
    //Serial.println("Out");
  }


 //check for transition
  if (perimeterLastState != perimeterInside){      
      perimeterLastTransitionTime = millis(); 
      perimeterLastState = perimeterInside; 
  }   

}

double smoother(double NewMag, double smoothMag, double rate){
    //KhsmoothMag = 0.70 * KhsmoothMag + 0.3 * (NewMag); //attempt to remove wild range values
    //Margin boundary method
    double Margin;
    double diffMag;
    double diffMagpos;
    
    diffMag = (NewMag - smoothMag); 
    diffMagpos = abs(diffMag);    
    Margin = abs(NewMag/rate); //% band max change high rate divisor number = high damping  
    //Margin = Margin * 20; //amplifier of smoothed signal to restore fast transitions
    
    if (diffMagpos > Margin){
      if (diffMag < 0) smoothMag = smoothMag - Margin;
      if (diffMag > 0) smoothMag = smoothMag + Margin;
    }
    else{
      smoothMag = 0.70 * smoothMag + 0.3 * (NewMag); //attempt to remove wild range values
    } 
    return smoothMag;
//what further can I do   
}
























//
