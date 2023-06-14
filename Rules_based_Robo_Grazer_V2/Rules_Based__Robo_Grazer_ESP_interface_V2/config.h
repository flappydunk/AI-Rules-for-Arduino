
//***********************************************
//        I2C Comms
//***********************************************
const byte My_Address = 10;
const byte Slave_1_Address = 0x13;//Slave_1 Motor
const byte Slave_2_Address = 0x11;//Slave_1 Monitor
//const byte Slave_3_Address = 0x13;//Slave_3 

String Send_Data ;
String Rec_Data ;
char ETX = 0x03; //End of text
String ACK = "ACK";
String NACK = "NACK";
String Slave_Data;
boolean I2C_active;
boolean Slave_data_active = false; //Retrieving data from slaves

//int I2C_1 = 12 ; //D6 on ESP data request Slave_1
//int I2C_2 = 13 ; //D7 on ESP data request Slave_2
//int I2C_3 = 14 ; //D5 on ESP data request Slave_3

boolean Status_Send = false; //echo to UDP client 

//Mode of test
//0 = Polling, 1 = Flag, 2 = I2C scanner
int Mode = 2;//scanner
boolean SerialData_A = false;

long int Polling_Next;
long int Polling_Duration = 1000;

String my_command;
String reply;

//***********************************************
//        UDP and coms
//***********************************************
#ifdef MCU
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>  
  #include <FS.h>
  ESP8266WebServer server(80); //Server on port 80

#endif

//#ifdef ESP32
  //#include <WiFi.h>
  //#include <WebServer.h>
  //WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)
  //#include "SPIFFS.h"
//#endif

const char* ssid = "MY_SSID";
const char* password = "MY_PWD";
unsigned int localUdpPort = 4210;  // local port to listen on for Perimeter Sender


int long UDP_next;
int long UDP_poll_time = 2000; //mS


//***********************************************
//     Web Console data
//***********************************************
//boolean webprint = true;
boolean WebMessage = true;
int const MaxWebStore = 40;
String WebMessageStore[MaxWebStore] = {"","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",""};

long int Mow_Dur_mins = 0;
long int Charge_Dur_mins = 0;
String batvolt;
String chgvolt;

boolean Send_Update = true; //send time field at boot
boolean Sched_Active;

//***********************************************
//     Web Time
//***********************************************
//this is for those settings that can be changed and saved
int const eeprom_size = 10; //Node MCU max = 512
int eeprom_settings[eeprom_size] = {0,0,0,0,0,0,0,0,0,0};  //Sched_ON_Time,Sched_OFF_Time,etc
int long WT_next_time;
int long WT_poll_time = 600000; //mS
String current_time;

int long secs_next_time;
int secs;
int mins;

//***********************************************
//       Sender Status
//***********************************************
int Peri_State;
double Sender_Current;
int active_Perimeter = 1; //multiple garden loops allowed, -1 = error no signal
boolean Sender_UDP_OK; //checks udp working
//***********************************************
//       web page LED colour Status
//***********************************************
#define NUM_LEDS 8
const char* LEDNames[] ={"STOPPED","DOCKED","BATT","CHARGING","GOING HOME","MOW","SHUTDOWN","PERI"};
char cLEDStatus[] = {'O','O','O','O','O','O','O','O'}; //default colours O = OFF
int Status_name;

String Mow_error = "OK";
int Mow_error_int = 0;
int Last_Mow_error_int = 0;

//***********************************************
//     Mowing timeouts
//***********************************************
String forward_count = "6" ; //a series of short forwards counts is an error
String perimeter_timeout = "40";//time duration for longest run
int UDP_fail_count_max = 3;
int UDP_fail_count;
//***********************************************
//     Battery
//***********************************************
float batFullCurrent   = 0.65;// (was 0.32 & 0.13) current flowing through sender, inc robot, when battery is fully charged  (amp),

//***********************************************
//     Temperature
//***********************************************
String temp; //from monitor







//
