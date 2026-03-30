# 8-Bit Breadboard Computer

<div align="center">

![Build Status](https://img.shields.io/badge/status-in%20progress-yellow?style=flat-square)
![Architecture](https://img.shields.io/badge/architecture-SAP--1-blue?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-lightgrey?style=flat-square)
<!--[Clock Speed](https://img.shields.io/badge/clock-variable%20Hz-green?style=flat-square)-->

**A fully functional 8-bit computer built from discrete 74LS-series logic ICs on breadboards.**  
Inspired by Ben Eater's SAP-1 architecture, extended with STM32-driven real-time visualization.

[📖 Full Documentation](./fulldocumenation/) · [🔌 Schematics](./docs/schematics/) · [💾 Firmware](.docs/firmware/) · [📋 BOM](./hardware/BOM.md)

---

<img src="https://github.com/user-attachments/assets/20b6e02a-6d24-4301-82a8-a23c3fe94a1b" alt="Image" width="400">

In-Progress Computer Build

</div>

---

## Executive Summary

The 8-Bit Breadboard Computer is a fully functional stored-program computer constructed from discrete 74LS-series TTL logic ICs on breadboards. Inspired by Ben Eater's SAP-1 architecture and supervised by Dr. Abdul Ofoli at the University of Tennessee at Chattanooga, the design extends the baseline architecture with three key additions: custom EEPROM-based microcode using AT28C16 devices for flexible instruction definition, a custom 7-segment display decoder programmed directly into a second EEPROM, and a real-time bus visualization system built around an STM32 microcontroller that monitors 71 internal signals simultaneously using nine daisy-chained 74HC165 shift registers over SPI.

The computer executes a complete fetch-decode-execute cycle driven by a variable-frequency clock, with all subsystems: ALU, registers, program counter, RAM, and output stage, implemented in physical hardware. The project is divided across three team members: Braylon Nix leads the clock and arithmetic subsystems, Andrew Adams is responsible for RAM, program counter, and physical enclosure, and Philip Segraves owns control logic, EEPROM programming, and the STM32 visualizer firmware.

The result is both a functional 8-bit computing platform and a tangible demonstration of computer architecture from first principles — with complete schematics, open-source firmware, and documented test procedures.

## Problem Statement

Computer architecture is a foundational subject in electrical and computer engineering, yet it is almost universally taught through simulation and abstraction. Students interact with CPUs through software models, HDL simulations, or high-level emulators — tools that, while useful, obscure the physical reality of how a processor actually works. The gap between "understanding a block diagram" and "understanding why a control signal pulse causes a register to latch a value" is significant, and most curricula never close it.

There is no hands-on hardware platform in the current UTC EE curriculum that allows students to build, observe, and debug a working CPU at the gate level. Furthermore, existing open-source SAP-1 implementations provide no mechanism to observe real-time internal bus activity during execution — making it difficult to verify correct operation or diagnose faults without external test equipment.

This project addresses all three gaps directly: by constructing a fully functional 8-bit computer from discrete 74LS TTL logic, the team makes every architectural concept physically tangible. By adding an STM32-driven visualization system monitoring 71 signals in real time, the computer becomes its own debugging instrument — something no existing SAP-1 implementation offers.

## High-level Overview

| Spec | Value |
|---|---|
| Data bus width | 8 bits |
| Address space | 16 bytes (4-bit MAR) |
| Clock | Variable (manual step to ~1 kHz) |
| Instruction set | Custom 16-instruction |
| Microcode storage | AT28C16 EEPROMs |
| Control signals | 16 active-low/high control lines |
| Visualization | STM32-based bus monitor *(in progress)* |

---

## Architecture

<img src="https://github.com/user-attachments/assets/18fbd141-401d-40cf-bfd1-50ad22ab90a3" width="500">

---

## Modules

| Module | Status | Description |
|---|---|---|
| [Clock](docs/modules/clock.md) | ✅ Complete | Astable/monostable 555-based variable clock |
| [Program Counter](docs/modules/program-counter.md) | ✅ Complete | 4-bit 74LS163 counter |
| [Memory Address Register](docs/modules/mar.md) | ✅ Complete | 4-bit 74LS173 register |
| [RAM](docs/modules/ram.md) | ✅ Complete | 16-byte static RAM + manual programming |
| [Instruction Register](docs/modules/instruction-register.md) | ✅ Complete | 8-bit 74LS173 pair + 74LS245 bus transceiver |
| [ALU](docs/modules/alu.md) | ✅ Complete | Full adder + subtractor with flags |
| [Register A / B](docs/modules/registers.md) | ✅ Complete | 8-bit general purpose registers |
| [Output Register](docs/modules/output.md) | ✅ Complete | 74LS273 + 7-segment display driver |
| [Control Logic](docs/modules/control-logic.md) | ✅ Complete | EEPROM-based microcode sequencer |
| [STM32 Visualizer](docs/modules/stm32-visualizer.md) | 🔧 In Progress | Real-time bus monitoring over UART |

---

## Getting Started

### Running a Program

1. Set the clock to **manual step** mode
2. Use the **programming switches** to enter your program into RAM
3. Assert **CLR** to reset the program counter
4. Switch to **auto clock** and set your desired speed
5. Watch the output register for results

### Reprogramming Microcode

See [`firmware/eeprom/`](./firmware/eeprom/) for the EEPROM programmer code and microcode definitions. Instructions are defined in `microcode.h` as control word arrays indexed by `[instruction][step]`.

---

## Repository Structure

```
UPDATE IT
```

---

## Build Log

| Date | Milestone |
|---|---|
| TBD | Project started |
| TBD | Clock + PC complete |
| TBD | RAM + MAR complete |
| TBD | ALU complete, first ADD instruction executes |
| TBD | Full instruction set running |
| TBD | STM32 visualizer integration |

---

## Architecture Terms

| Acronym | Meaning | Description |
|--------|--------|------------|
| SAP | Simple-As-Possible Computer | A minimal educational computer architecture demonstrating basic CPU operation and the fetch–decode–execute cycle. |
| ALU | Arithmetic Logic Unit | Performs arithmetic and logical operations on data from registers. |
| MAR | Memory Address Register | Holds the address of the memory location being accessed. |
| RAM | Random Access Memory | Stores program instructions and data. |
| PC | Program Counter | Holds the address of the next instruction to be executed. |
| IR | Instruction Register | Stores the current instruction being executed. |
| CU | Control Unit | Generates control signals to coordinate CPU operations; implemented via microcode in EEPROM. |
| EEPROM | Electrically Erasable Programmable Read-Only Memory | Stores microcode that defines control logic behavior. |
| CLK | Clock | Provides timing signals to synchronize system operations. |

---
## Contact Info


Philip Segraves : philipsegraves121@gmail.com
Andrew Adams : andrew.b.a.2003@gmail.com
Braylon Nix : brs279@mocs.utc.edu

---
## References


- [Ben Eater's 8-bit Computer Series](https://eater.net/8bit)
- [74LS173 Datasheet](https://www.ti.com/lit/ds/symlink/sn74ls173a.pdf)
- [AT28C16 EEPROM Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/doc0540.pdf)

---

## License

MIT — use it, fork it, build on it.
