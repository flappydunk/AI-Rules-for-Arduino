//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================

void handleMessage() {
  String Req_Update = server.arg("State"); 
  if (Req_Update == "2"){displayweb_flags(); WebMessage = true;} //need to refresh log data
  
  String logMessage;
  if (WebMessage){ //new message waiting
    //Write all last messages
    logMessage = "M: ,"; //prefix. This is stripped off by the web page
    //populates all last saved messages
    for (int i=0; i < MaxWebStore; i++){
      if (WebMessageStore[i] != ""){
         logMessage += WebMessageStore[i];    
      }
    }
    logMessage += ",";
    //Serial.println(logMessage);
    WebMessage = false;
  }
  else {
       
    //LEDNames 0 to 7 ={"PERIM POS","DOCKED","OverCharge","CHARGING","GOING HOME","","LEAF","PERI ON/OFF"};
    String LEDStatus =","; //spare field
    for(int i = 0; i < NUM_LEDS; i++ ) {
        //runs through saved settings 
        if      (cLEDStatus[i] == 'R') LEDStatus = LEDStatus + 'R' +',' ;
        else if (cLEDStatus[i] == 'G') LEDStatus = LEDStatus + 'G' +',' ;
        else if (cLEDStatus[i] == 'B') LEDStatus = LEDStatus + 'B' +',' ;
        else if (cLEDStatus[i] == 'O') LEDStatus = LEDStatus + 'O' +',' ;
        else if (cLEDStatus[i] == 'Y') LEDStatus = LEDStatus + 'Y' +',' ;
    }
    logMessage += LEDStatus;//fields 2,3,4,5,6,7,8,9
    logMessage +=  ","; //
    logMessage += String(Mow_Dur_mins) + ","; // Field 10 = Mow duration
    logMessage += String(active_Perimeter) + ","; // Field 11 = Garden loop
    logMessage +=   Mow_error + ","; // Mow errors FIELD = 12
    logMessage += batvolt + ","; // Field 13 = batt
    logMessage += String(Sender_Current) + ","; // Field 14 = Current from Sender charge or perimeter
    logMessage += String(chgvolt) + ","; // Field 15 = Charging volt from monitor
  
    logMessage += String(LEDNames[Status_name])  + ",";//Field 16 = Overall Status at top of page
    logMessage += String(Charge_Dur_mins) + ","; // Field 17 = Charging duration
    if (Sched_Active){logMessage += "ON,";} else {logMessage += "OFF,";} //Field 18 = Scheduler
    if (Send_Update || Req_Update == "1"){ //variable settings from web page
      logMessage += String(eeprom_settings[0]) + ","; // Field 19 = Mow Start time hr
      logMessage += String(eeprom_settings[1]) + ","; // Field 20 = Mow OFF time hr
      logMessage += String(eeprom_settings[2]) + ","; // Field 21 = Short forward error count
      logMessage += String(eeprom_settings[3]) + ","; // Field 22 = Max perimeter transition time seconds
    }
    else{
      logMessage += "X,"; // Mow Start time hr
      logMessage += "X,"; // Mow OFF time hr
      logMessage += "X,"; // Short forward error count
      logMessage += "X,"; // Max perimeter transition time seconds
    }
    logMessage += String(temp) + ","; // Field 23 = Temperature
    logMessage += current_time + ","; // Current Time
  }
  //Serial.println(logMessage);
  server.send(200, "text/plane", logMessage); //Send message only to client ajax request
  logMessage = ""; //reset string
  Send_Update = false;
}



void eepromData() {
  String LogData;
  int K1_state = server.arg("K1state").toInt(); 
  int K2_state = server.arg("K2state").toInt(); 
  //int K3_state = server.arg("K3state").toInt(); 
  //int K4_state = server.arg("K4state").toInt();
  

  //Check data
  if (K1_state > 23 || K1_state < 1 ) {LogData = "Sched ON error ";}
  if (K2_state > 23 || K2_state < 1 ) {LogData = LogData + "Sched OFF error ";}
 
 
  if (LogData == "") {
    //Update variables here for future storage
    eeprom_settings[0] = K1_state; //Mower ON Time
    eeprom_settings[1] = K2_state; //Mower OFF Time
    //eeprom_settings[2] = K3_state; //Short forward error count
    //eeprom_settings[3] = K4_state; //Max perimeter transition time seconds
    
    eeprom_Save_ALL( eeprom_settings, eeprom_size); //save to eeprom
    eeprom_Read_ALL( eeprom_settings, eeprom_size); //check eeprom

    forward_count = String(eeprom_settings[2]);
    perimeter_timeout = String(eeprom_settings[3]);
    //Serial.println(forward_count);
    //Serial.println(perimeter_timeout);
    LogData = "Fields Updated ";
  }
  //Serial.println(LogData);
   
  reply = LogData; //action the menu
  webConsoleprint(LogData, false);
  server.send(200, "text/plane", ""); //Send web page
  Send_Update = true;
}


String readMenu(char b1, char b2) { //read chars from web or serial interface
  //Serial.println(b1);
  String I2C_command = "";
  String reply;
  if (b1 == '2' && b2 == '0'){ //Mow
     bitSet(system_current,12);
     //reply = "Start Mow";
  }
  if (b1 == '1' && b2 == '2'){ //Go Home
   bitSet(system_current,14);  
   //reply = "Start Home";
  }
  if (b1 == '1' && b2 == '0'){ //Stop
    bitSet(system_current,13);
    //reply = "STOP";
  }
  if (b1 == '1' && b2 == '6'){ //Power OFF
    bitSet(system_current,14); //Go Home
    bitSet(system_current,22); //prepare shutdown
    webConsoleprint("SHUTDOWN", true);
  }
  if (b1 == 'C' && b2 == '0'){ //Clear log
    Clear_WebMessages();
    displayweb_flags();
  }
  if (b1 == 'R' && b2 == '0'){ //Reset
    Clear_WebMessages();
    init_slave_rules();//all rules
  }
  if (b1 == '5' && b2 == '0'){ //Flags
    displayweb_flags();
  }
  if (b1 == 'D' && b2 == '0'){ //Scheduler button
    Sched_Active = !Sched_Active;//changes web page icon
  }
  if (b1 == 'S' && b2 == '2'){ //Toggle Peri & turn OFF Peri
    if (active_Perimeter == 1) {
      active_Perimeter = 2; 
      send_UDP("2,2,");
    }
    else {
      active_Perimeter = 1; //perimeter toggle
      send_UDP("2,1,");
    }
  }
  
  return reply;  
}


void setMenu() {
   
 String reply;
 String M_state = server.arg("MenuState"); 
 String A_state = server.arg("ArgState");
 //Serial.println( M_state);
 //Serial.println( A_state);
 reply = readMenu(M_state[0], A_state[0]) ; //action the menu
 webConsoleprint(reply, false);
 server.send(200, "text/plane", ""); //Send web page
}

void check_Scheduler(){
  if (!Sched_Active) { return;}
  
  int hr = Get_NTP_hr(); 
  
  if ((bitRead(system_current,23) == 0) && (hr == eeprom_settings[0]) ){
    bitSet(system_current,23);//auto active ON
    bitSet(system_current,12);//Auto mow ON
  }
  if ((bitRead(system_current,23) == 1) && (hr == eeprom_settings[1]) ){
    bitClear(system_current,23);//auto active OFF
    bitSet(system_current,14);//Auto home on
  }
}

void check_durations(){
  if (bitRead(system_current,12) == 1) Mow_Dur_mins++; //logs duration of mower
  if (bitRead(system_current,8) == 1) Charge_Dur_mins++; //logs duration of mower
}












































//
