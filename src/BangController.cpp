#include "BangController.h"
#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>


BangController::BangController(uint8_t _coolPin, uint8_t _heatPin, \
                               String _configFile,String  _profileFile) {
  configFile = _configFile;
  profileFile = _profileFile;
  coolPin = _coolPin ;
  heatPin = _heatPin ;
  pinMode(coolPin, OUTPUT) ;
  pinMode(heatPin, OUTPUT) ;
  digitalWrite(coolPin, LOW) ;
  digitalWrite(heatPin, LOW) ;
  SPIFFS.begin();
}

BangController::~BangController() {
  //nothing to destruct
}

void BangController::heat(bool _status) {
  if( _status ) {
    digitalWrite(heatPin, HIGH) ;
    status = 1 ;
  }
  else {
    digitalWrite(heatPin, LOW) ;
    lastHeat = (millis() / 1000) ;
    status = 0 ;
  }
}

void BangController::cool(bool _status) {
  if ( _status ) {
    digitalWrite(coolPin, HIGH) ;
    status = 2 ;
  }
  else {
    digitalWrite(coolPin, LOW) ;
    lastCool = (millis() / 1000);
    status = 0 ;
  }
}

String BangController::getConfig(float _temp) {
  String jsonOutput =  "{\"id\":" ;
  jsonOutput += ESP.getChipId() ;
  jsonOutput += "," ;
  jsonOutput += "\"address\":" ;
  jsonOutput += "\"" ;
  jsonOutput += WiFi.localIP().toString();
  jsonOutput += "\"" ;
  jsonOutput += "," ;
  jsonOutput += "\"uptime\":";
  jsonOutput += millis() / 1000;
  jsonOutput += "," ;
  jsonOutput += "\"mode\":" ;
  jsonOutput += "\"" ;
  jsonOutput += mode;
  jsonOutput += "\"";
  jsonOutput += "," ;
  jsonOutput += "\"status\":" ;
  jsonOutput += "\"" ;
  jsonOutput += status;
  jsonOutput += "\"";
  jsonOutput += "," ;
  jsonOutput += "\"temperature\":" ;
  jsonOutput += _temp;
  jsonOutput += "," ;
  jsonOutput += "\"target temperature\":" ;
  jsonOutput += targetTemp ;
  jsonOutput += "," ;
  jsonOutput += "\"heat differential\":" ;
  jsonOutput += heatDiff ;
  jsonOutput += "," ;
  jsonOutput += "\"cool differential\":" ;
  jsonOutput += coolDiff ;
  jsonOutput += ",";
  jsonOutput += "\"heat timer\":" ;
  jsonOutput += heatTimer ;
  jsonOutput +=  "," ;
  jsonOutput += "\"fridge timer\":" ;
  jsonOutput += coolTimer ;
  jsonOutput += "," ;
  jsonOutput += "\"heat trigger\":" ;
  jsonOutput += targetTemp + heatDiff ;
  jsonOutput += "," ;
  jsonOutput += "\"cool trigger\":" ;
  jsonOutput += targetTemp - coolDiff ;
  jsonOutput += "}" ;
  return jsonOutput ;
}

void BangController::run(float _temp ) {


  float _maxHeat = targetTemp + heatDiff ;
  float _maxCool = targetTemp - coolDiff ;

  //Mode 0 == Off
  if (mode == 0) {
    BangController::heat(false) ;
    BangController::cool(false) ;
    return ;
  }

  //Mode 1 == AUTOMATIC
  if (mode == 1) {
    //Starting heat if temp is too cool and heat is off
    if ( _temp <= _maxCool && status != 1 ) {
      if ((millis() / 1000) >= ( lastHeat + heatTimer)) {
        BangController::heat(true) ;
        return ;
      }
    }

    //Starting cool if temp is too hot and cool is off
    if ( _temp >= _maxHeat && status != 2 ) {
      if ((millis() / 1000) >= (lastCool + coolTimer)) {
        BangController::cool(true) ;
        return ;
      }
    }

    //Stop heating if setpoint is reached
    if ( _temp >= targetTemp && status == 1 ) {
      BangController::heat(false) ;
      return ;
    }

    //Stop cooling if setpoint is reached
    if ( _temp <= targetTemp && status == 2 ) {
      BangController::cool(false) ;
      return ;
    }
  }

  //Mode 2 == Only Heating
  if (mode == 2) {
    if ( _temp <= _maxCool && status != 1 ) {
      if ( (millis() / 1000 ) >= lastHeat + heatTimer ) {
        BangController::heat(true) ;
        return ;
      }
    }

    if ( _temp >= targetTemp && status == 1 ) {
      BangController::heat(false) ;
      return ;
    }
  }

  //Mode 3 == Only Cooling
  if (mode == 3) {
    if ( _temp >= _maxHeat && status != 2 ) {
      if ( (millis() / 1000 ) >= lastCool + coolTimer ) {
        BangController::cool(true) ;
        return ;
      }
    }


    if ( _temp <= targetTemp && status == 2 ) {
      BangController::cool(false) ;
      return ;
    }
  }
}

//Adjust individual configs
void BangController::setConfig(String _key, String _value)  {

  if (_key == "mode") {
    if (_value == "OFF") {
      mode = 0;
    }

    if (_value == "Automatic") {
      mode = 1;
    }

    if (_value == "Heating") {
      mode = 2;
    }

    if (_value == "Cooling") {
      mode = 3;
    }
  }

  if (_key == "heatTimer") {
   heatTimer = _value.toInt() ;
  }

  if (_key == "coolTimer") {
    coolTimer = _value.toInt() ;
  }

  if (_key == "heatDiff") {
    heatDiff = _value.toFloat() ;
  }

  if (_key == "coolDiff") {
    coolDiff = _value.toFloat();
  }

  if (_key == "targetTemp") {
    targetTemp = _value.toFloat() ;
  }
}

//Save all configs to file
void BangController::writeConfig() {
  String _config ;
  _config +=  "mode=" ;
  _config +=  mode ;
  _config +=  '\n' ;
  _config +=  "heatTimer=" ;
  _config +=  heatTimer ;
  _config +=  '\n' ;
  _config +=  "coolTimer=" ;
  _config +=  coolTimer ;
  _config +=  '\n' ;
  _config +=  "coolDiff=" ;
  _config +=  coolDiff ;
  _config +=  '\n';
  _config +=  "heatDiff=" ;
  _config +=  heatDiff ;
  _config +=  '\n' ;
  _config +=  "targetTemp=" ;
  _config +=  targetTemp ;
  _config +=  '\n' ;
  _config +=  "lastRamp=" ;
  _config += lastRamp ;
  _config +=  '\n' ;
  _config += "controlStep=" ;
  _config += controlStep ;
  _config +=  '\n' ;
  _config += "profileRun=" ;
  _config += profileRun ;

  File _file = SPIFFS.open(configFile, "w") ;
    _file.print(_config);
    _file.close() ;
}

//load config from file
void BangController::loadConfig() {
  File _file = SPIFFS.open(configFile, "r") ;
  while( _file.available()) {
    String _line   = _file.readStringUntil('\n') ;
    int _separator = _line.indexOf('=') ;
    String _key    = _line.substring(0, _separator) ;
    String _value  = _line.substring(_separator + 1) ;

    if ( _key == "mode") {
      mode = _value.toInt() ;
    }

    if ( _key == "heatTimer") {
      heatTimer = _value.toInt() ;
    }

    if ( _key == "coolTimer") {
      coolTimer = _value.toInt() ;
    }

    if ( _key == "coolDiff") {
      coolDiff = _value.toFloat() ;
    }

    if ( _key == "heatDiff") {
      heatDiff = _value.toFloat() ;
    }

    if (_key == "targetTemp") {
      targetTemp = _value.toFloat() ;
    }

    if ( _key == "lastRamp") {
      lastRamp = _value.toInt() ;
    }

    if ( _key == "controlStep") {
      controlStep = _value.toInt() ;
    }

    if ( _key == "profileRun") {
      profileRun = _value.toInt() ;
    }
  }
  _file.close() ;
}

float BangController::getTargetTemp() {
  return targetTemp ;
}

uint32_t BangController::getLastRamp() {
  return lastRamp;
}

//Save user steps to file, start profile and set first control step
void BangController::initProfile(float _step[14], uint32_t _timestamp) {
  //write profile file
  File _file = SPIFFS.open(profileFile, "w") ;
  for (uint8_t i = 0 ; i <= 15 ; i++ ) {
    _file.println(_step[i]) ;
  }
  _file.close() ;

  mode = 1 ;
  if (_step[0] != 255 ) {
    targetTemp = _step[0] ;
  }

  controlStep = 0 ;
  lastRamp = _timestamp ;
  profileRun = 1 ;
  BangController::writeConfig() ;
}

uint8_t BangController::getProfileRun() {
  return profileRun ;
}

// ProfileRun and control step are already loaded into boot.
// I only need to load a "profile.txt" to array and apply target temperature
void BangController::runProfile(uint32_t _timestamp) {
  float step[15] ;
  File _profile = SPIFFS.open(profileFile, "r") ;
  uint8_t i = 0 ;
  while ( _profile.available()) {
    String _line = _profile.readStringUntil('\n') ;
    step[i] = _line.toFloat()   ;
    i++ ;
  }

  //If next step is ok apply and write config.
  if (step[ controlStep + 1 ] != 255 && controlStep + 1 <= 13 ) {
    controlStep++ ;
    targetTemp = step[controlStep] ;
    lastRamp = _timestamp ;
    BangController::writeConfig() ;
    return ;
  }

  //If last step, set profileRun to 0 and write config;
  //Control Step won´t matter here
  if( controlStep + 1 == 14 ) {
    profileRun = 0 ;
    lastRamp = _timestamp ;
    BangController::writeConfig() ;
    return ;
  }

  // Interface uses 255 to signal an empty input by user.
  // Just increment control step and won´t mess with target temperature
  if (step[controlStep + 1] == 255 && controlStep <= 13) {
    controlStep++ ;
    lastRamp = _timestamp;
    BangController::writeConfig() ;
    return ;
  }
}
