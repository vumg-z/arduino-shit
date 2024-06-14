#include <Servo.h>
#include <math.h>  // Include math library for sine function

Servo myServo;  // Create servo object

const int servoPin = 2;    // Servo control pin
const int trigPin = 3;     // Trig pin for the ultrasonic sensor
const int echoPin = 4;     // Echo pin for the ultrasonic sensor
const int ledPin = 53;     // LED pin

const int centerAngle = 120;  // Center position
int amplitude = 20;  // Amplitude of the sinusoidal movement
const int period = 8000;   // Period of the sinus wave in milliseconds
const int stepDelay = 40;  // Increased delay for slower movement

const int minTargetDistance = 6;  // Minimum target distance in cm
const int maxTargetDistance = 13;  // Maximum target distance in cm

unsigned long startTime;  // Start time for sinusoidal calculation

int lastAngle = centerAngle;  // To store the last angle for smooth transition

void setup() {
  Serial.begin(9600);    // Start the serial communication
  myServo.attach(servoPin);  // Attach the servo to the control pin
  pinMode(trigPin, OUTPUT);  // Configure the trig pin as output
  pinMode(echoPin, INPUT);   // Configure the echo pin as input
  pinMode(ledPin, OUTPUT);   // Configure the LED pin as output

  myServo.write(centerAngle);  // Start at the center position
  delay(1000);  // Wait for a second to stabilize the servo
  startTime = millis();  // Initialize start time
}

// Fuzzy membership function for the error
float membershipError(float error, float a, float b, float c) {
  if (error <= a || error >= c) return 0.0;
  else if (error == b) return 1.0;
  else if (error > a && error < b) return (error - a) / (b - a);
  else return (c - error) / (c - b);
}

// Fuzzy inference
float fuzzyInference(float error) {
  // Fuzzy sets for error: Negative, Zero, Positive
  float errorNeg = membershipError(error, -8, -4, 0);
  float errorZero = membershipError(error, -1, 0, 1);
  float errorPos = membershipError(error, 0, 4, 8);

  // Fuzzy sets for control: Decrease, NoChange, Increase
  float controlDecrease = max(errorPos, 0.7 * errorZero);
  float controlNoChange = errorZero;
  float controlIncrease = max(errorNeg, 0.7 * errorZero);

  // Defuzzification using the centroid method
  float numerator = (controlDecrease * -1) + (controlNoChange * 0) + (controlIncrease * 1);
  float denominator = controlDecrease + controlNoChange + controlIncrease;

  if (denominator == 0) return 0; // Avoid division by zero
  return numerator / denominator;
}

void loop() {
  // Read the distance from the ultrasonic sensor
  long duration;
  int distance;

  // Trigger the ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the pulse duration
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert to distance

  // Calculate the error
  float error = (minTargetDistance + maxTargetDistance) / 2.0 - distance;

  // Adjust amplitude based on the distance to target
  if (distance >= minTargetDistance && distance <= maxTargetDistance) {
    amplitude = 5;  // Smaller amplitude for fine control
  } else if (distance > maxTargetDistance) {
    amplitude = 20;  // Larger amplitude for greater corrections
  } else if (distance < minTargetDistance) {
    amplitude = 20;  // Larger amplitude for greater corrections
  }

  int lowerAngle = centerAngle - amplitude;   // Lower target angle
  int upperAngle = centerAngle + amplitude;   // Upper target angle

  // Calculate the desired angle using sinusoidal function for smooth movement
  unsigned long currentTime = millis();  // Get current time
  float timeElapsed = (currentTime - startTime) / (float)period;  // Calculate elapsed time as a fraction of the period
  int desiredAngle = centerAngle + amplitude * sin(2 * PI * timeElapsed);

  // Fuzzy inference to get the control output
  float control = fuzzyInference(error);

  // Calculate the next angle incrementally for smoother transition
  int nextAngle = lastAngle;

  // Gradually move to the desired angle with smaller steps
  if (abs(error) > 0.1 && (distance < minTargetDistance || distance > maxTargetDistance)) { // Adjust only if the error is significant and out of range
    if (desiredAngle > lastAngle) {
      nextAngle++;
    } else if (desiredAngle < lastAngle) {
      nextAngle--;
    }
  }

  // Ensure the angle is within bounds
  nextAngle = constrain(nextAngle, lowerAngle, upperAngle);

  myServo.write(nextAngle);  // Set servo position
  lastAngle = nextAngle;     // Update last angle

  // Turn on the LED if the distance is within the target range
  if (distance >= minTargetDistance && distance <= maxTargetDistance) {
    digitalWrite(ledPin, HIGH);  // LED on when distance is within target range
  } else {
    digitalWrite(ledPin, LOW);   // LED off when distance is outside target range
  }

  // Send the data to the serial port
  Serial.print("Angle: ");
  Serial.print(lastAngle);
  Serial.print(", Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Error: ");
  Serial.print(error);
  Serial.println();

  delay(stepDelay);  // Wait to slow down the movement
}
