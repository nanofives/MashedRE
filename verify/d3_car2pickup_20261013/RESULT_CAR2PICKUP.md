# RESULT: the car-2 opening-pickup miss (U-9682)

Pre-registration: `PREREG_CAR2PICKUP.md` (`370c5829`, before any run or code). Branch `work/car2pickup`, base `d7f5a32b`.
Model: claude-opus-5-5 (account2). (b) scored with the merged scorer (`352efab4`, regime C at 196 calls per car).

## 0. Verdict first

> **Cause found, two port defects, both fixed from the bytes.** Car 2 runs into the parked player on the same tick on both
> sides. In the original the contact `FUN_00469df0` (call `0x00470bcd`) gives **both** cars an impulse and arms
> `+0xaec = 480`. `FUN_00470c70` then **skips A4 (`FUN_00470670`) for every car whose `+0xaec` is still >= 0 after
> subtracting the 50 ms chunk** (`0x0047101b..0x00471048`). So both cars coast 9 frames on the post-contact velocity, and
> car 2 stays on the line that crosses pool 4. The port lost the impulse on the other car (the adapter IN re-seeded its
> record velocity from a stale copy) and had no reader of `+0xaec`. Neither correction alone brings car 2 into the
> pickup sphere (closest pass 1.0708 / 1.0792), both together do (1.0017): car 2 collects pool 4 (type 12) and holds it.
> **G-FIX passes on the M3B stack and fails as registered on the mode-7 stack.** There, car 2 collects 5 ticks early
> and keeps 2 of its 20 in-window mode-7 entries. Both come from 2 refdist-0 returns at frames 16-17, before the contact.
> The fix is in the default path with two opt-outs (decision 1). **STOP for the user:** (b) moves A 2/3 -> 1/3,
> B 1/3 -> 2/3.

| gate | result |
|---|---|
| **G-PICK** | **PASS.** Original `o_p2a` (regime B) and `o_p2b` (A): car 2 / pool 4 / type 12 at driving tick **42.0** in both, 3-D distance 1.023047 < 1.033389, `c010:2:12`, slot `+0xa8` 0 -> `0x5f9a58`. Exe before the fix (`M3B_ON`, = `R_` arms with both opt-outs): no pool-4 collection, closest pass 1.098253 at registry frame 33 |
| **G-COAST** | **PASS.** Exactly one record dword steps by -50 per frame from the contact: `+0xaec` 480, 430, ..., 30, -20, -70 ... (`o_p2a` car 2 frames 855.., `o_p2b` 851.., `o_p0a` player 888..). Velocity `+0x9b0..+0x9b8` bit-identical on the 10 frames with `+0xaec > 0` on all three captures. Reader: `FUN_00470c70` `0x0047101b..0x00471048` (`esi` = record + 0x9f0, `[esi+0xfc]`); the displacement sweep could not see it (`findoffset-blind-to-computed-bases`) |
| **G-CAUSE** | **PASS at the quantity, not at one code site.** First divergent car-2 quantity on the path: its velocity over the 10 frames after the contact (original 2374.48 -> 1050.41 then constant, exe 2375.12 -> 2399.86 -> 2424.26). Producers: (1) `FUN_00469df0`'s impulse on the OTHER car, overwritten in the port by the adapter IN; (2) the `+0xaec` A4 skip in `FUN_00470c70`, unported. Corrections alone: keep-only 1.070848, gate-only 1.079153 (both FAIL); both 1.001727 (inside). The 1.6 cm grid pose (§2, step 1) is not needed to pass |
| **G-FIX** | **FAIL as registered, 2 of 4 legs.** M3B stack (`R_FIX`): pool 4 / type 12 at registry frame 41 = original tick 40, **delta -2, PASS**; held word 0 -> 12 from there, PASS. Mode-7 stack (`M_FIX`): pool 4 / type 12 at registry frame 38 = original tick 37, **delta -5, FAIL**. Car-2 returns of 2 inside the original's window **20 -> 2** (registered -> 0, **FAIL**). Over the whole run 52 -> 2 (registered "fewer", PASS) |
| **G-CTRL** | **PASS.** Opt-out arm `C_OPT` (`MASHED_NO_RECORD_VEL_KEEP MASHED_NO_A4_CONTACT_GATE`) = `ae67b13a / c8317ce2 / 31db9384`. On the mode-7 stack `M_OPT` = `M4_ON` `d57dd28d / b18f69f2 / 803b179c`. New default, deterministic: `C_DEF` = `C_DEF2` = **`991a5fff / be0d47ea / bc75b405`** |
| D2 (decision-1 guard) | 0 `VehicleCarCarContact` entries in the D2 controlled arm (`a8_run_port.py ... MASHED_MEASURE_NOOPP=1`, `d2arm/`). Control: the same build logs 1394 entries in `C_DEF.carcar.log`. The only `+0xaec` writer in the port is the contact body (`CarCarContacts.cpp:204-205`), and the only cross-record velocity writer is the same body, so both changes are inert there. The slip run from that arm is **not** a measurement: 27 rows in band, wall-clocked while the (b) arms ran |
| (b) per regime (report) | fix **A 1/3, B 2/3, C 0/3**; old default (`P_K*`) A 2/3, B 1/3, C 0/3. §4 |
| collateral (report) | same side: nothing diverges before a contact frame. Cross side, regime A: 39 / 94 fields past the floor vs 42 / 94 before; 3 moved inside, 0 outside. §5 |

## 1. What the original does (bytes from `MASHED.exe.unpatched`, `re/tools/disasm_va.py`, `decomp_pc.py`)

- `FUN_004709a0` loops over the 16 vehicles **inside each substep**. Vehicle i: `FUN_0046e9e0`, `FUN_0046f6c0`,
  `FUN_00469aa0` / `FUN_0046ef70`, then `j = i+1..15` -> `FUN_00469df0` (`0x00470bcd`; `ECX` = self i, `EAX` = other j).
  The impulse written to j is in j's record before j integrates the same substep.
- `FUN_00469df0` writes the impulse to both cars' `+0x9b0..+0x9b8` and stores `0x1e0` to `+0xaec` on each car whose
  `+0xaec` is 0 (`0x0046ac6c..0x0046ac88`). Other writers of `+0xaec`: `0x0046bc7e` (0, record reset) and `0x0046c543`
  (480, another function).
- `FUN_00470c70`, per-vehicle A4 loop, `esi` = record + 0x9f0:
  `0x00470ffe` `+0x14 != 0` -> no A4; `0x0047100a` / `0x00471012` `+0x4 != 0 || +0x10 == 1`;
  `0x0047101b` load `[esi+0xfc]` (= `+0xaec`), `0x00471029` 0 -> A4;
  `0x0047102b` `sub eax,[esp+0x10]` (the chunk, `min(remaining, 0x32)`); `0x0047102f..0x0047103c` below -1200 -> 0;
  `0x00471042..0x00471048` `>= 0` -> jump past the A4 call `0x00471076`.
- `0x004710a1..0x004710da`, the loop after the A4 loop, same `+0x4` / `+0x10` gate: `+0x9e4 = |+0x9b0|`,
  `+0x9e8 = |+0x9bc|` (`FUN_004c3ac0`), every frame.
- So on a skipped frame nothing writes the velocity, and the substeps advance the position on it. That is the 10-frame
  straight line, and the reason car 2's held brake (`c1` 255) has no effect.

## 2. Divergence chain (exe vs original, with code sites)

1. **Grid pose** (open, not this leg). Exe minus original at the first driving row: car 2 dx +0.0157, dy +0.0402,
   dz -0.0083 (PREREG §0.3). The exe's hull test returns 0 with car 2 at x 1.0173 (dx to the player 0.516; the
   original's contact came at dx 0.503). The first `ret=1` comes one tick later, at (1.0024, -1.7826)
   (`C_DEF.carcar.log` line 11). The grid port exists behind `MASHED_ROUND_GRID` (pending user decision).
2. **Contact, same tick on both sides.** `i=0 j=2 pass=0 ret=1`, then a pass-1 retry (`C_DEF.carcar.log`).
3. **Impulse on the other car lost** (FIXED). `VehiclePhysics_StepCar` adapter IN (`VehiclePhysicsRun.cpp`, the block
   under "adapter IN") wrote `io.vel` / `io.speed` over the record every frame. For AI cars `io.vel` is `a.vel`
   (`TrackRenderer.cpp:3802-3803`), the previous frame's adapter OUT. Car 0 steps first (`TrackRenderer.cpp:3446`
   before `:3812`), so car 2's impulse was overwritten before car 2 stepped. This holds for every pair: the other car is
   always a later slot.
4. **No coast** (FIXED). The port had no reader of `+0xaec` (grep: only the two stores in `CarCarContacts.cpp`).
5. **Outcome after both fixes** (`C_DEF`, default recipe): car 2 2375.12 -> 1057.66 -> **1050.685 for 9 steps**
   (frames 25..33; original 1050.409 for 9). `R_FIX`: coast on registry frames 24..33, closest pass 1.001727, collects pool 4.

## 3. Code (`mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp`, exe-only TU per `exe_sources.rsp`)

- **Adapter IN keeps the record** unless the caller changed the value since the last adapter OUT (bitwise compare against
  `g_outVel` / `g_outSpeed`, set at adapter OUT). Callers that change it on purpose (respawn, spin-out, off-mesh, the
  spawn settle) still write through. Opt-out `MASHED_NO_RECORD_VEL_KEEP=1`.
- **`A4ContactGate`**: the `0x00470ffe..0x00471048` transcription. It runs for all participating slots at the frame
  boundary (slot 0's step, the place `Fi_UpdateBoostOrder` already uses), because the original's A4 loop precedes every
  car's substeps. A slot not covered there runs its own. On a skipped frame A4 (`VehicleControlIntegrate`) does not
  run, and `+0x9e4` / `+0x9e8` are refreshed (`0x004710c3..0x004710da`). `ReassertContacts` (port scaffolding) still
  runs. Opt-out `MASHED_NO_A4_CONTACT_GATE=1`.
- No new constant: 480 is the existing `kCC_BounceTimer`, -0x4b0 and the chunk come from the bytes above. No hook, no
  `.asi` body: **no run_diff / path2 leg exists** for this change (path2 keys: none).
- Stated residuals (§6): the every-frame `0x004710a1` refresh is applied on skipped frames only; the `0x00471023` store
  (`+0xc = 0`) is not ported (no port reader); the other car's A4 still runs in the contact frame, on the post-impulse
  velocity (the port steps cars one by one), giving the 1057.66 step.

## 4. Before / after tables

Pickup, car 2, pool 4 (type 12):

| arm | stack | closest pass | first frame inside 1.033389 | collect |
|---|---|---|---|---|
| original `o_p2a` / `o_p2b` | native | 1.015342 (tick 44) | tick 42 | tick 42, `c010:2:12` |
| `M3B_ON` (old) | M3B | 1.098253 (f33) | none | none |
| `R_KEEP` (keep only) | M3B | 1.070848 (f42) | none | none |
| `R_GATE` (gate only) | M3B | 1.079153 (f33) | none | none |
| `R_FIX` (both) | M3B | 1.001727 (f44) | f41 (= tick 40) | pickup tick 42, `c010:2:12` |
| `M_FIX` (both) | M3B + MODE7 | not traced | | pickup tick 39 (= tick 37) |

Mode 7, returns of 2 per car (`score_u9219.py`, `SCORE_MODE7.txt`; held word `held_timeline.py`):

| | inside the original's window (rd3 length) c1 / c2 / c3 | whole run c1 / c2 / c3 | car 2 first held != 0 |
|---|---|---|---|
| original `o_rd2` = `o_rd3` | 49 / 0 / 0 | 49 / 0 / 0 (ba8c 6) | driving call 27 |
| `M_OPT` (= `M4_ON`, old) | 25 / 20 / 0 | 25 / 52 / 43 | call 1742 |
| `M_FIX` | **49** / **2** / 0 | 89 / 2 / 77 | call 38 |

Car 2's two remaining returns: driving calls 16 and 17 (frames 850, 900), held 0, **refdist 0**. They come before the
contact, so this fix does not reach them (`ret2_rows.py`). Cars 1 and 3 add returns after the original's round has ended.

(b) per regime, merged scorer (`SCORE_B_merged.txt`; regime label per car = the run's own `flag_a368`):

| regime | old default `P_K00/01/26` | fix `B_K00/01/26` | per car (fix) |
|---|---|---|---|
| A | 2/3 (car 2 FAIL) | **1/3** | car 1 FAIL `abs_steer~` 0, car 2 PASS, car 3 FAIL `abs_steer~` 0 |
| B | 1/3 | **2/3** | car 1 PASS, car 2 FAIL `c1#` 60 / `steer#` 93, car 3 PASS |
| C (196/car) | 0/3 | 0/3 | all FAIL |

Contact coasts per AI car in the first 220 steps (`coast_events.py`: >= 8 equal `rec_9e4` steps):

| | car 1 | car 2 | car 3 |
|---|---|---|---|
| original `o_rd3`, `o_p2b` (A) | none | steps 24, 61, 207 | step 207 |
| exe `C_DEF` | steps 64, 130, 177 | step 25 | steps 64, 129, 172 |
| exe `C_OPT` | none | none | none |

The regime-A drop is cars 1 and 3: the exe's cars 1 and 3 run into **each other** at steps 64 and 129/130, and the
original's never do inside the window. Those contacts existed before the fix (`1-3` rows in `C_DEF.carcar.log`, 4
`ret=1`). They now coast as the original's contacts do. The cause is upstream: car 1 / car 3 paths (U-9185 / U-9186
pace), not this transcription. The original's car-2 coast at step 61 (speed 1341.1) has no exe counterpart.

## 5. Collateral

- Same side, `--mode paired`, A = `C_OPT`, B = `C_DEF`, floors `U_CTRL` (same hash as `C_OPT`) and `C_DEF2`, both 0:
  171 of 244 fields diverge. First divergence by car: **car 2 frame 24** (its contact), **car 3 frame 63**,
  **car 1 frame 64** (their mutual contact). No field moves before a contact frame (`COLLATERAL_SAME.txt`).
- Cross side, `--mode banded` on `s.v2.rec_9e4`, regime A both arms, first 600 driving frames, anchor
  `s.v2.substate >= 6`. A = `o_p2b`, floor `o_p0a`; B = `C_DEF`, floor `C_DEF2`: **39 / 94** paired fields past the
  floor in some band. With B = `C_OPT`, floor `U_CTRL`: **42 / 94**. Moved inside the floor with the fix: `s.v1.c4`,
  `s.v1.c5`, `s.v3.c0`. Moved outside: none (`cross_diff.py`). A first whole-run pass (regime B original `o_p2a`,
  whole run) was off-regime and medians 3000 frames apart, so it was discarded.
- Exploratory finding, not acted on: after the contact the original's car-2 AI holds **`c1` = 255 for 16 steps** and
  ramps `c0` 15, 10, 5, 0. The exe's keeps `c1` 0 and `c0` 8-10. That's the AI command side. Unported or unmatched
  reader [UNCERTAIN: which AI input reacts to the contact]. During the 9 coast frames the command has no physical
  effect (A4 is skipped). After them it does.

## 6. Proposed tracker transaction (for re-classify, not applied here)

- **U-9682** -> cause found and fixed in the default path (two port defects, §2 steps 3-4). G-FIX passes on the M3B stack,
  fails on the mode-7 stack by delta -5 ticks and 2 / 0 in-window returns, both traced to car 2's refdist-0 returns at
  frames 16-17 (pre-contact). Keep open as PARTIAL until the user's keep / revert decision. Evidence
  `verify/d3_car2pickup_20261013/RESULT_CAR2PICKUP.md`.
- **New row (semantic):** the original's AI brake response after a car contact (`c1` 255 x16, `c0` ramp), absent in the
  exe (§5). Path: hook `FUN_00416250` (the aistep writer) inputs around `o_p2a` frame 855.
- **New row (structural):** `0x004710a1..0x004710da` (`+0x9e4` / `+0x9e8` refresh after the A4 loop) runs every frame in
  the original. The port applies it on skipped frames only, and its `+0x9e4` on other frames is A6a's entry value.
  Relates to D2 §22.2's `+0x9e4 / |v|` = 1.017 at the fixup entry. D2-visible; needs a pre-registered arm.
- **New row (semantic) or fold into U-9185 / U-9186:** exe-only car 1 / car 3 mutual contacts at steps 64 / 130 (original:
  none in the window), the regime-A (b) cost of this fix.
- Car-2 refdist-0 at frames 16-17 on the mode-7 stack: append to U-9683 or the grid-pose row (§2 step 1).
- hooks.csv: no C-level change. Notes only: `00470c70` += "exe port of the `+0xaec` A4 gate `0x0047101b..0x00471048`
  and the skip-frame refresh `0x004710a1..0x004710da` in `VehiclePhysicsRun.cpp` (exe-only, no `.asi` body), behaviour
  in `verify/d3_car2pickup_20261013`". `00469df0` += "other-car impulse now reaches the other car in the exe (adapter
  IN, same note)".

Proposed CHANGELOG line:
`2026-10-13 U-9682 car-2 opening pickup: two port defects fixed in VehiclePhysicsRun.cpp (adapter IN re-seeded the other
car's record velocity, erasing the FUN_00469df0 impulse; the +0xaec A4 skip of FUN_00470c70 0x0047101b..0x00471048 was
unported). Car 2 now coasts 9 frames at 1050.685 (orig 1050.409) and collects pool 4 type 12 (M3B stack delta -2 ticks).
Default-ON, opt-outs MASHED_NO_RECORD_VEL_KEEP / MASHED_NO_A4_CONTACT_GATE (= ae67b13a/c8317ce2/31db9384). (b) A 2/3->1/3,
B 1/3->2/3, C 0/3. verify/d3_car2pickup_20261013/RESULT_CAR2PICKUP.md`

## 7. Decisions needed (user)

1. **Keep the two corrections default-ON**, or flip them to default-OFF knobs? They reproduce the original's coast and
   the car-2 pickup, are D2-inert, and move (b) A 2/3 -> 1/3 (cars 1/3, exe-only mutual contacts) and B 1/3 -> 2/3. The
   new default control trio is `991a5fff / be0d47ea / bc75b405` (`default-ctrl-hashes-after-b2-flip` would need updating).
2. **Next leg for G-FIX on the mode-7 stack:** car 2's refdist-0 returns at frames 16-17 (pre-contact), which point at the
   grid pose (`MASHED_ROUND_GRID`) or the refdist order (U-9683).
3. Open the AI-brake-after-contact row (§5) and the every-frame `0x004710a1` refresh row (§6)?

## 8. Raw

Original (force-added `.msd`): `o_p2a.msd*` (car 2, regime B), `o_p2b.msd*` (car 2, A), `o_p0a.msd*` (player, A); argv in
each provenance (`o_rd3` recipe, `--statediff-car` 2 / 2 / 0, `--statediff-aistep --refdist-probe --cb-probe --hold 60`),
PIDs 33700 / 2804 / 28932, no collision AV. Exe (inline `run_h4.ps1` env from the worktree, 14400 frames): `C_OPT`,
`C_DEF`, `C_DEF2` (step + gates committed), `R_FIX` / `R_KEEP` / `R_GATE` (`.registry.csv`, `.cb.tsv` committed; step /
gates hashes `02a7426c/61dc9293`, `dee2c0ca/41870cce`, `8d7c9008/fc51e7cc`, not committed), `M_FIX` / `M_OPT` (`.c30.tsv`,
`.cb.tsv` committed; `fd498815/342b989b/c899c830`, `d57dd28d/b18f69f2/803b179c`). (b): `B_K00/01/26.step.csv`
(`3cb5c20e`, `b47ae9cd`, `4e0f01eb`; 1200 frames, `MASHED_RING_SKIP`). Contact log `C_DEF.carcar.log` (force-added).
Tools: `traj.py`, `allcars.py`, `speed.py`, `coast_fields.py`, `timer_window.py`, `gpick_orig.py`, `gpick_exe.py`,
`ret2_rows.py`, `coast_events.py`, `regime_label.py`, `cross_diff.py`. Scores: `SCORE_MODE7.txt`, `SCORE_B.txt` (pre-merge
scorer), `SCORE_B_merged.txt`, `COLLATERAL_*`.
