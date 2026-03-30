# Microcode Overview

The microcode is stored in two **AT28C16** EEPROMs and programmed via two custom .bin files produced from `microcode.c`. This page covers how the programmer works, the code structure, and how to modify/reprogram the microcode.

---

## Summary

The control logic requires two EEPROMs since each microcode control word is 16 bits wide, but the AT28C16 only outputs 8 bits at a time. One EEPROM stores the high byte of each control word, the other stores the low byte. Both chips are addressed identically,and the same address lines feed both simultaneously. Their outputs are combined to form the full 16-bit control word on every clock cycle.

---

## Address Layout

The EEPROM address is 10 bits wide, with each field encoding different information:
```
Bit: 9  8  |  7  |  6  5  4  3  |  2  1  0
     flags | byte| instruction  |   step
```

| Field | Bits | Values | Meaning |
|---|---|---|---|
| Flags | 9-8 | 0-3 | ZF/CF state (Z0C0, Z0C1, Z1C0, Z1C1) |
| Byte | 7 | 0-1 | 0 = high byte EEPROM, 1 = low byte EEPROM |
| Instruction | 6-3 | 0-15 | Which of the 16 instructions |
| Step | 2-0 | 0-7 | Which clock cycle within the instruction |

Total: 1024 addresses. Each EEPROM gets 512 bytes, one for each byte_sel state.

---

## Code Structure
```
8bitComputerCode/
├── microcode.c            ← Generates the two .bin files
├── microcode_high.bin      ← Flash to EEPROM 1 (high byte)
└── microcode_low.bin       ← Flash to EEPROM 2 (low byte)
```

---

## Control Signal Definitions

All control signals are defined as bit masks against a 16-bit control word:
```c
#define HLT 0b1000000000000000  // Halt clock
#define MI  0b0100000000000000  // Memory address register in
#define RI  0b0010000000000000  // RAM data in
#define RO  0b0001000000000000  // RAM data out
#define IO  0b0000100000000000  // Instruction register out
#define II  0b0000010000000000  // Instruction register in
#define AI  0b0000001000000000  // A register in
#define AO  0b0000000100000000  // A register out
#define EO  0b0000000010000000  // ALU out
#define SU  0b0000000001000000  // ALU subtract
#define BI  0b0000000000100000  // B register in
#define OI  0b0000000000010000  // Output register in
#define CE  0b0000000000001000  // Program counter enable
#define CO  0b0000000000000100  // Program counter out
#define J   0b0000000000000010  // Jump (program counter in)
#define FI  0b0000000000000001  // Flags in
```

---

## Microcode Table

Each instruction is defined as 8 steps. Every instruction starts with the same two fetch steps:

- **Step 0: `MI|CO`** — put program counter on memory address bus
- **Step 1: `RO|II|CE`** — load instruction from RAM, increment counter

Steps 2-7 are instruction-specific:
```c
uint16_t MICROCODE_TEMPLATE[16][8] = {
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 0000 - NOP
  { MI|CO, RO|II|CE, IO|MI,  RO|AI,  0,           0, 0, 0 }, // 0001 - LDA
  { MI|CO, RO|II|CE, IO|MI,  RO|BI,  EO|AI|FI,    0, 0, 0 }, // 0010 - ADD
  { MI|CO, RO|II|CE, IO|MI,  RO|BI,  EO|AI|SU|FI, 0, 0, 0 }, // 0011 - SUB
  { MI|CO, RO|II|CE, IO|MI,  AO|RI,  0,           0, 0, 0 }, // 0100 - STA
  { MI|CO, RO|II|CE, IO|AI,  0,      0,           0, 0, 0 }, // 0101 - LDI
  { MI|CO, RO|II|CE, IO|J,   0,      0,           0, 0, 0 }, // 0110 - JMP
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 0111 - JC
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 1000 - JZ
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 1001 - unused
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 1010 - unused
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 1011 - unused
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 1100 - unused
  { MI|CO, RO|II|CE, 0,      0,      0,           0, 0, 0 }, // 1101 - unused
  { MI|CO, RO|II|CE, AO|OI,  0,      0,           0, 0, 0 }, // 1110 - OUT
  { MI|CO, RO|II|CE, HLT,    0,      0,           0, 0, 0 }, // 1111 - HLT
};
```

---

## Conditional Jump Handling

`JC` and `JZ` need to behave differently depending on the CPU flags. The template defines them as no-ops (step 2 = 0). After copying the template into all four flag states, the relevant entries are patched:
```c
// CF=1: JC actually jumps
microcode[FLAGS_Z0C1][JC][2] = IO|J;

// ZF=1: JZ actually jumps
microcode[FLAGS_Z1C0][JZ][2] = IO|J;

// Both set: both jump
microcode[FLAGS_Z1C1][JC][2] = IO|J;
microcode[FLAGS_Z1C1][JZ][2] = IO|J;
```

The flags are part of the EEPROM address itself. The CPU drives A8 and A9 with the current flag state, so the EEPROM automatically outputs different microcode for the same instruction depending on flags. No external logic needed.

---

## How the .bin Files Are Generated

`microcode.c` loops through all 1024 addresses, extracts each field using bitmasking, looks up the correct control word, and writes the high or low byte to the appropriate array:
```c
// High byte EEPROM
for (int address = 0; address < 1024; address++) {
    if ((address & 0b0010000000) == 0) {
        int flags       = (address & 0b1100000000) >> 8;
        int instruction = (address & 0b0001111000) >> 3;
        int step        = (address & 0b0000000111);
        eeprom_high[high_idx++] = microcode[flags][instruction][step] >> 8;
    }
}

// Low byte EEPROM
for (int address = 0; address < 1024; address++) {
    if ((address & 0b0010000000) != 0) {
        int flags       = (address & 0b1100000000) >> 8;
        int instruction = (address & 0b0001111000) >> 3;
        int step        = (address & 0b0000000111);
        eeprom_low[low_idx++] = microcode[flags][instruction][step];
    }
}
```

Each array is then written to a 512-byte .bin file.

---

## Programming Procedure

1. Compile and run `microcode.c`:
```
gcc -std=c99 -o microcode microcode.c && ./microcode
```
2. Open TL866 software
3. Select chip: **AT28C16**
4. Load `microcode_high.bin` → program into EEPROM 1
5. Load `microcode_low.bin` → program into EEPROM 2
6. EEPROM 1 goes on the high byte data lines, EEPROM 2 on the low byte data lines
