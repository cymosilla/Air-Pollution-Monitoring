#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define CHUNK_SIZE 32
#define BUFFER_TIMEOUT 10000

DynamicJsonDocument doc(2048);

#define READING_PERIOD 10000 // This sets the time period of reading data from the sensors. 10000 means 10 seconds

// TODO: Need to create macros for the serial ports
SoftwareSerial pmsSerial(2, 3);
 
struct pms5003data
{
  uint16_t data_marker;     // this accepts the 16 bit "marker" value
  uint16_t framelen;
  uint16_t pm10_standard;
  uint16_t pm25_standard;
  uint16_t pm100_standard;
  uint16_t pm10_env;
  uint16_t pm25_env;
  uint16_t pm100_env;
  uint16_t particles_03um;
  uint16_t particles_05um;
  uint16_t particles_10um;
  uint16_t particles_25um;
  uint16_t particles_50um;
  uint16_t particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
 
union
{
  struct pms5003data data;    // access this as dbunion.data
  uint8_t buffer[32];         //    and this as dbunion.buffer
} dbunion;


#include <DFRobot_SCD4X.h>

DFRobot_SCD4X SCD4X(&Wire, SCD4X_I2C_ADDR);

// O3 ***********************************************************************************
const int ozonePin = A3;  //Ozone in pin 3 on board

 
void setup() {
  // Initialize ESP32 as hardware serial
  Serial.begin(9600);

  // PM 2.5
  // sensor baud rate is 9600
  pmsSerial.begin(9600);


  // CO2 ***********************************************************************
  // Init the sensor
  while( !SCD4X.begin() ){
    // Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  // Serial.println("Begin ok!");

  SCD4X.enablePeriodMeasure(SCD4X_STOP_PERIODIC_MEASURE);

  SCD4X.setTempComp(4.0);

  float temp = 0;
  temp = SCD4X.getTempComp();
  // Serial.print("The current temperature compensation value : ");
  // Serial.print(temp);
  // Serial.println(" C");

  SCD4X.setSensorAltitude(540);

  uint16_t altitude = 0;
  altitude = SCD4X.getSensorAltitude();
  // Serial.print("Set the current environment altitude : ");
  // Serial.print(altitude);
  // Serial.println(" m");

  /**
  * @brief set automatic self calibration enabled
  * @param mode - automatic self-calibration mode:
  * @n       true : enable automatic self-calibration
  * @n       false : disable automatic self-calibration
  * @return None
  * @note When executing the command, the sensor can't be in period measurement mode
  */
  // SCD4X.setAutoCalibMode(true);

  /**
  * @brief get automatic self calibration enabled
  * @return Automatic self-calibration mode:
  * @n        true : enable automatic self-calibration
  * @n        false : disable automatic self-calibration
  * @note When executing the command, the sensor can't be in period measurement mode
  */
  // if(SCD4X.getAutoCalibMode()) {
  //   Serial.println("Automatic calibration on!");
  // } else {
  //   Serial.println("Automatic calibration off!");
  // }

  /**
  * @brief persist settings
  * @details Configuration settings such as the temperature offset, sensor altitude and the ASC enabled/disabled 
  * @n  parameter are by default stored in the volatile memory (RAM) only and will be lost after a power-cycle.
  * @return None
  * @note To avoid unnecessary wear of the EEPROM, the persist_settings command should only be sent 
  * @n  when persistence is required and if actual changes to the configuration have been made. 
  * @n  The EEPROM is guaranteed to endure at least 2000 write cycles before failure.
  * @note Command execution time : 800 ms
  * @n When executing the command, the sensor can't be in period measurement mode
  */
  // SCD4X.persistSettings();

  /**
  * @brief reinit reinit
  * @details  The reinit command reinitializes the sensor by reloading user settings from EEPROM.
  * @return None
  * @note Before sending the reinit command, the stop measurement command must be issued.
  * @n  If the reinit command does not trigger the desired re-initialization, 
  * @n  a power-cycle should be applied to the SCD4x.
  * @n  Command execution time : 20 ms
  * @n When executing the command, the sensor can't be in period measurement mode
  */
  //SCD4X.moduleReinit();

  /**
  * @brief set periodic measurement mode
  * @param mode - periodic measurement mode:
  * @n       SCD4X_START_PERIODIC_MEASURE : start periodic measurement, signal update interval is 5 seconds.
  * @n       SCD4X_STOP_PERIODIC_MEASURE : stop periodic measurement command
  * @n       SCD4X_START_LOW_POWER_MEASURE :  start low power periodic measurement, signal update interval is approximately 30 seconds.
  * @return None
  * @note The measurement mode must be disabled when changing the sensor settings; after giving the stop_periodic_measurement command, the sensor needs to wait 500ms before responding to other commands.
  */
  SCD4X.enablePeriodMeasure(SCD4X_START_PERIODIC_MEASURE);

  // Serial.println();
  // CO2 END ********************************************************************


}

unsigned long timer = millis();


void loop() {

    // Gets current time
    unsigned long current_time = millis();

    // Gets readings if the time difference is greater than READING_PERIOD
    if (current_time - timer > READING_PERIOD) {
      
      // Ozone readings
      readOzoneData(doc);

      // PMS readings
      readPMSdata(doc);

      // CO2 readings
      readCO2data(doc);

      // Sends data to esp32
      serializeJson(doc, Serial);
      doc.clear();

      timer = current_time;
    }
}

void readOzoneData(DynamicJsonDocument& doc) {
  int reading = analogRead(ozonePin); //read from ozone pin
  doc["ozone"] = reading;
}

void readCO2data(DynamicJsonDocument& doc) {
  if(SCD4X.getDataReadyStatus()) {
    DFRobot_SCD4X::sSensorMeasurement_t data;
    SCD4X.readMeasurement(&data);

    doc["co2_ppm"] = data.CO2ppm;
    doc["temp_c"] = data.temp;
    doc["humidity"] = data.humidity;
  }
}

boolean readPMSdata(DynamicJsonDocument& doc)
{
  Stream& s = pmsSerial;
  if (!s.available())
  {
    return false;
  }
 
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s.peek() != 0x42)
  {
    s.read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s.available() < 32)
  {
    return false;
  }
 
  s.readBytes(dbunion.buffer, 32);
 
  // calculate expected checksum, don't sum the final "checksum" field
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 30; ++i) 
  {
    sum += dbunion.buffer[i];
  }
 
  /* debugging
  for (uint8_t i=0; i<32; i++)  // print ALL of the data
  {
    Serial.print("0x");
    Serial.print(dbunion.buffer[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  */
 
  // The data comes in endian'd, this solves it so it works on all platforms
  for (uint8_t i = 0; i < 32; i += 2)
  {
    uint8_t temp = dbunion.buffer[i];
    dbunion.buffer[i] = dbunion.buffer[i+1];
    dbunion.buffer[i+1] = temp;
  }
 
  if (sum != dbunion.data.checksum)
  {
    // Serial.println("Checksum failure");
    return false;
  }
 
  // success!
  doc["pm10_standard"] = dbunion.data.pm10_standard;
  doc["pm25_standard"] = dbunion.data.pm25_standard;
  doc["pm100_standard"] = dbunion.data.pm100_standard;
  doc["pm10_env"] = dbunion.data.pm10_env;
  doc["pm25_env"] = dbunion.data.pm25_env;
  doc["pm100_env"] = dbunion.data.pm100_env;
  doc["particles_03um"] = dbunion.data.particles_03um;
  doc["particles_05um"] = dbunion.data.particles_05um;
  doc["particles_10um"] = dbunion.data.particles_10um;
  doc["particles_25um"] = dbunion.data.particles_25um;
  doc["particles_50um"] = dbunion.data.particles_50um;
  doc["particles_100um"] = dbunion.data.particles_100um;

  return true;
}
