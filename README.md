# AI-Rules-for-Arduino
This project was designed to provide a Rules based control system. The example project demonstrates the method whereby a control system can be developed with little coding of the core process. The Rules system is flexible and will allow updates to the command and control more simply while you develop it. It Includes real time monitoring and allowing a complete view of your state of the system readily available. The system can be developed on a spreadsheet to generate HEX codes for the Rules that are added to the "Rules_prog.h" arrays. I have put together a demonstration of a simple rules based system to illustrate, see Rules-Demo branch. 

The Demo is shown using a Arduino and has been tested with a Nano, NodeMCU, ESP32. The code is designed to have as little impact as possible on memory space using PROGMEM. I expect if this method is useful that it can be developed using standard C class functions to create multiple instances of Rules if the processor is powerful enough. However I have not needed to do this even with a Nano it has provided surprisingly enough functionality within its memory footprint. 

Other projects will be added using this method. 
This project is a robotic lawnmower "Robo Grazer". A fully functional mower. Partly based on "Ardumower" hardware, motors, perimeter etc but with a completely different control system to enhance functionality and keep costs down.
Open the Robo Grazer.xlsx file to see the Rules and generation of the HEX codes that are copied to the Rules_Prog.h file.

Robo Grazer uses a NodeMCU for the Web interface, Arduino Nano for the mower control and Perimeter detection, and another Arduino Nano for the Monitoring of the battery, environment, bumpers, etc. These are interconnected using I2C. Each Arduino has its own Rules withe the NodeMCU handling the web interface and overview of Robo Grazer.
The perimeter Sender, based on Ardumower, has been adapted to handle multiple zones, and provide a UDP interface as well as being controlled by Robo Grazer. The Charging current data is derived from the the Sender and monitored by the NodeMCU.
