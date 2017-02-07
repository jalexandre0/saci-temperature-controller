#include "DS18B20.h"

OneWire  pin(D2);
DallasTemperature tempSensor(&pin) ;

float beerTemp() {
  static float _temp  ;
  tempSensor.requestTemperatures() ;
  //naive attempt to make a low pass filter
  _beerTemp = (0.95 * _beerTemp ) + ( 0.05 * tempSensor.getTempCByIndex(0));
  return _beerTemp ;
}

float fridgeTemp() {
  _fridgeTemp = (0.95 * _fridgeTemp ) + ( 0.05 * tempSensor.getTempCByIndex(1));
  return _fridgeTemp ;
}

void sensorInit() {
  tempSensor.begin() ;
  tempSensor.setResolution(12);
  tempSensor.requestTemperatures();
  _beerTemp   = tempSensor.getTempCByIndex(0) ;
  _fridgeTemp = tempSensor.getTempCByIndex(1) ;
}
