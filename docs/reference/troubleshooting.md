# Troubleshooting

Common issues and how to diagnose them.

---

## Bus Issues

### Bus stuck HIGH or LOW

Check for:
- Two `*Out` signals asserted simultaneously (bus conflict)
- A tristate output with OE floating (should be pulled HIGH to disable)
- Faulty 74LS245 — pull the IC and test in isolation

### Register not latching

Check for:
- Clock enable pins (G1/G2 on 74LS173) not both LOW
- Clock not reaching the IC — probe the CLK pin directly
- VCC/GND not connected on that IC (breadboard missed connection)

---

## Clock Issues

### Clock not running in auto mode

- Check 555 oscillator output with a logic probe — should be toggling
- Verify `HLT` signal is LOW (not asserted)
- Check the clock mode switch for clean contact

### Clock running but computer not advancing

- Verify the step counter is incrementing — probe the 74LS138 inputs
- Check that the step counter reset isn't stuck asserted

---

## Microcode / EEPROM Issues

### Computer executing wrong instructions

- Re-read the EEPROM contents and compare against the expected microcode table
- Verify the EEPROM address lines are connected correctly (A0–A10)
- Check that the flags bits (address lines A8–A9) are wired from the flags register outputs

### EEPROM not programming

- Ensure WE is pulsed LOW for each byte write (don't hold LOW continuously)
- Confirm 10ms delay after each byte write for the internal programming cycle
- Check VCC is 5V (not 3.3V) — AT28C16 requires 5V

---

## ALU Issues

### Wrong ADD result

- Verify carry chain between the two 74LS283 adders (C4 of lower adder → C0 of upper adder)
- Check XOR gates for subtraction inversion — should only invert when `SU` is HIGH
- Probe B register outputs directly to confirm correct value loaded

### Flags not updating

- `FI` must be asserted on the same step as `EO` for ALU results
- Verify the zero flag logic — zero is typically detected with a NOR tree across all 8 ALU output bits

---

## Output Display Issues

### Display showing wrong value

- Check the 74LS273 output register is latching correctly
- Verify the 7-segment decoder or custom EEPROM decoder table
- For negative numbers displayed as 2's complement: verify the display EEPROM handles the sign bit

---

## General Debugging Strategy

1. **Isolate the module** — disconnect from the bus and test inputs/outputs independently
2. **Use single-step clock** — manually step through each microcode step and probe signals
3. **Check power** — measure VCC at several points. Long bus wires on breadboards have resistance
4. **Decoupling caps** — if a module works at slow clock speeds but fails fast, it likely needs a decoupling cap on VCC (0.1µF ceramic close to the IC)
5. **Re-seat ICs** — breadboard contacts get unreliable, especially with many insertions
