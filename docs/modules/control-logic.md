# Control Logic

<span class="chip chip-green">Complete</span> &nbsp; ICs: `AT28C16 ×2` · `74LS163 ×1` · `74LS138 ×1`

The control logic is the brain of the computer. It takes the current **instruction opcode**, the **execution step**, and the **processor flags**, and outputs a 16-bit control word that tells every other module what to do on each clock cycle.

---

## How It Works

### The Core Idea

Rather than implementing the instruction decoder with combinational logic gates (which would require hundreds of gates for even a small instruction set), the control logic uses two EEPROMs as a lookup table. The EEPROM address encodes the current state; the output data encodes what to do.

```
  IR[7:4]    ──►┐
  Step[2:0]  ──►├── EEPROM Address ──► Control Word [15:0]
  Flags[1:0] ──►┘
```

This makes the instruction set **fully reprogrammable** without changing any wiring. Adding a new instruction is just a matter of reprogramming the EEPROMs.

### Step Counter

A **74LS163** 4-bit binary counter counts the current microcode step (0–7). It increments on each rising clock edge. A **74LS138** decoder detects the terminal count and resets the counter after the last step of each instruction.

The reset step varies per instruction — `LDI` finishes at step 2, `ADD` at step 4. The programmer handles this by setting remaining steps to a control word with all outputs inactive, and the counter is reset by detecting a specific pattern.

### EEPROM Address Layout

```
A[10:9] = Flags {Carry, Zero}
A[8:5]  = Instruction opcode (IR bits 7–4)
A[4:2]  = Step count (0–7)
A[1:0]  = Not used (set to 0)
```

Two EEPROMs share the same address bus but have separate chip selects. EEPROM 1 outputs control bits 15–8; EEPROM 2 outputs bits 7–0.

### Conditional Jumps

Conditional jumps (`JC`, `JZ`) are handled entirely within the EEPROM table. The flags bits in the address mean the same opcode at the same step produces different control words depending on whether a flag is set. No extra logic required.

---

## Schematic

![Control Logic Schematic](../assets/images/control-logic-schematic.png)

---

## ICs Used

| Ref | Part | Function |
|---|---|---|
| U1, U2 | AT28C16 | 2Kx8 EEPROM — microcode storage |
| U3 | 74LS163 | Step counter |
| U4 | 74LS138 | Step counter decode / reset |

### AT28C16 — 2Kx8 EEPROM

- 11-bit address space (A0–A10), 8-bit data output
- Access time: 150ns (sufficient for kHz clock speeds)
- 5V supply, CMOS compatible outputs
- No erase cycle needed — byte write overwrites directly
- Internal 10ms write cycle; programmer must wait between writes

---

## Control Signals

The control logic doesn't receive control signals — it generates them. Its inputs are:

| Input | Source | Notes |
|---|---|---|
| IR[7:4] | Instruction Register | Opcode bits |
| Step[2:0] | 74LS163 counter | Current microcode step |
| Flags[1:0] | Flags register | {Carry, Zero} |
| CLK | Clock module | Steps the counter |
| CLR | Global reset | Resets step counter |

---

## Microcode

See [Microcode Reference](../firmware/microcode-reference.md) for the full control word table.

See [EEPROM Programmer](../firmware/eeprom-programmer.md) for the Arduino sketch that generates and burns the microcode.

---

## Notes

**EEPROM output timing**: The AT28C16 has a 150ns access time. At 1 kHz clock (500µs half-period), this is not a concern. If you run at higher speeds, verify the control word is stable before the rising clock edge.

**No reset on power-up**: The step counter starts at an undefined value. Always assert `CLR` before running a program. The reset pushbutton handles this.

**Both EEPROMs must be programmed consistently**: If only one is reprogrammed, the control words will be corrupt. Always reprogram both simultaneously using the paired write function in the programmer sketch.
