#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Do not include mqtt.h because callback stops works
//Need more investigation
//#include "mqtt.h"

#include "../DS18B20.h"
#include "../BangController.h"
#include "../config.h"

extern BangController saci ;

WiFiClient espClient;
PubSubClient client(espClient);

void mqttPublish() {
  char msg[6] ;

  //dtostrf(double, buffer size, digits, buffer)
  dtostrf(beerTemp(), 6, 2, msg);
  client.publish("beerTemp", msg, true) ;

  dtostrf(saci.getTargetTemp(), 6, 2, msg);
  client.publish("targetTemp", msg, true) ;

  dtostrf(fridgeTemp(), 6, 2, msg);
  client.publish("fridgeTemp", msg, true) ;

  //ito(integer, buffer, base)
  itoa(saci.getStatus(), msg, 10);
  client.publish("status", msg, true) ;

  itoa(saci.getMode(), msg, 10);
  client.publish("mode", msg, true) ;

  itoa(saci.getControlStep(), msg, 10);
  client.publish("step", msg, true) ;

  itoa(saci.getProfileRun(), msg, 10);
  client.publish("profile", msg, true) ;

}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "setTargetTemp") == 0) {
    String _value = "";
    for (int i = 0; i < length; i++) {
     _value += (char)payload[i] ;
   }
   saci.setConfig("targetTemp", _value ) ;
  }
  saci.writeConfig() ;
  mqttPublish() ;
}

void mqttReconnect() {
  //quick and dirty hack to generate mqtt-ID
  // Space is not a problem on ESP8266 after all ;)
  String ID = "saci-" ;
         ID += ESP.getChipId() ;

  char mqttId[16] ;
  ID.toCharArray(mqttId, 16);

  if (!client.connected()) {
    client.connect(mqttId, mqttUser, mqttPass);
    client.subscribe("setTargetTemp");
    mqttPublish() ;
  }
}

void mqttInit() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  mqttPublish() ;
}

void mqttLoop() {
  static uint32_t lastSend ;
  if (!client.connected()) {
    mqttReconnect();
  }

  client.loop();
}
