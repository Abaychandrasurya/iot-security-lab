/*
ESP32 IoT Security Classroom - Unified firmware
- Supports compile-time flags to enable vulnerability behaviours
- Features: WiFi provisioning portal, SD config, NRF24 comms, MQTT, Blynk, OTA, Web Serial (WebSocket)
- Hardware assumed:
DHT not used here; using LDR on ADC
nRF24L01 on SPI
SD on SPI CS pin
*/


#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <RF24.h>


// Optional Blynk
#ifdef BLYNK
#include <BlynkSimpleEsp32.h>
#endif


// Hardware pins (example)
#define LDR_PIN 34 // ADC1_CH6
#define LED_PIN 2
#define BUZZER_PIN 14
#define BUTTON_PIN 15
#define SD_CS 13


// nRF24 pins
#define RF_CE 5
#define RF_CSN 15
RF24 radio(RF_CE, RF_CSN);


// Global objects
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiClient espClient;
PubSubClient mqttClient(espClient);


String deviceId;
String modeName;


// Simple logger buffer and web socket broadcast
String logBuffer;


void logEvent(const String &s){
String line = String(millis()) + ": " + s;
Serial.println(line);
// keep small buffer
logBuffer = line;
// broadcast to websocket clients
webSocket.broadcastTXT(line);
}


// SD config struct
struct Config {
String ssid;
String pass;
String mqtt_server;
int mqtt_port;
String mqtt_user;
String mqtt_pass;
String blynk_token;
String device_id;
String mode;
} cfg;


bool loadConfig(){
if(!SD.begin(SD_CS)){
logEvent("SD init failed");
return false;
}
if(!SD.exists("config.json")){
logEvent("config.json not found on SD");
return false;
}
}