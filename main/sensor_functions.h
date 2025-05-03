#ifndef SENSOR_FUNCTIONS_H
#define SENSOR_FUNCTIONS_H

#include <ArduinoJson.h>
#include <Arduino.h>
#include <DFRobot_SCD4X.h>
#include "macros.h"

void initDFRobot(DFRobot_SCD4X& SCD4X);
void readSCD4XData(DFRobot_SCD4X& SCD4X, JsonDocument& doc);
void readOzoneData(JsonDocument& doc);
void readPMSdata();


#endif