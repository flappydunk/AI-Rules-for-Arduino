

void read_I2C_Command(char reply[]){
  int my_Command;
  int my_Action;
  char segdata[32];
  char tempresult[6];//
  if (strlen(Rec_Data) != 0){
    //split off command from data
    get_comma(Rec_Data, 1,segdata );
    my_Command = atoi(segdata);
  }
  else{ return;}

  //Command Error default
  strcpy(reply,NACK);
  strcat(reply,",");

  if (my_Command == 0){//Request for data
    

  }
  else if (my_Command == 10) { //set any flag
    get_comma(Rec_Data, 2,segdata );
    my_Action = atoi(segdata);
    bitSet(system_current,my_Action);
    strcpy(reply,ACK);
  }
  else if (my_Command == 20) { //reset any flag
    get_comma(Rec_Data, 2,segdata );
    my_Action = atoi(segdata);
    bitClear(system_current,my_Action);
    strcpy(reply,ACK);
  }
  else if (my_Command == 30) { //status message for ESP
    get_comma(Rec_Data, 2,segdata );
    my_Action = atoi(segdata);
    if (my_Action == 1) {rules_restart();}//restart
    if (my_Action == 2) {//get status
         //select flags for status
         if (bitRead(system_current,0) == 0){segdata[0] = '0';} else {segdata[0] = '1';} ; //Bumper
         segdata[1] = ',';
         if (bitRead(system_current,1) == 0){segdata[2] = '0';} else {segdata[2] = '1';} ; //stop
         segdata[3] = ',';
         if (bitRead(system_current,2) == 0){segdata[4] = '0';} else {segdata[4] = '1';} ; //Docked
         segdata[5] = ',';
         if (bitRead(system_current,3) == 0){segdata[6] = '0';} else {segdata[6] = '1';} ; //Batt_poor
         segdata[7] = ',';
         if (bitRead(system_current,4) == 0){segdata[8] = '0';} else {segdata[8] = '1';} ; //Rain
         segdata[9] = ',';
         if (bitRead(system_current,5) == 0){segdata[10] = '0';} else {segdata[10] = '1';} ; //Going Home
         segdata[11] = ',';
         if (bitRead(system_current,6) == 0){segdata[12] = '0';} else {segdata[12] = '1';} ; //Charge relay
         segdata[13] = ',';
         if (bitRead(system_current,10) == 0){segdata[14] = '0';} else {segdata[14] = '1';} ; //Over heat
         segdata[15] = ',';
         segdata[16] = '\0';
         dtostrf(batVoltage, 4, 2,  tempresult);
         strcat(segdata,tempresult) ;
         strcat(segdata,",") ;
         dtostrf(chgVoltage, 4, 2,  tempresult);
         strcat(segdata,tempresult) ;
         strcat(segdata,",") ;
         itoa(temp,tempresult,10);//base 10
         strcat(segdata,tempresult) ;
         strcat(segdata,",") ;
         strcat(segdata,"\0");
         strcpy(reply,segdata);
      }
       
      if (my_Action == 3) {//Power relay
         //Set a servo
        get_comma(Rec_Data, 3,segdata);
        if (atoi(segdata) == 0) {New_Power_Servo_State = 0;} else {New_Power_Servo_State = 1;}
        //Reply
        strcpy(reply,ACK);
      }
   }
   else if (my_Command == 40) { //Reset rules
      rules_restart();
   }
   
  //terminate array correctly to stop unwanted chars
  int l = strlen(reply); 
  reply[l]= ETX; //remove string terminator and replace
  reply[l+1]='\0';
  Rec_Data[0]='\0'; //Clear command
}


















//
