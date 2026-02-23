# Instruction Register

<span class="chip chip-green">Complete</span> &nbsp; ICs: `74LS173 ×2` · `74LS245 ×1`

The instruction register (IR) holds the currently executing instruction. It captures the full 8-bit value from the bus during the fetch cycle, then splits it: the **upper 4 bits** (opcode) are routed to the control logic, and the **lower 4 bits** (operand/address) can be placed back on the bus for instructions that reference memory.

---

## How It Works

### Fetch Cycle

During the fetch cycle, the control logic asserts `II` (Instruction Register In). On the rising edge of the clock, both 74LS173 registers latch the current byte on the data bus. This is the opcode+operand byte stored at the current PC address.

### Opcode Routing

The upper nibble (bits 7–4) goes directly to the EEPROM address lines in the control logic. The microcode ROMs use this to look up the correct control word for each step of the instruction — it never needs to go back on the bus.

### Operand Output

The lower nibble (bits 3–0) is routed through the `74LS245` bus transceiver. When `IO` (Instruction Register Out) is asserted, the transceiver drives these 4 bits onto the lower half of the data bus. This is how an instruction like `LDA 0b1101` puts the address `0b1101` on the bus to load into the MAR.

---

## Schematic

![Instruction Register Schematic](../assets/images/ir-schematic.png)

> *Export from KiCad: File → Export → Export as Image → PNG @ 300 DPI*

---

## ICs Used

| Ref | Part | Function |
|---|---|---|
| U14 | 74LS173 | Upper nibble register (bits 7–4, opcode) |
| U15 | 74LS173 | Lower nibble register (bits 3–0, operand) |
| U13 | 74LS245 | Bidirectional bus transceiver for operand output |

### 74LS173 — 4-bit D-type Register

The 173 is a quad D flip-flop with three-state outputs and synchronous clock enable. Key pins:

| Pin | Name | Description |
|---|---|---|
| CLK | Clock | Rising-edge triggered |
| M, N | Output enable | Both LOW to enable outputs |
| G1, G2 | Clock enable | Both LOW to allow latching |
| CLR | Clear | Active HIGH async reset |
| D0–D3 | Data in | |
| Q0–Q3 | Data out | |

Both `M` and `N` must be LOW simultaneously to drive the outputs. This is how the `II`/`IO` control signals gate the register on and off the bus.

### 74LS245 — Octal Bus Transceiver

The 245 provides bidirectional buffered data transfer. In this design it's used unidirectionally — only ever driving the operand nibble *onto* the bus when `IO` is active.

| Pin | Function |
|---|---|
| DIR | Direction: LOW = B→A, HIGH = A→B |
| OE  | Output enable (active LOW) |

---

## Control Signals

| Signal | Active | Effect |
|---|---|---|
| `II` | LOW | Clock enables asserted; IR latches bus on next CLK ↑ |
| `IO` | LOW | 74LS245 OE asserted; lower nibble drives bus |
| `CLR` | HIGH | Async reset both registers to 0x00 |

---

## LED Indicators

8 LEDs (D26–D33) with 220Ω current-limiting resistors show the full 8-bit value of the IR at all times:

- **Blue LEDs** (D26–D30): upper nibble (opcode)
- **Yellow LEDs** (D31–D33): lower nibble (operand)

These are driven directly from the register Q outputs and are always visible regardless of bus state.

---

## Notes & Gotchas

**Both 74LS173 M/N pins must be LOW to output.** Leaving either HIGH puts the outputs in high-Z. This burned a few debugging sessions early on — if the IR seems to not be outputting, check both enable pins.

**The CLR line is shared** with the rest of the computer's global reset. Asserting reset clears the IR immediately regardless of clock, which is correct behavior.

**Capacitor C13** (0.1µF on VCC) is a decoupling cap. Don't skip it — the 74LS series draws current spikes on clock edges and this prevents noise coupling into the control lines.
