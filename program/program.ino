#include "ESP8266WiFi.h"
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <dhcpserver.h>

#define DEBUG 1

void createAP(const char *ssid, const char *pass){

  delay(5000);
  WiFi.softAP(ssid, pass);

  #if DEBUG
  Serial.printf("\nSuccessfully routing to AP '%s'",ssid);
  #endif

  // Initialize the NAT feature
  ip_napt_init(512, 32);

  // Enable NAT on the AP interface
  ip_napt_enable_no(1, 1);

  // Set the DNS server for clients of the AP to the one we also use for the STA interface
  dhcps_set_dns(0,WiFi.dnsIP(0));
  dhcps_set_dns(1,WiFi.dnsIP(1));
}

int connect(){
  int count = 0;
  int ret = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    // Break if connection is too long
    if(count >30){ ret=1; break; }
      #if DEBUG
      Serial.print(".");
      #endif
    count++;
  }

  if(!ret){
    delay(1000);
  
    // Retrieve IP after connection
    #if DEBUG
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

  // Define wireless networks
  // 1 = Open Network, 0 = Normal Network with password
  // Format: SSID,username,password,network type
  char *wifiDetails[][4] = { {"VM5328000_2G","","b7dmJcppykgd","0"},{"","","","0"} };

  // AP Settings ssid & password
  const static char *APssid = "Robs Wifi";
  const static char *APpass = "Landy964i22@$";


  while(1){
    int networksFound = WiFi.scanNetworks();
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
      #if DEBUG
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
      #if DEBUG
      Serial.write("\nConnecting to ");
      Serial.write(wifiDetails[index][0]);
      #endif
      WiFi.begin(wifiDetails[index][0]);

      // Connect and return status
      int status = connect();

      // Set prevmac
      if(status == 0){
        strcpy(prevmac,mac);
        createAP(APssid,APpass);
      }else{
        strcpy(prevmac,"");
      }
      
    }else{
      #if DEBUG
      Serial.write("\nConnecting to ");
      Serial.write(wifiDetails[index][0]);
      #endif
      
      WiFi.begin(wifiDetails[index][0], wifiDetails[index][2]);

      // Connect and return status
      int status = connect();

      // Set prevmac
      if(status == 0){
        strcpy(prevmac,mac);
        createAP(APssid,APpass);
      }else{
        strcpy(prevmac,"");
      }
    }
    index = -1;

  delay(100000);
  }
}

void loop() 
{
  delay(1000);
}
