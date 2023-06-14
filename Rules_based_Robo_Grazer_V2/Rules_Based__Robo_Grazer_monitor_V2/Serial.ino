//Serial interface
char waitCharConsole() {
  while (Serial.available() == 0);
  return (char)Serial.read();
}


void readSerial() {
  // serial input  
  if (Serial.available() > 0) {     
     char cmd = waitCharConsole();
     readComs(cmd); //single char only
  }    
}

void readComs(char cmd){
   
  if (cmd == 's'){ //display current state
    display_current();
     return;
  }

  if (cmd == 'r'){ //re start rules
    run_rules = true;
    rules_restart();
    display_current();
    return;
  }
  if (cmd == 'S'){ //power servo
    if (New_Power_Servo_State == 1){New_Power_Servo_State = 0;}else{New_Power_Servo_State = 1;}
  }
 
  if (cmd == 't'){ //suspend rules for testing
    run_rules = false;
    return;
  }
  
  if (cmd == 'b'){ //toggle Bat condition
   if (bitRead(system_current,3) == 0) {bitSet(system_current,3); } else {bitClear(system_current,3);}
    display_current();
    return;
  }
  if (cmd == 'd'){ //toggle Docking condition
   if (bitRead(system_current,2) == 0) {bitSet(system_current,2); } else {bitClear(system_current,2);}
    display_current();
    return;
  }
  if (cmd == 'm'){ //toggle Rain condition
   if (bitRead(system_current,4) == 0) {bitSet(system_current,4); } else {bitClear(system_current,4);}
    display_current();
    return;
  }
  if (cmd == 'h'){ //toggle go home
   if (bitRead(system_current,5) == 0) {bitSet(system_current,5); } else {bitClear(system_current,5);}
    display_current();
    return;
  }
   if (cmd == 'f'){ //toggle finish
   if (bitRead(system_current,1) == 0) {bitSet(system_current,1); } else {bitClear(system_current,1);}
    display_current();
    return;
  }
  if (cmd == 'p'){ //Power relay
   if (bitRead(system_current,6) == 0) {bitSet(system_current,6); } else {bitClear(system_current,6);}
    display_current();
    return;
  }
  if (cmd == 'F'){ //Fan
    if (bitRead(system_current,8) == 0) {bitSet(system_current,8); } else {bitClear(system_current,8);}
    return;
  }
 
}


























//
