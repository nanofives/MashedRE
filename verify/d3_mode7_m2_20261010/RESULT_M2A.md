# RESULT: mode 7 leg M2a, the pickup tick `FUN_00459000` (logic half) verbatim on the original-layout pool

Pre-registration: `PREREG_M2.md` (`ea7bb084`, before any M2 code). Knob `MASHED_PICKUPS_ORIG` (default OFF, needs
`MASHED_REGISTRY`).

## 0. Verdict first

> **Every M2a gate PASSES.** Replaying 1,890 original calls (`o_pk1`, 240 s, Training) through the port gives the
> pool, every frame translation and every registered descriptor **byte-identical on 1,890 / 1,890**, at PC_24 and
> PC_53, with 0 re-roll calls. In the exe with the knob ON, the registry holds the 5 Training pickups every frame
> (13,858 / 13,858), type 7, r 0.5, callback `0x00458920`, at positions **bit-identical to the original's**, in the
> original's slot order. **Knob OFF and knob ON: `ae67b13a / c8317ce2 / 31db9384`.**

| gate | result |
|---|---|
| G-M2a-CAPTURE | `o_pk1.msd.pool.tsv`: 1,890 calls (first 1,500 + every 30th of 13,221); 5 pickups, types 0x13 x2, 7, 9, 0x0c |
| G-M2a-REPLAY PC_24 | **1,890 / 1,890 exact**: pool 0x7d0 bytes, frame translations, `cc`, registered slots |
| (diagnostic) PC_53 | 1,890 / 1,890 exact |
| G-M2a-LIVE | 5 type-7 entries on 13,858 / 13,858 frames; positions bit-identical to `o_pk1`'s `+0x38` (5 / 5), same `&entry`, same order |
| G-M2a-OFF / INERT | `M2A_CTRL` and `M2A_ON`: `ae67b13a / c8317ce2 / 31db9384` |

## 1. Coverage of the replayed arms

- state 1 -> 1 (live, registered): 8,848 ticks; state 2 -> 2 (respawn countdown): 600; **state 2 -> 1 (respawn): 2**,
  one of them type 0x13 through the `FUN_004576b0` gate (bitmaps captured; `0x0068a254` non-zero on every row).
- The velocity re-seed branch (`|0.95 v|^2 < 1e-5`) runs on every live tick, but every pickup sits on its anchor,
  so `FUN_004584e0` takes its `|d|^2 <= 1e-6` early-out: **the spring arithmetic (normalize, length) is
  unexercised.** It needs a bump (`FUN_00458920`'s "car cannot take it" arm writes `+0x44..+0x4c = 0, 0.04, 0`),
  which did not occur in 240 s. Recorded (U-9217).
- `FUN_00458d00` (random re-roll) 0 calls, as predicted (`+0x28 = 0` on every entry).

## 2. Findings

1. The registered position is the pickup's RW frame translation, set each tick by `FUN_004c1340(frame, +0x38, 0)`;
   modelled with 25 fake objects/frames bound to `+0x00`. The rotation (`FUN_004c1520`, pre-concat) is not
   modelled: it cannot move the translation and nothing in the exe reads it.
2. `+0x2c` is the anchor and `+0x38` the live position (`FUN_004584e0` steers `+0x38` toward `+0x2c`).
   `PickupPoolSpawn.cpp` names them the other way round, and its "0x0068b198 would clobber our own data" note
   is stale: the address lies in `g_b17_low_arena_pad` (`exe_main.cpp:247`). Comment-only, not changed here.
3. `PickupField` copies positions out of the pool once at `InitReal` (`PickupField.cpp:194-218`) and never
   reads it again, so moving the pool to the original layout cannot change the exe's pickups (measured: INERT).

## 3. Next: M2b (cars), then M3

M2b: the car registrant `FUN_0041f710` (`PREREG_M2.md` §4). Measure first: capture the original's car entries
(type `0xff00 + v`) together with each car record's `+0x958..+0x960` on the same frame (one more read in
`--reg-probe`), to decide whether the LTM translation `FUN_004c0ed0(car frame)+0x30` equals the record position
the exe has; and find the writer of the radius `+0x264` (0.533 on every frame). Then M3 (callbacks live).

## 4. Raw

`o_pk1.msd.pool.tsv` (+ log, provenance), `m2a_replay_pk1_pc{24,53}.txt`, `run_m2a_replay.ps1`, `decomp_m2*.txt`,
`../d3_gatefire_20261008/M2A_{CTRL,ON}.*` (`M2A_ON.registry.csv` = the exe's registry per frame).
