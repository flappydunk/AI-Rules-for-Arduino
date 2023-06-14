
//I2C Library of functions
//Design specifically with the ESP32 which does not support clock stretching
//Working with slow devices can cause errors.
#include "Wire.h"

//########## Slave #########################

boolean I2C_RecOverflow;//
boolean I2C_Rec;//
int I2C_Slave_pin = 0xFF; //if unused use 0xFF


void start_I2C_Slave(int Address){
  Wire.begin(Address);
  //Wire.setClock(400000);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  pinMode(I2C_Slave_pin, OUTPUT); //ESP Flag for 12C coms
  Serial.print("My I2C address = ");
  Serial.println(Address);
}

void onReceive(int len){
   if (Slave_Rec_Data[0] != '\0'){ I2C_RecOverflow = true;}
   
  //service new data
  byte c;
   byte i=0;
  while( Wire.available()){
    if (!I2C_RecOverflow){
      Slave_Rec_Data[i] = (char)Wire.read();
      i++;
    }
    else {
      //dump command Slave not ready
      c = (char)Wire.read();
    } 
  }
  Slave_Rec_Data[i] = '\0'; //terminate array
  
  //Serial.print("Rec: "); //dont leave in can cause errors
  //Serial.println(Slave_Rec_Data);
  char reply[32];
  read_I2C_Command(reply);
  
  I2C_RecOverflow = false;
  I2C_Rec = true;
}

// function that executes whenever data is requested from this device
void onRequest(){
  digitalWrite(I2C_Slave_pin, LOW); //tell ESP we have recieved it
  char Slave_Send_Data[32];

  read_I2C_Command(Slave_Send_Data);
  
  //Serial.print("Snd: "); //dont leave in can cause errors
  //Serial.println(Slave_Send_Data);

  if (Slave_Send_Data[0] == '\0'){
    Wire.write(ETX);  //Try again wait for Slave
    Serial.println("E");
  }
  else{
    //Wire.write(Slave_Send_Data.c_str());  //send request 
    Wire.write(Slave_Send_Data,sizeof(Slave_Send_Data));
    
    Slave_Send_Data[0] = '\0';
  }  
}

void reset_slaveRec(){
  Slave_Rec_Data[0] = '\0';
  I2C_Rec = false;
}

boolean get_slaveStat(){
  return I2C_Rec;
}

void get_slaveRec(char myData[]){
  strcpy(myData,Slave_Rec_Data);
  Slave_Rec_Data[0] = '\0';
}

void set_slaveSend(char myData[]){
  strcpy(Slave_Send_Data, myData);
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
