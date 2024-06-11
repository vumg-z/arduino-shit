#include <Servo.h>
#include <math.h>  // Include math library for sine function

Servo myServo;  // Create servo object

const int servoPin = 2;    // Servo control pin
const int centerAngle = 120;  // Center position
const int amplitude = 20;  // Amplitude of the sinusoidal movement
const int period = 3000;   // Period of the sinus wave in milliseconds
const int lowerAngle = centerAngle - amplitude;   // Lower target angle
const int upperAngle = centerAngle + amplitude;   // Upper target angle
const int stepDelay = 25;     // Delay in milliseconds between movements

unsigned long startTime;  // Start time for sinusoidal calculation

void setup() {
  myServo.attach(servoPin);  // Attach the servo to the control pin
  myServo.write(centerAngle);  // Start at the center position
  delay(1000);  // Wait for a second to stabilize the servo
  startTime = millis();  // Initialize start time
}

void loop() {
  unsigned long currentTime = millis();  // Get current time
  float timeElapsed = (currentTime - startTime) / (float)period;  // Calculate elapsed time as a fraction of the period

  // Calculate the next angle using sinusoidal function for smooth movement
  int nextAngle = centerAngle + amplitude * sin(2 * PI * timeElapsed);

  // Ensure the angle is within bounds
  nextAngle = constrain(nextAngle, lowerAngle, upperAngle);

  myServo.write(nextAngle);  // Set servo position
  delay(stepDelay);  // Wait to slow down the movement

  // Debugging output to monitor angle
  Serial.print("Angle: ");
  Serial.println(nextAngle);
}
