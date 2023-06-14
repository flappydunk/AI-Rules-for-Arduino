

//Command list for all I2C
//Needs to fast acting and signals using various states
void read_I2C_Command(char reply[]){
  int my_Command;
  char segdata[32];
  char tempresult[6];//

  
  if (Rec_Data[0] != '\0'){
    //split off command from data
    get_comma(Rec_Data, 1,segdata );
    my_Command = atoi(segdata);
  }
  else{ return ;}

  
  //Default reply
  strcpy(reply,ACK);
  
  switch (my_Command) {
    case 0:
      strcpy(reply,request_data); //Slave wants to send data
      digitalWrite(I2C_1, LOW); //Reset message Line
      break;
    case 1: //perimeter ON
      if (active_Perimeter == 1) { //anything else default perimeter
        strcpy(reply,"1,1,1,") ;
      }
      else {
        strcpy(reply,"1,1,2,") ;
      }
      enableSender = true;
      
      break;
    case 2:  //All perimeters OFF also sets perimeter loop
      strcpy(reply,"1,0,") ;
      get_comma(Rec_Data, 2,segdata);
      if (atoi(segdata) == 1) { //sets perimeter loop
        active_Perimeter = 1; 
        strcat(reply,"1,");
      }
      else{
        active_Perimeter = 2;
        strcat(reply,"2,");
      }
      enableSender = false;     
      break; 
    case 3: //Charge ON (NOT USED)
      isCharging = true; //no actions associated with this except an LED
      break;
    case 4:  //Charge OFF
      isCharging = false;
      break; 
                
    case 99:
      get_comma(Rec_Data, 2,segdata);
      if (atoi(segdata) == 1) HardwareTest = true; else HardwareTest = false;
        test_counter = 0;
      break;
    case 100:
      strcpy(reply,"100,") ;
      dtostrf(periCurrentAvg, 4, 2,  tempresult);
      strcat(reply,tempresult) ;
      strcat(reply,",") ;
      if(enableSender) strcat(reply,"1,"); else strcat(reply,"0,");
      //provides current for charge and Perimeter
      break; 
     default:
      //Command Error
      strcpy(reply,NACK);
  } //end of switch
  
  int l = strlen(reply); 
  reply[l]= ETX; //remove string terminator and replace
  reply[l+1]='\0';
  
  return reply;
}
