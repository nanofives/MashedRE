# RESULT: D-11072 leg P2, per-frame `FUN_00408610`, byte-identical on 11,996 original calls

Pre-registration: `PREREG_P2.md` (`b6d1ea89`, before any gate). Knob `MASHED_PATHPROG`, default OFF.

## 0. Verdict first

> **G-P2-REPLAY PASS: 11,996 of 11,996 call pairs byte-identical** (the whole 0x30c-byte per-car block:
> node, progress `0x008a96e8`, race_pct `0x008a96ec`, collision normal, lap counter, lap times, visited
> flags, split flags), replaying the original's own recorded calls (`o_pp1`, Training, 4 cars, ~50 s)
> through the port at x87 precision **PC_24**. At PC_53 and PC_64 only 3,460 of 11,996 are exact.
> **G-P2-OFF PASS:** knob OFF reproduces `ae67b13a / c8317ce2 / 31db9384` (`P2_CTRL`).
> **G-P2-LIVE (report):** with the knob ON the exe's progress is live: in [0, 30], node ids follow the route
> 0..29 and wrap, lap counters advance. Frame 1 AI progress 29.7277 / 29.4599 / 29.1849 vs the
> original's first in-race values 29.7277 / 29.4549 / 29.1828.

| gate | result |
|---|---|
| G-P2-REPLAY PC_24 | **11,996 / 11,996 exact** |
| (diagnostic) PC_53 | 3,460 / 11,996 (mismatch at +0xac, +0xbc..c4, +0xc8, +0xcc, +0xe0..ec) |
| (diagnostic) PC_64 | 3,460 / 11,996 (same table) |
| G-P2-OFF | `ae67b13a / c8317ce2 / 31db9384` |
| G-P2-LIVE | live; frame-1 AI progress within 0.005 of the original |

The predicted ordering risk did not materialise: no node or normal mismatch on any pair, so the
stream-order triangle walk agrees with the original's collision-tree order on this data.

## 1. Findings beyond P2

1. **The original's RACE LOOP runs at x87 PC_24, not only its track load.** `FUN_00408610` is a
   per-frame function, and it reproduces only at 24-bit precision (100% vs 29%). The project rule
   "ST-resident values are `double`" (memory `st-resident-values-must-be-double`) gives 53-bit results;
   the D2 physics x87 TUs follow that rule and the exe runs them at PC_53. Whether that costs any D2
   bit-identity is not measured here. It is the strongest lead so far on the physics residue and is a
   user decision to pursue (it touches D2).
2. **Knob ON changes the AI windows** (step `4f675870`, gates `90e76845`, ptrace `4701b993`): the
   faithful race_pct `0x008a96ec` replaces the bridge's value for the AI's existing readers. Not scored
   yet; that belongs with P3.
3. **Two live differences, not in the step:** the player's grid position sits 0.0009 nodes past the
   line (original 0.00002 behind it), so its lap counter starts at 0 instead of -1. And the port runs
   `FUN_00407a60` only at race load, while the original's `FUN_004111c0` also runs per round
   [UNCERTAIN: per-round call not confirmed]; car 2's lap counter reaches 17 across rounds. Neither
   touches `0x008a96e8`, the value P3 needs.

## 2. Method

`FUN_00408610` and 17 callees are verbatim naked transcriptions generated from the anchored bytes
(`re/tools/gen_naked_asm.py`, spec `re/tools/naked_specs/pathprog_p2.json`), with `.rdata` constants as
exe-safe copies and the RW sqrt LUT base pointed at `Math/RwLutTables.cpp`. C++ only for the vehicle
state accessor, `_ftol2`, the RW world query (stream-order walk of the AI BSP through the verbatim
`FUN_00547450`, callback semantics of `FUN_00407a90`) and two no-op stubs. Generator fixes found on the
way: Ghidra's size for `FUN_00408610` stops 7 bytes short of its `ret` (real size 1,073), and MSVC needs
bare string instructions (`rep stosd`).

## 3. Next

Leg P3: `FUN_00414a70` + `FUN_00414300` + the `+0xbc` speed write, default-OFF, scored against the
original's window counts (car 2: 35 mode-3, 10 brake). Then (b) per regime and the collateral review.

## 4. Raw

`o_pp1.msd.pathprog.csv` (original, 12,000 calls), `p2_replay_pc{24,53,64}.txt`, `run_p2_replay.ps1`,
`../d3_gatefire_20261008/P2_{CTRL,LIVE}.*` (`P2_LIVE.pathprog.csv` = the live trace), `listing_*.txt`.
