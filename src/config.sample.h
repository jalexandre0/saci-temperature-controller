// 1 - rename this file to config.h
// 2 - adjust your wireless configurarion
// 3 - OPTIONAL - Put your thingspeak token if you want enable it,
//            or leave any string < 16 to disable it
//            (null is a good idea, anyway..)
// 4 - enjoy your  beer. :P

#ifndef config_H
#define config_H
#include <Arduino.h>

//Wireless Setup
#define ssid   "my_wireless_ssid"
#define pass   "my_wireless_password"

//ThingSpeak.com write token channel
#define tsToken "null"

#endif
