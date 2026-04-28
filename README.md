# f280025c-ds2484-ds1825u

Firmware for **TI LaunchPad F280025C** using a **DS2484 I²C→1‑Wire bridge** to read one or more **DS1825U** temperature sensors.

This project is not finished yet. Communication with the DS2484 and DS1825U is working, but the **final step of decoding the temperature value is still incorrect**. Expect wrong or unstable readings until this part is fixed.

- Temperature calculation still has a bug (values jump or show unrealistic numbers).  
- Needs refinement of scratchpad decoding / resolution timing.  
- Not production-ready — use only as a reference for now.

## Features
- I²C init on I2C-A and DS2484 device reset.
- 1‑Wire reset/presence, ROM search, and per‑device Match ROM.
- Temperature conversion (up to 12‑bit) and scratchpad CRC check.
- Celsius conversion using `raw × 0.0625` (LSB = 1/16 °C at 12‑bit resolution).
- Simple out‑of‑range alert hook (toggle LED or add your own callback).

## Hardware
- **MCU:** TI LaunchPad F280025C (C2000).
- **Bridge:** Maxim DS2484 (I²C address 0x18 by default).
- **Sensors:** DS1825U (two sensors supported; works with 1+).

### Wiring (example — adjust to your LaunchPad pinout)
| Function | F280025C Pin | DS2484 Pin | Notes |
|---|---|---|---|
| I²C SDA | *GPIOx* | SDA | 4.7kΩ pull‑up to 3V3 |
| I²C SCL | *GPIOy* | SCL | 4.7kΩ pull‑up to 3V3 |
| 1‑Wire DQ | — | 1‑Wire to DS1825U bus | 4.7kΩ pull‑up to 3V3 on DQ |
| VCC | 3V3 | VDD | — |
| GND | GND | GND | Common ground |

> If you use parasitic power, ensure the DS2484 **strong pull‑up** is configured during conversion.

## Build (Code Composer Studio)
1. Open **CCS** (tested on recent CCS and C2000Ware).
2. Create or import an empty project for **F280025C**.
3. Add `src/` to **Sources** and `include/` to **Include Search Paths**.
4. Make sure I²C driverlib is linked (C2000Ware).  
   This project uses the **I2C controller API** from `driverlib.h`.
5. Build and flash.

## How it works
1. `DS2484_RESET()` ensures the bridge is alive (checks `RST` bit in status).  
2. `DS2484_1WireResetAndPoll()` generates 1‑Wire reset and waits for presence.  
3. ROM search (`DS2484_SearchROM`) collects 64‑bit ROM IDs.  
4. For each sensor: `Match ROM` → `Convert T` → wait (max 750ms for 12‑bit) → `Read Scratchpad` (9 bytes).  
5. Temperature: `raw = (MSB<<8) | LSB;  °C = raw × 0.0625`.  
6. Optional: verify `CRC` of 8 bytes against byte 8 of scratchpad.

## Repo layout
```
.
├── src/            # C sources (MCU‑side)
├── include/        # Headers
├── docs/           # PDFs
```

## Quick run (single device path shown in `main.c`)
- The default loop converts the temperature and reads the scratchpad every second.
- If presence pulse is missing or CRC mismatches, code stops at `ESTOP0` to ease debugging.

## Troubleshooting
- **I²C bus busy:** ensure proper pull‑ups and STOP is issued; power‑cycle the DS2484 if stuck.
- **No presence pulse:** check pull‑up on DQ and sensor power; verify DS2484 `PPD` bit after reset.
- **Jumping readings:** make sure you wait long enough for your resolution (12‑bit → 750ms).

## References
- DS1825U datasheet (see `docs/ds1825.pdf`).
- DS2484 datasheet (see `docs/ds2484.pdf`).
- TI driverlib for F280025C.


---


MIT License. See [LICENSE](./LICENSE).
