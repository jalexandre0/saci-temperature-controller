#ifndef DS18B20_H
#define DS18B20_H

#include <OneWire.h>
#include <DallasTemperature.h>

extern OneWire  pin;
extern DallasTemperature tempSensor ;

float readTemp() ;
void sensorInit() ;

#endif
