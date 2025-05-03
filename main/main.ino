#include "sd_functions.h"
#include "macros.h"
#include "sensor_functions.h"


JsonDocument doc;

const String jsonFields[FIELD_COUNT] = {
  "ozone",
  "co2_ppm",
  "temp_c",
  "humidity",
};
 

DFRobot_SCD4X SCD4X(&Wire, SCD4X_I2C_ADDR);

void setup() {
  // Initialize ESP32 as hardware serial
  Serial.begin(9600);

  // Initialize components
  initSD();
  initDFRobot(SCD4X);
  // TODO: init wifi

  Serial.println("Init complete");


}

unsigned long timer = millis();


void loop() {

    // Gets current time
    unsigned long current_time = millis();

    // Gets readings if the time difference is greater than READING_PERIOD
    if (current_time - timer > READING_PERIOD) {
      
      // Get readings
      readOzoneData(doc);
      // readPMSdata();
      readSCD4XData(SCD4X, doc);

      // Prints data
      serializeJson(doc, Serial);
      Serial.print("\n");

      // Saves data to csv
      saveToCSV(FILE_PATH, jsonFields, doc);

      // Optional debug statement
      printFromCSV(FILE_PATH);

      doc.clear();

      timer = current_time;
    }
}

