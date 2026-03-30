# 8-Bit Breadboard Computer

<div align="center">

![Build Status](https://img.shields.io/badge/status-in%20progress-yellow?style=flat-square)
![Architecture](https://img.shields.io/badge/architecture-SAP--1-blue?style=flat-square)
![Clock Speed](https://img.shields.io/badge/clock-variable%20Hz-green?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-lightgrey?style=flat-square)

**A fully functional 8-bit computer built from discrete 74LS-series logic ICs on breadboards.**  
Inspired by Ben Eater's SAP-1 architecture, extended with custom control logic, EEPROM-based microcode, and STM32-driven real-time visualization.

[📖 Full Documentation](https://yourusername.github.io/8-bit-computer) · [🔌 Schematics](./schematics/) · [💾 Firmware](./firmware/) · [📋 BOM](./hardware/BOM.md)

---

<!-- Replace with an actual photo or GIF of your build -->
![Build Photo](docs/assets/images/build-placeholder.png)

</div>

---

## Overview

This project implements a custom 8-bit CPU entirely from TTL logic on breadboards. It executes a hand-defined instruction set stored in 28C16 EEPROMs as microcode, making the control logic fully reprogrammable without rewiring.

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
8-bit-computer/
├── README.md
├── docs/                   # GitHub Pages (Docsify)
│   ├── index.html
│   ├── _sidebar.md
│   ├── overview.md
│   └── modules/
├── schematics/             # KiCad source files + PNG exports
├── firmware/
│   └── eeprom/             # EEPROM programmer (Arduino)
└── hardware/
    ├── BOM.md
    └── notes.md
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

## References

- [Ben Eater's 8-bit Computer Series](https://eater.net/8bit)
- [74LS173 Datasheet](https://www.ti.com/lit/ds/symlink/sn74ls173a.pdf)
- [AT28C16 EEPROM Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/doc0540.pdf)
- [James Bates' Extended ISA](https://github.com/jamesbates/8bit)

---

## License

MIT — use it, fork it, build on it.
