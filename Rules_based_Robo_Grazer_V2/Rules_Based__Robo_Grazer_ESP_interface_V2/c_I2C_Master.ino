

// Include the required Wire library for I2C<br>#include 

void I2C_setup() {   
  // test the connection to the I2C bus, sometimes it doesn't connect
    // keep trying to connect to I2C bus if we get an error
    boolean stop_I2C = false;
    boolean error_1 = true;
    //boolean error_2 = true;
    //boolean error_3 = true;
    int i=0;
    while ((error_1) && !stop_I2C){

        Wire.beginTransmission(Slave_1_Address);
        error_1 = Wire.endTransmission(); // if error = 0, we are properly connected
        delay(100);
        //error_1 = 0; 
        //Wire.beginTransmission(Slave_2_Address);
        //error_2 = Wire.endTransmission(); // if error = 0, we are properly connected
        //error_2 = 0; 
        //Wire.beginTransmission(Slave_3_Address);
        //error_3 = Wire.endTransmission(); // if error = 0, we are properly connected
        //error_3 = 0;
        if (error_1 != 0 ) { // if we aren't properly connected, try connecting again and loop
          
          if (error_1 != 0){Serial.print("I2C Error Slave_1 ,");}
          //if (error_2 != 0){Console.print("I2C Error Slave_2 ,");}
          //if (error_3 != 0){Console.print("I2C Error Slave_3 ,");}
          Serial.println("Not connected to I2C, trying again : ");
         
          if (i > 5){ //Re Try
            Serial.println("I2C Failed");
            stop_I2C = true;
            //Console.println(" Reset ALL ");
            //Execute("999,I2C failed,");
            //digitalWrite(Reset,LOW); //Hard Reset
            //delay(500); //delay 50mS
            //digitalWrite(Reset,HIGH); //no Reset
            //delay(500); //delay 50mS
            //ESP.restart();
          }

          delay(1000);
          Wire.begin(My_Address);
          i++;
          }
    }
    //Console.println("Properly connected to I2C");
    if (!stop_I2C) Serial.println("Properly connected to I2C") ;         
}

 

String Send_Command(int Address, String Send_Data){
  byte error;
  int Size = 30; //length of received packet
  if (Send_Data.length() > 31){
    Serial.println("Send length Error");
    return Send_Data + " Length Error";
  }

  if (SerialData_A){
     Serial.print("Snd: "); //bluetooth interface
     Serial.print(Address);
     Serial.print(" : ");
     Serial.println(Send_Data); //bluetooth interface
  }

  //Write data Slave
  Wire.beginTransmission(Address); // transmit to device
  Wire.write(Send_Data.c_str());
  error = Wire.endTransmission();// stop transmitting
  //if (error != 0){Serial.println("I2C Error " +String(Address) + " : " + Send_Data);}
  
  delay(1);  //https://github.com/arduino/Arduino/issues/1477  Timing bug
  
  //if (verboseI2C) Serial.println("T " + String(Address) + ": " + Send_Data);
  
  //Request response from Slave
  Wire.requestFrom(Address, Size); /* request & read data of size xx from slave */
 
  String Rec_Data = "";
  boolean ETX_found = false;
  char c;
  while( Wire.available()&& ETX_found == false){
    c =  (char)Wire.read();
    if ((c != ETX) & (ETX_found == false)){   //ETX 
      Rec_Data += c;
      //Console.println(Rec_Data);
    }
    else {
       ETX_found=true;
       //Console.println("ETX");
    }
  }
  //Console.println(Rec_Data);

   if (SerialData_A){
     Serial.print("Rec: "); //bluetooth interface
     Serial.println(Rec_Data); //bluetooth interface
  }
  //if (verboseI2C) Serial.println("F " + String(Address) + ": " + Rec_Data);
  return Rec_Data;
}

void I2CScanner(){
  byte error, address,data;
  int nDevices = 0;
 
  Serial.println("Scanning for I2C devices...");
  for(address = 1; address < 127; address++ )
  {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
   
      if (error == 0)
      {
        //Console.print("I2C device found at address 0x");
        if (address<16)
          nDevices++;
          switch (address){
            case 0x0B: Serial.println("0x0B: Nano_1 Monitor"); break;  
            case 0x0C: Serial.println("0x0C: Nano_2 Perimeter"); break;  
            case 0x0D: Serial.println("0x0D: Nano_3 Motors"); break; 
            case 0x0E: Serial.println("0x0E: ESP8266-01"); break; 
                    
          default: Serial.println("unknown module)");
        }
      }
      else if (error==4)
      {     
        Serial.print("Unknown error at address 0x");
        Serial.println(address,HEX);
      }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done");   
}



String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String get_comma(String Rec_Data, int position) {
  //get the required field off the string and returns it
  String Data;
  int len = Rec_Data.length();
  int Comma_pos_1 = -1;
  int Comma_pos_2 = 0;
  for (position; position > 0; position--) {
    Comma_pos_2 = Rec_Data.indexOf(",", Comma_pos_1 + 1);
    if (position > 1 ) { //this is the field we want
      Comma_pos_1 = Comma_pos_2;
    }
  }
  return Rec_Data.substring(Comma_pos_1 + 1, Comma_pos_2); //field data
}
