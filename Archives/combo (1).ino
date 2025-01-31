#include <SoftwareSerial.h>
 
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



// CO2 ********************************************************************************** 
/*!
 * @file  periodMeasure.ino
 * @brief  This sample shows how to configure period measurement mode, compensation and calibration.
 * @note  The actual compensation and calibration parameter should be changed according to the operating environment
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [qsjhyy](yihuan.huang@dfrobot.com)
 * @version  V1.0
 * @date  2022-05-11
 * @url  https://github.com/DFRobot/DFRobot_SCD4X
 */
#include <DFRobot_SCD4X.h>

/**
 * @brief Constructor
 * @param pWire - Wire object is defined in Wire.h, so just use &Wire and the methods in Wire can be pointed to and used
 * @param i2cAddr - SCD4X I2C address.
 */
DFRobot_SCD4X SCD4X(&Wire, /*i2cAddr = */SCD4X_I2C_ADDR);



// O3 ***********************************************************************************
  const int ozonePin = A3;  //Ozone in pin 3 on board







 
void setup() {


  // ALL
    // our debugging output
    Serial.begin(115200);

  // PM 2.5
    // sensor baud rate is 9600
    pmsSerial.begin(9600);


// CO2 ***********************************************************************
        // Init the sensor
        while( !SCD4X.begin() ){
          Serial.println("Communication with device failed, please check connection");
          delay(3000);
        }
        Serial.println("Begin ok!");

        /**
        * @brief set periodic measurement mode
        * @param mode - periodic measurement mode:
        * @n       SCD4X_START_PERIODIC_MEASURE : start periodic measurement, signal update interval is 5 seconds.
        * @n       SCD4X_STOP_PERIODIC_MEASURE : stop periodic measurement command
        * @n       SCD4X_START_LOW_POWER_MEASURE :  start low power periodic measurement, signal update interval is approximately 30 seconds.
        * @return None
        * @note The measurement mode must be disabled when configuring the sensor; after giving the stop_periodic_measurement command, the sensor needs to wait 500ms before responding to other commands.
        */
        SCD4X.enablePeriodMeasure(SCD4X_STOP_PERIODIC_MEASURE);

        /**
        * @brief set temperature offset
        * @details T(offset_actual) = T(SCD4X) - T(reference) + T(offset_previous)
        * @n T(offset_actual): the calculated actual temperature offset that is required
        * @n T(SCD4X): the temperature measured by the sensor (wait for a period of time to get steady readings)
        * @n T(reference): the standard reference value of the current ambient temperature
        * @n T(offset_previous): the previously set temperature offset
        * @n For example : 32(T(SCD4X)) - 30(T(reference)) + 2(T(offset_previous)) = 4(T(offset_actual))
        * @param tempComp - temperature offset value, unit ℃
        * @return None
        * @note When executing the command, the sensor can't be in period measurement mode
        */
        SCD4X.setTempComp(4.0);

        /**
        * @brief get temperature offset
        * @return The current set temp compensation value, unit ℃
        * @note When executing the command, the sensor can't be in period measurement mode
        */
        float temp = 0;
        temp = SCD4X.getTempComp();
        Serial.print("The current temperature compensation value : ");
        Serial.print(temp);
        Serial.println(" C");

        /**
        * @brief set sensor altitude
        * @param altitude - the current ambient altitude, unit m
        * @return None
        * @note When executing the command, the sensor can't be in period measurement mode
        */
        SCD4X.setSensorAltitude(540);

        /**
        * @brief get sensor altitude
        * @return The current set ambient altitude, unit m
        * @note When executing the command, the sensor can't be in period measurement mode
        */
        uint16_t altitude = 0;
        altitude = SCD4X.getSensorAltitude();
        Serial.print("Set the current environment altitude : ");
        Serial.print(altitude);
        Serial.println(" m");

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

        Serial.println();
    // CO2 END ********************************************************************


}


    
  
int o3count = 0;
int pmcount = 0;




void loop() {


    // O3**************************************************************************
      int reading = analogRead(ozonePin); //read from ozone pin

      if (o3count == 5000) {
      //detection range: 10-1000 ppb
        Serial.print("Ozone: ");
       Serial.print(reading);
        Serial.println(" ppb");
          o3count = 0;
      }
      else {
        o3count = o3count + 1;
      }

    // O3 END **********************************************************************


    // CO2 ************************************************************************* 
      /**
      * @brief get data ready status
      * @return data ready status:
      * @n        true : data ready
      * @n        false : data not ready
      */
      if(SCD4X.getDataReadyStatus()) {
        /**
        * @brief set ambient pressure
        * @param ambientPressure - the current ambient pressure, unit Pa
        * @return None
        */
        // SCD4X.setAmbientPressure(96000);

        /**
        * @brief Read the measured data
        * @param data - sSensorMeasurement_t, the values measured by the sensor, including CO2 concentration (ppm), temperature (℃) and humidity (RH)
        * @n  typedef struct {
        * @n    uint16_t   CO2ppm;
        * @n    float   temp;
        * @n    float   humidity;
        * @n  } sSensorMeasurement_t;
        * @return None
        * @note CO2 measurement range: 0~40000 ppm; temperature measurement range: -10~60 ℃; humidity measurement range: 0~100 %RH.
        */
        DFRobot_SCD4X::sSensorMeasurement_t data;
        SCD4X.readMeasurement(&data);

        Serial.print("Carbon dioxide concentration : ");
        Serial.print(data.CO2ppm);
        Serial.println(" ppm");

        Serial.print("Environment temperature : ");
        Serial.print(data.temp);
        Serial.println(" C");

        Serial.print("Relative humidity : ");
        Serial.print(data.humidity);
        Serial.println(" RH");

        Serial.println();
      }
    // CO2 END **********************************************************************

  // PM 2.5
    if (readPMSdata(&pmsSerial))
    {
    // reading data was successful!

    if (pmcount == 5) {

      Serial.println();
      Serial.println("---------------------------------------");
      Serial.println("Concentration Units (standard)");
      Serial.print("PM 1.0: "); Serial.print(dbunion.data.pm10_standard);
      Serial.print("\t\tPM 2.5: "); Serial.print(dbunion.data.pm25_standard);
      Serial.print("\t\tPM 10: "); Serial.println(dbunion.data.pm100_standard);
      Serial.println("---------------------------------------");
      Serial.println("Concentration Units (environmental)");
      Serial.print("PM 1.0: "); Serial.print(dbunion.data.pm10_env);
      Serial.print("\t\tPM 2.5: "); Serial.print(dbunion.data.pm25_env);
      Serial.print("\t\tPM 10: "); Serial.println(dbunion.data.pm100_env);
      Serial.println("---------------------------------------");
      Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(dbunion.data.particles_03um);
      Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(dbunion.data.particles_05um);
      Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(dbunion.data.particles_10um);
      Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(dbunion.data.particles_25um);
      Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(dbunion.data.particles_50um);
      Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(dbunion.data.particles_100um);
      Serial.println("----------------------------------------------------------------------------------");

      pmcount = 0;

    }
    else
      pmcount = pmcount + 1;
    }


}
 






boolean readPMSdata(Stream *s)
{
  if (! s->available())
  {
    return false;
  }
 
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42)
  {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32)
  {
    return false;
  }
 
  s->readBytes(dbunion.buffer, 32);
 
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
    Serial.println("Checksum failure");
    return false;
  }
 
  // success!
  return true;
}