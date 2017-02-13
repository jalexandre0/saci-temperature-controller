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
  char msg[10] ;

  //dtostrf(double, buffer size, digits, buffer)
  dtostrf(beerTemp(), 6, 2, msg);
  client.publish("beerTemp", msg, true) ;

  dtostrf(saci.getTargetTemp(), 6, 2, msg);
  client.publish("targetTemp", msg, true) ;

  dtostrf(fridgeTemp(), 6, 2, msg);
  client.publish("fridgeTemp", msg, true) ;

  if (saci.getStatus() == 0) {
    client.publish("status", "idle", true) ;
  }

  if (saci.getStatus() == 1) {
    client.publish("status", "cooling", true) ;
  }

  if (saci.getStatus() == 2) {
    client.publish("status", "heating", true) ;
  }

  if (saci.getMode() == 0) {
    client.publish("mode", "off", true) ;
  }

  if (saci.getMode() == 1) {
    client.publish("mode", "auto mode", true) ;
  }

  if (saci.getMode() == 2) {
    client.publish("mode", "heat mode", true) ;
  }

  if (saci.getMode() == 3) {
    client.publish("mode", "cool mode", true) ;
  }

  //Human readable step
  //We start counting for 1, not zero ;)
  itoa((saci.getControlStep() + 1), msg, 10);
  client.publish("step", msg, true) ;

  if (saci.getProfileRun() == 1 ) {
    client.publish("profile", "Running", true) ;
    itoa(saci.getControlStep(), msg, 10);
    client.publish("step", msg, true) ;
  }
  else {
    client.publish("profile", "off", true) ;
    client.publish("step", "-", true) ;
  }

  itoa((millis() / 1000), msg, 10);
  client.publish("uptime", msg, true) ;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "setTargetTemp") == 0) {
    String _value = "";
    for (int i = 0; i < length; i++) {
     _value += (char)payload[i] ;
   }
   saci.setConfig("targetTemp", _value ) ;
  }

  if (strcmp(topic, "setMode") == 0) {
    String _value = "";
    for (int i = 0; i < length; i++) {
     _value += (char)payload[i] ;
   }

   if (_value == "off") {
    saci.setConfig("mode", "OFF" ) ;
   }

   if (_value == "auto") {
    saci.setConfig("mode", "Automatic" ) ;
   }

   if (_value == "heat") {
    saci.setConfig("mode", "Heating" ) ;
   }

   if (_value == "cool") {
    saci.setConfig("mode", "Cooling" ) ;
   }
 }

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
  String ID = "saci-" ;
         ID += ESP.getChipId() ;

  char mqttId[16] ;
  ID.toCharArray(mqttId, 16);

  if (!client.connected()) {
    client.connect(mqttId, mqttUser, mqttPass);
    client.subscribe("setTargetTemp");
    client.subscribe("setMode");
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
