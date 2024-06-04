#include <Servo.h>
#include <ArduinoJson.h>

Servo myservo;  // crea un objeto servo para controlar un servomotor

class Sensor {
  public:
    int trigPin;
    int echoPin;
    bool available;

    Sensor(int tPin, int ePin) {
      trigPin = tPin;
      echoPin = ePin;
      available = false;
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }

    int readDistance() {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      long duration = pulseIn(echoPin, HIGH);
      int distance = duration * 0.034 / 2;
      return distance;
    }

    void updateAvailability() {
      int distance = readDistance();
      available = distance >= 10;
    }

    String getState() {
      return available ? "GREEN" : "RED";
    }
};

const int relayPin = 38;
const int buzzerPin = 36;
const int inputPin = 52;

Sensor sensors[] = {
  Sensor(42, 10),  // Sensor 1
  Sensor(26, 24),  // Sensor 2
  Sensor(28, 30)   // Sensor 3
};

const int trigPin4 = 31;
const int echoPin4 = 33;

const int ledPins[][3] = {
  {2, 3, 4},   // LED 1
  {5, 6, 7},   // LED 2
  {8, 11, 12}  // LED 3
};

long duration4;
int distance4;

unsigned long objectDetectedStartTime = 0;
bool isObjectDetected = false;

void setup() {
  // Configura el pin del relé como salida
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  // Buzzer
  pinMode(buzzerPin, OUTPUT);

  // Initialize pins for ultrasonic sensor 4
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);

  // Attach the servo to the servo pin
  myservo.attach(48);

  // Move the servo to the initial position
  myservo.write(45);

  // Initialize LEDs
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i][0], OUTPUT);
    pinMode(ledPins[i][1], OUTPUT);
    pinMode(ledPins[i][2], OUTPUT);
    setLedColor(i, "GREEN");
  }

  Serial1.begin(9600); // Start serial communication with ESP8266

  // Initialize input pin
  pinMode(inputPin, INPUT);
}

void loop() {
  StaticJsonDocument<200> jsonDoc;
  for (int i = 0; i < 3; i++) {
    sensors[i].updateAvailability();
    String state = sensors[i].getState();
    setLedColor(i, state);
    jsonDoc["S" + String(i)] = state;
  }

  String message;
  serializeJson(jsonDoc, message);
  message += "\n"; // Add a newline to signify the end of the message
  Serial1.print(message);

  // Read and process sensor 4 separately for servo and buzzer logic
  distance4 = readDistance(trigPin4, echoPin4);

  unsigned long currentTime = millis();

  if (distance4 < 2) {
    if (!isObjectDetected) {
      isObjectDetected = true;
      objectDetectedStartTime = currentTime;
      myservo.write(140); // Move servo to "arriba" position
    }
  }

  if (isObjectDetected && (currentTime - objectDetectedStartTime >= 1000)) {
    // Move servo back to "en medio" position after 1 second
    myservo.write(45);
    isObjectDetected = false;
  }

  // Relay control logic based on the state of input pin 52
  if (digitalRead(inputPin) == HIGH) {
    digitalWrite(relayPin, HIGH); // Turn on the relay
  } else {
    digitalWrite(relayPin, LOW); // Turn off the relay
  }

  delay(100); // Short delay before next loop
}

int readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

void setLedColor(int index, String color) {
  int pinR = ledPins[index][0];
  int pinG = ledPins[index][1];
  int pinB = ledPins[index][2];

  if (color == "RED") {
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, LOW);
  } else if (color == "GREEN") {
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, HIGH);
    digitalWrite(pinB, LOW);
  }
}
