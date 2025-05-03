#include "sd_functions.h"

// Initializes SD card
void initSD() {
  pinMode(SS, OUTPUT);
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("SD init failed");
  }
  
  // Debug only. Removes saved csv file
  SD.remove(FILE_PATH);
}

// Prints all the content of the file as 
void printFromCSV(const char* filePath) {
  String csvData = "";
  File csvFile = SD.open(filePath);
  if (csvFile) {
    while (csvFile.available()) {
      char newChar = csvFile.read();
      Serial.print(newChar);
    }
    csvFile.close();
  }
  else {
    Serial.println("Failed to open: ");
    Serial.println(filePath);
  }  
}

void createCSV(const char* filePath, const String* jsonFields, JsonDocument& doc) {
  // Create the specified CSV file. Overwrite if it exists
  if (SD.exists(filePath)) {
    SD.remove(filePath);
  }

  String csvHeader = "";

  // Create the header row
  for (int i = 0; i < FIELD_COUNT; i++) {
    csvHeader += jsonFields[i];
    if (i != FIELD_COUNT - 1) {
      csvHeader += ",";
    }
  }
  
  writeRowToCSV(csvHeader, filePath);
}

void saveToCSV(const char* filePath, const String* jsonFields, JsonDocument& doc) {
  String newRow = "";

  for (int i = 0; i < FIELD_COUNT; i++) {
    String currentField = jsonFields[i];

    if (doc[currentField].is<JsonVariant>()) {
      newRow += doc[currentField].as<String>();
    }
    else {
      newRow += "null";
    }

    if (i != FIELD_COUNT - 1) {
      newRow += ",";
    }
  }

  // Debug statement
  // Serial.println("Writing new row to: " + filePath);
  // Serial.println(newRow);

  // Creates a new csv file if it doesn't exist yet
  if (!SD.exists(filePath)) {
    createCSV(filePath, jsonFields, doc);
  }

  writeRowToCSV(newRow, filePath);
}

// Write data as a new row to the specified file
void writeRowToCSV(const String& data, const char* filePath) {
  File csvFile = SD.open(filePath, FILE_APPEND);
  if (csvFile) {
    csvFile.println(data);
  }
  else {
    Serial.println("Failed to open: ");
    Serial.println(filePath);
  }
  csvFile.close();
}