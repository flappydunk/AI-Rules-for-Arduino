

//#include <Servo.h>



// Create servo objects

//Servo Servo1;
void Servo_loop() {
  int ServoNum;
  int Pos;

  //test if leaf is connected
  if (digitalRead(Leaf) == LOW ){
    New_Leaf_Servo_State = 1; //connected 
  }
  else{
    New_Leaf_Servo_State = 0; //not connected 
  }
  
  if (New_Power_Servo_State != Last_Power_Servo_State){
      ServoNum = 0; 
      if (New_Power_Servo_State == 1)  {
        //Turn servo ON
        Pos = sServo_ON;
      }
      else  {
        //Turn servo OFF
        Pos = sServo_OFF;
      }
      action_servo(ServoNum,Pos);
      Last_Power_Servo_State = New_Power_Servo_State;
  }
  if (New_Leaf_Servo_State != Last_Leaf_Servo_State){
      ServoNum = 1; 
      if (New_Leaf_Servo_State == 1)  {
        //Turn servo ON
        Pos = sServo_ON;
      }
      else  {
        //Turn servo OFF
        Pos = sServo_OFF;
      }
      action_servo(ServoNum,Pos);
      Last_Leaf_Servo_State = New_Leaf_Servo_State;
  }
 
}
void action_servo(int ServoNum,int Pos){
  //Serial.println(sServoPins[ServoNum]);
  Servo1.attach(sServoPins[ServoNum]);
  Servo1.write(Pos);
  delay(1000); //allow time to move
  //Serial.print(F("Servo "));
  //Serial.print(ServoNum);
  //now check its position 
  if (Servo1.read() > 45){   
    //Serial.println(F(" = OFF"));
    sServo_Flags[ServoNum]=0;
  }
  else{
    //Serial.println(F(" = ON"));
    sServo_Flags[ServoNum]=1;
  }
  //Serial.print("Servo Angle = ");
  //Serial.println(Servo1.read());
  Servo1.detach();
  New_Servo_State[0]='\0';
}

void zero_Servos(){
  //Set all servos off
  int i;
  for (i = 0 ; i < Max_Servo ; i++){
    Servo1.attach(sServoPins[i]);
    Servo1.write(sServo_OFF);
    //delay(2000); //allow time to move
    delay(1000); //allow time to move
    Servo1.detach();
    delay(15); //allow time to move
    sServo_Flags[i]=0;
    wdt_reset(); //reset watchdog
    //Serial.print("Servo Angle = ");
    //Serial.println(Servo1.read());
  }
  
}

void set_Servos(){
  //Set all servos on
  int i;
  for (i = 0 ; i < Max_Servo ; i++){
    Servo1.attach(sServoPins[i]);
    Servo1.write(sServo_ON);
    //delay(2000); //allow time to move
    delay(1000); //allow time to move
    Servo1.detach();
    delay(15); //allow time to move
    sServo_Flags[i]=0;
    wdt_reset(); //reset watchdog
    //Serial.print("Servo Angle = ");
    //Serial.println(Servo1.read());
  }
  
}
void read_Servo_Status(char reply[]){
  //prepares a string for transmission to ESP
  int i;
  strcpy(reply,"");
  for (i = 0 ; i < Max_Servo ; i++){
    if (sServo_Flags[i] == 0) { strcat(reply,"0,");} else {strcat(reply,"1,");}
  }
  strcat(reply,"0,"); //fudge for spare servo not used
  //Serial.print("Servos: ");
  //Serial.println(reply);
}
