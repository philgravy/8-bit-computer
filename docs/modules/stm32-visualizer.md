# STM32 Bus Visualizer

<span class="chip chip-orange">In Progress</span> &nbsp; MCU: `STM32F4xx`

The STM32 visualizer is an external monitoring system that observes the computer's bus and control signals in real time and transmits state information over UART for display on a PC or LCD.

---

## Goals

- Display the current bus value, register contents, and active control signals at each clock cycle
- Provide a readable, timestamped execution trace without interrupting normal operation
- Serve as an educational overlay for demonstrations and debugging

---

## Planned Architecture

```
  Computer buses/signals
         │
    ┌────┴─────┐
    │  STM32   │  ← GPIO inputs (non-invasive monitoring)
    │  Monitor │
    └────┬─────┘
         │ UART
    ┌────┴─────┐
    │  PC / LCD│  ← Display / logging
    └──────────┘
```

The STM32 monitors signals purely as inputs — it never drives the computer's bus.

### Signals to Monitor

| Signal | GPIO | Notes |
|---|---|---|
| BUS[7:0] | PA0–PA7 | Full data bus |
| CLK | PB0 | Clock for edge detection |
| IR[7:0] | PC0–PC7 | Instruction register |
| Control signals | TBD | Key subset: MI, RO, AI, etc. |
| Flags (C, Z) | PD0–PD1 | ALU flags |

### Output Format

```
CLK#  STEP  INST        BUS   CTRL
0005  0     LDA [0xC]   0x00  CO|MI
0006  1     LDA [0xC]   0x1C  RO|II|CE
0007  2     LDA [0xC]   0x0C  IO|MI
0008  3     LDA [0xC]   0x42  RO|AI
```

---

## Implementation Status

- [ ] GPIO input mapping finalized
- [ ] Clock edge detection interrupt
- [ ] Bus sampling on clock edge
- [ ] UART output formatted
- [ ] PC-side terminal display script
- [ ] Integration testing

---

## Code

*Coming soon.* Will be located in `firmware/stm32/`.
