# Round-restart grid Stage B (per-round slot order): port + knob-OFF gates (2026-10-11)

Verdict: **ported behind `MASHED_ROUND_GRID_ORDER` (default OFF; needs MASHED_ROUND_GRID + MASHED_PATHPROG).**
- The assignment matches the original bytes 1296/1296 (negative control 3/3).
- The original-bytes oracle reproduces the live original's per-round order 7/7.
- Knob-OFF and Stage-A hashes are unchanged.
- The knob has not been switched on in a race: `PREREG_STAGE_B.md` first.

## What the original does (FUN_0040e590, decomp `FUN_0040e590_decomp.txt`)

**The input.** Per-car rank codes `DAT_008a9520[i]` (FUN_0040b410). They are each round's score deltas: written
by FUN_0040b290 at 0x0040b360, reset to -1000 by FUN_0040b250 (0x0040b266) just before the placement loop, and
seeded by FUN_0040b180 (0x0040b1e5).
- The `--grid-probe` rows at FUN_00408b00 read -1000 because of that reset.
- The camera-path rows (callers 0x0044847a / 0x00448494) before each placement still carry the codes.

**Mode 4** (`DAT_008a94d0` = 4 here), rule 0, `DAT_0067ea64` = 0:
- **C:** code 2 → slot 3, 1 → 2, -1 → 1, -2 → 0, otherwise unassigned. `local_44 = 1` iff none is assigned.
- **D:** each unassigned car, in order, takes the first free slot 0..3.
- **E:** de-duplicate 0 → 1 → 2 → 3, and 3 → 0 (repeat).
- **Placement value:** `local_44 ? active index + 4 : local_2c[i]`.

**FUN_00408b00** cases 0..3 and 4..7 give the same four offsets:
- `0.48F`; `-1.12F - 1.1L`; `+1.12F - 2.2L`; `-0.48F - 3.3L`.
- So the exe's formation slot = value & 3. Checked against the probe positions at nodes 6 and 10 to 1e-5.

**The exe input.** `score_delta_[i]` (TrackRenderer.cpp:5396, `= DAT_008a9520`, set by ScoreAward = 0x0040b290).
- It starts at 0, not -1000. That is equivalent: both are unassigned, and the oracle gives 0,0,0,0 → 4,5,6,7.
- The port keeps a private copy: taken at round end, used at round start, then reset to -1000 (FUN_0040b250).

## Gates (knob OFF)

| gate | result | file |
|---|---|---|
| oracle vs live original | 7/7 placements (codes before each placement → oracle == the original's slot values) | `o_grid1.msd.grid.tsv` + `slotorder_m4.tsv` |
| Python mirror vs oracle | 1296/1296 | `assign_mirror.py` |
| G-RG-ORDER (the exe's `RoundGrid_AssignMode4` vs oracle) | 1296/1296 | `order_oracle.txt` |
| negative control (3 corrupted rows) | 1293/1296, bad = 3 | `order_negctl.txt` |
| G-CTRL (knob OFF) | `ae67b13a / c8317ce2 / 31db9384` | RG_CTRL |
| Stage A regression (ORDER off) | RG_ON `c374f936 / 3caa0b26 / d92d7b56` (unchanged) | RG_ON |

**Oracle:** `re/tools/slotorder_unicorn.py`. It runs FUN_0040e590 verbatim with 19 side-effect callees stubbed,
keeps FUN_0040b410 / FUN_0042f500 / FUN_0046c6d0 verbatim, records FUN_00408b00's 2nd argument, and is exhaustive
over codes {-1000, -2, -1, 0, 1, 2}^4.

## Limits

- Only the mode-4, rule-0, `DAT_0067ea64 == 0` path is ported. That is what D3 Training runs (probe: d94d0 = 4,
  fd0 = 0, ea64 = 0).
- Modes 1..3, the rule overrides (fd0 1/2/4/8/9), and the sorted-finish path (FUN_0042f500 == 1) are not.
- FUN_00408b00 itself stays the exe's formation, which is equivalent in position (Stage A gate).
