//Routines to read and save settings

#include <EEPROM.h>

int eeprom_size = 5; //Node MCU max = 512


void eeprom_init(){
  //one off setup
  //EEPROM.begin(eeprom_size);  //Initialize EEPROM
  //delay(10);
}
 
void eeprom_Save(int addr , int value){
  if (addr > eeprom_size ) return;
    EEPROM.write(addr, value);
}

int eeprom_Read(int addr){
  return EEPROM.read(addr);
}

void eeprom_Save_ALL( int settings[], int arraySize){
  //stores the first value of the array in addr = 0
  int value;
  for(int addr=0;addr<arraySize;addr++) {
    value = settings[addr];
    EEPROM.write(addr, value); 
    //Serial.println(value); 
  }
  // written to flash
  //EEPROM.commit();
}

void eeprom_Read_ALL( int settings[], int arraySize){
  //stores the first value of the array in addr = 0
  for(int addr=0;addr<arraySize;addr++) {
    settings[addr] = EEPROM.read(addr);
    //Serial.println( settings[addr]);
  }
}
