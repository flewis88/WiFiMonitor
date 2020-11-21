#include <WiFi.h>
#include <ESP32Ping.h>
#include <SSD1306Wire.h>

 
const char* ssid = "YOURSSID";
const char* password =  "YOURPASSWORD";

const char* ssid2 = "YOURSSID";
const char* password2 =  "YOURPASSWORD";

unsigned long oldTime = 0;
unsigned long newTime = 0;
unsigned long internetStateStart = 0;
unsigned long routerStateStart = 0;
unsigned long internetUpTime = 0;
unsigned long routerUpTime = 0;
unsigned long interval = 6000;
unsigned int internetDrops = 0;
unsigned int routerDrops = 0;
unsigned int wifiAttempts = 0;
bool internetNewState = 0;
bool internetOldState = 0;
bool routerNewState = 0;
bool routerOldState = 0;
String internetUpDisplay = "";
String routerUpDisplay = "";
String networkDrops = "";


IPAddress routerIP (192, 168, 1, 1); // The router IP to ping
String internetAddress = "www.google.com";

// OLED Display 128x64
SSD1306Wire  display(0x3c, 5, 4);

void dataRefresh() {
    long rssi = WiFi.RSSI();
    Serial.print("RSSI: ");
    Serial.println(rssi);
    String rssiStr=String(rssi);
    String networkDrops = "I:"+String(internetDrops)+" - R:"+String(routerDrops);

    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "SSID: ");
    display.drawString(0, 12, "RSSI: ");
    display.drawString(0, 24, "Internet: ");
    display.drawString(0, 36, "Router: ");
    display.drawString(0, 48, "Drops: ");
    display.drawString(45, 0, ssid);
    display.drawString(45, 12, rssiStr);
    display.drawString(45, 24, internetUpDisplay);
    display.drawString(45, 36, routerUpDisplay);
    display.drawString(45, 48, networkDrops);
    display.display();
  
    Serial.println(millis());
    oldTime = millis();
  
  bool internetPing = Ping.ping("www.google.com");
 
  if(internetPing){
    Serial.println("Google Ping");
    internetNewState=1;
    //internetUpTime++;
  }
  else{
    Serial.println("No Google Ping");
    internetNewState = 0;
    //internetUpTime = 0;
    //internetNewState=0;
  }

  if(internetNewState!=internetOldState){
    internetStateStart = millis();
  }

  bool routerPing = Ping.ping(routerIP, 3);
 
  if(routerPing){
    Serial.println("Router Ping");
    routerNewState=1;
    //routerUpTime++;
  }
  else{
    Serial.println("No Router Ping");
    routerNewState = 0;
    //internetNewState=0;
  }

    if(routerNewState!=routerOldState){
    routerStateStart = millis();
  }

  if(internetNewState==0 && internetOldState==1){
    internetDrops = internetDrops + 1;
  }

  if(routerNewState==0 && routerOldState==1){
    routerDrops = routerDrops + 1;
  }

  internetOldState = internetNewState;
  routerOldState = routerNewState;

  internetUpTime = millis() - internetStateStart;
  Serial.print("Internet State Time: ");
  Serial.println(internetUpTime);
  routerUpTime = millis() - routerStateStart;
  Serial.print("Router State Time: ");
  Serial.println(routerUpTime);

  internetUpDisplay = upTimeDisplay(internetUpTime, internetNewState);
  routerUpDisplay = upTimeDisplay(routerUpTime, routerNewState);
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "SSID: ");
  display.drawString(0, 12, "RSSI: ");
  display.drawString(0, 24, "Internet: ");
  display.drawString(0, 36, "Router: ");
  display.drawString(0, 48, "Drops: ");
  display.drawString(45, 0, ssid);
  display.drawString(45, 12, rssiStr);
  display.drawString(45, 24, internetUpDisplay);
  display.drawString(45, 36, routerUpDisplay);
  display.display();

}

String upTimeDisplay(unsigned long upTime, bool stateDisplay){

  Serial.print("upTime: ");
  Serial.println(upTime);
  unsigned long upTimeMins = upTime/60000;
  Serial.print("upTimeMins: ");
  Serial.println(upTimeMins);
  Serial.print("stateDisplay: ");
  Serial.println(stateDisplay);
  String displayString = "";
  int tempVal = 0;

  if (upTimeMins >= 60){ //Checks if over an hour
    tempVal = upTimeMins/60;
    if (tempVal >= 24){ //Checks if over a day
      tempVal = tempVal/24;
      if (tempVal >= 7){ //Checks if over a week
        tempVal = tempVal/7;
        if (stateDisplay == 0){
          displayString = "DOWN ";
        }
        else{
          displayString = "UP ";
        }
        displayString = displayString + String(tempVal);
        displayString = displayString + " wks";
        Serial.print("Time Display: ");
        Serial.println(displayString);
        return displayString;
      }
      else {
        if (stateDisplay == 0){
          displayString = "DOWN ";
        }
        else{
          displayString = "UP ";
        }
        displayString = displayString + String(tempVal);
        displayString = displayString + " days";
        Serial.print("Time Display: ");
        Serial.println(displayString);
        return displayString; 
      }
    }
    else{
      if (stateDisplay == 0){
          displayString = "DOWN ";
        }
        else{
          displayString = "UP ";
        }
        displayString = displayString + String(tempVal);
      displayString = displayString + " hrs";
      Serial.print("Time Display: ");
      Serial.println(displayString);
      return displayString;
    }
}
else{
    tempVal = upTimeMins;
    if (stateDisplay == 0){
          displayString = "DOWN ";
        }
        else{
          displayString = "UP ";
        }
        displayString = displayString + String(tempVal);
    displayString = displayString + " mins";
    Serial.print("Time Display: ");
    Serial.println(displayString);
    return displayString;
}


}
 
void setup() {
  Serial.begin(115200);
  
  // OLED display init
  display.init();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Connecting to WiFi...");
  display.display();
 
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  delay(1000);
  
  dataRefresh();
   

 
}
 
void loop() { 
	
  while (WiFi.status() != WL_CONNECTED) {
    if(wifiAttempts<10){
      WiFi.begin(ssid, password);
      ++wifiAttempts;
    }
    else if(wifiAttempts>=10 && wifiAttempts<20){
      WiFi.begin(ssid2, password2);
      ++wifiAttempts;
    }
    else{
      WiFi.begin(ssid, password);
      wifiAttempts=0;
    }

    delay(5000);
    Serial.println("Connecting to WiFi...");
    display.drawString(45, 0, "Connecting.");
    display.display();
  }

  //Serial.println("WiFi Connected");
  newTime = millis();

  if(newTime > oldTime+interval || newTime < oldTime){
    dataRefresh();
    
  }

}
