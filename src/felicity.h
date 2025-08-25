#ifndef FELICITY_BMS_H
#define FELICITY_BMS_H

#include <SoftwareSerial.h>

#include "crc.h"

extern volatile bool systemShutdown;

inline uint16_t be16(uint8_t * val) {
  return ((uint16_t)(*val++ << 8) | *val);
}

enum BmsDataType {
    BMS_TYPE_VERSION_INFO,
    BMS_TYPE_BATTERY_INFO,
    BMS_TYPE_CHARGE_DISCHARGE,
    BMS_TYPE_CELL_VOLTAGES
};

struct BmsMessage {
    uint8_t deviceId;
    BmsDataType type;
    union {
        struct {
            uint16_t version;
        } versionInfo;
        struct {
            bool batteryChargeEnable;
            bool batteryChargeImmediately;
            bool batteryDischargeEnable;
            bool faultCellVoltageHigh;
            bool faultCellVoltageLow;
            bool faultChargeCurrentHigh;
            bool faultDischargeCurrentHigh;
            bool faultBMSTemperatureHigh;
            bool faultCellTemperatureHigh;
            bool faultCellTemperatureLow;
            float voltage;
            float current;
            uint16_t soc;
            uint16_t temp;
        } batteryInfo;

        struct {
            float chargeVoltLimit;
            float dischargeVoltLimit;
            float chargeCurrentLimit;
            float dischargeCurrentLimit;
        } chargeDischarge;

        struct {
            float cellVoltages[16];
            uint16_t cellTemperatures[8];
        } cellInfo;
    } payload;
};

class FelicityBMS
{

private:

    EspSoftwareSerial::UART * serial;
    // SoftwareSerial * serial;
    int num_slaves;
    QueueHandle_t bmsQueue;

public:

    FelicityBMS(int rx, int tx, int num_slaves);
    ~FelicityBMS(void);

    uint16_t SendAPDU(uint8_t sid, uint8_t cmd, uint16_t addr, uint16_t len);
    int ReceiveAPDU(uint8_t * buffer, uint16_t len);
    void SetQueue(QueueHandle_t bmsq);
    void bmsTask(void *param);

    static void bmsTaskWrapper(void *param) {
        FelicityBMS* self = static_cast<FelicityBMS*>(param);
        if (self)
            self->bmsTask(nullptr);
    }
};

#endif // FELICITY_BMS_H
                                       