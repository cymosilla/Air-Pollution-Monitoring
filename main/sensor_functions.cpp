#include "sensor_functions.h"

static char errorMessage[64];
static int16_t error;

// Initializes the DFRobot_SCD4X sensor
void initSCD40(SensirionI2cScd4x& sensor) {
  
  // Ensure sensor is in clean state
  error = sensor.wakeUp();
  if (error != NO_ERROR) {
      Serial.print("Error trying to execute wakeUp(): ");
      errorToString(error, errorMessage, sizeof errorMessage);
      Serial.println(errorMessage);
  }
  error = sensor.stopPeriodicMeasurement();
  if (error != NO_ERROR) {
      Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
      errorToString(error, errorMessage, sizeof errorMessage);
      Serial.println(errorMessage);
  }
  error = sensor.setSensorAltitude(IRVINE_ALTITUDE);
  if (error != NO_ERROR) {
      Serial.print("Error trying to execute setSensorAltitude(): ");
      errorToString(error, errorMessage, sizeof errorMessage);
      Serial.println(errorMessage);
  }

  error = sensor.reinit();
  if (error != NO_ERROR) {
      Serial.print("Error trying to execute reinit(): ");
      errorToString(error, errorMessage, sizeof errorMessage);
      Serial.println(errorMessage);
  }
  error = sensor.startPeriodicMeasurement();
  if (error != NO_ERROR) {
      Serial.print("Error trying to execute startPeriodicMeasurement(): ");
      errorToString(error, errorMessage, sizeof errorMessage);
      Serial.println(errorMessage);
  }

}

// Reads CO2, temperature, humidity data from SCD4X
void readSCD40Data(SensirionI2cScd4x& sensor, JsonDocument& doc) {
  bool dataReady = false;
  uint16_t co2Concentration = 0;
  float temperature = 0.0;
  float relativeHumidity = 0.0;

  error = sensor.getDataReadyStatus(dataReady);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute getDataReadyStatus(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }
  //
  // If ambient pressure compenstation during measurement
  // is required, you should call the respective functions here.
  // Check out the header file for the function definition.
  error = sensor.readMeasurement(co2Concentration, temperature, relativeHumidity);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }
  doc["co2_ppm"] = co2Concentration;
  doc["temp_c"] = temperature;
  doc["humidity"] = relativeHumidity;
}

// Reads ozone data
void readOzoneData(JsonDocument& doc) {
  int reading = analogRead(OZONE_PIN); //read from ozone pin
  doc["ozone"] = reading;
}


// TODO: complete this function 
void readPMSdata() {


}
