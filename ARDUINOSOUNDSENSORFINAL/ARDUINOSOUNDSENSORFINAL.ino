#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Hijikata-san";             // Replace with your WiFi SSID
const char* password = "huhuhuhuhu";           // Replace with your WiFi password
const char* server = "192.168.71.247";          // Replace with your server IP address

const int soundSensorPin = A0;                // Analog pin for sound sensor

// Define LED pins (2 red, 2 green, 2 orange)
const int ledPins[] = {D0, D1, D2, D3, D4, D5, D6, D7}; // Pins connected to LEDs
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);

  // Initialize LED pins as outputs
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Ensure all LEDs are off initially
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int soundLevel = analogRead(soundSensorPin); // Read sound level
  int invertedSoundLevel = 1023 - soundLevel; // Invert the sound level reading

  Serial.print("Sound Level: ");
  Serial.println(invertedSoundLevel);

  // Determine which LEDs to light up based on the sound level
  if (invertedSoundLevel < 200) {
    lightUpLeds(0); // No LEDs on
  } else if (invertedSoundLevel < 400) {
    lightUpLeds(1); // 1 red LED
  } else if (invertedSoundLevel < 600) {
    lightUpLeds(2); // 2 red LEDs
  } else if (invertedSoundLevel < 700) {
    lightUpLeds(3); // 2 red LEDs + 1 green LED
  } else if (invertedSoundLevel < 800) {
    lightUpLeds(4); // 2 red LEDs + 2 green LEDs
  } else if (invertedSoundLevel < 900) {
    lightUpLeds(6); // 2 red LEDs + 2 green LEDs + 1 orange LED
  } else {
    lightUpLeds(8); // All LEDs on
  }

  // Send data to the backend server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(server) + ":3000/api/audio";
    http.begin(wifiClient, url); // Correct usage of http.begin with WiFiClient and URL
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["level"] = invertedSoundLevel;
    String postData;
    serializeJson(jsonDoc, postData);

    int httpResponseCode = http.POST(postData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
  
  // No delay to ensure real-time response
}

void lightUpLeds(int numLedsToLight) {
  for (int i = 0; i < numLeds; i++) {
    if (i < numLedsToLight) {
      digitalWrite(ledPins[i], HIGH); // Turn on LED
    } else {
      digitalWrite(ledPins[i], LOW); // Turn off LED
    }
  }
}
