# PREREG: D-11072 leg P2, per-frame `FUN_00408610` (path progress 0x008a96e8)

Date 2026-10-10. Committed before any P2 gate is run. Parent: `PREREG_PATHPROG.md` §2 (leg P2),
`RESULT_P1.md` (course block byte-identical behind `MASHED_PATHPROG`).

## 1. What is ported and how

- `FUN_00408610` and 17 callees as VERBATIM naked transcriptions generated from the anchored bytes
  (`re/tools/gen_naked_asm.py`, spec `re/tools/naked_specs/pathprog_p2.json`):
  `0x00408610, 0x00407e20, 0x00408590, 0x00407b00, 0x00407b70, 0x00407be0, 0x00407cd0, 0x00407d70,
  0x00407db0, 0x00426cb0, 0x00426cc0, 0x00426d00, 0x00426bb0, 0x00426090, 0x00426cf0, 0x004c39b0,
  0x004c3b90, 0x00547450`. `.rdata` constants are exe-safe copies; the RW sqrt LUT base points at
  `Math/RwLutTables.cpp`.
- C++ only for: `FUN_0046cbb0` (exe-rebound `VehicleCarStateRead`), `_ftol2`, the RW world query
  `FUN_00538c80`/`FUN_00539ec0` + callback `FUN_00407a90` (a stream-order walk of the AI BSP triangles
  through the verbatim `FUN_00547450`), and no-op stubs for `wprintf` / the RW error reporter.
- Race reset runs `FUN_00407a60`; the per-frame loop mirrors `FUN_0040da50`'s `FUN_00408610` call,
  placed before the AI tick. With `MASHED_PATHPROG` the race_pct bridge stands down (the faithful
  producer writes `0x008a96ec`). All behind `MASHED_PATHPROG`, default OFF.

## 2. Gates

- **G-P2-REPLAY (acceptance).** `o_pp1.msd.pathprog.csv` (original, Training, 12,000 calls, entry
  hook): for every call k and the next call k' of the same car, load k's block, feed k's recorded inputs
  (rec matrix, state words, clock), run the port step, compare all 0x30c bytes with k''s entry block.
  **PASS = every pair byte-identical** at one x87 precision setting. Run order: PC_24 first (the
  measured track-load setting), then PC_53 and PC_64 as diagnostics. The setting that passes becomes the
  live default; if none passes, RED with the per-offset mismatch table.
- **G-P2-OFF.** Knob OFF: `run_h4.ps1` control reproduces `ae67b13a / c8317ce2 / 31db9384`.
- **G-P2-LIVE (report, trajectory-dependent).** Knob ON in the exe: `0x008a96e8` stays in [0, 30) on
  Training, node ids advance along the route `0..29`, the lap counter goes -1 -> 0 at the line.
  Prediction for frame 0: progress 30.00 / 29.73 / 29.45 / 29.18 within 0.01 if the port's grid
  positions equal the original's (not gated, the port's grid is not proven identical).

## 3. Predictions

- The only non-verbatim part is the triangle visiting order inside a sector. Gates are ~4 units apart
  and the sphere radius is 0.6, so two different nodes should never be hit in one call: predicted no
  REPLAY mismatch from ordering. A mismatch at `+0x20` (node) or `+0xbc..+0xc4` (normal) with
  everything else equal would refute that.
- The verbatim code makes precision the only other free variable; predicted PASS at PC_24.
