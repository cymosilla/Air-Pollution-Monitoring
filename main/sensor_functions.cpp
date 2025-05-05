#include "sensor_functions.h"

// Initializes the DFRobot_SCD4X sensor
void initDFRobot(DFRobot_SCD4X& SCD4X) {
  
  while( !SCD4X.begin() ){
    Serial.println("Communication with SCD4X failed, please check connection");
    delay(3000);
  }
  // Stop periodic measure to configure sensor
  SCD4X.enablePeriodMeasure(SCD4X_STOP_PERIODIC_MEASURE);
  SCD4X.setTempComp(4.0);
  SCD4X.setSensorAltitude(17);
  SCD4X.enablePeriodMeasure(SCD4X_START_PERIODIC_MEASURE); 
}

// Reads CO2, temperature, humidity data from SCD4X
void readSCD4XData(DFRobot_SCD4X& SCD4X, JsonDocument& doc) {
  if(SCD4X.getDataReadyStatus()) {
    DFRobot_SCD4X::sSensorMeasurement_t data;
    SCD4X.readMeasurement(&data);

    doc["co2_ppm"] = data.CO2ppm;
    doc["temp_c"] = data.temp;
    doc["humidity"] = data.humidity;
  }
}

// Reads ozone data
void readOzoneData(JsonDocument& doc) {
  int reading = analogRead(OZONE_PIN); //read from ozone pin
  doc["ozone"] = reading;
}


// TODO: complete this function 
void readPMSdata() {


}
