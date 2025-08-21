#include "driverlib.h"
#include "device.h"
#include <string.h>
#include <DS2484.H>
#include <DS1825U.H>

// Reset DS2484 Device
void DS2484_RESET(void)
{
    uint16_t rstStatus = 0;
    uint32_t timeout;

    I2C_setConfig(I2CA_BASE, (I2C_CONTROLLER_SEND_MODE | I2C_REPEAT_MODE));
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (timeout == 0) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_putData(I2CA_BASE, DS2484_CMD_DRST);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (timeout == 0) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_setConfig(I2CA_BASE, (I2C_CONTROLLER_RECEIVE_MODE | I2C_REPEAT_MODE));
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendNACK(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_RX_DATA_RDY) && --timeout);
    if (timeout == 0) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    rstStatus = I2C_getData(I2CA_BASE);
    I2C_sendStopCondition(I2CA_BASE);

    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (timeout == 0) {
        ESTOP0;
    }

    if (!(rstStatus & DS2484_RST_BIT_MASK)) {
        ESTOP0;
    }
}

// Set DS2484 read pointer
void DS2484_SetReadPointer(uint8_t reg)
{
    uint32_t timeout;

    I2C_setConfig(I2CA_BASE, (I2C_CONTROLLER_SEND_MODE | I2C_REPEAT_MODE));
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (timeout == 0) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_putData(I2CA_BASE, DS2484_CMD_SRP);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (timeout == 0) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_putData(I2CA_BASE, reg);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (timeout == 0) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_sendStopCondition(I2CA_BASE);

    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (timeout == 0) {
        ESTOP0;
    }
}

// Read a byte from DS2484
uint8_t DS2484_ReadByte(void)
{
    uint32_t timeout;
    uint8_t readData;

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_SEND_MODE | I2C_REPEAT_MODE);
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    I2C_putData(I2CA_BASE, DS2484_CMD_1WRB);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    I2C_putData(I2CA_BASE, DS2484_CMD_SRP);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    I2C_putData(I2CA_BASE, 0xE1);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    I2C_sendStopCondition(I2CA_BASE);
    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (!timeout) { ESTOP0; }

    DEVICE_DELAY_US(1000);

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_RECEIVE_MODE | I2C_REPEAT_MODE);
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendNACK(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_RX_DATA_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    readData = I2C_getData(I2CA_BASE);

    I2C_sendStopCondition(I2CA_BASE);
    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (!timeout) { ESTOP0; }

    return readData;
}

// Encode DS2484 config byte
uint8_t DS2484_EncodeConfig(uint8_t upperNibble)
{
    uint8_t upper = (upperNibble >> 4) & 0x0F;
    uint8_t lower = ~upper & 0x0F;
    return (upper << 4) | lower;
}

// Write DS2484 configuration
void DS2484_WriteDeviceConfig(uint8_t config)
{
    uint32_t timeout;
    uint8_t configByte = DS2484_EncodeConfig(config);

    uint8_t status = DS2484_ReadStatus();
    if (status & DS2484_STATUS_1WB_MASK) {
        ESTOP0;
    }
    DEVICE_DELAY_US(500);

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_SEND_MODE | I2C_REPEAT_MODE);
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout || (I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK)) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }
    I2C_putData(I2CA_BASE, 0xD2);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout || (I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK)) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }
    I2C_putData(I2CA_BASE, configByte);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout || (I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK)) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_sendStopCondition(I2CA_BASE);
    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (!timeout) { ESTOP0; }
}

// Read current DS2484 register
uint8_t DS2484_ReadCurrentRegister(void)
{
    uint32_t timeout;
    uint8_t data;

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_RECEIVE_MODE | I2C_REPEAT_MODE);
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendNACK(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_RX_DATA_RDY) && --timeout);
    if (!timeout) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    data = I2C_getData(I2CA_BASE);

    I2C_sendStopCondition(I2CA_BASE);
    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (!timeout) {
        ESTOP0;
    }

    return data;
}

// Read DS2484 status register
uint8_t DS2484_ReadStatus(void)
{
    DS2484_SetReadPointer(0xF0);
    DEVICE_DELAY_US(100);
    return DS2484_ReadCurrentRegister();
}

// Perform 1-Wire reset and poll for completion
uint8_t DS2484_1WireResetAndPoll(void)
{
    uint32_t timeout;
    uint8_t status = 0xFF;

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_SEND_MODE | I2C_REPEAT_MODE);
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    I2C_putData(I2CA_BASE, 0xB4);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    do {
        DEVICE_DELAY_US(100);
        DS2484_SetReadPointer(0xF0);
        DEVICE_DELAY_US(1000);
        status = DS2484_ReadCurrentRegister();
    } while (status & 0x01);

    return status;
}

// Write a byte to the 1-Wire bus
void DS2484_1WireWriteByte(uint8_t data)
{
    uint32_t timeout;

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_SEND_MODE | I2C_REPEAT_MODE);
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    I2C_putData(I2CA_BASE, DS2484_CMD_1WWB);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout) { I2C_sendStopCondition(I2CA_BASE); ESTOP0; }

    I2C_putData(I2CA_BASE, data);

    I2C_sendStopCondition(I2CA_BASE);
    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (!timeout) { ESTOP0; }

    timeout = 100000;
    uint8_t status;
    do {
        DEVICE_DELAY_US(10);
        status = DS2484_ReadStatus();
        if (timeout-- == 0) { ESTOP0; }
        DEVICE_DELAY_US(10);
    } while (status & DS2484_STATUS_1WB_MASK);
}

// Match ROM for a specific device
void DS2484_MatchROM(uint8_t *romCode)
{
    DS2484_1WireWriteByte(DS2484_CMD_MROM);
    DEVICE_DELAY_US(100);
    int i;
    for (i = 0; i < 8; i++) {
        DS2484_1WireWriteByte(romCode[i]);
        DEVICE_DELAY_US(100);
    }
}

// Perform 1-Wire triplet operation
uint8_t DS2484_Triplet(uint8_t directionBit)
{
    uint32_t timeout;

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_SEND_MODE | I2C_REPEAT_MODE);
    I2C_setTargetAddress(I2CA_BASE, DS2484_I2C_ADDRESS);
    I2C_sendStartCondition(I2CA_BASE);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout || (I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK)) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_putData(I2CA_BASE, DS2484_CMD_1WT);
    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout || (I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK)) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_putData(I2CA_BASE, directionBit ? 0x80 : 0x00);

    timeout = 100000;
    while (!(I2C_getStatus(I2CA_BASE) & I2C_STS_REG_ACCESS_RDY) && --timeout);
    if (!timeout || (I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK)) {
        I2C_sendStopCondition(I2CA_BASE);
        ESTOP0;
    }

    I2C_sendStopCondition(I2CA_BASE);
    timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout);
    if (!timeout) { ESTOP0; }

    uint8_t status;
    timeout = 100000;
    do {
        DEVICE_DELAY_US(10);
        status = DS2484_ReadStatus();
        if (timeout-- == 0) { ESTOP0; }
        DEVICE_DELAY_US(10);
    } while (status & DS2484_STATUS_1WB_MASK);

    return status;
}

// Search for ROM codes
int DS2484_SearchROM(uint8_t (*foundROMs)[8], int maxDevices)
{
    int found = 0;
    uint8_t rom[8] = {0};
    uint8_t lastDiscrepancy = 0;
    uint8_t done = 0;
    uint8_t lastZeroBranch = 0; // Track the last position where we took the 0 branch

    while (!done && found < maxDevices) {
        uint8_t discrepancyMarker = 0;
        memset(rom, 0, 8); // Reset ROM code for each new search iteration
        DEVICE_DELAY_US(100);

        uint8_t status = DS2484_1WireResetAndPoll();
        if (!(status & 0x02)) { // Check for presence pulse
            break;
        }

        DEVICE_DELAY_US(500);
        DS2484_1WireWriteByte(0xF0); // Search ROM command
        DEVICE_DELAY_US(100);

        int i;
        for (i = 0; i < 64; i++) {
            uint8_t direction;

            // Determine the direction bit based on the current bit position and last discrepancy
            if (i < lastDiscrepancy) {
                // Follow the same path as the previous search up to the last discrepancy
                direction = (rom[i / 8] >> (i % 8)) & 0x01;
            } else if (i == lastDiscrepancy) {
                // At the last discrepancy, take the 1 branch (we'll take 0 on the next iteration)
                direction = 1;
            } else {
                // After the last discrepancy, take the 0 branch unless we already did so
                direction = (i == lastZeroBranch) ? 1 : 0;
            }
            DEVICE_DELAY_US(150);

            uint8_t tripletStatus = DS2484_Triplet(direction);
            uint8_t sbr = (tripletStatus >> 5) & 1; // Single Bit Result
            uint8_t tsb = (tripletStatus >> 6) & 1; // Triplet Second Bit
            uint8_t dir = (tripletStatus >> 7) & 1; // Direction

            // Determine the bit value to write to the ROM code
            uint8_t bitValue = (sbr == 0 && tsb == 0) ? dir : sbr;

            // Set or clear the bit in the ROM code
            if (bitValue) {
                rom[i / 8] |= (1 << (i % 8));
            } else {
                rom[i / 8] &= ~(1 << (i % 8));
            }

            // Update discrepancy marker if there's a conflict (both 0 and 1 are present)
            if (sbr == 0 && tsb == 0) {
                discrepancyMarker = i + 1;
            }

            // If we took the 0 branch at this position, record it
            if (bitValue == 0 && i >= lastDiscrepancy) {
                lastZeroBranch = i + 1;
            }

            // If both bits are 1, no devices responded, so exit the search
            if (sbr == 1 && tsb == 1) {
                done = 1;
                break;
            }
        }

        if (!done) {
            // Copy the discovered ROM code to the foundROMs array
            memcpy(foundROMs[found], rom, 8);
            found++;

            // Update the last discrepancy for the next iteration
            if (discrepancyMarker == 0 || discrepancyMarker <= lastDiscrepancy) {
                done = 1; // No more discrepancies to explore
            } else {
                lastDiscrepancy = discrepancyMarker;
            }
        }
    }

    // Ensure the I2C bus is not busy before returning
    uint32_t timeout = 100000;
    while (I2C_isBusBusy(I2CA_BASE) && --timeout) {
        I2C_sendStopCondition(I2CA_BASE);
        DEVICE_DELAY_US(10);
    }
    if (!timeout) { ESTOP0; }

    return found;
}


