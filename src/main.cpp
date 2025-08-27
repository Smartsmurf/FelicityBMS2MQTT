//
// This is part of the FelicityBMS2MQTT project
//
// https://github.com/Smartsmurf/FelicityBMS2MQTT
// 
// 
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#include "felicity.h"
#include "html.h"
#include "main.h"
#include "mqtt.h"
#include "preferences.h"

FelicityBMS * bms;
QueueHandle_t bmsQueue;
unsigned long lastWifiCheck = 0;

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
    lastWifiCheck = millis();
    WiFi.begin(ssid.c_str(), password.c_str());
}

void setup() {

  Serial.begin(9600);
  Serial.println("Starting up...");

  loadSettings();

  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  if (ssid != "") {
    WiFi.begin(ssid.c_str(), password.c_str());
    WiFi.setAutoReconnect(true);
    Serial.print("Trying to connect to SSID: ");
    Serial.println(ssid);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ WIFI connected.");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      startWebServer();

      // TODO: use configured ports - as soon as HW project catches up ;)
      bms = new FelicityBMS(RS485_RX_PIN, RS485_TX_PIN, batteryCount);
      bmsQueue = xQueueCreate(10, sizeof(BmsMessage));
      bms->SetQueue(bmsQueue);

      xTaskCreatePinnedToCore(FelicityBMS::bmsTaskWrapper, "BMS", 4096, bms, 1, NULL, 1);
      xTaskCreatePinnedToCore(mqtt_task, "MQTT", 4096, NULL, 1, NULL, 1);

      Serial.println("System started.");

      analogWrite(LED_PIN, 255);

      return;
    } else {
      Serial.println("\n⚠️ WIFI connection failed.");
    }
  }

  // no WIFI - run config AP
  startConfigPortal();

}

void loop() {
  
  server.handleClient();

  // check WIFI state
  unsigned long now = millis();
  if (now - lastWifiCheck >= 10000) {
    lastWifiCheck = now;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WIFI disconnected! Trying reconnect...");

      WiFi.disconnect();  // sanity
      WiFi.begin(ssid.c_str(), password.c_str());
    }
  }

}
