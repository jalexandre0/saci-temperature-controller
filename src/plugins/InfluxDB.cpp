#include <WiFiClient.h>
#include "InfluxDB.h"
#include <ESP8266WiFi.h>
#include "../DS18B20.h"

void influxSend() {
  WiFiClient client ;
  static uint32_t _lastSend; ;
  const char* host = "192.168.1.3" ;
  const int port = 8086 ;
  uint8_t _delay = 15 ;
  uint32_t now = ( millis() / 1000 ) ;

  if (now > _lastSend + _delay) {
    String  _payload = "devices,uniqueID=";
            _payload += ESP.getChipId() ;
            _payload += " " ;
            _payload += "address=\"";
            _payload +=  WiFi.localIP().toString() ;
            _payload += "\",uptime=" ;
            _payload +=  millis() / 1000 ;
            _payload += ",mode=" ;
            _payload +=  saci.getMode();
            _payload += ",status=" ;
            _payload +=  saci.getStatus();
            _payload += ",beerTemperature=" ;
            _payload +=  beerTemp();
            _payload += ",fridgeTemperature=" ;
            _payload +=  fridgeTemp();
            _payload += ",targetTemperature=" ;
            _payload +=  saci.getTargetTemp();
            _payload += ",profile=" ;
            _payload +=  saci.getProfileRun();
            _payload += ",epochTime=\"" ;
            _payload +=  timeClient.getEpochTime();
            _payload += "\",lastRamp=\"" ;
            _payload +=  saci.getLastRamp();
            _payload += "\"" ;
            _payload += ",controlStep=" ;
            _payload +=  saci.getControlStep() ;
            Serial.println(_payload) ;

    if (client.connect(host, port)) {
      if ( client.connect(host, port) ) {
        client.println("POST /write?db=saci HTTP/1.1") ;
        client.print("Host: ") ;
        client.println(host) ;
        client.println("User-Agent: SACI/1.0");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(_payload.length());
        client.println();
        client.println(_payload);
        _lastSend = millis() / 1000;
      }
    }
  }
}
