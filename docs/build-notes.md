# Build Notes

Practical notes from the construction process — lessons learned, wiring conventions, and tips for anyone replicating this build.

---

## Wiring Conventions

Consistent wire colors make debugging dramatically faster. This build uses:

| Color | Signal |
|---|---|
| Red | VCC (5V) |
| Black | GND |
| Yellow | Data bus lines |
| Blue | Control signals (active LOW) |
| Green | Control signals (active HIGH) |
| White | Clock |
| Orange | Address lines |

---

## Power Distribution

The 74LS series is surprisingly picky about power supply noise. Key practices:

- **0.1µF ceramic decoupling cap** on VCC pin of every IC, placed as close to the IC as physically possible
- **10µF electrolytic** bulk capacitor every 4–5 ICs on the power rail
- **Separate power rails** for the clock module and the main logic — the 555 can generate noise that causes spurious clocking

A clean 5V bench supply or regulated wall adapter is sufficient. Don't power from an Arduino — the onboard 5V regulator can't supply enough current for all ICs simultaneously.

---

## Breadboard Layout

Modules are organized left to right in rough execution order:

```
[Clock] [PC] [MAR] [RAM] [IR] [REG A/B] [ALU] [FLAGS] [OUTPUT] [CONTROL]
```

This keeps the data flow mostly left-to-right and makes it easier to trace a single instruction visually.

The bus runs as a horizontal spine across the top of all breadboards. Each module taps into the bus via vertical wires. This prevents the bus from becoming a tangled nest.

---

## Known Issues

| Issue | Workaround |
|---|---|
| RAM programming switches cause glitches at high clock speed | Switch to manual mode before programming |
| Clock noise on long bus wires | Added 1kΩ series resistors on clock distribution |
| 74LS189 RAM outputs are active LOW | Inverter array after each RAM output |

---

## Lessons Learned

**Build and test one module at a time.** Don't connect everything and then debug. Each module should work in isolation before joining the bus.

**LEDs are your best debugger.** Every register, bus line, and control signal has an LED. The ability to see the exact state at each clock step is invaluable.

**The step counter reset is subtle.** Getting the step counter to reset at the right time — not too early, not after extra no-op steps — took the most debugging time of any single issue.

**Decoupling caps are not optional.** Two ICs that worked perfectly for weeks started glitching when moved to a different breadboard section that lacked decoupling caps. 15 minutes and one 0.1µF cap fixed it.
