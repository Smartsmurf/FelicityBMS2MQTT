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

void setup() {

  Serial.begin(9600);
  Serial.println("Starting up...");

  loadSettings();

  if (ssid != "") {
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Verbinde mit WLAN: ");
    Serial.println(ssid);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ WLAN verbunden!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      startWebServer();

      bms = new FelicityBMS(RS485_RX_PIN, RS485_TX_PIN, batteryCount);
      bmsQueue = xQueueCreate(10, sizeof(BmsMessage));
      bms->SetQueue(bmsQueue);

      xTaskCreatePinnedToCore(FelicityBMS::bmsTaskWrapper, "BMS", 4096, bms, 1, NULL, 1);
      xTaskCreatePinnedToCore(mqtt_task, "MQTT", 4096, NULL, 1, NULL, 1);

      Serial.println("System started.");

      analogWrite(LED_PIN, 255);

      return;
    } else {
      Serial.println("\n⚠️ WLAN-Verbindung fehlgeschlagen.");
    }
  }

  // Kein WLAN → Starte Konfigurations-AP
  startConfigPortal();

}

void loop() {
  
  server.handleClient();

}

/*
void writeLog(const char *format, ...)
{
  char msg[100];
  va_list args;

  va_start(args, format);
  vsnprintf(msg, sizeof(msg), format, args); // do check return value
  va_end(args);

  // write msg to the log
  DBG_PRINTLN(msg);
//  DBG_WEBLN(msg);
}
*/