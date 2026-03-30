# STM32 Bus Visualizer
<span class="chip chip-orange">In Progress</span> &nbsp; MCU: `STM32F411`, known as 'STM32 Black Pill Board'

The STM32 visualizer is an external monitoring system that observes the computer's bus and control signals in real time and transmits state information over UART for display on a PC or LCD.

---

## Goals

- Display the current bus value, register contents, and active control signals at each clock cycle
- Provide a readable, timestamped execution trace without interrupting normal operation
- Serve as an educational overlay for demonstrations and debugging

---

## Architecture
```
  Computer buses/signals (71 signals)
         │
    ┌────┴──────────────────┐
    │  9× 74HC165           │  ← Daisy-chained shift registers
    │  Parallel in,         │    capture all signals in parallel
    │  serial out           │
    └────┬──────────────────┘
         │ SPI (3 pins: MISO, SCK, CS)
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

---

## Shift Registers

The STM32 Black Pill has approximately 32 usable GPIOs. The computer has 71 signals to monitor, far more than available pins. Direct wiring is not feasible.

Nine **74HC165** parallel-in serial-out shift registers are daisy-chained together. Each captures 8 signals simultaneously when the latch pin is pulsed. The STM32 then clocks out all 72 bits serially using only 3 pins (latch, clock, data). This is functionally equivalent to SPI.
```
8 signals    8 signals    8 signals         8 signals
    │            │            │                 │
    ▼            ▼            ▼                 ▼
┌───────┐    ┌───────┐    ┌───────┐    ...  ┌───────┐
│74HC165│───▶│74HC165│───▶│74HC165│───▶     │74HC165│
└───────┘    └───────┘    └───────┘         └───────┘
                                                 │
                                              STM32
                                             (3 pins)
```

The 74HC165s are powered from **3.3V** so their serial output drives the STM32 GPIO directly with no level shifter needed. Their inputs correctly read 5V signals from the 74LS chips because the input high threshold at 3.3V supply is ~2V, well within 74LS output levels.

---

## Signals to Monitor

| Group | Signals | 74HC165s |
|---|---|---|
| BUS[7:0] | PA0–PA7 | 1 |
| IR[7:0] | PC0–PC7 | 1 |
| Flags + Step + CLK + CLR | 6 signals | 1 (2 pins spare) |
| PC + MAR | 8 signals | 1 |
| Control signals | 16 signals | 2 |
| Register A | 8 signals | 1 |
| Remaining signals | TBD | 2 |
| **Total** | **71 signals** | **9** |

### STM32 Pin Usage

| Signal | STM32 Pin | Notes |
|---|---|---|
| PL (latch) | Any GPIO | Pulse LOW to snapshot all inputs |
| CLK | SPI_SCK | Hardware SPI clock |
| MISO | SPI_MISO (PA6) | Serial data in — PA6 is 5V tolerant |
| CE (chip enable) | Tied LOW | Always enabled |
| UART TX | USART pin | Output to PC/LCD |

Total: 3 pins for capture chain, 2 for UART output.

---

## How the Capture Works
```c
void read_shift_registers(uint8_t *buf, uint8_t num_chips) {
    // Pulse latch LOW — snapshots all inputs simultaneously
    PL_LOW();
    PL_LOW();  // brief hold
    PL_HIGH();

    // Shift out all bits serially, MSB first
    for (int chip = num_chips - 1; chip >= 0; chip--) {
        buf[chip] = 0;
        for (int bit = 7; bit >= 0; bit--) {
            if (READ_MISO()) {
                buf[chip] |= (1 << bit);
            }
            CLK_HIGH();
            CLK_LOW();
        }
    }
}
```

Called on every rising clock edge via interrupt:
```c
// Called on CLK rising edge interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == CLK_PIN) {
        uint8_t state[9];
        read_shift_registers(state, 9);
        // decode and transmit over UART
    }
}
```

---

## Output Format
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
- [ ] 74HC165 chain wired and verified with known test patterns
- [ ] Clock edge detection interrupt
- [ ] Bus sampling on clock edge via SPI
- [ ] UART output formatted
- [ ] PC-side terminal display script
- [ ] Integration testing

---

## Code

*Coming soon.* Will be located in `firmware/stm32/`.
