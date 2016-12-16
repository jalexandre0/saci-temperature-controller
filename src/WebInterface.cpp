#include <FS.h>
#include "WebInterface.h"
#include "DS18B20.h"

ESP8266WebServer interface(80) ;

//Spits a Json String if /status.json URL is called
void json() {
  String _jsonString = saci.getConfig(readTemp()) ;
  interface.send(200, "application/json", _jsonString ) ;
}

//A simple refresh for user feedback
void refresh() {
  String _content  = "<!DOCTYPE html>";
  _content  += "<html>";
  _content  += "<head>";
  _content  += "<meta charset=\"utf-8\">";
  _content  += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">";
  _content  += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  _content  += "</title>";
  _content  += "<meta http-equiv=\"refresh\" content=\"5; url=/\">" ;
  _content  += "</head>";
  _content  += "<body>";
  _content  += "Updating..." ;
  _content  += "</body>";
  _content  += "</html>" ;
  interface.send(200, "text/html", _content ) ;
}

//If you call config without argument, print a json string with config
void config() {
  if( !interface.args() ) {
    json() ;
    return ;
  }
  else {
    //Pass the values of argument array one by one to BangController::setConfig
    for (uint8_t i=0 ; i <= interface.args() ; i++ ) {
      if (interface.arg(i).length() > 0 ) {
        saci.setConfig(interface.argName(i), interface.arg(i)) ;
      }
    }
    //Write configuration into file
    saci.writeConfig() ;
  }
  refresh() ;
}

void setProfile() {
  float step[15] ;
  for (uint8_t i=0 ; i <= 14 ; i++ ) {
    if (interface.arg(i).length() > 0 ) {
      step[i] = interface.arg(i).toFloat() ;
    }
    //empty user input is marked with 255 value
    else {
      step[i] = 255 ;
    }
  }
  if ( timeClient.update() ) {
    saci.initProfile(step, timeClient.getEpochTime());
    refresh() ;
  }
  else {
    interface.send(200, "text/plain",  "NTP Error. Try again later." ) ;
  }
}

void interfaceInit() {
   interface.on("/status.json", json) ;
   //look at data/index.html to edit file
   interface.serveStatic("/", SPIFFS, "/index.html", "max-age=86400");
   interface.on("/config", config) ;
   interface.on("/setprofile", setProfile) ;
   interface.begin() ;
 }
