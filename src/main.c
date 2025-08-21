#include "driverlib.h"
#include "device.h"
#include <string.h>
#include <DS2484.H>
#include <DS1825U.H>

float tempC;
// Forward declarations
void initI2C(void);
void initGPIO(void);

int main(void)
{
    // Basic device/GPIO init
    Device_init();
    Device_initGPIO();

    // Configure I2C and 1-Wire pins
    initGPIO();
    initI2C();

    // Reset and configure DS2484
    DS2484_RESET();
    DEVICE_DELAY_US(1000);
    if (!(DS2484_1WireResetAndPoll() & 0x02)) ESTOP0;
    DEVICE_DELAY_US(1000);
    DS2484_WriteDeviceConfig(0xA5);
    DEVICE_DELAY_US(10000);
    if ((DS2484_ReadCurrentRegister() & 0x0F) != (0xA5 & 0x0F)) ESTOP0;
    DEVICE_DELAY_US(10000);

    // Search for exactly one sensor
    uint8_t romCodes[1][8];
    if (DS2484_SearchROM(romCodes, 1) != 1) ESTOP0;
    if (romCodes[0][0] != 0x3B || calculateCRC(romCodes[0], 7) != romCodes[0][7]) ESTOP0;

    // Scratchpad buffer
    uint8_t scratchpad[9];

    while (1)
    {
        // 1) Reset + presence
        if (!(DS2484_1WireResetAndPoll() & 0x02)) ESTOP0;
        DEVICE_DELAY_US(100);

        // 2) Match ROM
        DS2484_MatchROM(romCodes[0]);
        DEVICE_DELAY_US(100);

        // 3) Start conversion
        DS2484_1WireWriteByte(DS1825U_CMD_CONVERT_T);
        DEVICE_DELAY_US(750000);

        // 4) Reset + presence before read
        if (!(DS2484_1WireResetAndPoll() & 0x02)) ESTOP0;
        DEVICE_DELAY_US(100);

        // 5) Match ROM and send Read-Scratchpad (0xBE)
        DS2484_MatchROM(romCodes[0]);
        DEVICE_DELAY_US(100);
        DS2484_1WireWriteByte(0xBE);

        // 6) Read 9 bytes, polling 1WB between slots
        for (int i = 0; i < 9; ++i)
        {
            DEVICE_DELAY_US(100);
            // Trigger a 1-Wire read-slot
            DS2484_1WireWriteByte(DS2484_CMD_1WRB);
            DEVICE_DELAY_US(100);
            // Wait for 1WB to clear
            while (DS2484_ReadStatus() & DS2484_STATUS_1WB_MASK);
            DEVICE_DELAY_US(100);
            // Point to data register
            DS2484_SetReadPointer(0xE1);
            DEVICE_DELAY_US(100);
            // Read one byte
            scratchpad[i] = DS2484_ReadCurrentRegister();
        }

        // 7) Decode temperature (bytes 0,1)
        int16_t raw = ((int16_t)scratchpad[1] << 8) | scratchpad[0];
        tempC = raw * 0.0625f;

        // Delay before next reading
        DEVICE_DELAY_US(1000000);
    }

    return 0;
}

// Initialize I2C controller
void initI2C(void)
{
    I2C_disableModule(I2CA_BASE);
    I2C_initController(I2CA_BASE,
                      DEVICE_SYSCLK_FREQ,
                      100000,
                      I2C_DUTYCYCLE_50);
    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_SEND_MODE);
    I2C_setBitCount(I2CA_BASE, I2C_BITCOUNT_8);
    I2C_setAddressMode(I2CA_BASE, I2C_ADDR_MODE_7BITS);
    I2C_enableModule(I2CA_BASE);
}

// Configure GPIO for I2C pins
void initGPIO(void)
{
    // SDA
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SDAA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SDAA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SDAA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SDAA, GPIO_QUAL_ASYNC);
    // SCL
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCLA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCLA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCLA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCLA, GPIO_QUAL_ASYNC);
}
