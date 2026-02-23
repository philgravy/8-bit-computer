# Bill of Materials

Complete component list for the full 8-bit computer. Quantities include ~10% spare for the smaller passives.

---

## ICs — 74LS Series

| QTY | Part | Package | Function | Modules |
|---|---|---|---|---|
| 2 | 74LS00 | DIP-14 | Quad NAND | Clock, Control |
| 1 | 74LS02 | DIP-14 | Quad NOR | Clock |
| 2 | 74LS04 | DIP-14 | Hex inverter | Various |
| 2 | 74LS08 | DIP-14 | Quad AND | Control, Flags |
| 1 | 74LS32 | DIP-14 | Quad OR | ALU |
| 4 | 74LS86 | DIP-14 | Quad XOR | ALU (full adder) |
| 6 | 74LS173 | DIP-16 | 4-bit D register | IR, MAR, Flags, Output |
| 2 | 74LS189 | DIP-16 | 16×4 SRAM | RAM |
| 2 | 74LS245 | DIP-20 | Octal bus transceiver | IR, Registers |
| 4 | 74LS273 | DIP-20 | 8-bit D register | Registers A/B, Output |
| 2 | 74LS283 | DIP-16 | 4-bit adder | ALU |
| 1 | 74LS163 | DIP-16 | 4-bit counter | PC |
| 1 | 74LS138 | DIP-16 | 3-to-8 decoder | Step counter |
| 2 | 74LS157 | DIP-16 | Quad 2-to-1 mux | RAM addressing |

## ICs — Other

| QTY | Part | Package | Function | Module |
|---|---|---|---|---|
| 3 | NE555 | DIP-8 | Timer | Clock |
| 2 | AT28C16 | DIP-24 | 2Kx8 EEPROM | Control Logic |
| 2 | 28C16 compatible | DIP-24 | Alternate EEPROM | Control Logic |

## Discrete Components

| QTY | Part | Value | Notes |
|---|---|---|---|
| 1 | Potentiometer | 1MΩ | Clock speed adjust |
| 1 | Potentiometer | 1kΩ | Clock trim |
| 2 | Capacitor, film | 0.01µF | 555 timing |
| 1 | Capacitor, film | 0.1µF | 555 timing |
| 20 | Capacitor, ceramic | 0.1µF | Decoupling (one per IC) |
| 10 | Capacitor, electrolytic | 10µF | Power supply bulk |
| 10 | Resistor | 1kΩ | Pull-ups, bus termination |
| 8 | Resistor | 220Ω | LED current limiting (output display) |
| 60+ | Resistor | 220Ω | LED indicators (all modules) |
| 1 | Resistor | 10kΩ | Reset pull-up |

## LEDs

| QTY | Color | Purpose |
|---|---|---|
| 8 | Red | Bus lines |
| 8 | Yellow | Register A |
| 8 | Yellow | Register B |
| 8 | Blue | Instruction Register (opcode) |
| 4 | Yellow | Instruction Register (operand) |
| 4 | Green | Program Counter |
| 4 | Orange | MAR |
| 2 | White | Flags (C, Z) |
| 8 | Green | Output register |

## Displays & Connectors

| QTY | Part | Notes |
|---|---|---|
| 1 | 4-digit 7-segment display | Common anode, for output |
| 1 | 7-segment driver (or discrete) | e.g. 74LS47 or manual decode |
| 4 | 830-point breadboard | Main modules |
| 2 | 400-point breadboard | Smaller modules |
| 1 | DC barrel jack | 5V power input |
| 1 | 5V power supply, ≥1A | |
| 1 | Momentary pushbutton | Manual clock step |
| 1 | Momentary pushbutton | Reset (CLR) |
| 1 | SPDT toggle switch | Clock mode (manual/auto) |
| 8 | SPDT toggle switches | RAM programming |
| 1 | SPDT toggle switch | Write enable (RAM) |

## Tools Required

- Arduino Uno (EEPROM programmer)
- 28-pin ZIF socket (recommended for EEPROM programming)
- Logic probe or oscilloscope
- Multimeter
- Wire cutters / strippers
- 22 AWG solid hookup wire (multiple colors)
