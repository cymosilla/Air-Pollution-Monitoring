#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H

#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "macros.h"

void writeRowToCSV(const String& data, const char* filePath);
void printFromCSV(const char* filePath);
void createCSV(const char* filePath, const String* jsonFields, JsonDocument& doc);
void saveToCSV(const char* filePath, const String* jsonFields, JsonDocument& doc);

#endif