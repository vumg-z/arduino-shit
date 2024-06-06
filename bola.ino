#include <Servo.h>

Servo myServo1;  // crea un objeto Servo para controlar el primer servo
Servo myServo2;  // crea un objeto Servo para controlar el segundo servo

void setup() {
  Serial.begin(9600);  // inicia la comunicación serial a 9600 bps
  myServo1.attach(2);  // conecta el primer servo al pin digital 2
  myServo2.attach(3);  // conecta el segundo servo al pin digital 3
  
  myServo1.write(54);  // fija el primer servo en la posición inicial de 54 grados
  myServo2.write(54);  // fija el segundo servo en la posición inicial de 54 grados
  Serial.println("Servos inicializados en 54 grados (medio).");
}

void loop() {
  // Mueve los servos de 54 a 74 grados (myServo1) y de 54 a 34 grados (myServo2)
  for (int pos = 54; pos <= 74; pos++) {
    myServo1.write(pos);              // mueve myServo1 de 54 a 74 grados
    myServo2.write(108 - pos);        // mueve myServo2 en dirección opuesta, de 54 a 34 grados
    Serial.print("Moviendo Servo1 a "); 
    Serial.print(pos);               
    Serial.println(" grados");
    Serial.print("Moviendo Servo2 a "); 
    Serial.print(108 - pos);        
    Serial.println(" grados");
  }
  
  Serial.println("Servos alcanzaron los límites.");

  // Mueve los servos de 74 a 34 grados (myServo1) y de 34 a 74 grados (myServo2)
  for (int pos = 74; pos >= 34; pos--) {
    myServo1.write(pos);              // mueve myServo1 de 74 a 34 grados
    myServo2.write(108 - pos);        // mueve myServo2 en dirección opuesta, de 34 a 74 grados
    Serial.print("Moviendo Servo1 a "); 
    Serial.print(pos);               
    Serial.println(" grados");
    Serial.print("Moviendo Servo2 a "); 
    Serial.print(108 - pos);        
    Serial.println(" grados");
  }

  Serial.println("Servos alcanzaron los límites.");

  // Mueve los servos de 34 a 54 grados (myServo1) y de 74 a 54 grados (myServo2)
  for (int pos = 34; pos <= 54; pos++) {
    myServo1.write(pos);              // mueve myServo1 de 34 a 54 grados
    myServo2.write(108 - pos);        // mueve myServo2 de 74 a 54 grados
    Serial.print("Moviendo Servo1 a "); 
    Serial.print(pos);               
    Serial.println(" grados");
    Serial.print("Moviendo Servo2 a "); 
    Serial.print(108 - pos);        
    Serial.println(" grados");
  }

  Serial.println("Servos regresaron a 54 grados.");
}
