#include <WiFiClient.h>
#include "InfluxDB.h"
#include "../DS18B20.h"

void influxSend() {
  WiFiClient client ;
  uint32_t now = millis() / 1000 ;
  const char* host = "192.168.1.3" ;
  const int port = 8086 ;
  static uint32_t _lastSend;

  if (now > _lastSend + 180) {
    String  _payload = "devices,uniqueID=";
            _payload += ESP.getChipId() ;
            _payload += " " ;
            _payload += "temperature=";
            _payload += readTemp() ;
            _payload += ",target=" ;
            _payload += saci.getTargetTemp() ;

    if (client.connect(host, port)) {
      if ( client.connect(host, port) ) {
        client.println("POST /write?db=opensaci HTTP/1.1") ;
        client.print("Host: ") ;
        client.println(host) ;
        client.println("User-Agent: SACI/1.0");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(_payload.length());
        client.println();
        client.println(_payload);
        _lastSend = now  ;
      }
    }
  }
}
