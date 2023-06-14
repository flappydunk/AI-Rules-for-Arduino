

// ###########################   Define Rules Program ##############################

// Rules based ESP for Robo Grazer 
//{enabled,start value,command,timer,Flag_name[20]}
//command or timer = -1 = Not used
const FLAG  flag_arrayPROGMEM[] PROGMEM = {
{1,0,-1,-1,"Mon_G0_Home\0"},
{1,0,-1,-1,"Mon_Rain\0"},
{1,0,-1,-1,"Mon_Batt_Poor\0"},
{1,0,-1,-1,"Mon_docked\0"},
{1,0,-1,-1,"Mon_Stop\0"},
{1,0,-1,-1,"Mon_Bump\0"},
{1,0,-1,-1,"Sender\0"},
{1,0,-1,-1,"Going Home\0"},
{1,0,4,-1,"Charge_relay\0"},
{1,0,-1,-1,"Charge_Thresh\0"},
{1,0,-1,0,"Charge_Timer\0"},
{1,0,-1,-1,"Overcharge\0"},
{1,0,1,-1,"Web_Mow\0"},
{1,0,2,-1,"Web_stop\0"},
{1,0,0,-1,"Web_Go_Home\0"},
{1,0,-1,-1,"Mot_Mowing\0"},
{1,0,-1,-1,"Mot_Stopped\0"},
{1,0,5,-1,"Mot_Dock\0"},
{1,0,3,1,"Sender_Tim\0"},
{1,0,-1,-1,"Ready\0"},
{1,0,-1,2,"Cmd_timer\0"},
{1,0,-1,3,"Min_Ch_tim\0"},
{1,0,6,-1,"Web_Shutdown\0"},
{1,0,-1,-1,"Auto_Mow\0"}
};


//{enabled,mask,action,output_0,output_1,rule_name[20]}
const RULE  rule_arrayPROGMEM[] PROGMEM = {
{1,0x1040C8,0x4040,0x23800,0x104000,"Web_Home\0"},
{1,0x149040,0x1040,0x86800,0x101000,"Web Mow\0"},
{1,0x1000C9,0x41,0x23800,0x104000,"Mon Homing\0"},
{1,0x112040,0x2040,0x5000,0x102000,"Web Stop\0"},
{1,0x121008,0x8,0xC4000,0x120000,"Docked\0"},
{1,0xA1908,0x20008,0x40000,0x200500,"Charge_ON\0"},//A0908,0x20008,0x40000,0x200500
{1,0x280304,0x300,0x500,0x80000,"Batt_Ready\0"},
{1,0xF00,0x100,0x100,0x80800,"Batt_Over\0"},
{1,0x41040,0x1000,0x0,0x40000,"Sender_On\0"},
{1,0x480008,0x480008,0x400000,0x0,"Shutdown\0"},
{1,0xA040,0x8000,0x5000,0x2000,"Sender Fail\0"}
};


//{enabled,command_0,command_1,Command_name[20]}
const COMMAND command_arrayPROGMEM[]PROGMEM = {
{1,0x0,0x11,"Mot_Go_Home\0"},
{1,0x0,0x21,"Mot_Mow\0"},
{1,0x0,0x31,"Mot_Stop\0"},
{1,0x40,0x41,"Sender on\0"},
{1,0x50,0x51,"Charge Relay \0"},
{1,0x60,0x61,"Mot_Dock\0"},
{1,0x70,0x0,"Power OFF\0"}
};


//{enabled,Flag,timer_0,timer_1,timeout,multiplier}
//timeout should be set 0. This is used by the rules.
//multiplier is set to 0 = seconds, 1 = minutes
TIMER timer_array[] = {
{1,10,0,180,0,1},//charge timeout
{1,18,0,10,0,0},//Sender timer
{1,20,0,1,0,0},//cmd timer
{1,21,0,10,0,0}//min charge timer
}; 


// ###########################  End  #######################################
