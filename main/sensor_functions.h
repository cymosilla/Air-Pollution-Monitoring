#ifndef SENSOR_FUNCTIONS_H
#define SENSOR_FUNCTIONS_H

#include <ArduinoJson.h>
#include <Arduino.h>
#include <SensirionI2cScd4x.h>
#include <Wire.h>
#include "macros.h"

#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

void initSCD40(SensirionI2cScd4x& sensor);
void readSCD40Data(SensirionI2cScd4x& sensor, JsonDocument& doc);
void readOzoneData(JsonDocument& doc);
void readPMSdata();


#endif