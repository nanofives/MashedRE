# PREREG: mode 7 leg M2 (registrants). M2a pickups now; M2b cars registered for the next leg

Date 2026-10-10. Committed before any M2 code. Scope: `../d3_mode7_20261010/PREREG_MODE7.md` §2.
Evidence: `decomp_m2.txt`, `decomp_m2b.txt` (this folder), `../d3_mode7_20261010/decomp_registry.txt`.

## 1. Facts the design rests on (cited)

- Pool: 25 entries x 0x50 at `0x0068b198`, count `0x0068b9a8` (`PickupPoolSpawn.cpp`, `FUN_00458e00`). From
  `FUN_00459000` (`pfVar4 = 0x0068b1b4 + n*0x50`, walking down): `+0x00` RW object, `+0x04` angle, `+0x08` angle
  rate, `+0x0c` axis, `+0x18` respawn time (120.0 at spawn), `+0x1c` timer, `+0x20` state (1 live, 2 respawning),
  `+0x24` type, `+0x28` re-roll flag, `+0x2c` anchor, `+0x38` live position, `+0x44` velocity.
- `FUN_00458a40` (EDI = entry): `+0x38 += +0x44`; `+0x44 *= _DAT_005cc9dc`; `FUN_004c1340(frame, +0x38, 0)`
  (translate, replace); angle `+0x04 += +0x08`; `FUN_004c1520(frame, +0x0c, angle, 1)` (rotate, pre-concat: does not
  move the translation); when `|+0x44|^2 < _DAT_005cc990`, `+0x44 = FUN_004584e0() * _DAT_005cc9a0` (unit vector
  from the frame translation toward the anchor `+0x2c`).
- Registration (state 1): `{*(obj+4)+0x40..+0x48, 0.5 (0x3f000000), 7, &entry, 0, FUN_00458920}` via
  `FUN_00484cf0`. So the registered position is the frame translation that `FUN_004c1340` just set = `+0x38`.
- `FUN_00458e00` never writes `+0x00`: the objects pre-exist per slot. In the original it calls `FUN_00458dd0`
  (re-skin, render) and `FUN_004c15c0(frame)` (frame reset) at spawn.
- Frame order in `FUN_0040fc00`: `0x0040fc75` `FUN_0040da50` (cars register via `FUN_00420870` -> `FUN_0041f710`),
  `0x0040fcd0` `FUN_0045bba0` (pickup tick `FUN_00459000`), `0x0040fd07` `FUN_00485070` (publish).
- `_DAT_007f100c` (timer decrement) = `units * _DAT_005cc948` (`0x0040fc57`); written in the exe only under
  `MASHED_SUBSTATE`. `FUN_004576b0` reads `0x0068a300[0..1]` and `0x0068a254 + 0x10*i` (i = 2..3): no exe writer.
- `0x0068b198` and the registry lie inside `g_b17_low_arena_pad` (`exe_main.cpp:247`, 10 MB zero array covering
  the original arena): safe to use at the original addresses. `PickupPoolSpawn.cpp`'s "would clobber our own
  data" note is stale (that file's `+0x2c live` / `+0x38 anchor` names are also swapped vs the tick; same value at spawn).

## 2. M2a port shape

- Knob `MASHED_PICKUPS_ORIG` (default OFF; requires `MASHED_REGISTRY`): `PickupPool_Base()` / `CountPtr()` return
  the original addresses; 25 fake RW objects bound to `+0x00` once (`obj+4` -> a 0x50-byte fake frame).
- Verbatim naked (`re/tools/gen_naked_asm.py`, spec `pickups_m2a.json`): `FUN_00459000`, `FUN_00458a40`,
  `FUN_004584e0`, `FUN_004576b0`, plus `FUN_004c3ac0` / `FUN_004c39b0` copies (LUT patched). Shims:
  `FUN_004c1340` writes the fake frame's modelling matrix as identity + translation (RW replace semantics);
  `FUN_004c1520` leaves the translation (pre-concat) and does not model the rotation [no reader in the exe];
  `FUN_00458dd0`, `FUN_004769a0/d0/f0`, `FUN_00476a30/d00/df0` no-op (render); `FUN_00458d00` traps (counts calls:
  reachable only with `+0x28 != 0`, which the exe's rank-0/1 spawn never sets); `FUN_00484cf0` -> M1's registrar.
  `[0x007f100c]` -> an exe-safe copy set per frame as `(float)DAT_007f1008 * _DAT_005cc948`.
- Spawn: under the knob, after `FUN_00458e00`'s writes, reset the fake frame like `FUN_004c15c0` (identity, so
  the translation is 0 until the first tick).
- Call site: once per frame before `Registry_FrameTick`, as `0x0040fcd0` precedes `0x0040fd07`.

## 3. M2a gates

- **G-M2a-CAPTURE.** `scenario_launch.py --pool-probe N`, original, Training: entry + leave of `FUN_00459000` for the
  first N calls (N = 1500) plus every 30th after: count + 25 x 0x50 pool, each live entry's frame +0x10..+0x4c,
  `_DAT_007f100c`, the two bitmaps, `cc` at entry, and at leave the pool, frames and slots `[cc_in, cc_out)`.
- **G-M2a-REPLAY (transcription).** Per captured call, inputs -> port `FUN_00459000` -> PASS = pool bytes, every
  frame's translation `+0x40..+0x48`, and the registered descriptors identical to the leave dump, 100% of calls,
  PC_24. Fields not modelled (frame rotation rows) are excluded and listed.
- **G-M2a-LIVE (report).** Exe, knob ON: per-frame type-7 registry entries = live pool entries, r 0.5, callback
  `0x00458920`, position = pool `+0x38`. Count distribution vs `o_rg4`'s (4.8 per sampled frame) reported.
- **G-M2a-OFF / G-M2a-INERT.** Knob OFF, and knob ON (`MASHED_REGISTRY` + `MASHED_PICKUPS_ORIG`, callbacks still
  record-only, no AI reader): both `ae67b13a / c8317ce2 / 31db9384`. Prediction: PASS. The only shared state the
  knob moves is the pool's address, and `PickupField` reads it through the same accessors.
- Predictions: REPLAY PASS at PC_24. [UNCERTAIN] whether PC_53 also passes (the tick multiplies and normalizes).

## 4. M2b (registered, not run in this leg)

Car registrant `FUN_0041f710` (EAX = `0x0063d9e0 + v*0x2ac`): `{+0x258, +0x25c, +0x260, +0x264, +0x290 + 0xff00, 0, 0, 0}`.
`+0x258..+0x260` = `0x0063dc38..` written in `FUN_00420870` from `FUN_004c0ed0(car frame)+0x30..+0x38` (LTM translation);
radius `+0x264` writer unknown (0.533 on every o_rg4 frame). Measure first: the original's car entries vs record
`+0x958` (memory `msd-world-position-is-the-0x928-matrix-row`) on matching frames; then pre-register the bridge.
