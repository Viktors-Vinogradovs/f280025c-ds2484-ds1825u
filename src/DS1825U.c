#include "driverlib.h"
#include "device.h"
#include <string.h>
#include <DS2484.H>
#include <DS1825U.H>

// Read DS1825U ROM code
int DS1825U_ReadROM(uint8_t *romCode)
{
    uint8_t status = DS2484_1WireResetAndPoll();
    if (!(status & 0x02)) {
        return -1;
    }
    DEVICE_DELAY_US(1000);
    DS2484_1WireWriteByte(DS1825U_CMD_READ_ROM);
    DEVICE_DELAY_US(600);

    int i;
    for (i = 0; i < 8; i++) {
        DEVICE_DELAY_US(100);
        romCode[i] = DS2484_ReadByte();
    }

    return SUCCESS;
}

// Start DS1825U temperature conversion
void DS1825U_StartConversion(void)
{
    DS2484_1WireResetAndPoll();
    DEVICE_DELAY_US(1000);
    DS2484_1WireWriteByte(DS1825U_CMD_SKIP_ROM);
    DEVICE_DELAY_US(1000);
    DS2484_1WireWriteByte(DS1825U_CMD_CONVERT_T);
}

// Read DS1825U scratchpad
int DS1825U_ReadScratchpad(uint8_t *romCode, uint8_t *scratchpad)
{
    uint8_t status = DS2484_1WireResetAndPoll();
    if (!(status & 0x02)) {
        return -1;
    }
    DEVICE_DELAY_US(500);
    DS2484_MatchROM(romCode);
    DEVICE_DELAY_US(100);
    DS2484_1WireWriteByte(DS1825U_CMD_READ_SCRATCHPAD);
    DEVICE_DELAY_US(100);

    int i;
    for (i = 0; i < 9; i++) {
        scratchpad[i] = DS2484_ReadByte();
        DEVICE_DELAY_US(100);
    }
    return SUCCESS;
}

// Calculate temperature from DS1825U scratchpad
float DS1825U_CalculateTemperature(uint8_t *scratchpad)
{
    int16_t rawTemperature = ((int16_t)scratchpad[1] << 8) | scratchpad[0];
    return rawTemperature / 16.0f;
}

// Calculate CRC for 1-Wire data
uint8_t calculateCRC(uint8_t *data, int len)
{
    uint8_t crc = 0;
    int i, j;
    for (i = 0; i < len; i++) {
        uint8_t byte = data[i];
        for (j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ byte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            byte >>= 1;
        }
    }
    return crc;
}
