#include "wifi_functions.h"


const String& ssid = "UCInet Mobile Access";
const String& password = "";
const int& channels = 6;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  connectWiFi(ssid, password, channels);
}

void loop() {
  if (Serial2.available()) {
    String incoming = Serial2.readStringUntil('\n');

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, incoming);

    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
    }

    sendDataToURL(URL_ENDPOINT, doc);
  }
}
