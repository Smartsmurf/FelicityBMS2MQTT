//
// This is part of the FelicityBMS2MQTT project
//
// https://github.com/Smartsmurf/FelicityBMS2MQTT
// 
// 
#include <WiFi.h>
#include <PubSubClient.h>

#include "felicity.h"
#include "main.h"
#include "preferences.h"

char *mqtt_buildtopic(char *buffer, int device_id, const char *path, const char *optindex = "")
{
  sprintf(buffer,"%s/%d/%s%s",mqttTopic.c_str(), device_id, path, optindex);
  return buffer;
}

void mqtt_task(void *param) {
  WiFiClient client;
  PubSubClient mqttclient(client);

  char mqttbuff[256];
  char valuebuff[32];
  char mqttClientId[80];
  BmsMessage msg;

  uint32_t chipId = (uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF);
  sprintf(mqttClientId, "%s-%06X", mqttDevicename.c_str(), chipId);

  mqttclient.setServer(mqttServer.c_str(), mqttPort);
  writeLog("[MQTT] MQTT server connected\n");

  for (;;) {
    if (systemShutdown) {
      Serial.println("[MQTT] Shutting down task...\n");
      vTaskDelete(NULL);
    }

    if (!mqttclient.connected() && mqttServer.length() > 0){
      writeLog("[MQTT] MQTT client state: %d", mqttclient.state());

      if (mqttclient.connect(mqttClientId, mqttUser.c_str(), mqttPass.c_str()) ){
        writeLog("[MQTT] MQTT client connected.\n");
      }
  }

    if (xQueueReceive(bmsQueue, &msg, portMAX_DELAY) == pdTRUE) {
      switch (msg.type) {
        case BMS_TYPE_VERSION_INFO:
//            writeLog("[BMS%d] version=%d\n",
//                          msg.deviceId, msg.payload.versionInfo.version);
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "version"), itoa(msg.payload.versionInfo.version, valuebuff, 10));
            break;
        case BMS_TYPE_CELL_VOLTAGES:
//            writeLog("[BMS%d] cell voltages", msg.deviceId);
            for( int i = 0; i < 16; i++ ){
              mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "cell_voltage_", itoa(i, valuebuff, 10)),
                  dtostrf(msg.payload.cellInfo.cellVoltages[i], 3, 3, valuebuff));
            }
            // although state there are 8 fields, only 4 sensors yield useful results
            for( int i = 0; i < 4; i++ ){
              mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "cell_temperature_", itoa(i, valuebuff, 10)),
                  itoa(msg.payload.cellInfo.cellTemperatures[i], valuebuff, 10));
            }
            break;
        case BMS_TYPE_CHARGE_DISCHARGE:
//            writeLog("[BMS%d] charge_current_limit=%f, charge_voltage_limit=%f, discharge_current_limit=%f, discharge_voltage_limit=%f\n",
//                  msg.deviceId, msg.payload.chargeDischarge.chargeCurrentLimit, msg.payload.chargeDischarge.chargeVoltLimit,
//                  msg.payload.chargeDischarge.dischargeCurrentLimit, msg.payload.chargeDischarge.dischargeVoltLimit);
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "charge_current_limit"), dtostrf(msg.payload.chargeDischarge.chargeCurrentLimit, 3, 1, valuebuff));
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "charge_voltage_limit"), dtostrf(msg.payload.chargeDischarge.chargeVoltLimit, 3, 1, valuebuff));
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "discharge_current_limit"), dtostrf(msg.payload.chargeDischarge.dischargeCurrentLimit, 3, 1, valuebuff));
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "discharge_voltage_limit"), dtostrf(msg.payload.chargeDischarge.dischargeVoltLimit, 3, 1, valuebuff));
            break;
        case BMS_TYPE_BATTERY_INFO:
//            writeLog("[BMS%d] U=%.2fV I=%.1fA SOC=%u%% T=%d°C\n",
//                          msg.deviceId,
//                          msg.payload.batteryInfo.voltage,
//                          msg.payload.batteryInfo.current,
//                          msg.payload.batteryInfo.soc,
//                          msg.payload.batteryInfo.temp);
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "voltage"), dtostrf(msg.payload.batteryInfo.voltage, 3, 1, valuebuff));
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "current"), dtostrf(msg.payload.batteryInfo.current, 3, 1, valuebuff));
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "soc"), itoa(msg.payload.batteryInfo.soc, valuebuff, 10));
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "temperature"), itoa(msg.payload.batteryInfo.temp, valuebuff, 10));
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "battery_charge_enable"), msg.payload.batteryInfo.batteryChargeEnable ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "battery_charge_immediately"), msg.payload.batteryInfo.batteryChargeImmediately ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "battery_discharge_enable"), msg.payload.batteryInfo.batteryDischargeEnable ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "fault_bms_temperature_high"), msg.payload.batteryInfo.faultBMSTemperatureHigh ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "fault_cell_temperature_high"), msg.payload.batteryInfo.faultCellTemperatureHigh ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "fault_cell_temperature_low"), msg.payload.batteryInfo.faultCellTemperatureLow ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "fault_cell_voltage_high"), msg.payload.batteryInfo.faultCellVoltageHigh ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "fault_cell_voltage_high"), msg.payload.batteryInfo.faultCellVoltageLow ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "fault_charge_current_high"), msg.payload.batteryInfo.faultChargeCurrentHigh ? "true":"false");
            mqttclient.publish(mqtt_buildtopic(mqttbuff, msg.deviceId, "fault_discharge_current_high"), msg.payload.batteryInfo.faultDischargeCurrentHigh ? "true":"false");
            break;
        default:
            writeLog("[BMS%d] unknown message type\n", msg.deviceId);
            break;
      }
    }
  }
}

