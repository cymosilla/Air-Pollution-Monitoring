#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <DFRobot_SCD4X.h>
#include <SPI.h>
#include <SD.h>

const String UNPROCESSED_FILE_PATH = "unprocessed_data.csv";
const String PROCESSED_FILE_PATH = "processed_data.csv";
File csvFile;

DynamicJsonDocument doc(2048);
const int fieldCount = 16;
const String jsonFields[fieldCount] = {
  "ozone",
  "co2_ppm",
  "temp_c",
  "humidity",
  // "pm10_standard", // TODO: change these fields to reflect new PM2.5 sensor readings
  // "pm25_standard", 
  // "pm100_standard", 
  // "pm10_env", 
  // "pm25_env", 
  // "pm100_env", 
  // "particles_03um", 
  // "particles_05um", 
  // "particles_10um", 
  // "particles_25um", 
  // "particles_50um", 
  // "particles_100um"
};


#define READING_PERIOD 10000 // This sets the time period of reading data from the sensors. 10000 means 10 seconds

// TODO: Need to create macros for the serial ports
SoftwareSerial pmsSerial(2, 3);
 

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

  SCD4X.setSensorAltitude(540);

  SCD4X.enablePeriodMeasure(SCD4X_START_PERIODIC_MEASURE); 

  // CO2 END ********************************************************************


}

unsigned long timer = millis();


void loop() {

    // Gets current time
    unsigned long current_time = millis();

    // Gets readings if the time difference is greater than READING_PERIOD
    if (current_time - timer > READING_PERIOD) {
      
      // Get readings
      readOzoneData(doc);
      readPMSdata(doc);
      readCO2data(doc);

      // Sends data to esp32
      serializeJson(doc, Serial);

      // Saves data to csv
      saveToCSV(UNPROCESSED_FILE_PATH);

      // Optional debug statement
      printFromCSV(UNPROCESSED_FILE_PATH);

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

// TODO: complete this function 
void readPMSdata(DynamicJsonDocument& doc) {


}

// Prints all the content of the file as 
void printFromCSV(const String& filePath) {
  String csvData = "";
  csvFile = SD.open(filePath);
  if (csvFile) {
    while (csvFile.available()) {
      char newChar = csvFile.read();
      if (newChar == '\n') {
        Serial.println(csvData);
        csvData = "";
      }
      else {
        csvData += newChar;
      }
    }
    csvFile.close();
  }
  else {
    Serial.println("Failed to open: " + filePath);
  }
  
}

void createCSV(const String& filePath) {
  // Create the specified CSV file. Overwrite if it exists
  if (SD.exists(filePath)) {
    SD.remove(filePath);
  }

  String csvHeader = "";

  // Create the header row
  for (int i = 0; i < fieldCount; i++) {
    csvHeader += jsonFields[i];
    if (i != fieldCount - 1) {
      csvHeader += ",";
    }
  }
  
  writeRowToCSV(csvHeader, filePath);
}

void saveToCSV(const String& filePath) {
  String newRow = "";

  for (int i = 0; i < fieldCount; i++) {
    String currentField = jsonFields[i];

    if (doc[currentField].is<JsonVariant>()) {
      newRow += doc[currentField].as<String>();
    }
    else {
      newRow += "null";
    }

    if (i != fieldCount - 1) {
      newRow += ",";
    }
  }

  // Debug statement
  Serial.println("Writing new row to: " + filePath);
  Serial.println(newRow);

  // Creates a new csv file if it doesn't exist yet
  if (!SD.exists(filePath)) {
    createCSV(filePath);
  }

  writeRowToCSV(newRow, filePath);
}

// Write data as a new row to the specified file
void writeRowToCSV(const String& data, const String& filePath) {
  csvFile = SD.open(filePath, FILE_WRITE);
  if (csvFile) {
    csvFile.println(data);
    csvFile.close();
  }
  else {
    Serial.println("Failed to open: " + filePath);
  }
}