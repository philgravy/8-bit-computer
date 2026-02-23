# Overview

This project is a **fully functional 8-bit CPU** built from discrete 74LS-series TTL logic ICs on breadboards. It implements a simplified accumulator-based architecture (SAP-1) with a custom instruction set stored as microcode in AT28C16 EEPROMs.

The goal is to understand how a CPU works at the most fundamental level — every gate, every register, every timing edge is visible and physically real.

---

## What It Can Do

- Fetch, decode, and execute instructions from a 16-byte RAM
- Perform addition and subtraction with carry/zero flags
- Conditionally jump based on ALU flags
- Output results to a 7-segment decimal display
- Run at variable clock speeds from manual single-step up to ~1 kHz

---

## Design Philosophy

> Build it from scratch. Understand every wire.

This isn't emulation. Every control signal is a real voltage on a real wire. The instruction decoder isn't a lookup table in software — it's a pair of EEPROMs that output 16-bit control words. When the `LDA` instruction runs, you can watch the MAR load, the RAM put data on the bus, and Register A latch it, all at whatever speed your clock is set to.

---

## Key Deviations from Ben Eater's Design

| Feature | Ben Eater | This Build |
|---|---|---|
| Microcode | 2× AT28C16 | 2× AT28C16 (reprogrammed custom ISA) |
| Flags | Basic Z/C | Z/C with conditional jump logic |
| Visualization | LEDs only | LEDs + STM32 UART bus monitor |
| Schematics | Hand-drawn | KiCad (all modules) |

---

## Quick Start

**If you're here to understand the architecture:** start with [Architecture](architecture.md), then dig into individual modules.

**If you're here to replicate the build:** start with the [Bill of Materials](hardware-bom.md), then work through modules in order starting with [Clock](modules/clock.md).

**If you're here for the firmware:** jump to [EEPROM Programmer](firmware/eeprom-programmer.md) and [Microcode Reference](firmware/microcode-reference.md).

---

## Project Status

<span class="chip chip-green">Complete</span> Clock, PC, MAR, RAM, IR, ALU, Registers A/B, Output, Control Logic  
<span class="chip chip-orange">In Progress</span> STM32 bus visualizer  
<span class="chip chip-gray">Planned</span> Expanded RAM, additional instructions
