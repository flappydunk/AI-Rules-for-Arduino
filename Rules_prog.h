

// ###########################   Define Rules Program ##############################

// Demo 
//{enabled,start value,command,timer,Flag_name[20]}
//command or timer = -1 = Not used
const FLAG  flag_arrayPROGMEM[] PROGMEM = {
{1,0,-1,0,"OFF_Tim\0"},
{1,0,-1,1,"ON_Tim\0"},
{1,0,0,-1,"LED\0"},
{1,0,0,-1,"Temp_Sensor\0"},
{1,0,0,-1,"Fan\0"}
};

//{enabled,mask,action,output_0,output_1,rule_name[20]}
const RULE  rule_arrayPROGMEM[] PROGMEM = {
{1,0x15,0x10,0x0,0x6,"LED_ON\0"},
{1,0x16,0x14,0x4,0x1,"LED_OFF\0"},
{1,0x18,0x8,0x0,0x10,"Fan_ON\0"},
{1,0x18,0x10,0x14,0x0,"Fan_OFF\0"}
};

//{enabled,command_0,command_1,Command_name[20]}
const COMMAND command_arrayPROGMEM[]PROGMEM = {
{1,0x10,0x11,"LED\0"}
};

//{enabled,Flag,timer_0,timer_1,timeout,multiplier}
//timeout should be set 0. This is used by the rules.
//multiplier is set to 0 = seconds, 1 = minutes
TIMER timer_array[] = {
{1,0,0,5,0,0},//OFF_Tim on Flag = 1
{1,1,0,10,0,0}//ON_Tim on Flag = 1
}; 

// ###########################  End  #######################################
