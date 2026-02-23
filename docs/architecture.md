# Architecture

This computer implements a **Simple-As-Possible 1 (SAP-1)** architecture — a minimal but complete Von Neumann machine. Every subsystem connects to a single shared 8-bit bus.

---

## Block Diagram

```
                    ┌────────────────────────────────────┐
                    │           8-BIT DATA BUS           │
                    └──┬──────┬──────┬──────┬──────┬─────┘
                       │      │      │      │      │
                    ┌──┴─┐ ┌──┴─┐ ┌──┴─┐ ┌──┴─┐ ┌─┴──┐
                    │REG │ │REG │ │ IR │ │MAR │ │OUT │
                    │ A  │ │ B  │ │    │ │    │ │REG │
                    └──┬─┘ └──┬─┘ └──┬─┘ └──┬─┘ └─┬──┘
                       │      │      │      │      │
                       └──┬───┘      │   ┌──┴─┐  7-seg
                          │          │   │ RAM│  display
                       ┌──┴─┐        │   └────┘
                       │ALU │  opcode│
                       └──┬─┘   │    │
                          │  ┌──┴────┴──┐
                       ┌──┴─┐│  Control  │
                       │FLAG││  Logic    │◄── Step counter
                       │REG ││ (EEPROMs) │
                       └────┘└──────────┘
                                  │
                            ┌─────┴─────┐
                            │    PC     │
                            └───────────┘
```

---

## Bus Architecture

All data movement happens over a **single shared 8-bit bus**. At any given clock cycle, at most **one device drives the bus** (one output enabled) and **one or more devices read from it** (inputs latched).

This is enforced by hardware — every module's bus interface is either:
- A **tristate output** (74LS245, 74LS173 with output enable) that floats when not active
- An **input latch** that samples on the clock edge

Driving two outputs simultaneously causes a bus conflict (short circuit). The microcode must never assert two `*O` signals at the same time.

---

## Clock Domains

There is a single clock domain. All registers latch on the **rising edge**. The clock module provides:

- **Manual mode**: Single-step via debounced pushbutton
- **Auto mode**: Astable 555 oscillator, ~1 Hz to ~1 kHz adjustable via potentiometer
- **Halt**: Clock gated off by the `HLT` control signal

---

## Fetch-Decode-Execute

Every instruction follows the same three-phase pattern:

**Fetch (steps 0–1):** The program counter drives the bus, MAR latches the address, RAM puts the instruction byte on the bus, the IR latches it, and the PC increments.

**Decode (implicit):** The upper 4 bits of the IR are hardwired to the EEPROM address lines. No explicit decode step exists — the ROM simply outputs different control words based on which opcode is present.

**Execute (steps 2–N):** The EEPROM outputs instruction-specific control words, moving data between registers and RAM until the instruction completes. The step counter then resets to 0 for the next fetch.

---

## Signal Timing

```
CLK    ____╔════╗____╔════╗____╔════╗____
            ↑         ↑         ↑
          Fetch-0   Fetch-1   Execute-2
```

Control signals are combinational outputs of the EEPROM — they change *before* the rising edge (async with respect to the clock). Registers latch on the rising edge, capturing whatever the control logic has set up.

---

## Registers

| Register | Width | Type | Notes |
|---|---|---|---|
| A | 8 bits | General purpose accumulator | ALU input / result |
| B | 8 bits | ALU operand | Write-only from bus |
| IR | 8 bits | Instruction register | Upper nibble to control ROM |
| MAR | 4 bits | Memory address | Lower nibble only |
| PC | 4 bits | Program counter | Counts 0–15 |
| Flags | 2 bits | Carry + Zero | Feeds EEPROM address |
| Output | 8 bits | Display register | Drives 7-seg decoder |

---

## Memory Map

The address space is 4 bits wide — 16 bytes total.

| Address | Contents |
|---|---|
| 0x0–0xE | Program + data (user defined) |
| 0xF | Typically `HLT` or `JMP` |

There is no separation between code and data. Self-modifying code is possible but generally avoided.
