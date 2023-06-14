
//  Set up names for ESP12E
//reserved addresses
//2C:F4:32:57:A8:06   192.168.0.20
//CC:50:E3:70:D7:49   192.168.0.21

String MyHostName = "ESP_Collector"; //Host name
//String myMac; //Mac address for this module 
//String MAC = "2C:F4:32:57:A8:06"; //one of the ESP12E modules

//unsigned int Remote_Port; //port of last received data
//String  Remote_IP;
#include <WiFiUdp.h>
char incomingPacket[255];  // buffer for incoming packets
WiFiUDP Udp_Rules;

//Ip data for the Perimeter Sender
unsigned int Udp_Perimeter_Port = 4210;
IPAddress Udp_Perimeter_IP_Addr(192, 168, 0, 20);


void UDP_setup(){
  //Serial.begin(115200);
  //if (MAC==myMac){MyHostName=MyHostName+"_1";}else{MyHostName=MyHostName+"_2";} //set host name for permanent ip address

  Udp_Rules.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}


void UDP_Rec_loop(){
  int packetSize = Udp_Rules.parsePacket();
  if (packetSize)  {
    // receive incoming UDP packets. store ip address if needed
    //Remote_IP = Udp.remoteIP().toString().c_str();
    //Remote_Port = Udp.remotePort();
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp_Rules.remoteIP().toString().c_str(), Udp_Rules.remotePort());
    
    int len = Udp_Rules.read(incomingPacket, 255);
    if (len > 0){
      incomingPacket[len] = 0; //last char 0 a String?
      Execute(incomingPacket);
    }
  }
}

boolean send_UDP(char sendPacket[]){
  //sends a udp message to Sender ONLY
  // send back a reply, to the IP address and port we got the packet from
       
    if (!Udp_Rules.beginPacket(Udp_Perimeter_IP_Addr, Udp_Perimeter_Port)){
       Serial.println(F("Failed begin packet")); 
    return false;
    }
    Udp_Rules.write(sendPacket);
    if(!Udp_Rules.endPacket()){
      Serial.println(F("Failed ending packet")); 
      return false;
    }
    
    //######################
  
    if (SerialData_A){
      Serial.print(F("UDP send: "));
      Serial.println(sendPacket);
    }
    //######################
    
    return true; 
}

void send_Response(String reply){
  //Send response from a received packet
  int str_len = reply.length()+1;
  char  replyPacket[str_len];
  reply.toCharArray(replyPacket,str_len); 
  // send back a reply, to the IP address and port we got the packet from
  Udp_Rules.beginPacket(Udp_Rules.remoteIP(), Udp_Rules.remotePort());
  Udp_Rules.write(replyPacket);
  Udp_Rules.endPacket();  
  if (SerialData_A){
    Serial.print(F("UDP send: "));
    Serial.println(replyPacket);
  }
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
  int my_command = get_comma(UDP_command, 1).toInt();//first field for command number
  //Serial.println(my_command);
  //sender
  if (my_command == 100){ //Command: Status from sender MCU
    //sender monitor
    Sender_UDP_OK = true;     
    if (get_comma(UDP_command, 2).toInt() ==1 ){//Sender Ready
      bitSet(system_current,6);
    }
    else{
      bitClear(system_current,6);
    }
    active_Perimeter = get_comma(UDP_command, 3).toInt();
    Sender_Current = get_comma(UDP_command, 4).toDouble(); //see notes in config re current measures
    if (Sender_Current < 0) Sender_Current = 0; 
    return true; 
  }

  if (my_command == 0){ //Command: Restart ESP
      ESP.restart();//does a complete reboot
  }


  
  //***********************************************
 //        Send Relay :UDP Commands to Slaves
 //***********************************************
 

  //Motor command relay
  if (my_command == 11){ //Echo Command: Motors set Flag ON
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to motors for action  
  }
  if (my_command == 12){ //Echo Command: Motors set Flag OFF
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to motors for action  
  }
  if (my_command == 13){ //Echo Commands: 3,1 = reset rules,3,2 = status
    int my_action = get_comma(UDP_command, 2).toInt();//2nd field for command number
    reply = Send_Command(Slave_1_Address,UDP_command);  //pass to motors for action 
    if(my_action == 2) {
      webConsoleprint(reply, false);
    }
  }
  //Monitor Command Relay
   if (my_command == 10){ //Echo Command: Motors set Flag ON
    reply = Send_Command(Slave_2_Address,UDP_command);  //pass to monitor for action  
  }
  if (my_command == 20){ //Echo Command: Motors set Flag OFF
    reply = Send_Command(Slave_2_Address,UDP_command);  //pass to monitor for action  
  }
  if (my_command == 30){ //Echo Commands: 
    int my_action = get_comma(UDP_command, 2).toInt();//2nd field for command number
    reply = Send_Command(Slave_2_Address,UDP_command);  //pass to motors for action 
    if(my_action == 2) {
      webConsoleprint(reply,false);
    }
  }

 
   
  //reply to sender
  if (reply != ""){
    send_Response(reply);
  }
  return true;
}

























 
