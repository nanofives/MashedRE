# RESULT u9778: cars 2 / 3 split at 204 because the port steps one car's substeps at a time; FUN_004709a0 steps every car per substep (2026-10-21)

Branch `work/u9778` from `179c2f350` (rbflip default). PREREG `PREREG_u9778.md` (`fd711449a`), committed before any knob arm.
Final exe `d5e98070` (`MASHED_NO_DEPLOY=1`, copy `D:\MashedWT\u9778_data\exe_d5e98070.exe`), private roots `D:\MashedWT\u9778_root` /
`u9778_root13`. Evidence `D:\MashedWT\u9778_data` (`MOVED_OFF_GIT.txt`). No default changed.

## 1. First diverging operation: VehicleCarCarContact 0x00469df0, cars 2 / 3, frame 204, substep 2

New original capture `o_cc3_1` (o_c3 regime: `regcmp.py` equal k 0..399 against `o_tq_2` on cars 1..3) with `--carcar-probe 400`.
New port diagnostic `MASHED_CARCAR_RECDUMP` (the same TSV, both records at entry and at leave). `ccdiff.py` / `ccbase.py` /
`ccring.py` align the two on k. Every contact-call row exists on both sides (0-2 x166, 0-3 x22, 2-3 x4).

At the k=204 pass-0 entry (self car 2, other car 3) both cars' velocities are bit-equal and car 2's ring is bit-equal. **Car 3's
selector +0x9a8 is 1 in the original and 0 in the port.** Its ring slot 0 is equal (x 4.332221, the k=203 record). The original's
active slot 1 holds x 4.356687, between the k=203 and k=204 poses (4.413065), and the port's slot 1 holds x 4.306477 (k=202). So in
the original car 3 has run substep 1 of frame 204 when car 2's substep-2 contact reads it, and in the port it has run none. The
proximity test (0x00470b44..0x00470b70) and the body read car 3's pose one substep behind, and the leave velocities differ (car 2
vx 1808.6329 / 1809.1536). On record dumps car 3's selector is (0,1) on both sides at every k 0..203, so this exists only mid-frame.

From MASHED.exe.unpatched (capstone, `cslist.py`):
- FUN_00470c70 0x00471110..0x00471141: `sub = min(rem, 0x19)` (0x00471110 / 0x00471117), `FUN_004709a0((float)sub, ..)` at
  0x00471137, `rem -= sub` (0x0047113f). No car argument.
- FUN_004709a0: the car loop `[esp+0x14]` 0..15 (0x004709e5, 0x00470c47..0x00470c53 `inc / cmp eax,0x10 / jl 0x004709f0`). Per
  car: the substep body, the car<->car loop over j > car (0x00470b23..0x00470c1f, 0x00469df0 at 0x00470bcd), the retry
  (0x00470c25..0x00470c2b), then the selector swap (0x00470c31..0x00470c41).

So the original runs substep 1 for cars 0..15, then substep 2 for cars 0..15. The port's `VehiclePhysics_StepCar` runs one car's
substeps and tail before the next car's. The fix-L comment in `VehiclePhysicsRun.cpp` calls the two orders identical while remI <=
0x32. That holds for the chunk loop, not for the substeps. The difference shows only when a car<->car call in substep 2 reads a car
j > i. Car 3's A5 rubber-band gate is not involved (its refdist passes 7.0 at row 665).

## 2. Knob `MASHED_SUBSTEP_INTERLEAVE=1` (default OFF)

`Vehicle/SubstepInterleave.cpp`: each car's StepCar runs in a fiber (FIBER_FLAG_FLOAT_SWITCH) that yields after every substep
(after the selector swap, the analogue of 0x00470c47). The fibers are resumed in slot order once per round: pre-part + substep 1,
then substep 2, then the tails. It runs at the player's step for slot 0 plus the AI cars the A4 pre-phase (`A4_ORDER`, seeded)
stepped. The opponent loop takes each AI car's result. The per-call wheel cache `g_cOnMesh / g_cNormal / g_cAvgN / g_cGrounded` is
saved per slot across a yield. A car the opponent loop then skips (dead, spun), relocates (the port-only stuck stand-in), or feeds
a different io is restored from a pre-run snapshot and stepped the per-car way. A worker survey of StepCar and its callees found no
other non-diagnostic state held across a yield on the default path. The pre-part `ReassertContacts` that would read the cache is
off under the seeded `POS_MODEL`. Diagnostic logs keyed to one car (`MASHED_D2SINK`, A6a dump, settle diag) lose rows under the
knob.

**G-MECH** (`MASHED_SUBSTEP_INTERLEAVE_ORDER=car`, the fiber path in the per-car order): K0 `7ab70c5f` = the default. The first
build failed it (`69d979f9`, split at step frame 577). The cause was the stuck relocation of car 3 at frame 576 after the interleave
had stepped it. That is fixed by the restore above. Every number below is from the fixed build `d5e98070`. That build logs no io
mismatch on any arm.

## 3. Results on K0 with the knob

| car | default: bit-exact through | knob: bit-exact through | next difference |
|---|---|---|---|
| player (`o_a5_1`, `o_bq0_2`) | 219 | 219 | 220 |
| car 1 (`o_lt1_2`) | 219 | 219 | 220 |
| car 2 (`o_c2_2`, `o_c2_3`) | 203 | **217** | 218: vx +1.221e-04 = 1 ulp at 1923.34, position equal |
| car 3 (`o_cc3_1`, `o_tq_2`, `o_c3`) | 203 | **219** | 220 |

At k=204 the 2-3 contact is bit-equal at entry (car 3 selector 1 / 1) and at leave on both cars.

k=220 is not physics. In the original it is the round end: aistep `substate` goes 6 to 7, cars 1 and 2 stop producing rows, car 3's
command goes to c4 0 / c5 0, and the AI clock stops (`clk_0ff4` 56050 at 220 and 221). Then every car slows (car 3 |v| x0.8345 per
frame). The port stays in substate 6. There are no car<->car calls in k 206..221 on either side.

## 4. Gates

| gate | knob unset (exe `d5e98070`) | `MASHED_SUBSTEP_INTERLEAVE=1` |
|---|---|---|
| G-CTRL | `15594ec8 / 7285d708 / 26a6e4ad` (= rbflip default) | `af98e82c / 32e75acc / bdb3e07b` |
| B dumps K0 / K1 / K26 | `7ab70c5f / 15262e4a / d1b4bc64` (= default) | `9bd86ffe / 433abcd2 / 1a1ff727` |
| `flag_a368` per K | 0 / 0,1 / 0,2 | 0 / 0,1 / 0,2 (unchanged) |
| (b) | A 3/3, B 2/3 (car 2 FAIL), C 3/3 | **A 3/3, B 3/3, C 3/3** |
| D2 x3 alone | (rbflip: 0.1900 / 0.2550 / 1965.35..1965.67) | 0.1900 x3 / 0.2550 x3 / 1965.35, 1965.35, 1964.25, H1 398/400: PASS 3/3 |
| 13 tracks | (rbflip: 873 rows, 0 RED) | 39 frames, 877 rows, all cited, **0 RED**, instrument gates PASS |
| io mismatch lines | - | 0 on every B arm and on `H_IL` |

PREREG predictions: P1 holds (selector and ring equal at k=204, cars 2 / 3 past 204). P2 holds (player and car 1 through 219 with
and without the knob). P3 holds (0 mismatch lines). P4 holds: D2 has one car, so the interleave falls back to the plain step, and
the values equal the default's. P5: (b) B car 2 goes FAIL to PASS and no car goes PASS to FAIL. G-CTRL changes. 13-track rows
change on 8 frames (net +4 cited rows): 04_Highway 853, 06_SuperG 743 / 783, 08_Storm 968, 11_Warzone 584 and 13_Training 884 gain
one row each. 05_Sands 1084 and 13_Training 924 lose one each. P-MECH held only after the restore fix (section 2).

By the D3 rule the knob passes every gate measured here: (b) 9/9 against 8/9, no PASS to FAIL, D2 3/3, 0 RED, G-CTRL knob-off
unchanged. It stays default OFF. Flipping it is Mariano's decision.

## 5. Stub audit (the u9777 shape)

The worker audit of `ForceIntegratorStubs.cpp`, `ContactStubs.cpp` and the other stub files is grep-only, so writers through
pointers are not excluded. It found no new default-path case where a zero stand-in shadows a live address the port writes.
- `g_rubberBand` / `g_rubberRefCar` are the only such pair, fixed by `A5_RUBBER_LIVE`.
- Latent: `Fi_GameMode()` (DAT_0063ba8c) returns 6 while the live word is written only under the default-OFF `MASHED_SUBSTATE`.
- No writer at all: `Collision::g_playerCount` (DAT_00803320), read by the car-car impact scaling (`CarCarContacts.cpp:235`,
  `:447`), so the port always takes the `< 3` branch. The k=204 contact is bit-exact with it, so it takes the same branch as the
  original on this recipe.
- The inverse shape: DAT_007f101c is read live by `CameraEntries.cpp:433` / `:514`, but its only exe writer zeroes it
  (`FrameDispatch.cpp:181`).

## 6. Next

- U-9779: the round end at k=220 on K0 (original substate 6 -> 7, AI clock stops). The port has no such transition there. Since
  the 2-3 contact now matches, this is the next split for the player and cars 1 / 3.
- U-9780: car 2's 1-ulp vx at k=218 with the knob (position equal, no car<->car call at 206..221).

Scripts: `ccdiff.py` / `ccbase.py` / `ccring.py` (car-car rows, original `--carcar-probe` vs `MASHED_CARCAR_RECDUMP`), `velcmp.py`
(velocity, angular velocity, +0x9e4 per k), `cslist.py` (capstone listing), `vsum.py` (copy of rbflip's), `run_arms.py` (stacks `C0..C3`,
`IL`, `ILM`, `ILC0..ILC3`).
