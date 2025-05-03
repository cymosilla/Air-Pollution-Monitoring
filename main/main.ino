#include "sd_functions.h"
#include "macros.h"
#include <DFRobot_SCD4X.h>


//  Pass 1: 738 Byte free memory

// Pass 2: 740 Bytes (removed SoftwareSerial import)

// Pass 3: 766 Bytes (macros instead of gloabl variables) 


JsonDocument doc;

const String jsonFields[FIELD_COUNT] = {
  "ozone",
  "co2_ppm",
  "temp_c",
  "humidity",
};
 

DFRobot_SCD4X SCD4X(&Wire, SCD4X_I2C_ADDR);

// O3 ***********************************************************************************
const int ozonePin = A3;  //Ozone in pin 3 on board
 


void setup() {
  // Initialize ESP32 as hardware serial
  Serial.begin(9600);

  // Initialize SD
  pinMode(SS, OUTPUT);
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("SD init failed");
  }
  
  SD.remove(FILE_PATH);

  // CO2 ***********************************************************************
  // Init the sensor
  while( !SCD4X.begin() ){
    // Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }

  SCD4X.enablePeriodMeasure(SCD4X_STOP_PERIODIC_MEASURE);

  SCD4X.setTempComp(4.0);

  SCD4X.setSensorAltitude(540);

  SCD4X.enablePeriodMeasure(SCD4X_START_PERIODIC_MEASURE); 

  // CO2 END ********************************************************************
  Serial.println("Init complete");


}

unsigned long timer = millis();


void loop() {

    // Gets current time
    unsigned long current_time = millis();

    // Gets readings if the time difference is greater than READING_PERIOD
    if (current_time - timer > READING_PERIOD) {
      
      // Get readings
      readOzoneData();
      // readPMSdata();
      readCO2data();

      // Sends data to esp32
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

void readOzoneData() {
  int reading = analogRead(ozonePin); //read from ozone pin
  doc["ozone"] = reading;
}

void readCO2data() {
  if(SCD4X.getDataReadyStatus()) {
    DFRobot_SCD4X::sSensorMeasurement_t data;
    SCD4X.readMeasurement(&data);

    doc["co2_ppm"] = data.CO2ppm;
    doc["temp_c"] = data.temp;
    doc["humidity"] = data.humidity;
  }
}

// TODO: complete this function 
void readPMSdata() {


}

