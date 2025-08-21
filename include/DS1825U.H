#ifndef DS1825U_H
#define DS1825U_H



// DS1825U Commands
#define DS1825U_CMD_READ_ROM         0x33  // Read ROM command (returns 8 bytes)
#define DS1825U_CMD_SKIP_ROM         0xCC  // Skip ROM command (broadcast)
#define DS1825U_CMD_CONVERT_T        0x44  // Start temperature conversion
#define DS1825U_CMD_READ_SCRATCHPAD  0xBE  // Read Scratchpad command (returns 9 bytes)

// Return code for success
#define SUCCESS 0

// Function Prototypes
float DS1825U_CalculateTemperature(uint8_t *scratchpad);
int DS1825U_ReadROM(uint8_t *romCode);
void DS1825U_StartConversion(void);
int DS1825U_ReadScratchpad(uint8_t *romCode, uint8_t *scratchpad);
uint8_t calculateCRC(uint8_t *data, int len);

#endif // DS1825U_H



