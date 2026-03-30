# Microcode Reference

The control logic is implemented as a lookup table burned into two AT28C16 EEPROMs. For every combination of **instruction opcode** and **step counter** value, the ROMs output a 16-bit control word that drives the computer's control lines.

---

## Control Word Layout

The 16 control signals are split across two bytes (two EEPROMs):

| Bit | Signal | Active | Function |
|---|---|---|---|
| 15 | `HLT` | HIGH | Halt clock |
| 14 | `MI`  | LOW  | Memory Address Register In |
| 13 | `RI`  | LOW  | RAM In |
| 12 | `RO`  | LOW  | RAM Out |
| 11 | `IO`  | LOW  | Instruction Register Out (lower nibble) |
| 10 | `II`  | LOW  | Instruction Register In |
| 9  | `AI`  | LOW  | Register A In |
| 8  | `AO`  | LOW  | Register A Out |
| 7  | `EO`  | LOW  | ALU (Sum) Out |
| 6  | `SU`  | HIGH | Subtract (ALU mode) |
| 5  | `BI`  | LOW  | Register B In |
| 4  | `OI`  | LOW  | Output Register In |
| 3  | `CE`  | HIGH | Program Counter Enable (increment) |
| 2  | `CO`  | LOW  | Program Counter Out |
| 1  | `J`   | LOW  | Jump (load PC) |
| 0  | `FI`  | LOW  | Flags Register In |

> Active LOW signals are asserted by pulling the line to 0V. The EEPROM outputs 0 for active, 1 for inactive on these lines.

---

## EEPROM Address Mapping

Each EEPROM address encodes:

```
Address[10:8] = Flags (C, Z) — for conditional jumps
Address[7:4]  = Instruction opcode (upper nibble of IR)
Address[3:0]  = Microcode step (0–7, from 3-bit counter)
```

EEPROM 1 outputs the high byte of the control word (bits 15–8).  
EEPROM 2 outputs the low byte (bits 7–0).

---

## Fetch Cycle (All Instructions)

Steps 0 and 1 are identical for every instruction — this is the **fetch cycle**:

| Step | Control Word | Description |
|---|---|---|
| 0 | `CO\|MI` | PC → MAR (put PC address into MAR) |
| 1 | `RO\|II\|CE` | RAM → IR, increment PC |

After step 1, the opcode bits are on the IR and the ROM switches to the instruction-specific microcode.

---

## Instruction Microcode

### `NOP` (0000) — No Operation

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2–4 | *(none)* | Idle |

---

### `LDA` (0001) — Load A from RAM

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `IO, MI` | IR operand → MAR |
| 3 | `RO, AI` | RAM[MAR] → Register A |

---

### `ADD` (0010) — Add

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `IO, MI` | IR operand → MAR |
| 3 | `RO, BI` | RAM[MAR] → Register B |
| 4 | `EO, AI, FI` | ALU sum → A, update flags |

---

### `SUB` (0011) — Subtract

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `IO, MI` | IR operand → MAR |
| 3 | `RO, BI` | RAM[MAR] → Register B |
| 4 | `EO, AI, SU, FI` | ALU difference → A, update flags |

---

### `STA` (0100) — Store A to RAM

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `IO, MI` | IR operand → MAR |
| 3 | `AO, RI` | A → RAM[MAR] |

---

### `LDI` (0101) — Load Immediate

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `IO, AI` | IR lower nibble → A directly |

---

### `JMP` (0110) — Unconditional Jump

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `IO, J` | IR operand → PC |

---

### `JC` (0111) — Jump if Carry

Implemented via the flags address bits in the EEPROM. When carry flag = 0:

| Step | Active Signals |
|---|---|
| 2 | *(none — no jump)* |

When carry flag = 1:

| Step | Active Signals |
|---|---|
| 2 | `IO, J` |

---

### `JZ` (1000) — Jump if Zero

Same pattern as `JC` but gated on the zero flag bit in the EEPROM address.

---

### `OUT` (1110) — Output Register A

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `AO, OI` | A → Output register (display) |

---

### `HLT` (1111) — Halt

| Step | Active Signals | Description |
|---|---|---|
| 0 | `CO, MI` | Fetch |
| 1 | `RO, II, CE` | Fetch |
| 2 | `HLT` | Stop clock |

---

## Microcode Overview

See [Microcode Overview](Microcode Overview.md) to see how this is implemented.
