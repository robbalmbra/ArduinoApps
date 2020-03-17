#include "ESP8266WiFi.h"

#define DEBUG

int connect(){
  int count = 0;
  int ret = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    // Break if connection is too long
    if(count >30){ ret=1; break; }
      #ifdef DEBUG
      Serial.print(".");
      #endif
    count++;
  }

  if(!ret){
    delay(1000);
  
    // Retrieve IP after connection
    #ifdef DEBUG
    Serial.print("\nIP address: ");
    Serial.print(WiFi.localIP());
    #endif
  }
  return ret;
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  char prevmac[18];
  int previd = -1;

  // flush prevmac
  strcpy(prevmac,"");

  while(1){
    int networksFound = WiFi.scanNetworks();

    // 1 = Open Network, 0 = Normal Network with password
    // Format: SSID,username,password,network type
    char *wifiDetails[][4] = { {"","","","0"},{"","","","0"} };
    int rssitemp = -10000;
    int index = -1;
    char mac[18];
  
    for (int i = 0; i < networksFound; i++)
    {
      for(int j = 0; j < sizeof(wifiDetails) / sizeof(wifiDetails[0]); j++)
      { 
        // Matched against wifi details and is local to the device
        if(WiFi.SSID(i) == wifiDetails[j][0]){
  
          if(WiFi.RSSI(i) > rssitemp){
            rssitemp = WiFi.RSSI(i);
            sprintf(mac,"%s",WiFi.BSSIDstr(i).c_str());
            index = j;
          }
        }
      }
    }

    // Skip if no results are found
    if(index == -1){ delay(100000); continue; }

    // Check if connecting wifi is same as current, skip if it is
    if(strcmp(prevmac,mac) == 0){
      #ifdef DEBUG
      Serial.write("\nWarning - Already connected to ");
      Serial.write(wifiDetails[previd][0]);
      #endif
      
      delay(100000);
      strcpy(prevmac,mac);
      index = -1;
      continue;
    }

    previd = index;

    if(atoi(wifiDetails[index][3]) == 1){
      // Open WiFi AP with no credentials
      #ifdef DEBUG
      Serial.write("\nConnecting to ");
      Serial.write(wifiDetails[index][0]);
      #endif
      WiFi.begin(wifiDetails[index][0]);

      // Connect and return status
      int status = connect();

      // Set prevmac
      if(status == 0){
        strcpy(prevmac,mac);
      }else{
        strcpy(prevmac,"");
      }
      
    }else{
      #ifdef DEBUG
      Serial.write("\nConnecting to ");
      Serial.write(wifiDetails[index][0]);
      #endif
      
      WiFi.begin(wifiDetails[index][0], wifiDetails[index][2]);

      // Connect and return status
      int status = connect();

      // Set prevmac
      if(status == 0){
        strcpy(prevmac,mac);
      }else{
        strcpy(prevmac,"");
      }
    }
    index = -1;

  delay(100000);
  }
}

void loop() {}
