AI Rules Base Programming With Arduino : Demo

This is a simple AI method of programming. Using a rules based system rather than using only procedural code. This is a powerful method of creating adaptable control systems. I have used it to make a robotic lawn mower and in progress is a pond environment controller using the same basic core code. Both have web interfaces and you can see real time rule execution of the code.

The rules data is placed in PROGMEM to reduce the impact on memory usage and has allowed me to develop a whole robotic lawnmower on a arduino Nano including perimeter detection.

This method increases the reliability and reuse of your code and improves debugging times. Its a good for prototyping a control system and allows it to be adapted without major recoding.

The rules code deals with the internal core program. This can be tested separately by manipulation of the Flags to simulate the peripherals. The whole program can be worked out on a spreadsheet before you start you code.

Peripherals such as read only sensors, servos, motors, I2C, LED's etc need a stub of code to read, activate and deactivate them. These stubs can form a library, if you wish, so you can pick which ones are needed depending on your implementation.

Therefore this project just deals with Rules setting up and how to do it. This enables you to work out and put together the program using the rules and testing them beforehand. No major procedural coding is needed for this part, just a spreadsheet to define your program. This will help you understand your application before you need to start coding.

So as an introduction to this method I will illustrate using a Arduino Nano to test a basic Demo program. This will be a temperature sensor that operates a a FAN and simulate a flashing LED with a ON period of 10 seconds and a OFF period of 5 seconds. Have a look at the spreadsheet and you will see a grid for 32 Flags and what each rule will do. Also you can assign the commands and timers needed. This is explained in more detail later.

Step 1: Lets Define the Flags for the Demo
The basis of the rules control is having a Flag(s) for each important state. To make things easy and more compact the flags are held in a 32bit unsigned integer (system_current) . So one integer indicates the full state of you program and each bit represents a Flag. So 32 Flags can be monitored. Each Flag can be assigned a name, and starting state. Also if a rule activates a Flag it can be associated with a command (to action something) and a timer (to change its state).



Think carefully what Flags you need, break down your functionality, keep it simple.

To illustrate I will use a simple example. Here we simulate a temperature sensor. When its temp threshold is reached it turns on a FAN and a flashing LED. When the temperature is below its temp threshold the FAN is turned OFF and the LED stops flashing.

Each element needs a flag assigned

Flags are:

bit 0 : OFF_Tim [The timer the LED should be OFF]

bit 1 : ON_Tim [The timer the LED should be ON]

bit 2 : LED [The Flag with a command to indicate the LED state]

bit 3 : Temperature Sensor [Read only]

bit 4 : Fan [ The FAN state 1= ON, 0 = OFF]

Step 2: Lets Define Rules
First, how does a rule work. All rules are examined sequentially by the program. Each rule takes the system_current integer and compares it to a mask integer. The mask defines which Flags to look at.

Each rule has a action integer. This tells it the state the Flag must be in before it can fire. Only when all the Flags have the correct state compared to the action integer can a rule fire. So it's essentially a 32 input AND gate that can have up to 32 outputs.

Once a rule fires it can change the state of any Flag. This is done using two integers. If the Flag(s) are set to 0 we use output_0, similarly for 1 we use output_1.

Rules should not fire continuously so usually the state of the Flags is altered by the rule to stop it firing repeatedly.

So continuing our simple case we need to define rules. Each of the integers is given a HEX number relating to the Flags.

LED_ON: mask 0x15, action 0x10, output_0 0x0, output_1 0x6

LED_OFF: mask 0x16, action 0x14, output_0 0x4, output_1 0x1

FAN_ON: mask 0x18, action 0x8, output_0 0x0, output_1 0x10

FAN_OFF: mask 0x18, action 0x10, output_0 0x14, output_1 0x0



So a command could be associated with the LED Flag to send a remote signal, or print a message, when the LED was ON or OFF. Also you could read a Flag directly and set a pin for the FAN. For this Demo we will use print statements to reduce complexity.

Step 3: Rules Spreadsheet
As the number of Flags increase so does complexity of the codes and rule interactions. Examining raw code can be tedious to sort out system states particularly when the system is doing many things. With this rules based system the full program is displayed on a spreadsheet making amendments to your functionality much simpler and defines fully all the parts of you program in one view.

Working out the HEX integers can be tedious so I use a spreadsheet. By setting the Flags states as needed it works out the HEX for you.

Some Flags could be set by an external means. e.g a sensor. These would be read only so I usually shade the cell these to indicate it is pointless for a rule to change its state.

See the attached file. I was not able to load a .xls file so I have included a .pdf to show you how the spreadsheet is created using a series of "=BIN2HEX(CONCATENATE($B7,$C7,$D7,$E7),1)" formulas to create the HEX codes you need.

Step 4: Implementing Your Rules on Arduino
The code doesn't need specifically need a Arduino as I expect it could be used on other C platforms. The Arduino makes in convenient to test and I have had considerable success using Nano, NodeMCU, ESP32 etc.

The Rules code consists of two files Rules.h and Rules_prog.h. The former is the library functionality for the Rules. The other file is where you enter your HEX codes from the spreadsheet as well as setting up your Flags, commands and timers. These files generate serial messages that give the status of the rules as they run.

Also I provide a basic Serial interface on the file Serial.ino. This can help to set or unset Flags to simulate sensors. At its most basic it provides a method to suspend or restart rules without needing to reboot. It also provides a method to read the current state of the Flags. So on the arduino Serial Interface:

//Basic Rules commands

Type 'c' for Current State

Type 'd' for rules to toggle display print statements

Type 's' to stop rules

Type 'r' to restart rules

//Commands to test your rules

Type '3' to toggle temperature sensor. [sets and clears bit 3]



What should happen:

Here the temperature sensor Flag (Bit 3) = 0 initially. If the Flag is set = 1that turns on the FAN and starts the Flashing sequence of the LED, ON for 10 seconds then OFF for 5 seconds.

When the temperature sensor Flag(Bit 3) = 0. The FAN is OFF and the LED stops flashing.



For the basic Arduino code is file Arduino_Rules_Demo.ino

This file has your fragments of code to interface your functions for your peripherals, communication, LEDs pins etc. This means your have to amend the code in the command_execute() function. For this basic Demo I am only using Serial.print() to display the results of the commands.

A note on commands. When a rule activates a command it passes a HEX integer to a FIFO stack. The command_execute() function pops the stack and uses the HEX code to run your specific function. Your have a HEX code when the Flag is set and another HEX code for when the Flag is clear.

A note on timers. Timers are internal to the rules. The timer has two timer options but not both together. A timer can be activated when the Flag goes to 1 or 0 for a duration specified in seconds or minutes.

So if a timer_1 duration > 0 is specified and Flag associated with a timer is set to 1. It will stay at 1 for the duration then be set to 0 by the timer.

Similarly timer_0 duration > 0 is specified and Flag associated with a timer is set to 0. It will stay at 0 for the duration then be set to 1 by the timer.

Step 5: Create Your Interfaces
Input Sensors:

As you can see inputs from sensors have to activate Flags. This being a logic system each Flag has only states 0 and 1. Depending on your control your have options when creating your stubs of code to handle the external interface. For example:

1) The sensor value reaches a critical value and switches the Flag state. Off course the sensor value can still be displayed or sent communicated somewhere if you need it.

2) if the sensor value requires more complex mapping e.g to change the colour of an LED depending on a series of value you could increase the number of Flags. 3 would allow 8 sensor levels which would allow rules to activate for each sensor level and set a Flag to action a command.

Output Command and reading Flags:

When a rule activates it examine each Flag it is allowed to set and tests to see if a command is associated with it. If it has a command it sends a HEX code you programmed to the execute stack depending if the Flag was set to 0 or 1.

The Arduino basic code runs command_execute() in the loop(). This tests the stack and if a HEX code is retrieved it is tested against the stubs of code you create. i.e

if (mycommand == 0x10) {Serial.println(F("CMD: LED OFF"));}

if (mycommand == 0x11) {Serial.println(F("CMD: LED ON"));}



Within the brackets {--- your code --- } or process to call to action the rule.



Monitoring flags:

More care is needed as the command_execute() is repeatedly called and can result in running your code multiple times. I have included a Last_system_current variable which is updated every time command_execute() is call to minimise this.

So to monitor a Flag on bit 4 (FAN) we can use this method for example:

//Fan interface

 if ((bitRead(system_current,4) == 1) && (bitRead(Last_system_current,4) == 0) ){

  //Set Pin HIGH

  Serial.println(F("FAN ON"));

 } 

 if ((bitRead(system_current,4) == 0) && (bitRead(Last_system_current,4) == 1) ){

  //Set Pin LOW

  Serial.println(F("FAN OFF"));

 }



