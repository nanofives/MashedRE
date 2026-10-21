# RESULT substep: StepCar as phase functions, MASHED_SUBSTEP_OUTER (FUN_004709a0 order) equals the fiber form on every arm (2026-10-21)

Branch `work/substep` from `92de86464` (fflip default). PREREG `PREREG_substep.md` (`92ac16fdc`), committed before any knob
arm. Mariano's decision (2026-10-21). Final exe `e856ca97` (`MASHED_NO_DEPLOY=1`, copy `D:\MashedWT\substep_data\exe_e856ca97.exe`),
private roots `D:\MashedWT\substep_root`, `substep_root2`, `substep_root13`. Evidence `D:\MashedWT\substep_data`
(`MOVED_OFF_GIT.txt`, run logs copied here as `RUNLOG_arms.txt` / `RUNLOG_arms3.txt`). No default changed.

## 1. Frame map

The original's frame (RVAs in PREREG section 1): FUN_004111c0 sub-state 6 calls the round tail FUN_00411170 (0x004112cd), then
FUN_0040fc00 (0x004112d5), which calls FUN_00425a40 (0x0040fc6c): FUN_00424eb0 (car 0 heading assist), then FUN_00470c70
(0x00425a8d). FUN_00470c70: frame pre-part (boost order 0x00470e1a..0x00470f16, suspension globals 0x00470f1c..0x00470f3a), per
chunk the A4 loop over 16 cars (0x00470f80..0x00471099, A4 at 0x00471076), the refresh loop (0x004710a1..0x004710fc), the
substep loop calling FUN_004709a0 once per substep (0x00471137), then two frame-end loops over 16 cars (0x00471157..0x00471314
ring frame end and +0xaf0..+0xafc, 0x0047131a..0x004713e9 timers / FUN_0046f6c0(0) / respawn calls). FUN_004709a0: DAT_007f0fdc
-= sub once per call (0x004709b5..0x004709d6), then cars 0..15 (0x004709e5, 0x00470c47..0x00470c53), each with the retry body
(A9 0x00470ad3, FUN_0046f6c0(1) 0x00470ae0, scan 0x00470ae8, fixup 0x00470afe), the car-car loop j > i (0x00469df0 at
0x00470bcd) and the selector swap (0x00470c31..0x00470c41).

Port phases (`Vehicle/VehiclePhysicsRun.cpp`), per car:

| port phase | original | port-only content |
|---|---|---|
| `StepBegin` | the car's share of 0x00470e1a..0x00470f3a (boost order at slot 0, suspension globals) | A5 race-timer copy, spawn settle (nested per-car step), record adapter IN, start-boost seed, body basis |
| `StepChunkHead` | 0x00470f50..0x00470f61 chunk, the car's gate + A4 (0x0047101b..0x00471076), refresh (0x004710a1..0x004710da) | post-A4 `ReassertContacts` (off under the seeded POS_MODEL) |
| `StepSubstep` | FUN_004709a0's per-car body (0x004709f0..0x00470c41) | none (the retry snapshot stands in for the double buffer) |
| `StepChunkTail` | 0x00471143..0x00471151 | the `MASHED_A8_A4_FIRST=0` opt-out A4 |
| `StepEnd` | ring frame end 0x00471195..0x004711af | safety clamp, `drive_delta`, adapter OUT, diagnostics |

`TrackRenderer::UpdateCar`: AI tick, then the A4 pre-phase (`MASHED_A4_ORDER`, AI cars' gate + A4 + refresh), the player's
heading assist, the player's step (:5333, or the interleave / outer driver for all cars), `UpdateRace` (:5568, before the
opponent loop unless `MASHED_RACE_TAIL_LATE`), then the opponent loop with its port-only dead / spun skips and stuck relocation.

Not ported, and outside this lane: the DAT_007f0fdc decrement (its only reader, 0x00470bec, is in the DAT_007f0fd0 == 1 branch
the port omits, fd0 = 0 on recipe F), FUN_00470c70's frame-end loops beyond the ring frame end (+0xaf0..+0xafc, the +0x18
timer, FUN_0046f6c0(0), FUN_00480b70 / FUN_00480720 / FUN_004809e0).

## 2. Restructure (3 commits)

- `b42c60dbe` step 1: `ContactScratch` struct replaces `g_cOnMesh / g_cNormal / g_cAvgN / g_cGrounded`. `SolveWheelContacts` /
  `ReassertContacts` take it. One shared instance `g_cShared` on the per-car path (same semantics as the old statics),
  `g_cSlot[16]` for the outer driver, included in `IlSnapshot` / `IlRestore`.
- `7510d5e9d` step 2: StepCar split into `StepBegin`, `ChunkDue`, `StepChunkHead`, `SubstepDue`, `StepSubstep`, `StepChunkTail`,
  `StepEnd` over a per-call `StepCtx`. Bodies moved verbatim by script (array references keep every `sizeof`), cross-phase
  statics moved to `A4First()` / `FixL()` / `SettleDiagPath()` / `SvpLogPath()` with their comments. `VehiclePhysics_StepCar` =
  `StepCarImpl(slot, dt, io, &g_cShared)`, which runs the phases per car with the fiber yield between substeps as before.
  The TU is SSE2 (not in `x87_tus.txt`), so moving locals into a struct cannot change precision.
- `838670864` step 3: `MASHED_SUBSTEP_OUTER=1` (default OFF), `VehiclePhysics_StepCarsOuter(n, slots, dt, ios)`: every car's
  `StepBegin`, then per chunk every car's head, then substep rounds over the cars in slot order (one FUN_004709a0 per round),
  then every chunk tail, then every `StepEnd`. Each car uses its own contact cache. Diagnostic controls
  `MASHED_SUBSTEP_OUTER_ORDER=car` (phases one car at a time) and `=fiber` (the fiber's yield order: each car's pre-part fused
  with its substep 1). Call site (`TrackRenderer.cpp`): the fiber's cars, snapshot and restore, the outer driver takes precedence
  over `MASHED_SUBSTEP_INTERLEAVE`.

Files: `Vehicle/VehiclePhysicsRun.cpp`, `Vehicle/VehiclePhysicsRun.h`, `D3d9Render/TrackRenderer.cpp`. `SubstepInterleave.cpp`
unchanged.

## 3. Gate table

Knob-off hash after every step (B K0 / K1, `chk_s1..s3`): `7ab70c5f / 15262e4a` on exes `aa06d2a8`, `27aae07b`, `e856ca97`.

| gate | knob off | fiber (`MASHED_SUBSTEP_INTERLEAVE`) | `MASHED_SUBSTEP_OUTER=1` | `ORDER=car` | `ORDER=fiber` |
|---|---|---|---|---|---|
| G-CTRL | `ce087436 / b0a2973b / c4f7a000` | `f78de789 / 43bbe0fb / 161b1a6a` | `f78de789 / 43bbe0fb / 161b1a6a` | `ce087436 / b0a2973b / c4f7a000` | `f78de789 / 43bbe0fb / 161b1a6a` |
| B K0 / K1 / K26 | `7ab70c5f / 15262e4a / d1b4bc64` | `9bd86ffe / 433abcd2 / 1a1ff727` | `9bd86ffe / 433abcd2 / 1a1ff727` | `7ab70c5f / 15262e4a / d1b4bc64` | `9bd86ffe / 433abcd2 / 1a1ff727` |
| (b) | A 3/3, B 2/3, C 3/3 (fflip) | 9/9 (fflip) | **A 3/3, B 3/3, C 3/3** | - | - |
| io mismatch lines | - | 0 (B x3, H) | 0 (B x3, H) | - | 0 (H) |
| D2 x3 alone | 3/3 (fflip) | - | 0.1900 x3 / 0.2550 x3 / 1965.35, 1965.67, 1965.35, H1 398/400: **3/3** | - | - |
| 13 tracks (recipe F) | 873 rows, 0 RED (fflip) | 39 frames, 877 rows, all cited, 0 RED | 39 frames, 877 rows, all cited, 0 RED, gates PASS | - | - |

The fiber column was measured twice: on the base exe `ff92167a` (before any change) and on `e856ca97` (after the refactor),
same hashes. The fiber's G-CTRL on recipe F (`f78de789 / 43bbe0fb / 161b1a6a`) had not been measured before.

## 4. Equivalence with the fiber form

Equal on every arm: G-CTRL (step / gates / ptrace), B K0 / K1 / K26, (b), and 13 tracks. On 13 tracks every standalone capture
file is byte-identical (the score tables diff empty). The `diff_*.json` files differ only in their embedded output path and the
order of one index list. So no adjudication against the original captures was needed. Every PREREG prediction P0..P8 holds.

Why the two orders agree here: they differ only in where cars 1.. run their pre-part, after car 0's substep 1 (fiber) or
before it (outer, the original's order). With the A4 pre-phase on (FAITH seed) a steady-frame AI pre-part writes only its own
+0x9d4..+0x9dc and +0x4, which FUN_00469df0's port does not read. On a car's first step (settle, A4, start-boost seed in its own
pre-part) the outer order is the faithful one (the original's A4 loop runs before any substep), and on these recipes it made
no measured difference.

## 5. Faithful vs current

1. **Port-only snapshot / restore** (fiber and outer alike). A car the opponent loop skips or relocates after the interleaved
   step is restored from the pre-step snapshot and stepped per car. That drops any car-car impulse an earlier car's substep
   dealt to it in that frame. The per-car default keeps it (record velocity keep), and the original has neither the stuck
   relocation (FUN_00470c70's respawn calls 0x004713b2 / 0x004713d1 are not ported) nor the spin skip. Dead cars never enter the
   interleave (the A4 pre-phase skips them), so only the stuck relocation reaches the restore. How often it fires on G-CTRL is
   not counted [UNCERTAIN: no log on the restore path].
2. **Car-car retry counter** (substep body, not frame order, not changed here). FUN_004709a0 counts world retries and car-car
   contacts in one register (ebp: `inc ebp` 0x00470b0a, 0x00470bde, test `cmp ebp,2` 0x00470ab0). Two contacts in one pass 0
   reach 2 and leave the substep without a retry or a swap (jge 0x00470c47). The port's `pass` advances by 1 per retry, so it
   runs pass 1. Only a three-car pile-up in one substep can show it: on the knob-off B arms (`B_DEFL_K*`, `MASHED_CARCAR_LOG`, 860
   calls, 36 ret 1) no pass-0 loop has two ret-1 contacts. G-CTRL not counted. Logged as U-9782.
3. **Round tail placement.** The original runs FUN_00411170 before FUN_0040fc00, so before every car's physics. With the outer
   driver all cars step at the player's site, but the AI positions commit in the opponent loop, after `UpdateRace`. So
   `UpdateRace` still reads AI cars one frame behind unless `MASHED_RACE_TAIL_LATE` (default OFF) moves it to the end of the frame.
4. **Heading assist order.** The original runs FUN_00424eb0 (car 0) before every car's A4. The port's A4 pre-phase runs the AI
   cars' A4 before the player's heading assist. Not measured.

## 6. Decisions for Mariano

- Flip `MASHED_SUBSTEP_OUTER` to default ON? It passes every gate the fiber passed, with the same values.
- Delete the fiber form (`Vehicle/SubstepInterleave.cpp`, `MASHED_SUBSTEP_INTERLEAVE`, `IlYield`, `g_ilScratch`)? The outer driver
  reproduces it bit for bit (`ORDER=fiber` control) and the faithful order gives the same results.
- Keep the snapshot / restore, or move the skip / relocation decisions ahead of the physics step so the restore and its
  impulse loss go away (changes relocation frames against both the fiber and the default)?
- U-9782 (retry counter) as a separate knob lane?
