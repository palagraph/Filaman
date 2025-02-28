#include <Arduino.h>
#include "wlan.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiManager.h>
#include "display.h"
#include "config.h"

WiFiManager wm;
bool wm_nonblocking = false;

void initWiFi() {
    // Optimized WiFi settings

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    WiFi.setSleep(false); // disable sleep mode
    esp_wifi_set_ps(WIFI_PS_NONE);
    
    // Maximum transmission performance

    WiFi.setTxPower(WIFI_POWER_19_5dBm); // Set maximum transmit power
  
    // Optimize TCP/IP stack.
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    
    // Activate WiFi roaming for better stability

    esp_wifi_set_rssi_threshold(-80);
  
    if(wm_nonblocking) wm.setConfigPortalBlocking(false);
    wm.setConfigPortalTimeout(320); // Close the portal after 5 minutes

  
    oledShowTopRow();
    oledShowMessage("WiFi Setup");
    
    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    res = wm.autoConnect("FilaMan"); // anonymous ap
    // res = wm.autoConnect("spoolman","password"); // password protected ap
  
    if(!res) {
      Serial.println("Failed to connect or hit timeout");
      // ESP.restart();
      oledShowTopRow();
      oledShowMessage("WiFi not connected Check Portal");
    } 
    else {
      wifiOn = true;
  
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
  
      oledShowTopRow();
      display.display();
    }
  }