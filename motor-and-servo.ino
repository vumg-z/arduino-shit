#include <ESP8266WiFi.h>  // Include the ESP8266 WiFi library
#include <Servo.h>        // Include the Servo library
#include <ESP8266HTTPClient.h> // Include the HTTP client library
#include <WiFiClientSecure.h> // Include the WiFiClientSecure library
#include <ArduinoJson.h>  // Include the ArduinoJson library
#include <Stepper.h>      // Include the Stepper library

// Replace with your network credentials
const char* ssid = "Chinito";
const char* password = "vlogs123";

// Create a Servo object
Servo myservo;

// Define the servo pin
const int servoPin = 16;  // GPIO16 corresponds to D0

// Define the stepper motor pins
const int stepPin1 = 5;  // GPIO5
const int stepPin2 = 1;  // GPIO1

// Define the number of steps per revolution for your stepper motor (adjust as needed)
const int stepsPerRevolution = 200;

// Create a Stepper object
Stepper myStepper(stepsPerRevolution, stepPin1, stepPin2);

void setup() {
  // Start the serial communication to monitor the connection process
  Serial.begin(115200);
  
  // Begin WiFi connection
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // When connected, print the IP address
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Attach the servo to the pin
  myservo.attach(servoPin);
  Serial.println("Servo attached to pin 16");

  // Set the stepper motor speed (RPM)
  myStepper.setSpeed(60);
  Serial.println("Stepper motor initialized");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client; // Declare a WiFiClientSecure object
    client.setInsecure(); // Disable SSL certificate verification

    HTTPClient http;
    http.begin(client, "https://verlo.co/servo-pasos/update_state.php"); // Replace with your server address

    int httpCode = http.GET(); // Send the GET request

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      // Parse the JSON payload to extract the movement type
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        String movement = doc["movement"];
        Serial.println("Movement: " + movement);

        if (movement == "sweep") {
          Serial.println("Sweep movement detected");
          sweepMovement();
        } else if (movement == "oscillate") {
          Serial.println("Oscillate movement detected");
          oscillateMovement();
        } else if (movement == "linear_sweep") {
          Serial.println("Linear sweep movement detected");
          linearSweepMovement();
        } else if (movement == "stepper_sweep") {
          Serial.println("Stepper sweep movement detected");
          stepperSweepMovement();
        } else if (movement == "stepper_oscillate") {
          Serial.println("Stepper oscillate movement detected");
          stepperOscillateMovement();
        } else {
          Serial.println("Unknown movement detected");
        }
      } else {
        Serial.println("Failed to parse JSON");
      }
    } else {
      Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  delay(1000); // Check every second
}

void sweepMovement() {
  Serial.println("Performing sweep movement");
  myservo.write(0);
  delay(1000);

  myservo.write(90);
  delay(1000);

  myservo.write(180);
  delay(1000);

  myservo.write(90);
  delay(1000);
}

void oscillateMovement() {
  Serial.println("Performing oscillate movement");
  for (int pos = 0; pos <= 180; pos += 1) { 
    myservo.write(pos);             
    delay(15);                       
  }
  for (int pos = 180; pos >= 0; pos -= 1) { 
    myservo.write(pos);              
    delay(15);                       
  }
}

void linearSweepMovement() {
  Serial.println("Performing linear sweep movement");
  myservo.write(45);
  delay(1000);

  myservo.write(135);
  delay(1000);

  myservo.write(45);
  delay(1000);

  myservo.write(135);
  delay(1000);
}

void stepperSweepMovement() {
  Serial.println("Performing stepper sweep movement");
  myStepper.step(stepsPerRevolution);   // Move one revolution forward
  delay(1000);

  myStepper.step(-stepsPerRevolution);  // Move one revolution backward
  delay(1000);
}

void stepperOscillateMovement() {
  Serial.println("Performing stepper oscillate movement");
  for (int i = 0; i < 10; i++) {
    myStepper.step(stepsPerRevolution / 10);   // Move a fraction forward
    delay(100);
    myStepper.step(-stepsPerRevolution / 10);  // Move a fraction backward
    delay(100);
  }
}
