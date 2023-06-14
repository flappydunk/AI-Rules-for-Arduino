
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

  if (cmd == '0'){ //display current state
    ESP.restart();//does a complete reboot
     return;
  }

  //     Rules 
  if (cmd == 's'){ //display current state
    display_current();
     return;
  }

  if (cmd == 'r'){ //re start rules
    init_slave_rules();//all rules
    display_current();
    return;
  }
  if (cmd == 'F'){ //toggle all rules
    Run_Rules = !Run_Rules; //toggle rules
    if (!Run_Rules){ Serial.println("Rules OFF");}
    return;
  }
  if (cmd == 'P'){ //Sender Main Power relay
    //Power servo OFF
    Power_Relay_Action(false); //power off
    Send_Command(Slave_2_Address, "30,3,0,"); //power servo off);  //pass to mon for action 
    
    return;
  }
 
  //simulate 
  if (cmd == 'a'){ //avoid
    Serial.println("Avoid");
    bitSet(system_current,1);
    bitSet(system_current,2);
  }
  if (cmd == 'h'){ //Home
    Serial.println("Go Home");
   readMenu('1', '2') ;
  }
  if (cmd == 'm'){ //mow
    Serial.println("Mow");
    readMenu('2', '0') ;
  }
  if (cmd == 'f'){ //Stop - finish
    Serial.println("Stop");
    readMenu('1', '0'); 
  }
  
  if (cmd == 'i') {
      SerialData_A = !SerialData_A; //toggle console messages
  }
}
