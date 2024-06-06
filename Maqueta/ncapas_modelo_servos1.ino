#include <Servo.h>

Servo servo_9;  // Primer servo
Servo servo_10; // Segundo servo
const int slideSwitch_9 = 2;  // Primer pin del slide switch
const int slideSwitch_10 = 4; // Segundo pin del slide switch
int previousSwitchState_9 = LOW; // Estado anterior del primer switch
int previousSwitchState_10 = LOW; // Estado anterior del segundo switch

void abrirPuerta(Servo servo); // Función para abrir puerta

void setup() {
  servo_9.attach(9);  // Colocamos el primer servo al pin 9
  servo_10.attach(10); // Colocamos el segundo servo al pin 10
  pinMode(slideSwitch_9, INPUT_PULLUP);  // Configura el pin 
  pinMode(slideSwitch_10, INPUT_PULLUP); // 
  Serial.begin(9600);  
  servo_9.write(90);  // Posición inicial del primer servo en 90 grados
  servo_10.write(90); // Posición inicial del segundo servo en 90 grados
}

void loop() {
  int switchState_9 = digitalRead(slideSwitch_9);  // Lee el estado del primer slide switch
  int switchState_10 = digitalRead(slideSwitch_10); // Lee el estado del segundo slide switch

  Serial.print("Estado del switch 9: ");
  Serial.println(switchState_9);
  Serial.print("Estado del switch 10: ");
  Serial.println(switchState_10);

  if (switchState_9 == HIGH && previousSwitchState_9 == LOW) {  // Si el primer slide switch cambia a la posición ON
    abrirPuerta(servo_9);  // Abre la puerta controlada por el primer servo
  }

  if (switchState_10 == HIGH && previousSwitchState_10 == LOW) {  // Si el segundo slide switch cambia a la posición ON
    abrirPuerta(servo_10);  // Abre la puerta controlada por el segundo servo
  }

  previousSwitchState_9 = switchState_9;  // Almacena el estado actual del primer switch para la siguiente iteración
  previousSwitchState_10 = switchState_10; // Almacena el estado actual del segundo switch para la siguiente iteración
  delay(50);  // 
}

void abrirPuerta(Servo servo) {
  for (int angle = 90; angle <= 180; angle += 1) {
    servo.write(angle); 
    delay(15);  
  }

  delay(1000);  

  for (int angle = 180; angle >= 90; angle -= 1) {
    servo.write(angle);  
    delay(15);  
  }
}


