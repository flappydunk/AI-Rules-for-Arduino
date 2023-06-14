/*
 * ESP8266 NodeMCU AJAX Demo
 * Updates and Gets data from webpage without page refresh
 * https://circuits4you.com
 */



//#define LED 2  //On board LED

//SSID and Password of your WiFi router
//const char* ssid     = "NETGEAR_2GEXT";
//const char* password = "kindballoon175";









//==============================================================
//                  SETUP
//==============================================================
void ESP8266_setup(void){
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  //Onboard LED port Direction output
  //pinMode(LED,OUTPUT); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if ( WiFi.status() != WL_CONNECTED){
    Serial.println(F("WiFi Failed"));
    return;
  }

  //If connection successful show IP address in serial monitor
  Serial.println(F(""));
  Serial.print(F("Connected to "));
  Serial.println(ssid);
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  //LittleFS.begin();

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
  
  server.serveStatic("/", SPIFFS, "/Menu.html"); //initial page load
  server.on("/setSched", eepromData); //eeprom = scheduler
  server.on("/setMenu", setMenu); //buttons etc
  server.on("/console", handleMessage); //logging text box
  
  server.begin();                  //Start server
  Serial.println("HTTP server started");
  UDP_setup();//
}
//==============================================================
//                     LOOP
//==============================================================
void ESP8266_loop(void){
  server.handleClient();          //Handle client requests
  UDP_Rec_loop();
}


void webConsoleprint(String message, boolean timeStamp){

    //Web page text box
    String WebMessage;
    if (timeStamp){
      WebMessage = message + "\t" + current_time + "\r\n";
    }
    else{
      WebMessage = message + "\r\n";
    }
    Add_To_WebMessages( WebMessage );
    //Console.println(WebMessage);
}

void Add_To_WebMessages(String New_Message){
  //adds messages to the top and shift everything else down
  //make top space empty
  for (int i = MaxWebStore - 1; i > 0; i--){
    WebMessageStore[i] = WebMessageStore[i-1];  
  }
  //now add new one
  WebMessageStore[0] = New_Message;
  
  WebMessage = true;
}

void Clear_WebMessages(){
  //adds messages to the top and shift everything else down
  //make top space empty
  for (int i = MaxWebStore - 1; i > -1; i--){
    WebMessageStore[i] = "";  
  }
  webConsoleprint("Log Cleared",false);
}































//
