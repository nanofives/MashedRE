# Lap / progress / split-time subsystem (WS-F F4 RE) — 2026-06-16

RE'd read-only via Ghidra MCP (slot Mashed_pool11, MASHED.exe anchor). Every RVA
below was verified in-session (`decomp_function` / `reference_to` / `function_at`).
This is the "original lap FUN" the F4 deferral pointed at; it turns out to be a
**coupled multi-function subsystem**, not a localized lap counter.

## Per-car race struct

Stride `0x30c` bytes (`= 0xc3` ints), indexed by car index 0..3 (`iVar10 = car*0x30c`,
`car*0xc3` for the int view). Two base symbols name the same struct array:
`DAT_008a9620` (int fields) and `DAT_008a96e8` (float fields, +0xC8). Fields seen:

| field (global)        | off in struct | meaning (mechanical)                                  |
|-----------------------|---------------|-------------------------------------------------------|
| `DAT_008a9620`        | +0x000        | int — last-event tick (`FUN_004a2c48` timer snapshot) |
| `DAT_008a9628`        | +0x008        | int — timer snapshot (`FUN_004a2c48`) used for splits |
| `DAT_008a9640`        | +0x020        | int — **cur_gate** (current AI-gate index)            |
| `DAT_008a9644`        | +0x024        | int — **prev_gate** (cur_gate of the previous tick)   |
| `DAT_008a9648`        | +0x028        | int — **split counter** (clamped 0..0x1e=30)          |
| `DAT_008a964c`        | +0x02c        | int[] — **split-time array** (per split sector)        |
| `DAT_008a96e8`        | +0x0C8        | float — **path_prog** (getter `FUN_00408a50`)         |
| `DAT_008a96ec`        | +0x0CC        | float — **race_pct** (getter `FUN_00408ad0`)          |
| `DAT_008a96f0/f4/f8/fc`| +0x0D0..0xDC | int — spline racing-line node idx (cur/next/...)      |
| `DAT_008a9710`        | +0x0F0        | char[] — **per-gate visited flags** (size `FUN_00426bb0()` = gate count) |
| `DAT_008a9914`        | +0x2F4        | int[6] — **lap-line-crossed flags** (accessor `FUN_00407a00(car,slot)`) |

## Lap-line gate list

`FUN_00426cf0()` → `&DAT_0066d6e4` (an int[6] of AI-gate indices). Populated by the
LAPDATA `Lap_Line` Lua handler. Lua C-function name strings: `Lap_Line`
(0x005cddb4), `Lap_Line_End` (0x005cdda4), `Lap_Line_Change` (0x005cdd94),
`Split_Sector` (0x005cdd70); referenced from the registration code at
0x00440db1 / 0x00440ded.

## FUN_00408610 — per-AI per-tick tracker (0x00408610..0x00408a40)

`void FUN_00408610(int param_1 /*car ptr*/, undefined4 param_2, int param_3 /*car idx*/)`.
Gated by `FUN_0046cbb0(car,&state,..)` (CarStatePairGet, C3): runs when `state==0`,
inner block when `state!=1`.

- **Lap-line crossing detection** (~0x004086f0-0x00408760):
  ```
  prev_gate = cur_gate;                 // save previous (DAT_008a9644 = DAT_008a9640)
  zero crossed[0..5];                   // DAT_008a9914 per car
  lapLines = FUN_00426cf0();            // int[6] = DAT_0066d6e4
  for (k = 0; k < 6; ++k)
      if (cur_gate == prev_gate + 1 && cur_gate == lapLines[k])
          crossed[k] = 1;               // crossed this tick
  ```
  i.e. a lap-line is flagged only when the car ADVANCES exactly one gate onto it.
- **path_prog / race_pct**: computed from a spline racing-line projection via
  `FUN_00407b00`, `FUN_00407b70`, `FUN_00407e20`, `FUN_00408590` into the
  `DAT_008a95c0` scratch area (two candidate sets indexed 0/1), then stored to
  `DAT_008a96e8` (path_prog) / `DAT_008a96ec` (race_pct). This is the gate-advance
  substrate — the standalone has no equivalent (it advances gates by proximity).
- **Split-time recording**: gated by the visited-gate percentage (counts set bytes
  in `DAT_008a9710` over `FUN_00426bb0()` gates); when >50% (`0x32`) or the counter
  is -1, advance the split counter `DAT_008a9648` (clamp 30) and record
  `DAT_008a964c[idx] = timer(DAT_008a9628) - sum(prior splits)`. Timer = `FUN_004a2c48`.

## FUN_00411600 — Time-Trial intermediate/split recorder (0x00411600..0x0041174d)

Runs when game mode `FUN_0042f6a0() == 2` (Time Trial). Reads the crossed flags via
`FUN_00407a00(0,0)` / `FUN_00407a00(0,1)`; the first tick each intermediate is hit
(and not yet recorded) it stores the current time `param_1` into the TT record
struct `DAT_0063bb14` at +0x17c (intermediate 0) / +0x180 (intermediate 1), logging
"Hit intermediate 0/1". These intermediates are the `Lap_Line`/`Split_Sector` gates.

## What was NOT pinned

- The **racing lap counter increment** site (the actual `lap++`) was not located this
  session; the crossed flags feed the TT intermediates (`FUN_00411600`) and the split
  array, but the per-car race-lap counter increment is elsewhere. [UNCERTAIN]
- The spline racing-line projection internals (`FUN_00407b00/b70/e20`, `FUN_00408590`).

## Port assessment (NO-GUESSING / faithfulness)

A **bit-identical C4 port is not feasible** for the standalone without first porting
the racing-line spline-projection subsystem (`FUN_00407e20` family + the `DAT_008a95c0`
scratch + the per-car-struct spline node fields). The standalone's `TrackRenderer`
advances `r.gate` by **gate-center proximity** (within 3.0 u), a different substrate
than the original's spline projection, so `cur_gate` is not produced the same way and
the diff lane cannot bit-match `FUN_00408610`.

What CAN be ported faithfully onto the existing proximity-gate ribbon (algorithm-
faithful, citing the RVAs above, NOT bit-identical/C4):
- the **lap-line crossing rule** (flag a `LAPDATA` `Lap_Line` gate only on a forward
  one-gate advance onto it), and lap completion when the declared lap-line sequence is
  satisfied (the "why two lap lines" anti-shortcut: must cross gate 93 then gate 0);
- **`Split_Sector` split-time recording** keyed off the race clock at the declared
  split gates (mirroring the `DAT_008a964c` / `FUN_00411600` intermediate logic).
