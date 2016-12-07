#ifndef WebInterface_H
#define WebInterface_H

#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "BangController.h"
#include <NTPClient.h>

extern ESP8266WebServer interface;
extern BangController saci ;
extern NTPClient timeClient ;

void json() ;
void config() ;
void setProfile() ;
void interfaceInit() ;

#endif
