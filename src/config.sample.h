/* false -> Plugin disabled
   true -> Plugin enabled
*/

#ifndef config_H
#define config_H
#include <Arduino.h>

//Wireless Setup
#define ssid   "my_ssid"
#define pass   "my_wireless_password"

//Thing Speak
#define plugin_ThingSpeak  false
#define tsToken "my_thingspeak_token"

//InfluxDB
#define plugin_InfluxDB false
#define influxDB_host   "my_influxDB_host"
#define influxDB_port   8086

//UDPLog
#define plugin_UDPLog false

#endif
