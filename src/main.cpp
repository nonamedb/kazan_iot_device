#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "********";
const char* password = "*******";

// https://www.cloudmqtt.com
const char* mqtt_server = "example.cloudmqtt.com";


// #define MQTT_USER  "broker.mqttdashboard.com";
#define mqtt_port  12243
#define MQTT_USER  "******"
#define MQTT_PASSWORD  "********"
#define MQTT_SERIAL_PUBLISH_CH "/home/tx"
#define MQTT_SERIAL_RECEIVER_CH "/home/rx"


const int SoundPin = 16;
const int Button_left = 19;
const int Button_right = 15;
const int LED_BUILTIN = 2;
// const int SoundPin = 16;
// Update these with values suitable for your network.

WiFiClient wifiClient;

PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("/home", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
  if ((char)payload[0] == '1') {
    
      digitalWrite (SoundPin, HIGH);
      delay(500);	
      digitalWrite (SoundPin, LOW);
    }
  if ((char)payload[0] == '2') {
      digitalWrite (SoundPin, HIGH);
      delay(100);	
      digitalWrite (SoundPin, LOW);
      delay(100);	
      digitalWrite (SoundPin, HIGH);
      delay(100);	
      digitalWrite (SoundPin, LOW);
      delay(100);	
      digitalWrite (SoundPin, HIGH);
      delay(100);	
      digitalWrite (SoundPin, LOW);
    }
    Serial.write(length);
    Serial.println();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode (SoundPin, OUTPUT);
  pinMode (Button_left, INPUT);
  pinMode (Button_right, INPUT);
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}
void loop() {
   client.loop();
   if (digitalRead(Button_left)){
      client.publish("/home", "button L");
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(300);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(300);                       // wait for a second
   }
    if (digitalRead(Button_right)){
      client.publish("/home", "button R");
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(300);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(300);                       // wait for a second
   }
    if (digitalRead(Button_right) && digitalRead(Button_left) ){
      client.publish("/home", "button LR");
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(300);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(300);                       // wait for a second
   }
   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     publishSerialData(mun);
   }
 }

