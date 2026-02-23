# EEPROM Programmer

The microcode is stored in two **AT28C16** EEPROMs and programmed via a custom Arduino sketch. This page covers how the programmer works, the code structure, and how to modify/reprogram the microcode.

---

## Hardware Setup

The programmer uses an Arduino Uno to drive the EEPROM's address and data lines directly:

| EEPROM Pin | Arduino Pin | Notes |
|---|---|---|
| A0–A7 | D22–D29 (or shift register) | Address lines |
| A8–A10 | D30–D32 | Remaining address lines |
| D0–D7 | D2–D9 | Data lines (bidirectional) |
| OE (pin 20) | D11 | Output enable, active LOW |
| WE (pin 21) | D12 | Write enable, active LOW |
| CE (pin 18) | GND | Chip enable, tied LOW (always enabled) |

> **Page write mode**: The AT28C16 supports page writes (up to 16 bytes in one write cycle). The programmer uses byte-at-a-time writes for reliability with a 10ms post-write delay, though page writes work if you need faster programming.

---

## Code Structure

```
firmware/eeprom/
├── eeprom_programmer.ino   ← Main Arduino sketch
├── microcode.h             ← Control word definitions
└── README.md
```

### `microcode.h`

All control signals are defined as bit masks against a 16-bit control word:

```c
// Control signal definitions (active LOW signals inverted for readability)
#define HLT 0b1000000000000000  // Halt
#define MI  0b0100000000000000  // Memory Address Register In
#define RI  0b0010000000000000  // RAM In
#define RO  0b0001000000000000  // RAM Out
#define IO  0b0000100000000000  // Instruction Register Out
#define II  0b0000010000000000  // Instruction Register In
#define AI  0b0000001000000000  // Register A In
#define AO  0b0000000100000000  // Register A Out
#define EO  0b0000000010000000  // ALU Out
#define SU  0b0000000001000000  // Subtract
#define BI  0b0000000000100000  // Register B In
#define OI  0b0000000000010000  // Output Register In
#define CE  0b0000000000001000  // Program Counter Enable
#define CO  0b0000000000000100  // Program Counter Out
#define J   0b0000000000000010  // Jump
#define FI  0b0000000000000001  // Flags In
```

The microcode table is a 2D array indexed by `[opcode][step]`:

```c
// Fetch cycle is common to all instructions
#define FETCH_0  (CO|MI)
#define FETCH_1  (RO|II|CE)

uint16_t microcode[16][8] = {
  // 0000 - NOP
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },

  // 0001 - LDA (Load A from address)
  { FETCH_0, FETCH_1, IO|MI, RO|AI, 0, 0, 0, 0 },

  // 0010 - ADD
  { FETCH_0, FETCH_1, IO|MI, RO|BI, EO|AI|FI, 0, 0, 0 },

  // 0011 - SUB
  { FETCH_0, FETCH_1, IO|MI, RO|BI, EO|AI|SU|FI, 0, 0, 0 },

  // 0100 - STA (Store A to address)
  { FETCH_0, FETCH_1, IO|MI, AO|RI, 0, 0, 0, 0 },

  // 0101 - LDI (Load immediate)
  { FETCH_0, FETCH_1, IO|AI, 0, 0, 0, 0, 0 },

  // 0110 - JMP
  { FETCH_0, FETCH_1, IO|J, 0, 0, 0, 0, 0 },

  // 0111 - JC (Jump if carry) — no-jump variant (flags=00 or flags=01)
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },

  // 1000 - JZ (Jump if zero) — no-jump variant
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },

  // 1001–1101 - Unused
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },
  { FETCH_0, FETCH_1, 0, 0, 0, 0, 0, 0 },

  // 1110 - OUT
  { FETCH_0, FETCH_1, AO|OI, 0, 0, 0, 0, 0 },

  // 1111 - HLT
  { FETCH_0, FETCH_1, HLT, 0, 0, 0, 0, 0 },
};
```

### Conditional Jump Handling

`JC` and `JZ` are handled by the **EEPROM address flags bits**. The ROM is programmed four times — once for each combination of `[C, Z]` flags. The programmer iterates over all four flag combinations and writes the appropriate microcode:

```c
void programEEPROMs() {
  for (int flags = 0b00; flags <= 0b11; flags++) {
    for (int instruction = 0; instruction < 16; instruction++) {
      for (int step = 0; step < 8; step++) {

        uint16_t controlWord = microcode[instruction][step];

        // Override JC at step 2 when carry flag is set
        if (instruction == 0b0111 && step == 2 && (flags & 0b01))
          controlWord = IO|J;

        // Override JZ at step 2 when zero flag is set
        if (instruction == 0b1000 && step == 2 && (flags & 0b10))
          controlWord = IO|J;

        int address = (flags << 8) | (instruction << 4) | step;
        writeByte(EEPROM_1_CS, address, controlWord >> 8);
        writeByte(EEPROM_2_CS, address, controlWord & 0xFF);
      }
    }
  }
}
```

---

## Programming Procedure

1. Connect the Arduino to the EEPROM as shown above
2. Open `eeprom_programmer.ino` in the Arduino IDE
3. Upload the sketch — it programs automatically on startup
4. Open Serial Monitor at 57600 baud to watch progress
5. The programmer reads back and verifies every byte after writing

> **Erase first**: The AT28C16 doesn't require a separate erase — writing 0xFF to all addresses is sufficient before reprogramming.

---

## Verification

The programmer prints a hex dump of the EEPROM contents after programming. Verify the control words match the expected values from the [Microcode Reference](microcode-reference.md).

Example output:
```
Programming EEPROM 1...
0000: 04 4A 00 00 00 00 00 00 | 04 4A 44 08 00 00 00 00
0010: 04 4A 44 22 82 00 00 00 | 04 4A 44 22 C2 00 00 00
...
Verifying...
PASS — 1024 bytes verified
```
