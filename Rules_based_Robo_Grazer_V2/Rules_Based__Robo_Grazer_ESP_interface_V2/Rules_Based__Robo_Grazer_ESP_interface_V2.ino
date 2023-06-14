//rules based web interface to coordinate many other arduinos



//Select processor
#define MCU //NodeMCU
//#define ESP32 //ESP32 Do IT V1


#include "config.h"
#include <Wire.h>
#include "Rules.h"


void setup(){
  Serial.begin(115200);
  while (!Serial);
  Serial.flush();
  //pinMode(LED_BUILTIN, OUTPUT);//not
  ESP8266_setup();
  Time_Setup();//    
  eeprom_init(); // 
  eeprom_Read_ALL( eeprom_settings, eeprom_size); //retrieve settings
  I2C_setup(); 
  rules_setup(); 
  init_slave_rules();
  Serial.println(F("Programme : Rules Based Robo Grazer ESP_Interface_V2"));//This programme name
  
}



void loop(){
  //check console input
  readSerial();
  ESP8266_loop();
  set_time();
  
  
  rules_refresh_timers();//needed to ensure timers are managed
  int ruleResult = rules_run(current_rule);
  if (ruleResult > -1){
    display_current(); 
    memcpy_P( &rule_array, &rule_arrayPROGMEM[ruleResult], sizeof(RULE));
    displayweb_flags();
    webConsoleprint(String("Rule = ") + String(rule_array.rule_name), false);
    
  }
  command_execute(); 
  if (Polling_Next < millis()){
    //I2CScanner();   
    poll_I2C();

    Charge_monitor();
    Polling_Next = Polling_Duration + millis();
  } 
  //poll UDP for sender Data
  if ( millis() > UDP_next){
    if (!Sender_UDP_OK){
      UDP_fail_count++;
      if (UDP_fail_count > UDP_fail_count_max){//udp failure
        Mow_error_int = 6;
        bitClear(system_current,6);//Sender failed
      }
    }
    else{
      UDP_fail_count = 0;
    }  
      

    //display ANY errors
    if (Mow_error_int != Last_Mow_error_int){
      if (Mow_error_int == 0){
        Mow_error = String("OK");
      }
      else{
        Mow_error = String("ERR ");
        String Console_Mow_error = "ERR: ";
        Mow_error = Mow_error + String(Mow_error_int);
        
        if(Mow_error_int == 1){Console_Mow_error += "I2C fail Monitor";}
        else if(Mow_error_int == 2){Console_Mow_error += "I2C fail Motor";}
        else if(Mow_error_int == 3){Console_Mow_error += "mow_timeout";}
        else if(Mow_error_int == 4){Console_Mow_error += "Home_Timeout";}
        else if(Mow_error_int == 5){Console_Mow_error += "Jam";}
        else if(Mow_error_int == 6){Console_Mow_error += "udp failure";}
        else if(Mow_error_int == 7){
          Console_Mow_error += "Going Home: ";
          if(bitRead(system_current,1)==1){Console_Mow_error += "Rain";}
          if(bitRead(system_current,2)==1){Console_Mow_error += "Battery";}
          if(bitRead(system_current,24)==1){Console_Mow_error += "Over Heat";}
        }
        
        webConsoleprint(Console_Mow_error, true);
      }
      
  
      Last_Mow_error_int = Mow_error_int;
    }
    
    char send_UDP_Message[] = "100,"; //perimeter Status update
    send_UDP(send_UDP_Message); //sent right away to UDP
    Sender_UDP_OK = false;
    UDP_next = millis() +  UDP_poll_time; 
  }
}


void command_execute(){
 uint32_t mycommand = rules_pop();
 if (mycommand != -1){  
  //decode decide which proc to send command to 
  Serial.print("mycommand = ");
  Serial.println(mycommand,HEX);
  webConsoleprint(String("mycommand = ") + String(mycommand,HEX), false);
  if (mycommand == 0x11) {
    Send_Command(Slave_1_Address,String("11,10,"));  //Go Home Flag: pass to motor for action  
  }
  else if (mycommand == 0x21) {
    //start mow
    Mow_Dur_mins = 0;
    reply = Send_Command(Slave_1_Address,String("11,0,"));  //pass to mot for action 
  }  
  else if (mycommand == 0x31) {
    //mot stopped flag
    reply = Send_Command(Slave_1_Address,String("11,16,"));  //pass to mot for action  
  }      
  else if (mycommand == 0x41) {
    char send_UDP_Message[] = "1,1,"; //perimeter on , peri 1 selected  
    send_UDP(send_UDP_Message); //sent right away to UDP 
  }
  else if (mycommand == 0x40) {
    char send_UDP_Message[] = "2,1,"; //perimeter off, peri 1 selected   
    send_UDP(send_UDP_Message); //sent right away to UDP 
  }
  else if (mycommand == 0x51) {
    //Mon Charge relay ON
    Charge_Dur_mins = 0;
    reply = Send_Command(Slave_2_Address,String("10,6,"));  //pass to mon for action 
  }
  else if (mycommand == 0x50) {
    //Mon Charge relay OFF
    reply = Send_Command(Slave_2_Address,String("20,6,"));  //pass to mon for action 
  }
  else if (mycommand == 0x61) {
    //Mot Docked
    reply = Send_Command(Slave_1_Address,String("11,15,"));  //pass to mot for action 
  }
  else if (mycommand == 0x60) {
    //Mot not Docked
    reply = Send_Command(Slave_1_Address,String("12,15,"));  //pass to mot for action 
  }
  else if (mycommand == 0x70) {
    //Power servo OFF
    Power_Relay_Action(false); //power off
    delay(2000);
    reply = Send_Command(Slave_2_Address, "30,3,0,"); //power off);  //pass to mon for action 
    
    
  }
    
 }

 //Flag reads
 if (bitRead(system_current,2) == 1) {cLEDStatus[2] = 'R';} else {cLEDStatus[2] = 'G';} //Batt Poor
 if (bitRead(system_current,3) == 1) {cLEDStatus[1] = 'G';} else {cLEDStatus[1] = 'O';} //Docked
 if (bitRead(system_current,6) == 1) {cLEDStatus[7] = 'G';} else {cLEDStatus[7] = 'R';} //Sender Status
 if (bitRead(system_current,8) == 1) {cLEDStatus[3] = 'G';} else {cLEDStatus[3] = 'O';} //Charging
 if (bitRead(system_current,11) == 1){cLEDStatus[3] = 'Y';} //over charge

 //state
 //{"STOPPED","DOCKED","BATT","CHARGING","GOING HOME","MOW","SHUTDOWN","PERI"};
 if (bitRead(system_current,22) == 1) {Status_name = 6;} //Shutdown
 else if (bitRead(system_current,7) == 1) {Status_name = 4;} //Go Home
 else if (bitRead(system_current,8) == 1) {Status_name = 3;} //charging
 else if (bitRead(system_current,3) == 1) {Status_name = 1;} //Docked
 else if (bitRead(system_current,15) == 1) {Status_name = 5;} //mowing
 else if (bitRead(system_current,16) == 1) {Status_name = 0;} //stopped

 

}

void init_slave_rules(){
  //resets all rules
  Send_Command(Slave_2_Address,String("40,"));  //monitor
  Send_Command(Slave_1_Address,String("4,"));  //motor
  rules_restart();//esp
}


void displayweb_flags(){
  String logmessage;
  logmessage = "Flags Active:";
  for (int i=0; i < (unsigned int) max_flags ; i++){
      memcpy_P( &flag_array, &flag_arrayPROGMEM[i], sizeof( FLAG));
     if (bitRead(system_current,i) == 1){
        logmessage  +=  String(flag_array.flag_name);  
        logmessage  += String(", "); 
     }  
  } 
  logmessage  += String("\r\n");
  webConsoleprint(logmessage, false);
}





























//
