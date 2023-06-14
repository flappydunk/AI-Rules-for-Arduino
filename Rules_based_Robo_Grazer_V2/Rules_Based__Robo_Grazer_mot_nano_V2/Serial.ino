//Serial interface
char waitCharConsole() {
  while (Serial.available() == 0);
  return (char)Serial.read();
}


void readSerial() {
  // serial input  
  if (Serial.available() > 0) {     
     char cmd = waitCharConsole();
     readComs(cmd);
  }    
}

void readComs(char cmd){

  //ADC Manager
  if (cmd == 'c') {
      Serial.println(F("calibrating ADC (power off sender for this!)..."));
      Serial.flush();
      delay(5000);
      ADCMan.calibrate();
  }

  
  //     Rules 
  if (cmd == 's'){ //display current state
    display_current();
     return;
  }

  if (cmd == 'r'){ //re start rules
    rules_restart();
    display_current();
    return;
  }
 
  //simulate 
  if (cmd == 'a'){ //avoid
    Serial.println("Avoid");
    avoid();
  }
  if (cmd == 'd'){ //Docked
    Serial.println("Docked");
    bitSet(system_current,15);
  }
  if (cmd == 'h'){ //Home
    Serial.println("Go Home");
    bitSet(system_current,10);
  }
  if (cmd == 'm'){ //mow
    Serial.println("Mow");
    bitSet(system_current,0);
  }
  if (cmd == 'f'){ //Stop - finish
    Serial.println("Stop");
    bitSet(system_current,16);
    //bitSet(system_current,8);
    //bitClear(system_current,0);
    //bitClear(system_current,3);
    //bitClear(system_current,4);
    //bitClear(system_current,6);
  }
  if (cmd == 'M'){ //mow motor toggle
    if( bitRead(system_current,22)==1){bitClear(system_current,22);} else { bitSet(system_current,22);}
  }
  
 
}







































//
