#include <Servo.h>

Servo myServo;

// Ultrasonic sensor pins
const int trigPin = 4;
const int echoPin = 3;

// Servo parameters
const int servoPin = 2;
const int centerAngle = 120;
const int angleRange = 20;  // ±10 degrees from the center
const int minAngle = centerAngle - angleRange / 2;
const int maxAngle = centerAngle + angleRange / 2;

// Control parameters
const int targetDistance = 10;
const int numReadings = 10;
const float Kp = 0.5;
const float Ki = 0.1;  // Integral constant
const float Kd = 0.1;  // Derivative constant

// Variables
long duration;
int distance;
int angle = centerAngle;
int lastError = 0;
int total = 0;
int readIndex = 0;
int readings[numReadings];
int average = 0;
unsigned long lastTime;
unsigned long currentTime;
float integral = 0;

// Arrays for storing distance and time for derivative calculation
const int historySize = 10;
int distanceHistory[historySize];
unsigned long timeHistory[historySize];
int historyIndex = 0;

// Setup function
void setup() {
  myServo.attach(servoPin);
  myServo.write(angle);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);

  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  for (int i = 0; i < historySize; i++) {
    distanceHistory[i] = 0;
    timeHistory[i] = 0;
  }

  lastTime = millis();
}

// Function to get distance
void getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  if (distance > 200) {
    distance = 1;
  }
}

// Function to update readings
void updateReadings() {
  total -= readings[readIndex];
  readings[readIndex] = distance;
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  average = total / numReadings;
}

// Function to update history
void updateHistory() {
  distanceHistory[historyIndex] = average;
  timeHistory[historyIndex] = millis();
  historyIndex = (historyIndex + 1) % historySize;
}

// Function to calculate derivative
float calculateDerivative() {
  int oldestIndex = (historyIndex + 1) % historySize;
  int newestIndex = (historyIndex - 1 + historySize) % historySize;

  int distanceDiff = distanceHistory[newestIndex] - distanceHistory[oldestIndex];
  unsigned long timeDiff = timeHistory[newestIndex] - timeHistory[oldestIndex];

  if (timeDiff == 0) {
    return 0;
  }

  return (float)distanceDiff / timeDiff;
}

// PID control function
void controlServo() {
  int error = average - targetDistance;

  integral += error;
  float derivative = calculateDerivative();

  int deltaAngle = Kp * error + Ki * integral + Kd * derivative;
  angle = centerAngle + deltaAngle;
  angle = constrain(angle, minAngle, maxAngle);

  myServo.write(angle);

  // Reset integral and derivative components if close to target
  if (abs(error) <= 1) {
    integral = 0;
  }
}

// Loop function
void loop() {
  getDistance();
  updateReadings();
  updateHistory();
  controlServo();

  Serial.print("Time: ");
  Serial.print(millis());
  Serial.print(", Distance: ");
  Serial.print(average);
  Serial.print(", Angle: ");
  Serial.println(angle);

  delay(100);
}
