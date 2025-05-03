#include "wifi_functions.h"

// Connects to WiFi
void connectWiFi(const String& ssid, const String& password, const int& channels) {
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, channels);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConnected");

}

// Sends data to http endpoint
void sendDataToURL(const String& url, JsonDocument& doc) {
  String data;
  serializeJson(doc, data);

  HTTPClient http;
  http.setTimeout(HTTP_TIMEOUT);
  http.begin(url);

  http.addHeader("Content-Type", "application/json");
  int response_code = http.POST(data);
  if (response_code == 200) {
    Serial.println(http.getString());
  }
  else {
    Serial.print("Post failed: code ");
    Serial.println(response_code);
  }

  http.end();

}