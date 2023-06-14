

//Command list for all I2C
//Needs to fast acting and signals using various states
void read_I2C_Command( String Reply_Data){
  String sub_Command;
  int my_Command;
  String reply;
  
  if ( Reply_Data != ""){
    //split off command from data
    my_Command = get_comma( Reply_Data, 1).toInt();
  }
  else{ return;}
  //default reply
  reply =  Reply_Data + ACK + "," + ETX;
  switch (my_Command) {
    case 1: //Adhoc Status Message
      //Serial.println(sub_Command);
    break;
    default:
      //Command Error
      Serial.println("Command Error = " + String(my_Command));
  } //end of switch
  
}


void poll_I2C(){
  //get updates from slaves
  String I2C_command = "30,2,"; //Monitor status
  String Reply_Data;
  Reply_Data = Send_Command(Slave_2_Address,I2C_command);  // 
  Mow_error_int = 0;
  if (Reply_Data != ""){
    //Serial.println(Reply_Data);
    if(get_comma( Reply_Data, 1).toInt() == 1){bitSet(system_current,5);} else{bitClear(system_current,5);} //Bump
    if(get_comma( Reply_Data, 2).toInt() == 1){bitSet(system_current,4);} else{bitClear(system_current,4);} //Mon_Stop
    if(get_comma( Reply_Data, 3).toInt() == 1){bitSet(system_current,3);} else{bitClear(system_current,3);} //Docked
    if(get_comma( Reply_Data, 4).toInt() == 1){bitSet(system_current,2);} else{bitClear(system_current,2);} //Mon Batt_Poor
    if(get_comma( Reply_Data, 5).toInt() == 1){bitSet(system_current,1);} else{bitClear(system_current,1);} //Rain
    if(get_comma( Reply_Data, 6).toInt() == 1){bitSet(system_current,0);} else{bitClear(system_current,0);} //Mon Go Home
    if(get_comma( Reply_Data, 7).toInt() == 1){bitSet(system_current,8);} else{bitClear(system_current,8);} //Charge_Relay
    if(get_comma( Reply_Data, 8).toInt() == 1){bitSet(system_current,24);} else{bitClear(system_current,24);} //Overheat
    batvolt = get_comma(Reply_Data, 9);  //update global variable for web page
    chgvolt = get_comma(Reply_Data, 10);  //update global variable for web page 
    temp = get_comma(Reply_Data, 11);  //update global variable for web page 
  }
  else{
    Mow_error_int = 1;//I2C fail Monitor
  }
  //get updates from slaves
  I2C_command = "13,2,"; //Motor status
  Reply_Data = Send_Command(Slave_1_Address,I2C_command);  //pass to motors for action 
  if (Reply_Data != ""){
    if(get_comma( Reply_Data, 1).toInt() == 1){
      bitSet(system_current,15);//Mowing
      //running checks
      if(get_comma( Reply_Data, 5).toInt() == 1){Mow_error_int = 3;} //mow_timeout
      if(get_comma( Reply_Data, 7).toInt() == 1){Mow_error_int = 5;} //Jam
    } else{
      bitClear(system_current,15);
    } 
    if(get_comma( Reply_Data, 2).toInt() == 1){bitSet(system_current,16);} else{bitClear(system_current,16);} //Stopped   
    if(get_comma( Reply_Data, 4).toInt() == 1){bitSet(system_current,17);} else{bitClear(system_current,17);}///Docked
    if(get_comma( Reply_Data, 3).toInt() == 1){
      bitSet(system_current,7);//going home
      Mow_error_int = 7;//going home 
      if(get_comma( Reply_Data, 6).toInt() == 1){Mow_error_int = 4;} //Home_Timeout
    } else{
      bitClear(system_current,7);
    } 
  }
  else{
     Mow_error_int = 2;//I2C fail Motor
  }

  
}






























//
