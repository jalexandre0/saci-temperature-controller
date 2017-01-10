#include <Arduino.h>
#include <ArduinoOTA.h>
#include "BangController.h"
#include "DS18B20.h"
#include "config.h"
#include "WebInterface.h"
#include <NTPClient.h>
#include <ESP8266Wifi.h>

//Plugins I wrote for myself
#include "plugins/ThingSpeak.h"
//#include "plugins/UDPLog.h"
#include "plugins/InfluxDB.h"


uint8_t heatPin = D7 ;
uint8_t coolPin = D8 ;
//Pins, config and profile path
BangController saci(heatPin, coolPin, "/saci.txt", "saciprofile.txt");

//NTP  for profile
WiFiUDP udpSocket;
NTPClient timeClient(udpSocket, "ntp.on.br", -10800, 60000);

void setup () {
  // Starting watchdog.
  // IF hardware won´t receive a reset signal under 8seconds, reboot the unit
  wdt_enable(8000) ;
  Serial.begin(115200) ;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  ArduinoOTA.onStart([]() {
    //nothing to do on start
  });

  // Restart after Over The Air update.
  ArduinoOTA.onEnd([]() {
    ESP.restart();
  });

  ArduinoOTA.begin();

  //see DS18B20.h and WebInterface for sensor and interface Init ;
  sensorInit() ;
  interfaceInit() ;

  //NTP update
  timeClient.begin() ;
  timeClient.update() ;

  //load config from file to memory
  saci.loadConfig() ;
}

void loop() {
  //read temperature
  float _temp = readTemp() ;

  //main controller action
  saci.run(_temp) ;

  //fix_profile debug
  if(WL_CONNECTED && WiFi.localIP().toString() != "0.0.0.0") {
    if ( timeClient.update() ) {
        uint32_t now = timeClient.getEpochTime() ;
        if(saci.getProfileRun() == 1 && now > saci.getLastRamp() + 86400 ) {
          saci.runProfile(now) ;
        }
      }
    }

  //Web interface handler
  interface.handleClient() ;

  //My Plugins
  //thingSpeakSend() ;
  //influxSend() ;

  // OTA handler
  ArduinoOTA.handle() ;

  //Serial Output: Usefull for some debug
  Serial.println(saci.getConfig(_temp)) ;
  Serial.println(" ");

  //Restart watchdog timer
  wdt_reset() ;
}
