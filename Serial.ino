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
   
  if (cmd == 'c'){ //display current state
    display_current();
     return;
  }
  if (cmd == 'd'){ //display toggle
    Display = !Display;
    return;
  }
  if (cmd == 'r'){ //re start rules
    Run_Rules = true;
    rules_restart();//all rules
    display_current();
    return;
  }
  if (cmd == 's'){ //stop rules
    Run_Rules = false;
    return;
  }

  //Simulate sensors etc
  if (cmd == '3'){ //Temp sensor toggle
    if ((bitRead(system_current,3) == 1)){bitClear(system_current,3);}else{bitSet(system_current,3);}
  }


}















//
