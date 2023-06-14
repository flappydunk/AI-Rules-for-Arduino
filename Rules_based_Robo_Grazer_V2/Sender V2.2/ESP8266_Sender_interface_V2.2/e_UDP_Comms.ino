#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "MY_SSID";
const char* password = "MY_PWD";

//  Set up names for ESP12E
//reserved addresses
//2C:F4:32:57:A8:06   192.168.0.20
//CC:50:E3:70:D7:49   192.168.0.21

String MyHostName = "ESP12E_Collector"; //Host name
String myMac; //Mac address for this module 
String MAC = "2C:F4:32:57:A8:06"; //one of the ESP12E modules

unsigned int localUdpPort = 4210;  // local port to listen on
unsigned int Remote_Port; //port of last received data
String  Remote_IP;

char incomingPacket[255];  // buffer for incoming packets
//char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back
WiFiUDP Udp;

void UDP_setup(){
  //Serial.begin(115200);
  if (MAC==myMac){MyHostName=MyHostName+"_1";}else{MyHostName=MyHostName+"_2";} //set host name for permanent ip address

  WiFi.hostname(MyHostName);
  Serial.print(MyHostName);
  Serial.printf( " connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}


void UDP_Rec_loop(){
  int packetSize = Udp.parsePacket();
  if (packetSize)  {
    // receive incoming UDP packets. store ip address if needed
    Remote_IP = Udp.remoteIP().toString().c_str();
    Remote_Port = Udp.remotePort();
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    
    int len = Udp.read(incomingPacket, 255);
    if (len > 0){
      incomingPacket[len] = 0; //last char 0 a String?
      Execute(incomingPacket);
    }
  }
}

void UDP_Send_loop(char replyPacket[]){
// send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();  
}


//***********************************************
 //        Command and Control
 //***********************************************

boolean Execute( String UDP_command){
  //takes the string decodes and execute command accordingly
  //Serial.print("UDP packet contents: ");
  //Serial.printf("Rec UDP: %s", UDP_command.c_str());

  if (SerialData_A){
    Serial.print(F("UDP rec: "));
    Serial.println(UDP_command);
  }
  
  //returns response for sending back
  String reply = ""; //the reply back to sender

  //Parse command and data
  String my_command = get_comma(UDP_command, 1);//first field for command number
  //Serial.println(my_command);
  //***********************************************
 //        Send :UDP Commands to Slaves
 //***********************************************
 if (my_command == "0"){ //Command: Restart ESP
      ESP.restart();//does a complete reboot
  }
  if (my_command == "1"){ //Command: Perimeter ON
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to sender for action 
    Peri_State = get_comma(reply, 2).toInt(); 
    active_Perimeter = get_comma(reply, 3).toInt();
  }
  if (my_command == "2"){ //Command: Perimeter OFF
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to sender for action
    Peri_State = get_comma(reply, 2).toInt();
    active_Perimeter = get_comma(reply, 3).toInt();   
  }
  if (my_command == "3"){ //Command: Spare
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to sender for action  
  }
  if (my_command == "4"){ //Command: Spare
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to sender for action  
  }
  //if (my_command == "9"){ //Command: Charge/Perimeter current
    //reply = Send_Command(Slave_1_Address,UDP_command);  //pass to sender for action
    //String SenderCurrent = get_comma(reply, 2).toInt();   
  //}
 
    
  if (my_command == "94"){ //Command: Toggle Send UDP unsolicited data
    if(UDP_Send == false){UDP_Send = true;reply = "94,ON,";}else{UDP_Send = false;reply = "94,OFF,";}
  }
  if (my_command == "100"){
    //Build Status
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to sender for action
    String SenderCurrent = get_comma(reply, 2);  
    Peri_State = get_comma(reply, 3).toInt(); //MCU peri state should be the same as enablesender, if not sender error
    //System State
    reply = String("100,") + String(Peri_State) + "," + String(active_Perimeter) +"," + SenderCurrent + "," + ETX;  
  }
      
 
  //reply to sender
  if (reply != ""){
    send_Response(reply);
  }
  return true;
}

void send_Response(String reply){
  //Send response
  int str_len = reply.length()+1;
  char  replyPacket[str_len];
  reply.toCharArray(replyPacket,str_len); 
  UDP_Send_loop(replyPacket);
  if (SerialData_A){
    Serial.print(F("UDP send: "));
    Serial.println(replyPacket);
  }
  
}























 
