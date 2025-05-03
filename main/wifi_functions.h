#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "macros.h"
#include <ArduinoJson.h>

void connectWiFi(const String& ssid, const String& password, const int& channels);
void sendDataToURL(const String& url, JsonDocument& doc);

#endif