

// ###########################   Define Rules Program ##############################

// Rules Based Leaf Collector monitor 
//{enabled,start value,command,timer,Flag_name[20]}
//command or timer = -1 = Not used
const FLAG  flag_arrayPROGMEM[] PROGMEM = {
{1,0,-1,-1,"Bumper\0"},
{1,0,-1,-1,"Stop\0"},
{1,0,-1,-1,"Docked\0"},
{1,0,-1,-1,"Batt_Poor\0"},
{1,0,-1,-1,"Rain\0"},
{1,0,-1,-1,"Go_Home\0"},
{1,0,-1,-1,"C_Relay\0"},
{1,0,-1,-1,"Temp_Sense\0"},
{1,0,-1,-1,"Fan\0"},
{1,0,-1,-1,"Humidity\0"},
{1,0,-1,-1,"Overheat\0"}
};

//{enabled,mask,action,output_0,output_1,rule_name[20]}
const RULE  rule_arrayPROGMEM[] PROGMEM = {
{1,0x23,0x1,0x0,0x2,"Bump_stop_1\0"},
{1,0x27,0x2,0x2,0x0,"Bumper_clear\0"},
{1,0x6,0x4,0x20,0x2,"Docked\0"},
{1,0x28,0x8,0x0,0x20,"Batt_Homing\0"},
{1,0x30,0x10,0x0,0x20,"Rain_Homing\0"},
{1,0x180,0x80,0x0,0x100,"Fan ON\0"},
{1,0x180,0x100,0x100,0x0,"Fan OFF\0"},
{1,0x420,0x400,0x0,0x20,"Heat_Homing\0"}
};

//{enabled,command_0,command_1,Command_name[20]}
const COMMAND command_arrayPROGMEM[]PROGMEM = {
{1,0x10,0x11,"Spare\0"}
};

//{enabled,Flag,timer_0,timer_1,timeout,multiplier}
//timeout should be set 0. This is used by the rules.
//multiplier is set to 0 = seconds, 1 = minutes
TIMER timer_array[] = {
{0,0,0,0,0,0}
}; 

// ###########################  End  #######################################
