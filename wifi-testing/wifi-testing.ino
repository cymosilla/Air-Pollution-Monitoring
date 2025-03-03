#include <WiFi.h>
#include <ArduinoJson.h>

#define RX 26
#define TX 25

#define BUFFER_TIMEOUT 1000
#define HTTP_TIMEOUT 8000 // Must be less than the READING_PERIOD in combo.ino

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define WIFI_CHANNELS 6

#define URL_ENDPOINT "https://v2.jokeapi.dev/joke/Programming"

enum class Status {
  idle,
  uploading
};

DynamicJsonDocument doc(512);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX, TX);

  connectWiFi(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNELS);
  Serial.println("Setup done");
}
 
void loop() {
  waitForBuffer(Serial2, BUFFER_TIMEOUT);
  receiveJsonData(doc, Serial2);
  if (doc.isNull()) {
    Serial.println("No data received");
  }
  else {
    serializeJsonPretty(doc, Serial);
  }
}

// Connects to WiFi
void connectWiFi(const String& ssid, const String& password, const int& channels) {
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, channels);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConnected");

}

// Receives json data in chunks
void receiveJsonData(DynamicJsonDocument& doc, Stream& input) {
  // Clears the document. If data reception fails, it will remain empty. 
  // Use its emptiness to determine if the process was successful
  doc.clear();
  String data = input.readStringUntil('\n');
  DeserializationError error = deserializeJson(doc, data);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson failed: "));
    Serial.println(error.f_str());
    doc.clear();
    return;
  }
}

// Waits for input. Returns true if there is data on the input stream buffer before timeout is reached
bool waitForBuffer(Stream& stream, const int& timeout) {
  int start_time = millis();
  int current_time = millis();

  while (stream.available() == 0 && current_time - start_time < timeout) {
    delay(100);
    current_time = millis();
  }
  return stream.available() > 0;
}


