
// ------- baudrates---------------------------------
#define Serial_BAUDRATE    115200       // baudrate used for Serial


//***********************************************
//        I2C Comms
//***********************************************
const byte I2C_DEV_ADDR = 0x13;


//request I2C transfer pin (if used)
//this pin is used for a slave to signal the master that it wants to send data.
//if unused use 0xFF
#define I2C_PIN  0x07

//########## I2C Slave #########################
char Slave_Rec_Data[32]; //Global variable
char Slave_Send_Data[32];//Global variable
char ETX = 0x03; //End of text//Global variable
long int I2C_Req_timeout = 0; //tested for 5hrs OK
long int I2C_Req_time; //
long int I2C_start_time; //timer for checking responses from master
long int I2C_timeout = 1000;



//################   System monitor and sensor scan ########################
int long LED_next_time;
int long LED_poll_time = 1000; //mS

//################  eeprom ########################
//this is for those settings that can be changed and saved
int const eeprom_Settings_size = 8; //starts at 1 for size
int eeprom_settings[eeprom_Settings_size] = {0,0,0,0,0,0,0,0};


//***********************************************
//     Reset
//***********************************************
//int Reset = ; //= reset wire for nano and mpu disabled
int reset_counter;




//***********************************************
//        Motors
//***********************************************
int turndir;
#ifdef MCC
  //MA16 motors and MC3329 Controller Motor Pins
  // ------ pins definitions   (DO NOT CHANGE) ---------------------------------------
  #define pinMotorEnable  8         // EN motors enable
  #define pinMotorLeftPWM 5          // M1_IN1 left motor PWM pin
  #define pinMotorLeftDir 12         // M1_IN2 left motor Dir pin                                                         
  #define pinMotorRightPWM  6        // M2_IN1 right motor PWM pin
  #define pinMotorRightDir 4        // M2_IN2 right motor Dir pin
#endif

/*
#ifdef L298N
  //L298N Controller Motor Pins
  // ------ pins definitions   (DO NOT CHANGE) ---------------------------------------
  
  
  //Motor Pins
  #define enA  9 // PWM (D9) Left
  #define enB  10 // PWM (D10) Right
  #define in2 14 //Motor_L (A0)
  #define in1 15 //Motor_L (A1)
  #define in4 16 //Motor_R (A2)
  #define in3 17 //Motor_R (A3)

  //Collector and mower pins
  //to reduce the number of pins in1 & in3 are connected together, in2 & in4 are connected
  #define enC  11 // PWM (D11)Collector
  #define in5 7 // in1 & in3 on driver
  #define in6 8 // in2 & in4 on driver
#endif
*/

//mower motor
#define pinMotorMowPWM 9   

//STOP/sensor pin
#define pinSTOP A1 //External Signal to Stop the mower. Simulates a bumper

//Odometry
// ENCODER PINS
#define ENCODER_LEFT_PIN 2
#define ENCODER_RIGHT_PIN 3

// MEASUREMENTS
// The units for all measurements must be consistent. 
// You can use any length unit as desired.
#define RADIUS 34 // wheel radius in mm
#define LENGTH 235 // wheel base length in mm
#define TICKS_PER_REV 1910
double M_ticks = 0; //ticks for move
double m_to_t = 9830;//multiplier for ticks per metre

// Number of left and right tick counts on the encoder.
volatile unsigned int leftTicks, rightTicks;

//speeds
int Cur_ML, Cur_MR; //Target speed 0 to 255
double motorAccel = 0.5;
unsigned long lastSetMotorSpeedTime;//accel sample time
int motorLeftPWMCurr, motorRightPWMCurr;//actual current speed after acceleration
int motorMowPWMCurr;
int motorMowPWMMax = 200;
unsigned long nextTimeMotorMowControl;

//L298N
//int FWD_Def = 250; //default fwd. dont put it at 255
//int BWD_Def = -250; //default Bwd
//int FWD_L_Def = 150; //default Forward left turn
//int FWD_R_Def = 150; //default Forward right turn

//MCC329
int FWD_Def = 100; //default fwd. dont put it at 255
int BWD_Def = -100; //default Bwd
int FWD_L_Def = 150; //default Forward left turn
int FWD_R_Def = 150; //default Forward right turn


//***********************************************
//        Perimeter and tracking
//***********************************************

boolean perimeterUse = true;

boolean perimeterInside        = true;      // Local is inside perimeter?
boolean perimeterDir           = true;      // true = Clockwise
boolean perimeterLastState;

float perimeterPIDKp  = 16;       // perimeter PID controller
float perimeterPIDKi  = 8;
float perimeterPIDKd  = 0.8;
float perimeterPIDx_multiplier = 5; //default = 5
unsigned long perimeterLastTransitionTime;

//from ardumower
long int nextTimeMotorPerimeterControl;
int perimeterMagMaxValue  = 861;//measured threshold of the circuit. Used on perimeter
int MaxSpeedperiPwm       = 100;// was 100. speed max in PWM while perimeter tracking
unsigned long   trackingPerimeterTransitionTimeOut = 2500;   // was 1000,2500 , never<500 ms
char  trackingBlockInnerWheelWhilePerimeterStruggling = 1;//(advised)
int   leftSpeedperi;
int   rightSpeedperi;
unsigned long lastTimeForgetWire;
unsigned long trackingErrorTimeOut = 10000; // 0=disable
float PeriCoeffAccel ;

//Mag smoother
float KhsmoothMag;
double smoothMag_V1;
double smoothMag_V2;
double smoothMag_V3;

//jam counter
int jamCount = 0;
int jamCount_max = 5; 
unsigned long jam_time  = 30; //seconds
unsigned long jam_timeout;












//
