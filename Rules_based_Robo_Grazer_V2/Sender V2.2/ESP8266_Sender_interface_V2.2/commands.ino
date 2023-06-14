

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
