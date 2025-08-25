
#include "preferences.h"

Preferences prefs;

String ssid, password, mqttServer, mqttUser, mqttPass, mqttTopic, mqttDevicename;
int rxPin, txPin, rtsPin, mqttPort, batteryCount;

void loadSettings() {
  prefs.begin("config", true);  // read-only
  ssid = prefs.getString("ssid", "");
  password = prefs.getString("pass", "");
  mqttServer = prefs.getString("mqtt", "");
  mqttPort = prefs.getInt("mqtt_port", 1883);
  mqttTopic = prefs.getString("mqtt_topic", "bms");
  mqttUser = prefs.getString("mqtt_user", "");
  mqttPass = prefs.getString("mqtt_pass", "");
  mqttDevicename = prefs.getString("device_name", "felicity2mqtt");
  rxPin = prefs.getInt("rx_pin", 16);
  txPin = prefs.getInt("tx_pin", 17);
  rtsPin = prefs.getInt("rts_pin", 4);
  batteryCount = prefs.getInt("batt_count", 1);
  prefs.end();
}
