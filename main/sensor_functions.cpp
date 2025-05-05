#include "sensor_functions.h"

static char errorMessage[64];
static int16_t error;

// Prints SCD40 error to main serial
void showError() {
  if (error != NO_ERROR) {
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
  }
}

// Initializes the DFRobot_SCD4X sensor
void initSCD40(SensirionI2cScd4x& sensor) {
  
  // Ensure sensor is in clean state
  error = sensor.wakeUp();
  showError();

  error = sensor.stopPeriodicMeasurement();
  showError();

  error = sensor.setSensorAltitude(IRVINE_ALTITUDE);
  showError();

  error = sensor.reinit();
  showError();

  error = sensor.startPeriodicMeasurement();
  showError();

}

// Reads CO2, temperature, humidity data from SCD4X
void readSCD40Data(SensirionI2cScd4x& sensor, JsonDocument& doc) {
  bool dataReady = false;
  uint16_t co2Concentration = 0;
  float temperature = 0.0;
  float relativeHumidity = 0.0;

  error = sensor.getDataReadyStatus(dataReady);
  showError();

  if (!dataReady) return;

  // If ambient pressure compenstation during measurement
  // is required, you should call the respective functions here.
  // Check out the header file for the function definition.
  error = sensor.readMeasurement(co2Concentration, temperature, relativeHumidity);
  showError();
  
  doc["co2_ppm"] = co2Concentration;
  doc["temp_c"] = temperature;
  doc["humidity"] = relativeHumidity;
}

// Reads ozone data
void readOzoneData(JsonDocument& doc) {
  doc["ozone"] = analogRead(OZONE_PIN); //read from ozone pin
}


// Reads PM2.5 and PM1.0 ppm 
void readPMdata(JsonDocument& doc) {
  unsigned long pmMeasureStartTime = millis();
  float pm1LowPulseOccupancy = 0;
  float pm25LowPulseOccupancy = 0;

  // Accumulate LOW pulses for the sample time
  while ((millis() - pmMeasureStartTime) < PM_SAMPLE_TIME) {
    pm1LowPulseOccupancy += pulseIn(PM1_PIN, LOW) / 1000.0;  // Convert µs to ms
    pm25LowPulseOccupancy += pulseIn(PM25_PIN, LOW) / 1000.0;  // Convert µs to ms
  }

  // Calculate low ratio (%)
  float pm1LowRatio = (pm1LowPulseOccupancy / PM_SAMPLE_TIME) * 100.0;
  float pm25LowRatio = (pm25LowPulseOccupancy / PM_SAMPLE_TIME) * 100.0;


  // Calculate concentration in mg/m³ using empirical formula
  float pm1Concentration = 0.00258425 * pow(pm1LowRatio, 2) + 0.0858521 * pm1LowRatio - 0.01345549;
  float pm25Concentration = 0.00258425 * pow(pm25LowRatio, 2) + 0.0858521 * pm25LowRatio - 0.01345549;

  doc["pm1_mgm3"] = pm1Concentration;
  doc["pm25_mgm3"] = pm25Concentration;
}

