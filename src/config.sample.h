/* false -> Plugin disabled
   true -> Plugin enabled
*/

#ifndef config_H
#define config_H
#include <Arduino.h>

//Wireless Setup
#define ssid   "Wireless SSID"
#define pass   "Wireless Password"

//Thing Speak
#define thingSpeakPlugin  false
#define thingSpeakToken "Thing Speak Token"
//Delay between sends, in seconds
#define thingSpeakDelay 300

//InfluxDB
#define influxdbPlugin false
#define influxdbHost    "Influx DB IP Address"
#define influxdbPort    8086
#define influxdbDelay   40
//UDPLog
#define udplogPlugin false

//MQTT
#define mqttPlugin false
#define mqttUser   "Mqtt Username"
#define mqttPass   "Mqtt Pass"
#define mqttServer "Mqtt Server"
#define mqttPort   1883
#define mqttDelay  180

#endif
