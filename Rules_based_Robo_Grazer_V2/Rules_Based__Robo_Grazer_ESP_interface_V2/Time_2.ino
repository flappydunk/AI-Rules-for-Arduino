#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
//#include <time.h>
#include <Timezone.h> 

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "uk.pool.ntp.org");

// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

// United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         // Standard Time
Timezone UK(BST, GMT);
time_t UK_Time;

static String getDateTimeStringByParams(tm *newtime, char* pattern = (char *)"%d/%m/%Y %H:%M:%S"){
    char buffer[30];
    strftime(buffer, 30, pattern, newtime);
    return buffer;
}

static tm getDateTimeByParams(long time){
    struct tm *newtime;
    const time_t tim = time;
    newtime = localtime(&tim);
    return *newtime;
}

//static String getEpochStringByParams(long time, char* pattern = (char *)"%d/%m/%Y %H:%M:%S"){
static String getEpochStringByParams(long time, char* pattern = (char *)"%d/%m/%Y %H:"){
//    struct tm *newtime;
    tm newtime;
    newtime = getDateTimeByParams(time);
    return getDateTimeStringByParams(&newtime, pattern);
}

void Time_Setup(){
  timeClient.begin();
  timeClient.setTimeOffset(0);//GMT
  WT_next_time =  millis();//refresh right away
}



void set_time(){
  if (millis() > secs_next_time){
    secs++;
    if (secs > 59) {
      secs = 0;
      mins++;
      check_Scheduler(); //checks the Hr
      check_durations(); //refresh every minute
    }
    if (mins > 59) {mins = 0;}
    String Smins;
    String Ssecs;

    if(secs < 10){Ssecs = "0" + String(secs);} else { Ssecs = String(secs);}
    if(mins < 10){Smins = "0" + String(mins);} else { Smins = String(mins);}

    current_time = getEpochStringByParams(UK_Time) + Smins + ":" + Ssecs;     
    //Serial.println(current_time);
    secs_next_time = 1000 + millis();//second count
  }
   
   
  if (millis() > WT_next_time){ //600 seconds
    if (timeClient.update()){
       //Serial.println ( "Adjust local clock" );
       unsigned long epoch = timeClient.getEpochTime();
       setTime(epoch);
       
       WT_next_time = WT_poll_time + millis();
    }
    else{
       //Serial.println ( "NTP Update not WORK!!" );
       WT_next_time = 1000 + millis();//retry
    }
    //Serial.println(getEpochStringByParams(UK.toLocal(now())));
    current_time = getEpochStringByParams(UK.toLocal(now())) + String(secs);
    UK_Time = UK.toLocal(now()); 
    mins = minute(UK_Time); 
    secs = second(UK_Time); 
     
  } 
}

int Get_NTP_hr(){ 
  return hour(UK_Time);
}
