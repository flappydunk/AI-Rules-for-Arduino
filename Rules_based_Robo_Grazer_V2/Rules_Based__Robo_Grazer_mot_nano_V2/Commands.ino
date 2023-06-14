
void read_I2C_Command( char reply[]){  
   int my_Command;
  char segdata[32];
  char Rec_Data[32];
  int my_Action;
  char NACK[] = "NACK,";
  char ACK[] = "ACK,";
  
  if (!get_slaveStat()){return;}
  get_slaveRec(Rec_Data); //get the last data
  reset_slaveRec(); //free up I2C
  
  //default reply
  strcpy(reply,ACK);
  strcat(reply,'\0'); //terminate array
  
 //split off command from data from comma seperated array
 if (strlen(Rec_Data) != 0){
    //split off command from data
    get_comma(Rec_Data, 1,segdata );
    my_Command = atoi(segdata);
  }
  else{ 
    //Serial.println(F(" No Rec_Data"));
    strcpy(reply,NACK);
    strcat(reply,'\0'); //terminate array
    return;
 }


   
  //I2C Commands
  if (my_Command == 0){ //Request for data
    
  }
  else if (my_Command == 11) { //set any flag
    get_comma(Rec_Data, 2,segdata );
    my_Action = atoi(segdata);
    bitSet(system_current,my_Action);
  }
  else if (my_Command == 12) { //reset any flag
    get_comma(Rec_Data, 2,segdata );
    my_Action = atoi(segdata);
    bitClear(system_current,my_Action);
  }
  else if (my_Command == 13) { 
    get_comma(Rec_Data, 2,segdata );
    my_Action = atoi(segdata);
    if (my_Action == 1) {rules_restart();}//restart
    if (my_Action == 2) {//get status
       //select flags for status
       if (bitRead(system_current,0) == 0){segdata[0] = '0';} else {segdata[0] = '1';} ; //mowing
       segdata[1] = ',';
       if (bitRead(system_current,16) == 0){segdata[2] = '0';} else {segdata[2] = '1';} ; //stopped
       segdata[3] = ',';
       if (bitRead(system_current,10) == 0){segdata[4] = '0';} else {segdata[4] = '1';} ; //go home
       segdata[5] = ',';
       if (bitRead(system_current,15) == 0){segdata[6] = '0';} else {segdata[6] = '1';} ; //Docked
       segdata[7] = ',';
       if (bitRead(system_current,18) == 0){segdata[8] = '0';} else {segdata[8] = '1';} ; //mow timeout
       segdata[9] = ',';
       if (bitRead(system_current,19) == 0){segdata[10] = '0';} else {segdata[10] = '1';} ; //home timeout
       segdata[11] = ',';
       if (bitRead(system_current,20) == 0){segdata[12] = '0';} else {segdata[12] = '1';} ; //Jam counter
       segdata[13] = ',';
       segdata[14] = '\0';
       strcpy(reply,segdata);

    }
    if (my_Action == 3) {//Bump avoid
       avoid();
    }
  }
  else if (my_Command == 4) { //Reset rules
      rules_restart();
  }
 
  //terminate array correctly to stop unwanted chars
  int l = strlen(reply); 
  reply[l]= ETX; //remove string terminator and replace
  reply[l+1]='\0';
  Rec_Data[0]='\0'; //Clear command
}


void sync_eeprom_data(){
  //rules timers
  timer_array[0].time_out_1 = eeprom_settings[0];//
  timer_array[1].time_out_1 = eeprom_settings[2];//
}















//
