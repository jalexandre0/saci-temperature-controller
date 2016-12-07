#include <WiFiClientSecure.h>
#include "../DS18B20.h"
#include "ThingSpeak.h"

void thingSpeakSend() {
  static uint32_t _lastSend;

  /*
    Little warning here:
    The minimum delay allowed by ThingSpeak api is 15 seconds between sends
    but, every HTTP POST "freezes" MCU for 5 seconds,  and this is sux when
    you need to use web interface.
    So, stick with bigger intervals (3 minutes is fine to me).
  */

  uint8_t _delay = 180 ;
  uint32_t now = ( millis() / 1000 ) ;

  //no token, no submit =)
  if (now >= _lastSend + _delay && strlen(tsToken) == 16 ) {
  float _temp = readTemp() ;
  float _targetTemp = saci.getTargetTemp() ;

  //Arduino String Class don´t support macros (#define tsToken), so I
  //take the other way.
  char _payload[70];

  //Hack to print float values with sprintf. :)
  sprintf(_payload, "api_key=%s&field1=%d.%02d&field2=%d.%02d&field3=%d",\
         tsToken, (int)(_temp), (int)(_temp * 100)%100, (int)(_targetTemp),\
         (int)(_targetTemp * 100)%100, now) ;

    //HTTPS GET/POST  works ok with WiFiClientSecurelibrary
    WiFiClientSecure client ;
    const char* host = "api.thingspeak.com";
    const int port = 443 ;
    if ( client.connect(host, port) ) {
      client.println("POST /update HTTP/1.1") ;
      client.print("Host: ") ;
      client.println(host) ;
      client.println("User-Agent: SACI/1.0");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(strlen(_payload));
      client.println();
      client.println(_payload);
      _lastSend = now ;
    }
  }
}
