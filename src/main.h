//
// This is part of the FelicityBMS2MQTT project
//
// https://github.com/Smartsmurf/FelicityBMS2MQTT
// 
// 
#ifndef FELICITY_MAIN_H
#define FELICITY_MAIN_H

#define LED_PIN 02
#define writeLog(...) Serial.printf(__VA_ARGS__)

extern QueueHandle_t bmsQueue;

#endif
