#include "RTClib.h"                                 // RTC library
#include "Adafruit_PM25AQI.h"                       // PM 2.5 library
#include <MQ131.h>                                  // ozone library 

// If your PM2.5 is UART only, for UNO and others (without hardware serial)
// we must use software serial...
// pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
#include <SoftwareSerial.h>

const int ozonePin = A3;                           // OZONE: sensor in analog pin 3 on board. Do NOT use A5. 

RTC_PCF8523 rtc;                                   // RTC: creates RTC object (creates clock that gets all the times, etc.)

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

SoftwareSerial pmSerial(2, 3);                     // PM: establishes serial (the type of communication between the sensor and the board)
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();         // PM: establishes the aqi

void setup() {                                // beginning of general setup
  // Wait for serial monitor to open
  Serial.begin(9600);

  //make sure clock works before setting up PM 2.5
      // we need time for readings
  rtc_setup();                                    // calls rtc setup function (below)
  pm25_setup();                                   // calls PM 2.5 setup function (below)
}                                            // end of general setup

void loop() {                                // beginning of general loop
  DateTime now = rtc_reading();                   // establishes variable "now" as a DateTime variable. The variable values are found using the function rtc_reading (below)
  PM25_AQI_Data pm_ppb = pm25_reading();         // establishes variable "pm_ppb" as a AQI Data variable. The variable values are found using the function pm25_reading (below)
  int ozone_ppb = analogRead(ozonePin);          // establishes variable "ozone_ppb" as an integer variable. The variable values are found by reading from the ozone pin 

  print_data(now, pm_ppb, ozone_ppb);           // displays all of the data from the RTC, PM 2.5, and ozone sensor in the Serial Monitor window (for us to see)
  //save_data(now, pm_ppb, ozone_ppb);

  delay(1000);
}                                            // end of general loop

// **below are functions that we call in the general code above.**


void rtc_setup() {                                             // RTC: the process of setting up / calibrating the RTC

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  delay(2000);

  if (!rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  rtc.start();
}

DateTime rtc_reading() {
  DateTime now_reading = rtc.now();

  return now_reading;
}


void pm25_setup() {                                            // PM: the process of setting up / calibrating the PM 2.5 sensor
  Serial.println("Adafruit PMSA003I Air Quality Sensor");
  delay(1000);  // Wait one second for sensor to boot up
  pmSerial.begin(9600);

  if (!aqi.begin_UART(&pmSerial)) {  // connect to the sensor over software serial
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("PM25 found!");
}

PM25_AQI_Data pm25_reading() {
  PM25_AQI_Data pm_ppb;

  if (!aqi.read(&pm_ppb)) {  // !=not operator, &=referencing
    Serial.println("Could not read from AQI");
    delay(1000);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");

  return pm_ppb;
}



void print_data(DateTime now, PM25_AQI_Data pm_ppb, int ozone_ppb) {             // the process of displaying all of the data 
  
  Serial.println(F("\n***************************************\n"));       // separator line between each new set of data 
  Serial.print(now.year(), DEC);                                          // displaying RTC data 
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  Serial.println(F("_______________________________________"));         // separator between RTC and PM data 

  Serial.println(F("Concentration Units (standard)"));                  // displaying PM data 
  Serial.println(F("---------------------------------------"));
  Serial.print(F("PM 1.0: "));
  Serial.print(pm_ppb.pm10_standard);
  Serial.print(F("\t\tPM 2.5: "));
  Serial.print(pm_ppb.pm25_standard);
  Serial.print(F("\t\tPM 10: "));
  Serial.println(pm_ppb.pm100_standard);
  Serial.println(F("Concentration Units (environmental)"));
  Serial.println(F("---------------------------------------"));
  Serial.print(F("PM 1.0: "));
  Serial.print(pm_ppb.pm10_env);
  Serial.print(F("\t\tPM 2.5: "));
  Serial.print(pm_ppb.pm25_env);
  Serial.print(F("\t\tPM 10: "));
  Serial.println(pm_ppb.pm100_env);
  Serial.println(F("---------------------------------------"));
  Serial.print(F("Particles > 0.3um / 0.1L air:"));
  Serial.println(pm_ppb.particles_03um);
  Serial.print(F("Particles > 0.5um / 0.1L air:"));
  Serial.println(pm_ppb.particles_05um);
  Serial.print(F("Particles > 1.0um / 0.1L air:"));
  Serial.println(pm_ppb.particles_10um);
  Serial.print(F("Particles > 2.5um / 0.1L air:"));
  Serial.println(pm_ppb.particles_25um);
  Serial.print(F("Particles > 5.0um / 0.1L air:"));
  Serial.println(pm_ppb.particles_50um);
  Serial.print(F("Particles > 10 um / 0.1L air:"));
  Serial.println(pm_ppb.particles_100um);
  Serial.println(F("_______________________________________"));         // separator between PM and ozone data 

  Serial.println(F("Ozone: ")); 
  Serial.print(ozone_ppb);                                              // displaying ozone data 
  Serial.println(" ppb");
}
