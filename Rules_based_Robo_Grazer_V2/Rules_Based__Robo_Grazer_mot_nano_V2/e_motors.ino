/*  Arduino DC Motor Control - PWM | H-Bridge | L298N
*/

//first set all pins with a pullup to reduce noise
void initPins() {
  for(int i=2; i<=12; i++) {
    pinMode(i, INPUT_PULLUP);
  }
 pinMode(13, OUTPUT);        // LED Pin
  for(int i=14; i<=19; i++) {
    pinMode(i, INPUT_PULLUP);
  }
}

void Motor_setup() {
  initPins();
  /*
#ifdef L298N
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(enC, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);
#endif
*/
#ifdef MCC
   //wheel motors
  pinMode(pinMotorLeftPWM, OUTPUT);
  pinMode(pinMotorLeftDir, OUTPUT);
  pinMode(pinMotorEnable, OUTPUT);  
  pinMode(pinMotorRightPWM, OUTPUT);
  pinMode(pinMotorRightDir, OUTPUT);

  //All LOW to start
  digitalWrite(pinMotorLeftPWM, LOW) ;//New OR Gate
  digitalWrite(pinMotorLeftDir, LOW) ;//New OR Gate
  digitalWrite(pinMotorRightPWM, LOW) ;//New OR Gate
  digitalWrite(pinMotorRightDir, LOW) ;//New OR Gate
  
#endif
  pinMode(pinSTOP, INPUT); //STOP. External signal to stop the mower
  pinMode(pinMotorMowPWM, OUTPUT); //mow motor
  pinMode(ENCODER_LEFT_PIN, INPUT);
  pinMode(ENCODER_RIGHT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_PIN), pulseLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_PIN), pulseRight, FALLING);
  Cur_ML = Cur_MR = 0;
}

void pulseLeft() {
  leftTicks++;
  //Serial.println("L");
}
void pulseRight() {
  rightTicks++;
  //Serial.println("R");
}

//Forward straight
void FWD_straight(){
  //if (Cur_MR == Cur_ML  && Cur_MR == 0){
    //start up motors
    Cur_MR = FWD_Def;
    Cur_ML = FWD_Def;
    //rightTicks = 0;
    //leftTicks = 0;
  //}
  if (rightTicks > leftTicks){
     Cur_MR--;
     if (Cur_MR < 0) Cur_MR = 0;
  }
  if (leftTicks > rightTicks){
     Cur_MR++;
     if (Cur_MR > FWD_Def ) Cur_MR = FWD_Def;
  }
  motor_Drive(Cur_ML,Cur_MR, true);
}

//Forward Left
void FWD_Left(){
   //if (Cur_MR == Cur_ML  && Cur_MR == 0){
    //start up motors
    Cur_MR = FWD_L_Def;
    Cur_ML = 0;
   //}
  
  motor_Drive(Cur_ML,Cur_MR, true);
}
//STOP
void Stop(){
    Cur_MR = 0;
    Cur_ML = 0;
  
  motor_Drive(Cur_ML,Cur_MR, true);
}

//Forward Right
void FWD_Right(){
  //if (Cur_MR == Cur_ML  && Cur_MR == 0){
    //start up motors
    Cur_ML = FWD_L_Def;
    Cur_MR = 0;
  //}
  
  motor_Drive(Cur_ML,Cur_MR, true);
}

//Backward straight 
void BWD_straight(){
  if (Cur_MR == Cur_ML  && Cur_MR == 0){
    //start up motors
    Cur_MR = BWD_Def;
    Cur_ML = BWD_Def;
    rightTicks = 0;
    leftTicks = 0;
  }
  if (rightTicks > leftTicks){
     Cur_MR++;
     if (Cur_MR > 0) Cur_MR = 0; 
  }
  if (leftTicks > rightTicks){
     Cur_MR--;
      if (Cur_MR < BWD_Def ) Cur_MR = BWD_Def;
  }
  motor_Drive(Cur_ML,Cur_MR, true);
  //Serial.print(" ML ");
  //Serial.print(Cur_ML);
  //Serial.print("\t MR ");
  //Serial.print(Cur_MR);
 
}

//avoid
void avoid(){
  //sets up avoid sequence
  bitSet(system_current,21);
}

void motor_Drive(int pwmLeft,int pwmRight, boolean accel){
    if (accel){
      if (pwmLeft > motorLeftPWMCurr){
          motorLeftPWMCurr++;
          if (motorLeftPWMCurr > pwmLeft){motorLeftPWMCurr = pwmLeft;}
      }
      else{
        motorLeftPWMCurr--;
        if (motorLeftPWMCurr < pwmLeft){motorLeftPWMCurr = pwmLeft;}
      }
      if (pwmRight > motorRightPWMCurr){
          motorRightPWMCurr++;
          if (motorRightPWMCurr > pwmRight) {motorRightPWMCurr = pwmRight;}
      }
      else{
        motorRightPWMCurr--;
        if (motorRightPWMCurr < pwmRight) {motorRightPWMCurr = pwmRight;}
      }
    }
    else{
      motorLeftPWMCurr = pwmLeft;
      motorRightPWMCurr = pwmRight;
    }

  #ifdef MCC
    setMC33926(pinMotorLeftDir, pinMotorLeftPWM, motorLeftPWMCurr);
    setMC33926(pinMotorRightDir, pinMotorRightPWM, motorRightPWMCurr);
    return;
  #endif
  setL298N(motorLeftPWMCurr, motorRightPWMCurr);
}

void setMotormow(int pwmMow){
  if (pwmMow == 0){setMC33926(0, pinMotorMowPWM,0);}
  
  if (millis() < nextTimeMotorMowControl) return;
  nextTimeMotorMowControl = millis() + 100;
  
  if (pwmMow > motorMowPWMCurr){
      motorMowPWMCurr++;
      if (motorMowPWMCurr > pwmMow){motorMowPWMCurr = pwmMow;}
  }
  else{
    motorMowPWMCurr--;
    if (motorMowPWMCurr < pwmMow){motorMowPWMCurr = pwmMow;}
  }
  setMC33926(0, pinMotorMowPWM,motorMowPWMCurr);
}

// MC33926 motor driver
// Check http://forum.pololu.com/viewtopic.php?f=15&t=5272#p25031 for explanations.
//(8-bit PWM=255, 10-bit PWM=1023)
// IN1 PinPWM         IN2 PinDir
// PWM                L     Forward
// nPWM               H     Reverse
void setMC33926(int pinDir, int pinPWM, int speed){
  if (speed < 0){
    digitalWrite(pinDir, HIGH) ;
    analogWrite(pinPWM, 255-((byte)abs(speed)));
  } else {
    digitalWrite(pinDir, LOW) ;
    analogWrite(pinPWM, ((byte)speed));
  }
}

//Drive wheels L298N Driver
void setL298N(int ML, int MR) {
  /*
#ifdef L298N
  // minimum value for speed = -255, maximum = 255
  if  (ML == 0 && MR == 0) { // stop motors
    //Serial.println(F("Stop_Motors"));
    //Braking
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW); 
    digitalWrite(enA, LOW);
    digitalWrite(enB, LOW);
    return;
  }

  // Motor_Left
  if (ML >= 0) {
    // Set Motor forward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    //Serial.print(F(" AF "));
  }
  if (ML < 0) {
    // Set Motor backward
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    //Serial.print(F(" AB "));
    ML = ML * -1;//set positive speed for PWM
  }

  // Motor_Right
  if (MR >= 0) {
    // Set //Motor forward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    //Serial.print(F(" BF "));
  }
  if (MR < 0) {
    // Set Motor backward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    //Serial.print(F(" BB "));
    MR = MR * -1;//set positive speed for PWM
  }
    
  analogWrite(enA, ML);
  analogWrite(enB, MR);
#endif
*/
}


//PID controller: track perimeter
void PeriTrk() {

  //3 states
  //wire is lost
  //On the wire straight line pid fast action
  //Back slowly to the wire pid soft action
 
  //Control the perimeter motor only each 30ms
  if (millis() < nextTimeMotorPerimeterControl) return;
  nextTimeMotorPerimeterControl = millis() + 30; //possible 15ms with the DUE

  //because Highest can change dependent on the sample of data use!
  //tell to the pid where is the mower   (Pid.x)
    
  //Derived from Perimeter nano via I2C
  //perimeterPID.x = 5 * (double(perimeterMag) / perimeterMagMaxValue); //original
  perimeterPID.x = perimeterPIDx_multiplier * (double(KhsmoothMag) / perimeterMagMaxValue); //kh
 
   
  //tell to the Pid where to go (Pid.w)
  if (perimeterInside) {
    perimeterPID.w = -0.5;
  }
  else {
    perimeterPID.w = 0.5;
  }
  //parameter the PID 
  perimeterPID.y_min = -MaxSpeedperiPwm ;
  perimeterPID.y_max = MaxSpeedperiPwm ;
  perimeterPID.max_output = MaxSpeedperiPwm ;
  //and compute
 
  perimeterPID.compute();
  //First state wire is lost
  //important to understand TrackingPerimeterTransitionTimeOut It's if during this maximum duration the robot does not make a transition in and
  //out then it is supposed to have lost the wire, the PID is stopped to go into blocking mode of one of the two wheels. 
  // If the trackingPerimeterTransitionTimeOut is too large the robot goes too far out of the wire and goes round in a circle outside the wire without finding it

  // If the second condition is true the robot has lost the wire on a corner since more trackingPerimeterTransitionTimeOut 1000 was(2500ms) 
  //for example it is then necessary to stop one of the 2 wheels to make a half turn and find again the wire
  //if ((millis() > stateStartTime + 10000) && (millis() > perimeterLastTransitionTime + trackingPerimeterTransitionTimeOut)) {
  if ( millis() > (perimeterLastTransitionTime + trackingPerimeterTransitionTimeOut)) {//KH State timer removed
   //if (SerialData_B){
      //Serial.println("A");
   //}
    //Lost wire
    //If this condition is true one of the 2 wheels stop rotate the other continues with the result of the PID (advised)
    if (trackingBlockInnerWheelWhilePerimeterStruggling == 1) { //preset
      //if (SerialData_B){
          //Serial.println("B");
       //}
      if (perimeterInside) { 
        rightSpeedperi = max(-MaxSpeedperiPwm, min(MaxSpeedperiPwm, MaxSpeedperiPwm / 2  + perimeterPID.y));
        leftSpeedperi = 0;
      }
      else {
        rightSpeedperi = 0;
        leftSpeedperi = max(-MaxSpeedperiPwm, min(MaxSpeedperiPwm, MaxSpeedperiPwm / 2 - perimeterPID.y));
      }
    }
  
    //if (perimeterDir)setMotorPWM( leftSpeedperi, rightSpeedperi, false);
    //if (!perimeterDir)setMotorPWM( rightSpeedperi, leftSpeedperi, false);//KH

    if (perimeterDir){ motor_Drive( leftSpeedperi, rightSpeedperi, false);}
    if (!perimeterDir){ motor_Drive( rightSpeedperi, leftSpeedperi, false);}

    
    
    
    // we record The time at which the last wire loss occurred
    lastTimeForgetWire = millis();
    // if we have lost the wire from too long time (the robot is running in a circle outside the wire we stop everything)
    if (millis() > (perimeterLastTransitionTime + trackingErrorTimeOut) ) {
        Serial.println(F("Error: tracking error"));
        //addErrorCounter(ERR_TRACKING);  KH Suspended
        //setNextState(STATE_PERI_FIND, 0);
      }
      // out of the fonction until the next loop  
      return;
    } 

    //above works ok with perimeterLastTransitionTime not updated

    
    // here we have just found again the wire we need a slow return to let the pid temp react by decreasing its action (perimeterPID.y / PeriCoeffAccel)
    if ( (millis() - lastTimeForgetWire ) < trackingPerimeterTransitionTimeOut ) {
      //if (SerialData_B){
          //Serial.println("C");
       //}
      //PeriCoeffAccel move gently from 3 to 1 and so perimeterPID.y/PeriCoeffAccel increase during 3 secondes
      PeriCoeffAccel = (3000.00 - (millis() - lastTimeForgetWire))/1000.00 ;
      if (PeriCoeffAccel < 1.00) PeriCoeffAccel = 1.00;
      rightSpeedperi = max(0, min(MaxSpeedperiPwm, MaxSpeedperiPwm / 1.5 +  perimeterPID.y / PeriCoeffAccel));
      leftSpeedperi = max(0, min(MaxSpeedperiPwm, MaxSpeedperiPwm / 1.5 -  perimeterPID.y / PeriCoeffAccel));
    }
    else {
      //if (SerialData_B){
          //Serial.println("E");
       //}
    //we are in straight line the pid is total and not/2
      rightSpeedperi = max(0, min(MaxSpeedperiPwm, MaxSpeedperiPwm/1.5   + perimeterPID.y));
      leftSpeedperi = max(0, min(MaxSpeedperiPwm, MaxSpeedperiPwm/1.5  - perimeterPID.y));    
    }

      //if (SerialData_B){
          //Serial.println("F");
       //}
    //if (perimeterDir) setMotorPWM( leftSpeedperi, rightSpeedperi, false);
    //if (!perimeterDir)setMotorPWM( rightSpeedperi, leftSpeedperi, false);//KH

    if (perimeterDir){ motor_Drive( leftSpeedperi, rightSpeedperi, false);}
    if (!perimeterDir){ motor_Drive( rightSpeedperi, leftSpeedperi, false);}

  
    
    //if the mower move in perfect straight line the transition between in and out is 
    //longer so you need to reset the perimeterLastTransitionTime    
    if (abs(KhsmoothMag ) < perimeterMagMaxValue/4) { 
       //if (SerialData_B){
          //Serial.println("D");
       //}
      perimeterLastTransitionTime = millis(); //initialise perimeterLastTransitionTime in perfect straigth line
    } 

}






















//
