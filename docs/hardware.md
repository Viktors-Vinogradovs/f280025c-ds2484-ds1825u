# Hardware Notes

- MCU: TI LaunchPad F280025C
- Bridge: Maxim DS2484 (0x18 default I²C address)
- Sensors: Two DS1825U on shared 1‑Wire bus
- Pull‑ups:
  - I²C: 4.7kΩ to 3V3 (SDA,SCL)
  - 1‑Wire DQ: 4.7kΩ to 3V3

> Adjust wiring table to your exact LaunchPad pinout.
