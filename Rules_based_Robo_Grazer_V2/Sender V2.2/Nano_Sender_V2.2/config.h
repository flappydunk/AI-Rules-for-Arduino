



//***********************************************
//        I2C Comms
//***********************************************
//const byte Master_Address = 10; //
const byte My_Address = 11; //Slave address arduino 001
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

//System monitor
int long LED_next_time;
int long LED_poll_time = 500; //mS

//Testing
boolean HardwareTest = false;
long int test_counter;

//############### Perimeter #############
int active_Perimeter = 1; //supports 2 perimeters P1 and P2
























//
