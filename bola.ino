#include <Servo.h>

Servo myServo1;  // crea un objeto Servo para controlar el primer servo
Servo myServo2;  // crea un objeto Servo para controlar el segundo servo

const int trigPin = 4;   // pin de disparo del sensor ultrasónico
const int echoPin = 5;  // pin de eco del sensor ultrasónico

void setup() {
  Serial.begin(9600);  // inicia la comunicación serial a 9600 bps
  myServo1.attach(2);  // conecta el primer servo al pin digital 2
  myServo2.attach(3);  // conecta el segundo servo al pin digital 3
  
  pinMode(trigPin, OUTPUT); // configura el pin de disparo como salida
  pinMode(echoPin, INPUT);  // configura el pin de eco como entrada

  myServo1.write(54);  // fija el primer servo en la posición inicial de 54 grados
  myServo2.write(54);  // fija el segundo servo en la posición inicial de 54 grados
  Serial.println("Servos inicializados en 54 grados (medio).");
}

long readUltrasonicDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(triggerPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  long distance = readUltrasonicDistance(trigPin, echoPin);
  Serial.print("Distancia medida: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Mueve los servos de 54 a 84 grados (myServo1) y de 54 a 24 grados (myServo2)
  for (int pos = 54; pos <= 84; pos++) {
    myServo1.write(pos);              // mueve myServo1 de 54 a 84 grados
    myServo2.write(108 - pos);        // mueve myServo2 en dirección opuesta, de 54 a 24 grados
    delay(50);  // Añade un pequeño retraso para ver el movimiento de los servos
  }
  
  // Mueve los servos de 84 a 24 grados (myServo1) y de 24 a 84 grados (myServo2)
  for (int pos = 84; pos >= 24; pos--) {
    myServo1.write(pos);              // mueve myServo1 de 84 a 24 grados
    myServo2.write(108 - pos);        // mueve myServo2 en dirección opuesta, de 24 a 84 grados
    delay(50);  // Añade un pequeño retraso para ver el movimiento de los servos
  }

  // Mueve los servos de 24 a 54 grados (myServo1) y de 84 a 54 grados (myServo2)
  for (int pos = 24; pos <= 54; pos++) {
    myServo1.write(pos);              // mueve myServo1 de 24 a 54 grados
    myServo2.write(108 - pos);        // mueve myServo2 de 84 a 54 grados
    delay(50);  // Añade un pequeño retraso para ver el movimiento de los servos
  }
}
