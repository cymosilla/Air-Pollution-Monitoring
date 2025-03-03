#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define LED 2
#define RX 26
#define TX 25

#define BUFFER_THRESHOLD 200 // Minimum characters on buffer required to trigger JSON reception

#define HTTP_TIMEOUT 8000 // Must be less than the READING_PERIOD in combo.ino

#define WIFI_SSID "UCInet Mobile Access"
#define WIFI_PASSWORD ""
#define WIFI_CHANNELS 6

#define URL_ENDPOINT "https://httpbin.org/post" // Test url for reflecting the POST data

DynamicJsonDocument doc(512);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX, TX);

  connectWiFi(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNELS);
  Serial.println("Setup done");
}
 
void loop() {
  // Checks if the buffer thas more than BUFFER_THRESHOLD
  while (Serial2.available() > BUFFER_THRESHOLD) {
    // Receive data
    receiveJsonData(doc, Serial2);
    if (doc.isNull()) {
      break;
    }

    // Debug statement
    // serializeJsonPretty(doc, Serial);
    
    // Send data if the content is valid
    if (verifyJsonContent(doc)) {
      sendDataToURL(URL_ENDPOINT, doc);
    }
    else {
      Serial.println("Data received but is invalid");
    }
      
  }
  delay(100);
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

// Receives json data from input. Stores into doc.
void receiveJsonData(DynamicJsonDocument& doc, Stream& input) {
  // Clears the document. If data reception fails, it will remain empty. 
  // Use its emptiness to determine if the process was successful
  doc.clear();
  String data = input.readStringUntil('\n');
  DeserializationError error = deserializeJson(doc, data);

  // Clears doc if the data parsing fails
  if (error) {
    Serial.print(F("deserializeJson failed: "));
    Serial.println(error.f_str());
    doc.clear();
    return;
  }
}

// Sends data to http endpoint
void sendDataToURL(const String& url, DynamicJsonDocument& doc) {
  // Turns on LED to indicate uploading status
  setLED(true);

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

  // Turns off LED
  setLED(false);

}

// Checks if the json document is valid before sending
// Defaults to true for now
bool verifyJsonContent(DynamicJsonDocument& doc) {
  return true;
}

// Sets LED to on or off
void setLED(bool led_status) {
  if (led_status) {
    digitalWrite(LED, LOW);
  }
  else {
    digitalWrite(LED, HIGH);
  }
}