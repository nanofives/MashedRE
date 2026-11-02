# RESULT D4-DC-77b0-ST5-C4ACCEL (2026-11-02)

**Verdict: HANDOFF.** The static cause is found and fixed behind `MASHED_ST5_LEG2` (default stays OFF). No arm ran: the
display was asleep for the whole session (`py -3.12 scripts/diag.py probe-render` at 11:12 and 11:36, both
`RENDER: BLOCKED — D3DERR hr=0x8876086A (D3D9-NOTAVAIL)`). That is not a finding and moves no gate. The arms, their
commands and the scorer are below. No flip decision is filed yet.

Branch `orch/D4-DC-77b0-ST5-C4ACCEL` from `78be531e1` (orch/integration, contains 6a997bfa3 and ST5-HOLD 7bed2f65b).
Commits: PREREG `b28e1b609` (alone, before the code and before any arm), code `bbe67dc6e`, this RESULT.
Built exe (worktree, `MASHED_NO_DEPLOY=1`): `mashedmod/build/mashed_re.exe` sha256 `c89e597ac1d5c761...`.

## Why the exe AI writes c4 from sub-state-5 call 1 (static, MASHED.exe.unpatched)

FUN_00418560 (the per-car AI step FUN_00418860 calls):

| address | instruction | meaning |
|---|---|---|
| 0x0041857d..0x0041858b | `mov byte [edi+4/5/0/1/7/6], bl` (bl = 0) | ctrl block 0x007f1038 + slot*0x4c zeroed |
| 0x0041858e / 0x00418593 | `call 0x0040e350` / `cmp eax,5` | 0x0040e350 = `mov eax,[0x0063ba8c]; ret` |
| 0x00418596 | `jne 0x004185e5` | mode != 5: the control step (0x004185e7 `call 0x00417180` onward) |
| 0x00418598 / 0x0041859e | `mov [0x007f0ff8],ebx` / `call 0x00413fe0` | mode 5 |
| 0x004185a3 | `call 0x0040e4a0` | = `mov eax,[0x005f29b8]; ret` |
| 0x004185ab | `mov ecx,[ebp*0x74 + 0x0089a4f8]` | per-car cd |
| 0x004185b5..0x004185bd | `add ecx,0x40` / `imul ecx,ecx,0x64` / `cmp eax,ecx` / `jge 0x00418841` | cd >= 0 |
| 0x004185c3 | `mov byte [edi+4],0xff` then ret | c4 = 0xff only below the threshold |
| 0x004185cb..0x004185dd | `(0x4a - cd) * 0x64`, same compare, c4 = 0xff | cd < 0 |
| 0x00418841 | `pop edi / pop ebp / pop ebx / ret` | no control step, no replay-slot store |

So while DAT_0063ba8c == 5 the original never runs the control step: c4 stays 0 until 0x005f29b8 drops under
(cd + 0x40) * 100. This covers both AI ticks of a sub-state-5 call (leg 1 at 0x0041040e, leg 2 at 0x00425a73).

The exe's `Ai::VehicleStep` (Ai/AiStandalone.cpp) carries this branch only behind `MASHED_MODE5_RESET`, default OFF and
set by no arm. On every exe state-5 tick it ran `AiBankSwitch` + `ControlStep` (FUN_00416250), whose accel stores are
0xff (0x004167dc) and 0x40 (0x00416896). That is the 0xff x 112 (cars 1, 2) and 0xff then 0x40 x 111 (car 3) of the
ST5-HOLD diag. The branch's 2026-10-08 DEVIATION note ("0x005f29b8 is never written, elapsed reads 0") is stale: the
sub-state port writes 0x005f29b8 (phase-3 decrement, 0x00410371 store of 12000, 0x004103e3 decrement, 0x004104c0 zero).

## Change (knob ON only)

- `VehicleStep` takes the mode-5 branch when `MASHED_MODE5_RESET` is set or `MASHED_ST5_LEG2` is on. Knob unset: no
  code path changes. The DEVIATION note is replaced by the listing citations above.
- `MASHED_ST5_HOLD_DIAG` (log only): each `[st5hold]` line now also prints `cd=`, `c29b8=` and `bf4=` (record +0xbf4).

## Prediction on record (PREREG, before any arm)

Leg 1 feeds c4 to FUN_0046d7f0 after its own AI tick: +50 per read with c4 > 160, else -50, clamp 0..3000
(0x0046d845..0x0046d874). With 50 units per call and 0x005f29b8 = S - 50k on call k, if S = 12000 the knob gives
2 * cd reads of 0xff: with the exe's FUN_00414180 draw cd = 3,1,4,2 (v = 0..3), car 1 charge 100 and car 3 charge 200.
That would leave W-GO short of the original's 1100 / 1 at frame 0.

[UNCERTAIN] The original's cd. In all three original captures (ST5-HOLD w_c1/w_c2/w_c3) the three AI cars' c4 turn
0xff on the same frame (819, 813, 887), which needs one threshold for all three cars, while the exe draws distinct
cd. Missing evidence: the original's dwords 0x0089a4f8 + v*0x74 (v = 1..3) and 0x005f29b8 at state-5 entry in the
same recipe (`scenario_launch.py --track 0 --mode 10 --cars 4 --rule 0 --poke-ctrl-slots`).

## Handoff: what remains, exact commands

Display must render first: `py -3.12 scripts/diag.py probe-render` from the main checkout must not print `BLOCKED`.
The worktree's `original\` holds no game data, so arms use the private root below (already copied, 484 MB).

1. Arms, one lock hold (about 13 boots):
   ```
   py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label D4-DC-77b0-ST5-C4ACCEL -- bash D:/MashedWT/D4-DC-77b0-ST5-C4ACCEL_data/arms.sh
   ```
   `arms.sh` runs `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_ST5L2 B_ST5L2_K0
   B_ST5L2_K1 B_ST5L2_K26 B_ST5L2D_K0 B_RD1_K0 B_ST5L2RD1_K0 B_RD3_K0 B_ST5L2RD3_K0` with
   `A4_ROOT=D:/MashedWT/D4-DC-77b0-ST5-C4ACCEL_root`, `A4_OUT=D:/MashedWT/D4-DC-77b0-ST5-C4ACCEL_data/arms`, from this
   worktree (exe c89e597a). Stacks ST5L2 / ST5L2D / RD1 / RD3 / ST5L2RD1 / ST5L2RD3 already exist in run_arms.py.
2. Score:
   - G0: H_DEF step / gates / ptrace sha8 = 8284be46 / ba407a16 / 31fe043e, B_DEF_K0/K1/K26 = 1d5bf4f9 x3
     (re-read merge_train.json refs first; L-88's PORTSTATE flip may have moved the gates ref by then).
   - W-C4 and W-HOLD: `py -3.12 verify/d4dc77b0st5c4accel_20261102/score_c4.py D:/MashedWT/D4-DC-77b0-ST5-C4ACCEL_data/arms/B_ST5L2D_K0.log`
     (exit 0 = both hold; it also prints per-car cd, T, c29b8 range, 0xff count, bf4 at the exit call). Checked on
     ST5-HOLD's old log with synthetic columns: 333 W-C4 mismatches, 0 W-HOLD mismatches, as expected for the old exe.
   - W-GO: frame-0 +0xbf4 / +0xbf8 of cars 1 and 3 from `B_ST5L2RD1_K0.rec.msd` / `B_ST5L2RD3_K0.rec.msd` against
     `B_RD1_K0` / `B_RD3_K0` (default = original: 1100 / 1 both). Reader: `D:/MashedWT/D4-DC-77b0-ST5-HOLD_data/msd_fields.py`.
3. Outcome (PREREG rule):
   - All green: `roadmap.ps1 decision D4-DC-77b0-ST5-C4ACCEL "<flip MASHED_ST5_LEG2, expected refs = H_ST5L2 / B_ST5L2 sha8s, D2 + run13 in the flip pass>"`.
   - W-GO red with W-C4 green: the remaining gap is the cd draw or the state-5 start of 0x005f29b8. Next step reads
     the original's 0x0089a4f8 + v*0x74 and 0x005f29b8 at state-5 entry (one original boot, recipe above), compares
     with the diag's `cd=` / `c29b8=`. No flip filed.

## Files

Committed: `PREREG_D4-DC-77b0-ST5-C4ACCEL.md`, this RESULT, `score_c4.py`,
`mashedmod/src/mashed_re/Ai/AiStandalone.cpp`, `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`.

Not committed (D:/MashedWT): `D4-DC-77b0-ST5-C4ACCEL_root/` (robocopy of `original\`), `D4-DC-77b0-ST5-C4ACCEL_data/arms.sh`.
Original evidence cited (ST5-HOLD data, hashes in its RESULT): `orig/w_c1.msd.aictrl.csv` 5800d44c...,
`orig/w_c2.msd.aictrl.csv` df1087c4..., `orig/w_c3.msd.aictrl.csv` 035e4bab..., `arms3/B_ST5L2D_K0.log` baad5ecf...
