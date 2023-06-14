

// ###########################   Define Rules Program ##############################

// Rules based Leaf Collector motor
//{enabled,start value,command,timer,Flag_name[20]}
//command or timer = -1 = Not used
const FLAG  flag_arrayPROGMEM[] PROGMEM = {
{1,0,-1,-1,"Mow\0"},
{1,0,-1,-1,"Avoid_0\0"},
{1,0,-1,-1,"Avoid_1\0"},
{1,0,1,-1,"Fwd\0"},
{1,0,2,-1,"Bwd\0"},
{1,0,-1,0,"Bwd_Tim\0"},
{1,0,0,-1,"Turn\0"},
{1,0,-1,1,"Turn_Tim\0"},
{1,1,-1,-1,"Stop\0"},
{1,0,-1,2,"Stop_Tim\0"},
{1,0,-1,-1,"Go_Home\0"},
{1,0,-1,3,"Peri_Fnd_Tim\0"},
{1,1,-1,-1,"Peri_in\0"},
{1,0,-1,-1,"Peri_Trk\0"},
{1,0,-1,-1,"Peri_error\0"},
{1,0,-1,-1,"Docked\0"},
{1,0,-1,-1,"Stopped\0"},
{1,0,-1,-1,"Peri_Fnd\0"},
{1,0,-1,4,"Mow timer\0"},
{1,0,-1,5,"Home Timer\0"},
{1,0,3,-1,"Jam count max\0"},
{1,0,-1,-1,"Avoid\0"},
{1,0,-1,-1,"Mow_Motor\0"}
};

//{enabled,mask,action,output_0,output_1,rule_name[20]}
const RULE  rule_arrayPROGMEM[] PROGMEM = {
{1,0x1000E,0xE,0x58,0x300,"Avoid_stop_1\0"},
{1,0x10306,0x106,0x102,0x30,"Avoid_Bwd\0"},
{1,0x10036,0x14,0x10,0x300,"Avoid_stop_2\0"},
{1,0x10306,0x104,0x104,0xC2,"Avoid_Turn\0"},
{1,0x100C6,0x42,0x40,0x300,"Avoid_stop_3\0"},
{1,0x110306,0x102,0x2EA106,0x401009,"Avoid_Mow\0"},
{1,0x2BC00,0x1400,0x151,0x20808,"Peri_Fnd\0"},
{1,0x1C08,0xC08,0xA0008,0x2000,"Peri_Trk\0"},
{1,0x23C08,0x21408,0x401008,0x4100,"Peri_fail\0"},
{1,0x8100,0x8000,0x60245F,0x100,"Docked\0"},
{1,0x8001,0x8001,0x1D8058,0x6,"Home_Start\0"},
{1,0x11009,0x9,0x0,0x6,"Peri_out\0"},
{1,0x10100,0x10000,0x46245F,0x1100,"STOP\0"},
{1,0x409107,0x1101,0x1C0100,0x401008,"Ext_Start\0"},
{1,0x40009,0x40009,0x9,0x100,"Fail Bound\0"},
{1,0x82000,0x82000,0x2000,0x100,"Fail Home\0"},
{1,0x210009,0x200009,0x0,0x6,"Avoid\0"}
};

//{enabled,command_0,command_1,Command_name[20]}
const COMMAND command_arrayPROGMEM[]PROGMEM = {
{1,0x0,0x11," random dir\0"},
{1,0x0,0x21," Clear ticks\0"},
{1,0x0,0x31," Jam add\0"},
{1,0x41,0x0," Jam reset\0"}
};

//{enabled,Flag,timer_0,timer_1,timeout,multiplier}
//timeout should be set 0. This is used by the rules.
//multiplier is set to 0 = seconds, 1 = minutes
TIMER timer_array[] = {
{0,5,0,3,0,0},//bwd timer
{0,7,0,1,0,0},//turn timer
{0,9,0,1,0,0},//stop timer
{0,11,0,20,0,0},//perifnd timer
{0,18,20,0,0,0},//mow timer
{0,19,10,0,0,1}//Home timer
}; 

// ###########################  End  #######################################
