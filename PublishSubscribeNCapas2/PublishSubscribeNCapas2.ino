#include <ESP32Servo.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Pams"
#define WLAN_PASS       "Lua2004!"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "pamelagomez"
#define AIO_KEY         "##"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds for Publishing***************************************/
// Setup a feed called 'photocell' for publishing.
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

/****************************** Feeds for Subscribing***************************************/
// Setup a feed called 'slider' for subscribing to changes on the slider
//Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/slider", MQTT_QOS_1);

// Setup a feed called 'onoff' for subscribing to changes to the button
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/boton_servo", MQTT_QOS_1);

/*************************** Sketch Code ************************************/

Servo servo_9;  // Crea un objeto Servo para controlar el servo
int ledVerde = 12;  // Pin del LED verde
int ledRojo = 13;  // Pin del LED rojo

void onoffcallback(char *data, uint16_t len) {
  Serial.print("Hey we're in a onoff callback, the button value is: ");
  Serial.println(data);
  String message = String(data);
  message.trim();
  
  if (message == "ON") {
    digitalWrite(ledVerde, HIGH);  // Enciende el LED verde

    // Mueve el servo desde 90 a 180 grados
    for (int angle = 90; angle <= 180; angle += 1) {
      servo_9.write(angle);  // Establece la posición del servo
      delay(15);  // Espera para que el servo alcance la posición
    }

    // Mueve el servo desde 180 a 90 grados
    for (int angle = 180; angle >= 90; angle -= 1) {
      servo_9.write(angle);  // Establece la posición del servo
      delay(15);  // Espera para que el servo alcance la posición
    }
  } else if (message == "OFF") {
    digitalWrite(ledRojo, HIGH);  // Enciende el LED rojo
  }
}

void setup() {
  servo_9.attach(19);  // Adjunta el servo al pin 19
  pinMode(ledVerde, OUTPUT);  // Configura el pin del LED verde como salida
  pinMode(ledRojo, OUTPUT);  // Configura el pin del LED rojo como salida

  Serial.begin(115200);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledRojo, LOW);
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
  onoffbutton.setCallback(onoffcallback);
  
  // Setup MQTT subscription for feed.
  mqtt.subscribe(&onoffbutton);
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