
//***********************************************
//        I2C Comms
//***********************************************
const byte My_Address = 10;
const byte Slave_1_Address = 11;//Slave_1 ESP8266_01
//const byte Slave_2_Address = 12;//Slave_2 Perimeter
//const byte Slave_3_Address = 13;//Slave_3 Motors

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
boolean SerialData_A = true;

long int Polling_Next;
long int Polling_Duration = 1000;

String my_command;
String reply;

//***********************************************
//        UDP
//***********************************************
boolean UDP_Send = false;
String UDP_message;
//int long UDP_next;
//int long UDP_poll_time = 2000; //mS
//***********************************************
//       Status
//***********************************************
int Peri_State;
int active_Perimeter = 1;; //choice of two loops
