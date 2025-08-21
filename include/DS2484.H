

// DS2484 Status bit definitions
// Bit positions for DS2484 status (from MSB -> LSB):
// Bit 7: DIR (Direction, used in search algorithms)
// Bit 6: TSB (Triplet Second Bit)
// Bit 5: SBR (Single Bit Result)
// Bit 4: RST (Reset flag)
// Bit 3: LL (Logic Level on 1-Wire bus)
// Bit 2: SD (Short Detected)
// Bit 1: PPD (Presence Pulse Detect)
// Bit 0: 1WB (1-Wire Busy)

#ifndef DS2484_H
#define DS2484_H



// DS2484 7-bit address
#define DS2484_I2C_ADDRESS      0x18U
// DS2484 Commands
#define DS2484_CMD_DRST         0xF0U  // Device Reset
#define DS2484_CMD_1WWB         0xA5   // 1-Wire Write Byte
#define DS2484_CMD_1WRB         0x96   // 1-Wire Read Byte
#define DS2484_CMD_SRP          0xE1   // Set Read Pointer
#define DS2484_RST_BIT_MASK     0x10U  // Bit 4 (RST)
#define DS2484_STATUS_1WB_MASK  0x01   // Bit 0 (1WB)
#define DS2484_CMD_1WT          0x78   // Triplet command
#define DS2484_CMD_MROM         0x55   // Match ROM
#define DS2484_CMD_1WSB         0x87   // 1-Wire Single Bit
#define DS2484_REG_STATUS       0xF0
#define DS2484_REG_DATA         0xE1
#define DS2484_DEV_CONFIG       0xE1
// Function Prototypes
void DS2484_RESET(void);
void DS2484_SetReadPointer(uint8_t reg);
uint8_t DS2484_ReadByte(void);
void DS2484_WriteDeviceConfig(uint8_t config);
uint8_t DS2484_ReadCurrentRegister(void);
uint8_t DS2484_ReadStatus(void);
uint8_t DS2484_1WireResetAndPoll(void);
void DS2484_1WireWriteByte(uint8_t data);
void DS2484_MatchROM(uint8_t *romCode);
uint8_t DS2484_Triplet(uint8_t directionBit);
uint8_t DS2484_EncodeConfig(uint8_t upperNibble);
int DS2484_SearchROM(uint8_t (*foundROMs)[8], int maxDevices);
int OneWire_SearchBus(int max_dev, uint8_t (*rom_list)[8]);

#endif // DS2484_H


