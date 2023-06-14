// Include the required Wire library for I2C<br>#include <Wire.h>

//#include <Wire.h>


void I2C_setup() {
   // Start the I2C Bus 
  I2C_active = true;
  Wire.begin(My_Address); 
  // Attach a function to trigger when something is received or sent.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent); /* register request event */
  pinMode(I2C_1, OUTPUT); //ESP Flag for 12C coms
  Serial.print("My I2C DEC address = ");
  Serial.println(My_Address);
  I2C_active = false;
}

//function to be called when a slave device receives a transmission from a master
void receiveEvent(int howMany)
{
  if (I2C_active == false){
    I2C_active = true;
  }
  else{
    return false;
  }
  
  for (byte i=0;i<howMany;i++) {
    Rec_Data[i] = (char)Wire.read();
  }

 //Serial.print("Rec_Event : " );
 //Serial.println(Rec_Data);
 I2C_active = false;
}

// function that executes whenever data is requested from this device
void requestEvent() {
  if (I2C_active == false){
    I2C_active = true;
  }
  else{
    return false;
  }
 
  //Prepare response 
   if (Rec_Data[0] == '\0'){
      //receipt of command failed
      strcpy(Send_Data,NACK);
      strcat(Send_Data,",");
      strcat(Send_Data,ETX);
    }
    else{
      //Acknowledge receipt of data
      read_I2C_Command(Send_Data);
    }
     
   I2C_start_time = millis(); //reset clock
   //Serial.print("ReQ_Event : " );
   //Serial.println(Send_Data);
   Wire.write(Send_Data,sizeof(Send_Data));
   Send_Data[0]='\0';
   
   I2C_active = false;
   digitalWrite(I2C_1, LOW);//reset request
}



//used to signal ESP to retrive data from Slave
//see command "0"
void request_I2C(char my_data){
    if (digitalRead(I2C_1) == HIGH)return; //already sending one
    
    //if we need to send a message to the ESP
    //request_data = my_data + "," + ETX;
    strcpy(request_data,my_data);
    strcat(request_data,",") ;
    strcat(request_data,ETX) ;
    
    //report to ESP
    //Serial.print("Req_Data : " );
    //Serial.println(request_data);
    digitalWrite(I2C_1, HIGH); //tell ESP we have a message request
    I2C_start_time = millis();
}

void get_comma(char Comma_Data[], int find_pos, char Seg_Data[] ){ 
  //takes the char array finds the segment and stores in Seg_Data to return to the calling function.
  //test data
  //find_pos=2;
  //char Comma_Data[]={'1','0','0',',','2',','};

  int    ch = ','; //Find this char
  int start_pos = 0;
  int end_pos = 1;
  int i=0;
  int Seg_pos = 0;
  
  //find comma
  while (end_pos != find_pos && i < 32){
    //Finds commas
    if(Comma_Data[i] == ch ) {
      end_pos++; //Seg found
    } 
    i++;
  }
  //i = starting char of the segment
  while (Comma_Data[i] != ch && i < 32){
    Seg_Data[Seg_pos] = Comma_Data[i]; //first field starts without a comma
    i++;
    Seg_pos++;
  }
  //terminate segment
  Seg_Data[Seg_pos]='\0';
   
  
  //Serial.print("Seg ");
  //Serial.println(Seg_Data);
  
  
  }
