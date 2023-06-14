

//Rules V2 3/6/2023 Karl Humphrey


 //structure for each rule
struct RULE{
  int enabled; //1=enabled 0  = disabled
  uint32_t mask;
  uint32_t action; //actions 
  uint32_t output_0; //flags set to 0
  uint32_t output_1; //flags set to 1
  char rule_name[20];
};

//timers linked to a bit
struct TIMER{
  int enabled;
  int flag_pos;
  long int time_out_0; //this time if flag = 0
  long int time_out_1; //this time if flag = 1
  long int timeout; 
  int multiplier; //0 = seconds, 1 = minutes
};

//Flag name
struct FLAG{
  int enabled;
  int start_value; 
  int command;  
  int timer; 
  char flag_name[20];   
};
struct COMMAND{
  int enabled;  
  uint32_t command_0; //command  when flag = 0
  uint32_t command_1; //command  when flag = 1
  char command_name[20];
};



FLAG flag_array;
RULE  rule_array;
COMMAND command_array;
#include "Rules_prog.h"


// ############  Globals ###########################
//command Stack
int push( uint32_t value);
uint32_t pop();
int top = 0;
int bot = 0;
int stack_max = 5;
uint32_t stack[5];


//Rules
int current_rule; 
uint32_t system_current;
uint32_t Last_system_current;
boolean Run_Rules = true;
boolean Display = true;

//timers
long int min_time_check = 999999;
long int Next_min_time_check;

//arrays
unsigned int max_flags,max_rules,max_commands,max_timers;

// ############  Startup ###########################


void rules_setup(){
  //calculate size of arrays
  max_flags = sizeof(flag_arrayPROGMEM)/sizeof(flag_arrayPROGMEM[0]);
  max_rules = sizeof(rule_arrayPROGMEM)/sizeof(rule_arrayPROGMEM[0]);
  max_commands = sizeof(command_arrayPROGMEM)/sizeof(command_arrayPROGMEM[0]);
  max_timers = sizeof(timer_array)/sizeof(timer_array[0]);
  if (Display){
    Serial.println("Rules Config");
    Serial.print("Flags = ");
    Serial.println(max_flags);
    Serial.print("Rules = ");
    Serial.println(max_rules);
    Serial.print("Commands = ");
    Serial.println(max_commands);
    Serial.print("Timers = ");
    Serial.println(max_timers);
  }
  
  
   int i;
  //timers init refresh rate
 for (i=0; i <  (unsigned int)max_timers ; i++){
   if (timer_array[i].time_out_1 <  min_time_check) {min_time_check = timer_array[i].time_out_1;}
  if (timer_array[i].time_out_0 <  min_time_check) {min_time_check = timer_array[i].time_out_0;}  
 }
  


 //Flag init
  char thisname[20];
  for (i=0; i <  (unsigned int)max_flags ; i++){ 
    memcpy_P( &flag_array, &flag_arrayPROGMEM[i], sizeof(FLAG));
    if (flag_array.enabled == 1){
      if (flag_array.start_value == 1){
         bitSet(system_current,i);
      }
      else{
         bitClear(system_current,i);
      }    
    }
    
  }
  current_rule = 0; //start rules
}




// ############  timers ###########################

void rules_refresh_timers() {
 if (Next_min_time_check > millis()) {return;}
    
   for (int t=0; t <  (unsigned int)max_timers ; t++){      
    if ((timer_array[t].enabled == 1)  && (timer_array[t].timeout < millis()) && (timer_array[t].timeout > 0)){
      
      if (bitRead(system_current,timer_array[t].flag_pos) == 1){
         bitClear(system_current,timer_array[t].flag_pos);  //time completed = 0 
         timer_array[t].enabled = 0;       
      }
      else{
         bitSet(system_current,timer_array[t].flag_pos);  //time completed = 1 
         timer_array[t].enabled = 0;
      }
    }
  }
  Next_min_time_check = min_time_check + millis();
}

void del_timer(int pos){ //just disables it
  timer_array[pos].enabled = 0;
}

void rules_del_timer(int pos){ //just disables it
  timer_array[pos].enabled = 0;
}

void rules_start_timer(int timer,int enabled, int flag){
  long int multiplier;
  if (timer_array[timer].multiplier == 0){ 
    multiplier = 1000; //second
  }
  else {
    multiplier = 60000;//minute
  }
  
  if (flag == 1){
    if (timer_array[timer].time_out_1 > 0){
      timer_array[timer].timeout = (timer_array[timer].time_out_1 * multiplier) + millis();
    }
    else{
      timer_array[timer].timeout = 0;
    }
          
  }
  else{
    if (timer_array[timer].time_out_0 > 0){
      timer_array[timer].timeout = (timer_array[timer].time_out_0 * multiplier) + millis();
    }
    else{
      timer_array[timer].timeout = 0;
    }
  }
  
  if (timer_array[timer].timeout == 0){
    //no timeout just reseting the flag
    timer_array[timer].enabled = 0; 
  }  
  else{
    //start the clock
    timer_array[timer].enabled = 1;
  }
  
    
  
  Next_min_time_check = min_time_check + millis();
}

// ############  Command Stack ###########################

int rules_push( uint32_t value){
  if (bot == 0){
    if (stack_max == top) return -1 ; //full
  }
  if (bot - 1 == top) return -1; //full

  //ok to add
  
  top++;
  if (top > stack_max - 1) top = 0;
  //place it in stack
  stack[top] = value;
  return top;
}

uint32_t rules_pop(){
  if (bot == top) return -1; //empty
  bot++;
  if (bot > stack_max - 1) bot = 0;
  uint32_t value = stack[bot];
  return value;
}

// #######################   Display   ##############################
void display_system_current(char mask[]){
  for (int i = (unsigned int)max_flags+1; i > 0 ; i--){
    if (bitRead(system_current,i-1) == 0) {mask[(unsigned int)max_flags - i] = '0';} else { mask[(unsigned int)max_flags - i] = '1';} //you have to revearse the bits for display
  }
  mask[(unsigned int)max_flags] = '\0';
}

void display_current(){
  char logMessage[(unsigned int)max_flags+1];
  Serial.flush();
  if (Display){Serial.println("Flags Active:");}
  for (int i=0; i < (unsigned int) max_flags ; i++){
      memcpy_P( &flag_array, &flag_arrayPROGMEM[i], sizeof( FLAG));
     if (bitRead(system_current,i) == 1){
       if (Display){
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(flag_array.flag_name);
       }          
     }  
  }   
 
  
   if (Display){
    Serial.print(F("Current State [ HEX = "));
    Serial.print(system_current,HEX);
    Serial.print(" ] BITS = ");
    display_system_current(logMessage);
    Serial.println(logMessage);
   }
}


// ############  Rules ###########################


int rules_run(int pos) { 
  if (!Run_Rules) { return -1;}
  
  int rule_ran = -1; //used to see if the rule was activated
  memcpy_P( &rule_array, &rule_arrayPROGMEM[pos], sizeof(RULE));

  int enabled = rule_array.enabled;
  if (enabled == 1){
    uint32_t s = system_current; //all bits current value in the system
    uint32_t r = rule_array.mask; //specific bits used to test of the system_current
    uint32_t a = rule_array.action; //State of the mask bits to compare with the current
    uint32_t out_0 = rule_array.output_0; //if all actions are true set these bits
    uint32_t out_1 = rule_array.output_1; //if all actions are true set these bits


    //test bit pattern matches rule
    int i =0;
     for (i=0; i < (unsigned int) max_flags ; i++){
      if (bitRead(r,i) == 1){ //the rule mask wants this bit
        if (bitRead(s,i) != bitRead(a,i)){ //if its not equal
           //next rule
          current_rule++; //next
          if ((unsigned int)max_rules - 1 < current_rule){
            current_rule = 0;
          }
          return rule_ran; //stop the rule
        }
      }
     }
  
    //set flags check if command and timer associated
    int timer;
    int command;
    if (Display){
      Serial.println();
      Serial.print("Rule ");
      Serial.print(pos);
      Serial.print(": ");
      Serial.println(rule_array.rule_name);
    }
    for (i=0; i < (unsigned int) max_flags ; i++){
      memcpy_P( &flag_array, &flag_arrayPROGMEM[i], sizeof( FLAG));
      
      if (bitRead(out_1,i) == 1){ //the output rule mask wants this bit
        bitSet(s,i);
        //start timer if needed
        timer = flag_array.timer;
        if (timer > -1){
          rules_start_timer(timer,1,1);
        }
         if (Display){
          Serial.print("F1 ");
          Serial.print(i);
          Serial.print(" = ");
          Serial.println(flag_array.flag_name);
        }
        //add command to stack if needed
        command = flag_array.command;
        if (command > -1){
            memcpy_P( &command_array, &command_arrayPROGMEM[command], sizeof(COMMAND));
           if (command_array.enabled == 1  &&  command_array.command_1 > 0){
             rules_push(command_array.command_1);
             if (Display){
               Serial.print("C1 ");
               Serial.print(command);
               Serial.print(" = ");
               Serial.print(command_array.command_1,HEX);
               Serial.print(" : ");
               Serial.println(command_array.command_name);
             }
           }
           
        }
      }
      if (bitRead(out_0,i) == 1){ //the output rule mask wants this bit
        bitClear(s,i);
         //start timer if needed
          timer = flag_array.timer;
          if (timer > -1){
            rules_start_timer(timer,1,0);
          }
          if (Display){
            Serial.print("F0 ");
            Serial.print(i);
            Serial.print(" = ");
            Serial.println(flag_array.flag_name);
          }
          //add command to stack if needed
          command = flag_array.command;
          if (command > -1){
              memcpy_P( &command_array, &command_arrayPROGMEM[command], sizeof(COMMAND));
              if (command_array.enabled == 1 &&  command_array.command_0 > 0){
               rules_push(command_array.command_0);
                if (Display){
                 Serial.print("C0 ");
                 Serial.print(command);
                 Serial.print(" = ");
                 Serial.print(command_array.command_0,HEX);
                 Serial.print(" : ");
                 Serial.println(command_array.command_name);
                }
              }
          }
      }
    }
    system_current = s; 
    rule_ran = current_rule;
  }
  //next rule
  current_rule++; //next
  if ((unsigned int)max_rules - 1 < current_rule){
    current_rule = 0;
  }
  
  return  rule_ran;
}

void rules_refresh() { 
  rules_refresh_timers();//needed to ensure timers are managed
  int ruleResult = rules_run(current_rule);    
  if (ruleResult > -1){
    display_current();     
  } 
}

void rules_restart() {
  //Redo flags reset to start position
  int i; 
  if (Display){
    Serial.println("Restart");
  }
               
  for (i=0; i < (unsigned int) max_flags ; i++){ 
    memcpy_P( &flag_array, &flag_arrayPROGMEM[i], sizeof( FLAG));
    
    if (flag_array.enabled == 1 && flag_array.start_value == 1){
      bitSet(system_current,i);
    }
    else{
      bitClear(system_current,i);
    }
  }
}

















































    //
