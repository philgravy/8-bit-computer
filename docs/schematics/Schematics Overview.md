# Schematics Overview

The schematics in this folder are sourced from [Ben Eater's 8-bit computer project](https://eater.net/8bit) and are his original work. They are included here as reference material for this build.

The STM32 Bus Visualizer schematic is a custom addition not part of Ben Eater's original design. It is currently in progress — the KiCad files reflect the planned architecture but the design is not finalized.

---

## Ben Eater's Schematics

| File | Module |
|---|---|
| `high-level.png` | High-level architecture overview |
| `clock.png` | Clock module |
| `a-register.png` | A register |
| `b-register.png` | B register |
| `alu.png` | Arithmetic logic unit |
| `ir.png` | Instruction register |
| `mar.png` | Memory address register |
| `ram.png` | RAM module |
| `pc.png` | Program counter |
| `control.png` | Control logic |
| `output.png` | Output register and 7-segment display |

Full schematics and build documentation are available at [eater.net/8bit/schematics](https://eater.net/8bit/schematics).

---

## STM32 Bus Visualizer

The STM32 visualizer schematic was built on top of Ben Eater's control logic schematic in KiCad. It adds nine daisy-chained 74HC165 shift registers to monitor 71 signals across the computer's buses and control lines, feeding into an STM32F411 Black Pill over SPI.

This module is still in progress. See [STM32 Bus Visualizer](../STM32BusVisualizer.md) for current architecture and implementation status.
