#include <Arduino.h>
#include <ArduinoOTA.h>
#include "BangController.h"
#include "DS18B20.h"
#include "config.h"
#include "WebInterface.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>

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
NTPClient timeClient(udpSocket, "pool.ntp.br", -10800, 60000);

void setup () {

  /* Watchdog register will wait 8000ms for a signal.
     If signal do not come (ie function loop or hardware fail),
     micro processor will reset itself
  */

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
  //main controller action
  saci.run() ;

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

  //My Plugins (enable or disable adjusting "config.h")
  if (plugin_InfluxDB) {
    influxSend() ;
  }

  if (plugin_ThingSpeak) {
    thingSpeakSend() ;
  }

  // OTA handler
  ArduinoOTA.handle() ;

  //Serial Output: Usefull for some debug
  Serial.println(saci.getConfig()) ;
  Serial.println(" ");

  //Restart watchdog timer
  wdt_reset() ;
}
