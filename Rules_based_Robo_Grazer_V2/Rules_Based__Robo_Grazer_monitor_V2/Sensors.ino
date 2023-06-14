

void Sensors_setup(){
  pinMode(Moisture, INPUT);
  pinMode(pinBumperLeft, INPUT_PULLUP);
  pinMode(pinBumperRight, INPUT_PULLUP);
  pinMode(Leaf, INPUT_PULLUP);
  pinMode(STOP, OUTPUT);
  pinMode(IRF520,OUTPUT);//output to Fan
  digitalWrite(STOP, HIGH);
}




//Battery Monitoring and Charge control




//Determine voltage divider total voltage (series connection R1-R2, U2 known voltage, determine voltage per adc (0 to 1023)
float voltageDividerUges(float R1, float R2, float U2){
  return (U2/R2 * (R1+R2));  // U2 is the measured value across the grounded resistor R2
}
// ADC-value to voltage. output the voltage detected reff to IOREF
float ADC2voltage(float ADCvalue){
  return (ADCvalue /1023.0 * IOREF);   // ADCman works @ 10 bit
}  

void Bat_setup(){
   pinMode(SEN_BAT_VOLTAGE, INPUT); //ADC Charge voltage
   pinMode(SEN_CHG_VOLTAGE, INPUT); //ADC Battery Charge voltage
   pinMode(pinChargeRelay, OUTPUT); //charge Relay

   batFactor                  = voltageDividerUges(102, 10, 1);   // ratio of voltage divider per volt adjust with actual voltage measuremnets
   batChgFactor               = voltageDividerUges(102, 10, 1);   // where R1/R2 = (V/U)-1
   //chgFactor                  = ADC2voltage(1)*10;        // ADC to charging current ampere factor  (see mower.h for macros) 

}

void  Read_Sensors(){
  //non urgent sensors
  
  if (Sensors_next_time < millis()){
    Read_Bat();
    Moisture_Sense();
    DHT11_Sense();
    Sensors_next_time = Sensors_poll_time + millis();
  }
}

void Read_Bat(){
  char command[32];//initial array;
  char tempresult[5]; //initial array;
  
  if (millis() >= nextTimeBattery){
    // read battery
    nextTimeBattery = millis() + 1000;  
    
    //if ((abs(chgCurrent) > 0.04) && (chgVoltage > 5)){
      // charging
      //batCapacity += (chgCurrent / 36.0);
    //}
    // convert to double  
    float batADC = ADC2voltage(analogRead(SEN_BAT_VOLTAGE));
    //int currentADC = readSensor(SEN_CHG_CURRENT);
    float chgADC = ADC2voltage(analogRead(SEN_CHG_VOLTAGE));    
    //Console.println(currentADC);
    batvolt = ((double)batADC) * batFactor;  //resistor dividers adjustment 
    //Report_batvolt = batvolt; //saves result for web
    chgvolt = ((double)chgADC) * batChgFactor;  // resistor dividers adjustment  
    //double curramp = ((double)currentADC) * chgFactor;  //    

       
    // low-pass filter
    double accel = 0.1; //was 0.01
    //double accel = 1.0;
    if (abs(batVoltage-batvolt)>5)   batVoltage = batvolt; else batVoltage = (1.0-accel) * batVoltage + accel * batvolt;
    if (abs(chgVoltage-chgvolt)>5)   chgVoltage = chgvolt; else chgVoltage = (1.0-accel) * chgVoltage + accel * chgvolt;
   // if (abs(chgCurrent-curramp)>0.5) chgCurrent = curramp; else chgCurrent = (1.0-accel) * chgCurrent + accel * curramp; 
   
    //Monitor Batt voltage while in use
    if (batVoltage < batGoHomeIfBelow ){
       bitSet(system_current,3);//Batt Poor
    }
 }
}


//  ############## Bumper  ########################
int Bumper_Sense() {
  //read bumpers
  int bumper=0;
  if (digitalRead(pinBumperRight) == LOW) {
    bumper=1;
    //Serial.println("bumper Right");
  }
  if (digitalRead(pinBumperLeft) == LOW) {
    bumper=2;
    //Serial.println("bumper Left");
  } 

  if (bumper == lastBumper){
    //Serial.println("=");
    lastDebounceTime = millis();
  }
  
  if (((millis() - lastDebounceTime) > debounceDelay) && (bumper != lastBumper)) {
          //Serial.print("S");
          //Serial.println(bumper);
          if (bumper == 0){bitClear(system_current,0); } else {bitSet(system_current,0); }
          lastBumper = bumper;
  }
  
}

//  ############## Docking  ########################
void Docked() {
  if (chgvolt > 5.0 ){
    bitSet(system_current,2);
  }
  else{
    bitClear(system_current,2);
  }
 
}


//  ############## Moisture  ########################
int Moisture_Sense() {
  int output_value = analogRead(Moisture);
  //Serial.println(output_value);
  output_value = map(output_value,1023,306,0,100);
  if (output_value > 100) output_value = 100;

   if (output_value > Rain_threshold){
      //Serial.println("RAIN");
      bitSet(system_current,4);
   }
   else{
      //Serial.println("No RAIN");
      bitClear(system_current,4);
   }
  
}


//  ############## DHT11  ########################
void DHT11_Sense() {
  
  int chk = DHT.read11(DHT11_PIN);
  if (DHTLIB_OK == chk){
    // DISPLAY DATA
    //Serial.print(DHT.humidity, 1);
    //Serial.print(",\t");
    //Serial.println(DHT.temperature, 1);

    temp = DHT.temperature;    
    int h = DHT.humidity;  

    //Serial.println(temp);
    if (temp > Temp_threshold_ON){
      bitSet(system_current,7);//Temp Threshold exceeded
    }
    if (temp < Temp_threshold_OFF){
      bitClear(system_current,7);//temp OK
    }
    if (temp < Temp_threshold_OVR){
      bitClear(system_current,10);//Overheating
    }
  }
  else{
    Serial.println("DHT11_error");
  }














  
  //Serial.println(output_value);
  //output_value = map(output_value,1023,306,0,100);
  //if (output_value > 100) output_value = 100;

  
  
}
























//
