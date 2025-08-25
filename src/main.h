#ifndef FELICITY_MAIN_H
#define FELICITY_MAIN_H

#define LED_PIN 02
#define writeLog(...) Serial.printf(__VA_ARGS__)

#define RS485_DE_PIN 16
#define RS485_RE_PIN 17
#define RS485_TX_PIN 4
#define RS485_RX_PIN 5

inline void rs485TxEnable() {
    digitalWrite(RS485_DE_PIN, HIGH);
    digitalWrite(RS485_RE_PIN, HIGH);
}

inline void rs485RxEnable() {
    digitalWrite(RS485_DE_PIN, LOW);
    digitalWrite(RS485_RE_PIN, LOW);
}

extern QueueHandle_t bmsQueue;

#endif
