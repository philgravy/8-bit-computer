# Control Signals Reference

Quick reference for all 16 control lines. These are the outputs of the microcode EEPROMs and drive every data movement in the computer.

---

## Signal Table

| Signal | Full Name | Active | Driven By | Description |
|---|---|---|---|---|
| `HLT` | Halt | HIGH | EEPROM bit 15 | Disables the clock oscillator |
| `MI` | Memory Address Register In | LOW | EEPROM bit 14 | MAR latches bus on next clock ↑ |
| `RI` | RAM In | LOW | EEPROM bit 13 | RAM writes from bus at current MAR address |
| `RO` | RAM Out | LOW | EEPROM bit 12 | RAM drives bus with contents at MAR address |
| `IO` | Instruction Register Out | LOW | EEPROM bit 11 | IR lower nibble drives bus via 74LS245 |
| `II` | Instruction Register In | LOW | EEPROM bit 10 | IR latches bus on next clock ↑ |
| `AI` | Register A In | LOW | EEPROM bit 9 | Register A latches bus on next clock ↑ |
| `AO` | Register A Out | LOW | EEPROM bit 8 | Register A drives bus via 74LS245 |
| `EO` | ALU (Sum) Out | LOW | EEPROM bit 7 | ALU result drives bus |
| `SU` | Subtract | HIGH | EEPROM bit 6 | Inverts B input to ALU (2's complement sub) |
| `BI` | Register B In | LOW | EEPROM bit 5 | Register B latches bus on next clock ↑ |
| `OI` | Output Register In | LOW | EEPROM bit 4 | Output register latches bus on next clock ↑ |
| `CE` | Counter Enable | HIGH | EEPROM bit 3 | PC increments on next clock ↑ |
| `CO` | Counter Out | LOW | EEPROM bit 2 | PC value drives bus |
| `J` | Jump | LOW | EEPROM bit 1 | PC loads from bus on next clock ↑ |
| `FI` | Flags In | LOW | EEPROM bit 0 | Flags register latches carry/zero from ALU |

---

## Bus Contention Rules

**Only one output signal may be active per clock cycle.** The following signals drive the bus and are mutually exclusive:

- `RO` — RAM Out
- `IO` — IR Out
- `AO` — A Out
- `EO` — ALU Out
- `CO` — PC Out

Asserting two simultaneously causes a bus conflict. The microcode enforces this — verify any custom instructions against this constraint.

---

## Signal Timing

All control signals are **asynchronous combinational outputs** of the EEPROMs. They change whenever the step counter or IR changes, without waiting for a clock edge. Registers sample these signals on the **rising clock edge**.

This means:
- Control signals must be **stable before** the rising edge
- There is propagation delay through the EEPROM (~150ns for AT28C16)
- At high clock speeds, EEPROM propagation delay may cause setup time violations

The 74LS series has ~20ns setup time requirements. At 1 kHz clock, timing is not an issue. If you push toward 100 kHz+, consider faster EEPROMs (AT28C64 is faster) or adding pipeline registers.

---

## Step Counter Reset

The step counter resets to 0 after each instruction completes. This is done by decoding the step count and asserting reset on the counter when the last microcode step is reached. The specific step varies per instruction (LDA ends at step 3, ADD at step 4, etc.).

One approach: tie the reset line to a NOR gate that detects when all remaining microcode steps are `0x0000` (no-ops). This auto-resets without explicit programming.

Alternatively, explicitly include a reset control bit in the control word. Ben Eater uses this approach — it's more predictable at the cost of one extra EEPROM address bit.
