
#include <ESP32Servo.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       ""
#define WLAN_PASS       ""

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds for Subscribing***************************************/

// Setup feeds for subscribing to changes to the buttons
Adafruit_MQTT_Subscribe servo1Button = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/boton_servo", MQTT_QOS_1);
Adafruit_MQTT_Subscribe servo2Button = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/boton_pea", MQTT_QOS_1);

/*************************** Sketch Code ************************************/

Servo servo1;  // Primer servo
Servo servo2;  // Segundo servo 

void servo1Callback(char *data, uint16_t len) {
  Serial.print("Hey we're in a servo1 callback, the button value is: ");
  Serial.println(data);
  String message = String(data);
  message.trim();

  if (message == "ON") {
    // Mueve el servo1 desde 90 a 180 grados
    for (int angle = 90; angle <= 180; angle += 1) {
      servo1.write(angle);  // Establece la posición del servo
      delay(15);  // Espera para que el servo alcance la posición
    }

    // Mueve el servo1 desde 180 a 90 grados
    for (int angle = 180; angle >= 90; angle -= 1) {
      servo1.write(angle);  // Establece la posición del servo
      delay(15);  // Espera para que el servo alcance la posición
    }
  }
}

void servo2Callback(char *data, uint16_t len) {
  Serial.print("Hey we're in a servo2 callback, the button value is: ");
  Serial.println(data);
  String message = String(data);
  message.trim();

  if (message == "ON") {
    // Mueve el servo2 desde 90 a 180 grados
    for (int angle = 90; angle <= 180; angle += 1) {
      servo2.write(angle);  // Establece la posición del servo
      delay(40);  // Espera para que el servo alcance la posición
    }

    // Mueve el servo2 desde 180 a 90 grados
    for (int angle = 180; angle >= 90; angle -= 1) {
      servo2.write(angle);  // Establece la posición del servo
      delay(30);  // Espera para que el servo alcance la posición
    }
  }
}

void setup() {
  servo1.attach(18);  // Adjunta el primer servo al pin 18
  servo2.attach(22);  // Adjunta el segundo servo al pin 22


  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Callback Function
  servo1Button.setCallback(servo1Callback);
  servo2Button.setCallback(servo2Callback);
  
  // Setup MQTT subscription for feed.
  mqtt.subscribe(&servo1Button);
  mqtt.subscribe(&servo2Button);
}

void loop() {
  MQTT_connect();
  mqtt.processPackets(10000);

  if (!mqtt.ping()) {   // ping the server to keep the mqtt connection alive
    mqtt.disconnect();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000);  // wait 10 seconds
    retries--;
    if (retries == 0) {       // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
