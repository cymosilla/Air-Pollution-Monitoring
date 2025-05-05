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


// Reads PM2.5 and PM1.0 ppm 
void readPMdata(JsonDocument& doc) {
  doc["pm1_mgm3"] = readPMForPin(PM1_PIN);
  doc["pm25_mgm3"] = readPMForPin(PM25_PIN);
}

// Reads the PM data on the give pin
float readPMForPin(const int pinNumber) {
  unsigned long startTime = millis();
  float lowPulse = 0;

  // Accumulate LOW pulses for the sample time
  while ((millis() - startTime) < PM_SAMPLE_TIME) {
    lowPulse += pulseIn(pinNumber, LOW) / 1000.0;  // Convert µs to ms
  }

  // Calculate low ratio (%)
  float lowRatio = (lowPulse / PM_SAMPLE_TIME) * 100.0;

  // Calculate concentration in mg/m³ using empirical formula
  float concentration = 0.00258425 * pow(lowRatio, 2) + 0.0858521 * lowRatio - 0.01345549;
  return max(0.0f, concentration);
}
