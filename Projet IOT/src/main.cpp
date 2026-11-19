#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <time.h>

#include "secrets.h"

// Serial communication speed
constexpr unsigned long SERIAL_BAUD_RATE = 115200;

// Time intervals in milliseconds
constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 5000;
constexpr unsigned long MEASUREMENT_INTERVAL_MS = 10000;

// HTTP response code
constexpr int HTTP_OK_CODE = 200;

// NTP configuration
constexpr char NTP_SERVER[] = "pool.ntp.org";
constexpr long GMT_OFFSET_SEC = 0;
constexpr int DAYLIGHT_OFFSET_SEC = 0;

// The ESP32-S3 does not have a built-in humidity sensor.
// Humidity remains simulated until an external sensor is connected.
constexpr float SIMULATED_HUMIDITY = 20.2f;

// Timing variables
unsigned long lastWifiAttempt = 0;
unsigned long lastMeasurement = 0;

// NTP initialization status
bool ntpConfigured = false;


// Send measurement data to the FastAPI server
void sendMeasurement(float temperature, float humidity, const char* timestamp) {

    JsonDocument document;

    document["temperature"] = temperature;
    document["humidity"] = humidity;
    document["timestamp"] = timestamp;

    String jsonBody;
    serializeJson(document, jsonBody);

    HTTPClient http;

    if (!http.begin(SERVER_URL)) {
        Serial.println("Failed to initialize HTTP client.");
        return;
    }

    http.setTimeout(5000);
    http.addHeader("Content-Type", "application/json");

    Serial.print("Target URL: ");
    Serial.println(SERVER_URL);

    int statusCode = http.POST(jsonBody);

    if (statusCode == HTTP_OK_CODE) {
        Serial.println("HTTP POST: 200 OK");
    }
    else {
        Serial.print("HTTP error code: ");
        Serial.println(statusCode);

        Serial.print("HTTP error description: ");
        Serial.println(http.errorToString(statusCode));
    }

    http.end();
}


// Initialize serial communication and Wi-Fi
void setup() {

    Serial.begin(SERIAL_BAUD_RATE);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    lastWifiAttempt = millis();

    Serial.println("Connecting to Wi-Fi...");
}


// Main program loop
void loop() {

    // Check Wi-Fi connection
    if (WiFi.status() != WL_CONNECTED) {

        if (millis() - lastWifiAttempt >= WIFI_RECONNECT_INTERVAL_MS) {

            lastWifiAttempt = millis();

            Serial.println("Wi-Fi disconnected. Reconnecting...");
            WiFi.reconnect();
        }

        delay(100);
        return;
    }

    // Configure NTP after connecting to Wi-Fi
    if (!ntpConfigured) {

        Serial.print("Wi-Fi connected. ESP32 IP: ");
        Serial.println(WiFi.localIP());

        configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

        ntpConfigured = true;
    }

    // Wait until the next measurement interval
    if (millis() - lastMeasurement < MEASUREMENT_INTERVAL_MS) {
        delay(100);
        return;
    }

    // Retrieve current time from NTP
    struct tm timeInfo;

    if (!getLocalTime(&timeInfo, 100)) {
        Serial.println("Waiting for NTP synchronization...");
        delay(500);
        return;
    }

    // Format timestamp using ISO 8601
    char timestamp[25];

    strftime(
        timestamp,
        sizeof(timestamp),
        "%Y-%m-%dT%H:%M:%SZ",
        &timeInfo
    );

    // Read the internal ESP32-S3 chip temperature
    float temperature = temperatureRead();

    // Humidity is currently simulated
    float humidity = SIMULATED_HUMIDITY;

    lastMeasurement = millis();

    Serial.print("Internal chip temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    // Send measurement to FastAPI
    Serial.println("Sending HTTP POST...");

    sendMeasurement(temperature, humidity, timestamp);
}
