// ------- baudrates---------------------------------
#define CONSOLE_BAUDRATE    115200       // baudrate used for console


//************** Servo setup *********************************
Servo Servo1;

// Declare the Servo pin array

int sServoPins[] = {5,6}; //Pin servo is on
int sServo_ON = 0 ;//Angle for ON position
int sServo_OFF = 50 ; //Angle for OFF position
int const Max_Servo = 2;
bool sServo_Flags[Max_Servo] = {0,0}; //Flag showing ON = 1 or OFF = 0


//***********************************************
//        I2C Comms
//***********************************************
const byte My_Address = 0x11; //Slave address arduino 001
boolean data_Sent;
boolean I2C_active;
char Rec_Data[32];
char Send_Data[32];

char request_data[32]; //for command 0
char ETX = 0x03; //End of text
char ACK[] = "ACK";
char NACK[] = "NACK";
int my_Command;

int I2C_1 = 7 ;//request I2C transfer pin 7
long int I2C_timeout = 5000; //timeout for response
long int I2C_start_time; //timeout for response

long int I2C_Req_timeout = 30; //min delay between requests was 20
long int I2C_Req_time; //

//################   System monitor and sensor scan ########################
int long LED_next_time;
int long LED_poll_time = 1000; //mS

int long Sensors_next_time;
int long Sensors_poll_time = 1000; //mS
boolean run_rules = true;

//***********************************************
//        Battery Monitor and Charging
//***********************************************
//calibration tests measured with multimeter
//Measured from Web page
//First charge test flat battery = 1.47A. "Red" on PSU
//fully charged "Green" on PSU 0.32 Amps @ 27.47V


#define IOREF 4.5  // I/O reference measured voltage on 5v pin for Mega 2560/Nano
#define pinChargeRelay 3       //Pin D3 Charge Enable
#define SEN_CHG_VOLTAGE A0       //Pin A0 Charge Voltage
#define SEN_BAT_VOLTAGE A1       //Pin A1 Battery Voltage

unsigned long  nextTimeBattery = 0;
float batFactor       ;     // battery conversion factor
float batChgFactor ;     // battery conversion factor 
float chgvolt ;  // charge voltage (Volt)
float batvolt = 28;  // battery voltage (Volt)kh assign a temp value until refreshed by ADC
float chgVoltage; //smoothed value
float batVoltage; //smoothed value

float batGoHomeIfBelow = 23.2; // drive home voltage (Volt) KH was 23.7

float chgCurrent ;  // charge current  (Ampere)
//################  Servo status ########################
//Servos
char New_Servo_State[32];

//################  eeprom ########################
//this is for those settings that can be changed and saved
int const eeprom_Settings_size = 8; //starts at 1 for size
int eeprom_settings[eeprom_Settings_size] = {0,0,0,0,0,0,0,0};

//***********************************************
//        Bumpers
//***********************************************
#define pinBumperLeft 12       //Pin D12
#define pinBumperRight 11       //Pin D11
boolean Bumper_Actioned = false;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int lastBumper = 0;

//***********************************************
//        Emergency STOP
//***********************************************
#define STOP 10       //Pin D10


//***********************************************
//        Power Servo
//***********************************************
int New_Power_Servo_State = 0;
int Last_Power_Servo_State = 0;

//***********************************************
//        Leaf/Mower Servo
//***********************************************
int New_Leaf_Servo_State = 0;
int Last_Leaf_Servo_State = 0;
#define Leaf 4 //Leaf collector detector pin D4


//***********************************************
//        Moisture
//***********************************************
int Moisture = A2; //
int Rain_threshold = 2; //sensitivity of rain sensor. 
int long RAIN_next_time;
int long RAIN_poll_time = 1000; //mS


//***********************************************
//       Fan and temp and Humidity
//***********************************************
int IRF520 = 8; //Mosfet driver for Fan
int DHT11_PIN = 9; //temp and Humidity sensor
int Temp_threshold_ON = 25; //Fan ON temp. 
int Temp_threshold_OFF = 22; //Fan OFF temp. 
int Temp_threshold_OVR = 29; //Cooling failed. 
int temp;















//
