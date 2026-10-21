# PREREG u9778: cars 2 and 3 split at call 204 on the B recipe, K0 (2026-10-21)

Branch `work/u9778` from `179c2f350` (rbflip default). Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, private root
`D:\MashedWT\u9778_root`. Evidence `D:\MashedWT\u9778_data` (never committed, `MOVED_OFF_GIT.txt`). Runner `run_arms.py` (copy of
u9777's, stacks `C1` / `C2` / `C3` added: record dump of that slot + `MASHED_CARCAR_LOG` + `MASHED_CARCAR_RECDUMP`).

## Step 1 (measurement, no knob) - done before this PREREG

Port: exe `b0b90cef` (base) then `d887392f` (+ diagnostics below), K0 step dump `7ab70c5f` on both = the rbflip default.
`recpos0.py` against `o_c2_2` / `o_c2_3` (car 2) and `o_tq_2` / `o_c3` / new `o_cc3_1` (car 3): first record difference k=204 on
both cars, positions within 3.2e-2, velocities opposite in sign (car 2 dvx +0.345 dvz -2.313, car 3 dvx -0.960 dvz +4.393).

New diagnostics (default OFF, step dump unchanged): `MASHED_CARCAR_LOG` lines carry `fr=` (the record-dump frame index);
`MASHED_CARCAR_RECDUMP=<path>` writes one TSV row per `VehicleCarCarContact` call in the format of `scenario_launch.py
--carcar-probe` (frame, 0, self, other, pass, both records at entry, ret, both records at leave).

New original capture `o_cc3_1` (`--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-car 3 --statediff-aistep
--hold 25 --carcar-probe 400`): `regcmp.py` against `o_tq_2` equal k 0..399 on cars 1..3 (o_c3 regime). 192 contact-call rows:
0-2 x166, 0-3 x22, 2-3 x4. `ccdiff.py` / `ccbase.py` / `ccring.py` align them with the port's on k:

- Every (k, self, other, pass, ordinal) row exists on both sides; at k=204 both make the 2-3 call twice (pass 0 ret 1, pass 1
  ret 0), and again at k=205.
- At the k=204 pass-0 ENTRY of 0x00469df0 (self car 2, other car 3) the velocities +0x9b0..+0x9b8 of both cars are bit-equal,
  car 2's ring is bit-equal (selector +0x9a8 = 1 on both), and car 3's selector differs: original 1, port 0. Car 3's ring slot 0
  (+0x928..+0x964) is equal (x 4.332221 = car 3's k=203 record). The original's active slot 1 holds x 4.356687, between its
  k=203 pose and its k=204 pose 4.413065. The port's slot 1 holds x 4.306477 (the k=202 pose). The leave velocities then differ
  (car 2 vx 1808.6329 / 1809.1536).
- On records (frame boundary) car 3's selector is (0,1) on both sides at every k 0..203.

So car 3 has run one substep of frame 204 in the original when car 2's substep-2 car-car call reads it, and none in the port.

Mechanism, from MASHED.exe.unpatched (capstone, `cslist.py`):
- FUN_00470c70 0x00471110..0x00471141: `sub = min(rem, 0x19)`, `FUN_004709a0((float)sub, ..)` at 0x00471137, `rem -= sub`.
  No car argument.
- FUN_004709a0: car loop `[esp+0x14]` = 0..15 (0x004709e5 init, 0x00470c47..0x00470c53 `inc / cmp 0x10 / jl 0x004709f0`). Per car:
  the substep body, the retry (0x00470c25..0x00470c2b `je 0x00470ab0`), the car<->car loop over j > car (0x00470b23..0x00470c1f,
  0x00469df0 at 0x00470bcd), the selector swap (0x00470c31..0x00470c41).
- So the original runs substep 1 for cars 0..15, then substep 2 for cars 0..15. The port's StepCar runs one car's substeps
  and tail before the next car's (`VehiclePhysicsRun.cpp` substep loop inside `VehiclePhysics_StepCar`; the comment at the
  fix-L block calls the two orders identical while remI <= 0x32, which is true of the chunk loop and not of the substeps).
- The first diverging operation is therefore the proximity gate and body of 0x00469df0 for car 2 vs car 3 in frame 204's
  substep 2, which read car 3's [+0x9a8] half (0x00470b44..0x00470b70) one substep behind the original's.

## Knob (default OFF)

`MASHED_SUBSTEP_INTERLEAVE=1` (`Vehicle/SubstepInterleave.cpp`): each car's StepCar runs in a Windows fiber
(FIBER_FLAG_FLOAT_SWITCH) that yields after every substep (after the selector swap, the analogue of 0x00470c47). The
scheduler resumes the fibers in slot order once per round, so round 0 = each car's pre-part + substep 1, round 1 = substep 2,
round 2 = the tails. It runs at the player's step (`TrackRenderer.cpp`, where `VehiclePhysics_StepPlayer` was) for slot 0 plus
every AI car the A4 pre-phase (`MASHED_A4_ORDER`, default ON) stepped, with the io that pre-phase built. The opponent loop then
takes each AI car's result instead of stepping it, after a bitwise check that its io equals the pre-phase io (a mismatch writes
`[u9778] interleave: slot N io differs` to mashed_re.log and keeps the result). The per-call wheel-contact cache `g_cOnMesh /
g_cNormal / g_cAvgN / g_cGrounded` is saved and restored per slot across each yield. A worker survey of StepCar and its callees
found no other non-diagnostic state held across a yield on the default path (the pre-part ReassertContacts that would read it
is off under the seeded POS_MODEL). Diagnostic logs keyed to one car (`MASHED_D2SINK`, A6a dump, settle diag) lose rows under
the knob.

`MASHED_SUBSTEP_INTERLEAVE_ORDER=car` (diagnostic, needs the knob): same fiber path, each fiber run to completion in slot order.

## Arms

1. G-NOOP: knob unset, exe of this lane: K0 `7ab70c5f` (measured on `eee1226f` before this PREREG), K1 `15262e4a`, K26
   `d1b4bc64`, G-CTRL `15594ec8 / 7285d708 / 26a6e4ad`.
2. G-MECH: `MASHED_SUBSTEP_INTERLEAVE=1 MASHED_SUBSTEP_INTERLEAVE_ORDER=car`, K0 step dump.
3. `B_IL{C2,C3}_K0`: the knob + car 2 / car 3 record dumps + pair logs. `recpos0.py` against the originals above, `ccdiff.py`
   at k=204.
4. `B_IL_K0`, `B_IL_K1`, `B_IL_K26` (knob only): (b) with `flag_a368` per K (`vsum.py`).
5. D2 x3 alone (`D2_IL_n`), 13 tracks (`run13.py`), G-CTRL `H_IL` x1.

## Predictions

- P-MECH: G-MECH equals `7ab70c5f`. The car-order fiber path differs from the default only in where the AI cars' StepCar runs
  (at the player's step instead of in the opponent loop). If it differs, something between the two call sites reads or writes
  an AI record, and every knob result below is reported against G-MECH as well as against the default.
- P1: with the knob, car 3's selector at the k=204 pass-0 entry is 1 and its ring equals the original's; cars 2 and 3 hold
  bit-exact in record position and velocity past k=204 against the o_c3-regime captures. Report the next split.
- P2: the player and car 1 stay bit-exact through 219 on K0, the same as without the knob. A difference before 204 on any car
  means a contact at substep 2 before 204 read a car j > i (rows in the pair logs), and is reported as such.
- P3: the io-mismatch line count is 0 on every B arm, or each line is reported with its slot and frame.
- P4: D2 (MEASURE_SOLO, one car) step-identical to the default: with one car there is no car<->car call and no other car to
  interleave with. If D2 moves, that is a finding.
- P5: (b), G-CTRL and the 13 tracks change. No direction is predicted.

Pass rule (D3): no (b) car PASS -> FAIL against the default on any recipe, D2 3/3 inside the band, 13 tracks 0 RED, G-CTRL
knob-off unchanged. A knob that passes stays default OFF; flipping it is Mariano's decision.
