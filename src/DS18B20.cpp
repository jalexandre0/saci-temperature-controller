#include "DS18B20.h"

OneWire  pin(D2);
DallasTemperature tempSensor(&pin) ;

float readTemp() {
  tempSensor.requestTemperatures() ;
  return tempSensor.getTempCByIndex(0) ;
}

void sensorInit() {
  tempSensor.begin() ;
  tempSensor.setResolution(12);
}
