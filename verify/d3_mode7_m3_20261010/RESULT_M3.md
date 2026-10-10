# RESULT: mode 7 leg M3, the registry callbacks live (+ M3b, the respawn seed)

Pre-registration: `PREREG_M3.md` §0-§4 (`797a9b80`, before any port code) and §5, the M3b addendum (`29e6c6f1`, after the
M3 runs, before any M3b code). Code: `12612a4d`. Knob `MASHED_REG_CALLBACKS` (default OFF; effective only with
`MASHED_REGISTRY + MASHED_PICKUPS_ORIG + MASHED_REG_CARS`).

## 0. Verdict first

> **The pickup callback runs verbatim and collection now happens on the original's path.** `FUN_00458920` replays exact on
> all 6 original calls and on 3,888 / 3,888 emulator vectors covering every arm (PC_24 and PC_53); a corrupted copy fails
> exactly its 2 corrupted rows. Knob OFF: `ae67b13a / c8317ce2 / 31db9384` in all four control arms.
> Knob ON (M3): PickupField stood down cleanly and every relation held **except respawn timing, which FAILED as registered**
> (7,200 ticks, not 300): the exe's pool held `+0x18 = 120.0` where the original holds 5.0. The writer was the second call of
> the original's Lua glue, `FUN_00458fa0(idx, respawn)` (`0x0047b268`), which the exe never made.
> **M3b** makes it (one C3 body, moved to the shared TU): seed `0x40a00000` on all 5 entries, 10 / 10 respawns at exactly 300
> ticks, 10 collects + 12 bumps, 0 relation failures, knob OFF unchanged.

| gate | result |
|---|---|
| G-M3-REPLAY-LIVE (`o_cb1..3`, 6 calls, all collect) | **6 / 6** exact at PC_24 and PC_53 (`m3_replay_live_pc{24,53}.txt`) |
| G-M3-REPLAY-SYNTH (`synth_unicorn.cb.tsv`) | **3,888 / 3,888** at PC_24 and PC_53: collect 60, bump 60, vanish 246, no-op 3,522 |
| negative control (2 rows corrupted) | 3,886 / 3,888, the 2 corrupted rows flagged (`m3_replay_negctl.txt`) |
| G-M3-OFF | `M3_CTRL`, `M3_M2B` (trace on), `M3B_CTRL`, `M3B_M2B`: all `ae67b13a / c8317ce2 / 31db9384` |
| G-M3-STANDDOWN | `M3_ON` / `M3B_ON`: PickupField collections 0, unported callbacks 0, every grant from `FUN_0045c010` (grants = collect calls = bursts = sfx) |
| G-M3-LIVE (i) collect, (v) unported | PASS on both runs (4 and 10 collects) |
| G-M3-LIVE (ii) bump | not exercised in `M3_ON`; **PASS on 12 live bumps** in `M3B_ON` |
| G-M3-LIVE (iii) vanish (arms 2 / 4) | not exercised live (needs `fd0 == 5` or sub-state != 6 at a contact); covered by the 246 synthetic rows |
| **G-M3-LIVE (iv) respawn = 300 ticks** | **FAIL in `M3_ON`** (2 respawns at 7,200); **PASS in `M3B_ON`** (10 / 10 at 300) |
| G-M3b-SEED | PASS: `40a00000` x 5 (types 19, 7, 19, 9, 12 = `o_pk1` row 0) |
| G-M3b-PATH1 `run_diff ttl_set_68b1b0_bounds` | GREEN 6 / 6, non-degenerate (3 accepts write only their slot, 3 rejects return 0 with every sentinel intact) |
| G-M3b-PATH2 `run_verify_hook ttl_set_68b1b0_bounds` | PASS (E9 installed, reimpl fired 2 / 2) |
| G-M3-CTRL-ON (report) | `M3_ON` `8aaacd65 / a16708b9 / 8cd7507f`; `M3B_ON` `b0cc1af6 / eaf2b78e / 8cd7507f` |
| (b) per regime (report) | `M3_ON` and `M3B_ON` = control: regime A, car 1 PASS, car 2 FAIL (`|steer|` 0), car 3 PASS |
| collateral (report) | nothing diverges before step frame 1400; `c9ec` within the floor on every frame; no D2 reopen candidate (§4) |

## 1. What the original does (`o_cb1..3`, o_rg4 recipe + `--cb-probe`)

Two collections per capture, identical in all three and in o_rg4: car 2 takes pool 4 (type 12, P_MINE), then car 1 takes
pool 0 (type 19, OIL) 106 ticks later. Both cars' slots were empty; `fd0` 0, `DAT_0063ba8c` 6; the call order is
`FUN_004588c0`, `FUN_0045c010(v, type)`, `FUN_00465ca0(0x1c)`; the particle pool had 30 / 30 free slots, so each burst took
120 ring draws. Both pickups respawn 300 ticks later (timer sum 5.000010). The round ends 178 ticks after the first
collection: from then on every car position and the frame counter `0x007f0ff4` stay constant to the end of the capture.

## 2. Bytes and decisions (detail in PREREG §0.3, §1)

- **The bump arm runs when the car already holds a powerup** (`FUN_0045c330(v) != 0` = slot `+0xa8` non-zero; `test / je` at
  `0x004589ba..0x004589bd`). The kickoff had it reversed. Confirmed in the emulator and live (car 3 holding type 9).
- Verbatim naked (`re/tools/naked_specs/pickups_m3.json` -> `Gameplay/PickupCollect_naked.inc`, included in `PickupTick.cpp`):
  `FUN_00458920`, `FUN_00458570`, `FUN_0045c330`, `FUN_0046d4a0`, `FUN_00405890`, `FUN_00407580`.
- Bound: `FUN_0040e350` -> `aib_game_sub_mode()`; `FUN_0045bff0` -> `!pw_.Armed(v)`; `FUN_0045c010` ->
  `TrackRenderer::PickupGrant` -> `PowerupSystem::Activate`, run at PC_53 inside the registry's PC_24 scope.
- No-op (counted): `FUN_004588c0` (particles; the 120 ring draws per collection are not reproduced, same class as every
  render-side draw site), `FUN_00465ca0` (audio).
- Not ported: `FUN_00486630` (type 0x11; fires only after the round-end freeze, on one car, writes record `+0x144..+0x14c`,
  draws from the ring) and `FUN_0047cd50` (type 0x0A; 0 calls in the 4 captures that hook it, o_rg4 and o_cb1..3;
  PREREG §0.2 said "o_rg1..4", but o_rg1..3 predate the callback hooks). Their registrants are not in the exe.

## 3. Events, exe vs original

| run | collections (tick: car / pool / type) | respawn | other |
|---|---|---|---|
| original `o_cb1..3` | T: 2 / 4 / 12; T+106: 1 / 0 / 19 | 300 ticks | round ends at T+178 |
| exe, knob OFF (`M3_M2B`, PickupField radius 1.1007 xz) | 31: 2 / - / 12; 142: 1 / - / 19; 2971: 3 / 9; 8780: 3 / 19; 10755: 3 / 9; 11047: 3 / 19 | PickupField's own 5 s cooldown | 6 grants |
| exe, M3 (`M3_ON`) | 144: 1 / 0 / 19; 2878: 3 / 3 / 9; 8585: 3 / 0 / 19; 12898: 3 / 3 / 9 | 7,200 ticks (the 120.0 seed) | 4 grants |
| exe, M3b (`M3B_ON`) | 144: 1 / 0 / 19; 2878, 4504, 7562, 8375, 9188, 12837, 13650: 3 / 3 / 9; 5535, 11020: 3 / 0 / 19 | 300 ticks x 10 | 12 bumps (3691..3702: car 3 holding 9 over pool 3) |

As predicted (PREREG §0.5): under the knob car 2 does **not** take pool 4 at the start. The exe's car 2 passes it at 1.0983,
outside the sphere sum 1.0334 (the original's car 2 passes inside). The first exe collection is car 1 / pool 0 / type 19 at
registry frame 143 (= pickup tick 144; the tick counter runs one ahead of the registry frame), and in M3b pool 0 is back at
tick 445, both as predicted. Exe tick offsets are not comparable to the original's beyond the first pair: the original's round
ends at T+178 and the exe's race continues.

## 4. Collateral (`collateral.py --mode paired`, CTRL vs ON, floor = the M2b arm, 0 because the port is bit-deterministic)

`COLLATERAL_M3.txt` and `COLLATERAL_M3B.txt` agree: 181 of 497 fields past the floor, **none before step frame 1400**. The
first is car 2's fire byte `c7` (1 in the control, 0 under the knob): in the control, car 2 holds the P_MINE PickupField gave
it at tick 31 and fires it at 1400; under the knob it holds nothing. Car 2's trajectory follows from 1402, the other cars' gate
fields from 1517, the player's position from trace frame 1638 (contact). `c9ec`, the one D2 watch-list field in these
channels, stays within the floor on every frame. No D2 reopen candidate.

## 5. Findings

1. Training's respawn time is 5.0, set per spawn by the Lua glue `0x0047b250` through `FUN_00458fa0` after the spawn's 120.0
   default (`0x00458ef8`). Fixed (M3b). The exe's knob-OFF PickupField used its own 5 s cooldown, so this was invisible
   until the pool drove respawn.
2. The exe's car 2 misses the opening pickup by 0.065 units (1.0983 vs 1.0334): a car-2 line residual at frame 33, the car
   whose (b) item fails. Not tested further here.
3. The original's round in this recipe lasts 178 ticks past the first collection; all original pickup evidence comes from
   that window. Post-round, the type-0x11 callback acts on one car per capture.
4. The exe AI reads slot `+0xa8` directly at `0x0088fc88 + v*0xb4` (`AiControlStep.cpp:208`, `AiStandalone.cpp:1207/1327`),
   where the exe keeps it in `PowerupSystem` (reads 0). `FUN_00414490` (M4) reads the same field at `0x004144b3`. M4 input.
5. `FUN_00458fa0` already had a C3 body (`TtlSet68b1b0`, asi-only, absolute address). It now lives once, in the shared
   `Gameplay/PickupPoolSpawn.cpp`, with the pool's storage split; its old `indexed_table_set` registry entry has no handler in
   `diff_template.js`, so the new evidence is the `ttl_set_68b1b0_bounds` set.
6. The bump arm is now live in the exe (12 bumps), so the M2a tick's spring path runs in the port. U-9217 still needs an
   ORIGINAL capture spanning a bump; none of `o_cb1..3` has one (both original collections found empty slots).

## 6. Next: M4

`FUN_00414c30` + `FUN_00414490`, the mode-7 consumer (car 1: 49 mode-7 calls in the original). It reads the published count
`d8` while slots `[0, cc)` are being rewritten (the order of `FUN_00485070` vs the AI tick inside `FUN_0040fc00` decides what
it sees), and slot `+0xa8` (finding 4). Pre-register both before code.

## 7. Raw

`o_cb{1,2,3}.msd.cb.tsv` (+ provenance; `o_cb1.msd.reg.tsv`), `synth_unicorn.cb.tsv` (`re/tools/pu_cb_unicorn.py`),
`m3_replay_*.txt`, `run_m3_replay.ps1`, `decomp_m3{,b}.txt`, `COLLATERAL_M3{,B}{,_stdout}.txt`, `pivot_m3.py`,
`diff_ttl_set_68b1b0_bounds.csv`, `verify_hook_install_ttl_set_68b1b0_bounds.txt`,
`../d3_gatefire_20261008/M3{,B}_{ON,M2B}.*` (`*.cb.tsv` = exe trace, read with `re/tools/pu_cb_report.py [--check]`).
