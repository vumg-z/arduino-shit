#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "";
const char* password = "";

String deviceIp;
String lastGoodData;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("ESP8266 Ready");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  deviceIp = WiFi.localIP().toString(); // Get the local IP address
  Serial.println("IP Address: " + deviceIp);
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    Serial.println("Received: " + message);

    // Parse the received message
    StaticJsonDocument<200> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, message);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String sensorData;
    serializeJson(jsonDoc, sensorData);
    Serial.println("Parsed sensor data: " + sensorData);

    // Check for null values in the parsed data
    if (!jsonDoc.containsKey("S0") || !jsonDoc.containsKey("S1") || !jsonDoc.containsKey("S2")) {
      // Use the last known good data if current data is incomplete
      sensorData = lastGoodData;
      Serial.println("Using last known good data: " + sensorData);
    } else {
      // Update the last known good data
      lastGoodData = sensorData;
    }

    // Send HTTPS POST request with sensor data and IP address
    if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
      WiFiClientSecure client; // Declare a WiFiClientSecure object

      client.setInsecure(); // Disable SSL certificate verification

      HTTPClient http; // Declare an object of class HTTPClient

      http.begin(client, "https://verlo.co/estacionamiento/test.php"); // Specify the URL with https
      http.addHeader("Content-Type", "application/json"); // Specify content-type header

      String httpRequestData = "{\"sensor_data\":" + sensorData + ",\"ip_address\":\"" + deviceIp + "\"}";
      int httpResponseCode = http.POST(httpRequestData); // Send the request

      // If you need to check the response
      if (httpResponseCode > 0) {
        String response = http.getString(); // Get the response to the request
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }

      http.end(); // Free resources
    } else {
      Serial.println("WiFi Disconnected");
    }
  }

  delay(500); // Add a delay to allow for complete data to be received and processed
}
