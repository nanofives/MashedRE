# PREREG: U-9684, the original's post-contact AI command (c1 = 255 x16, c0 ramp)

Written 2026-10-13, before any run, any build and any code change on branch `work/u9684` (base `99056b2c`).
Model: claude-opus-5-5 (account2). Static reading only so far, from `MASHED.exe.unpatched`
(`re/tools/disasm_va.py`), `verify/d3_gatefire_20261008/ctrlstep_decomp.txt` (decomp of `FUN_00416250`) and
the committed original captures `verify/d3_car2pickup_20261013/o_p2a.msd.aistep.csv` (car 2, regime B) and
`verify/d3_modes37_20261009/o_tg1.msd.tgt.csv` (same recipe, `--tgt-probe`, all `FUN_00414a70` calls).

## 0. What the bytes already say (the G-SRC hypothesis, to be tested in section 2)

On `o_p2a` car 2, the 16 rows after the contact (frames 856..871) carry `ai_mode` 3, `ret14a70` 1, `ret16060` 1.
The row before (855) and the row after (872) carry mode 0, `ret14a70` 0. In `FUN_00416250`:

- `0x00416394` `call FUN_00414a70`. `0x0041639c` `cmp eax,1`. `0x004163ab` `call FUN_00416060` (LOS).
  `0x004163e6..0x004163f6`: look = cand (`[esp+0x2c]/[esp+0x30]` -> `[esp+0x34]/[esp+0x38]`), `mov ebx,3`.
- `0x00416863..0x00416885`: `if (180 < err && err < 330)` (`0x5cd09c` = 180.0, `0x5cd0e0` = 330.0)
  then `ctrl[4] = ctrl[5] = ctrl[1] = 0xff`.
- `0x00416738..0x00416758` (err > 180 branch, band state `[0x0089a4ec + v*0x74] != 2` and
  `[0x007f0ff4] - [0x0089a4f0 + v*0x74] < 0xc8`): `ctrl[0] = ftol((0xc8 - elapsed) * 0.005 (0x5cd0ec) *
  [0x0089a4f4 + v*0x74])`. With the stored value 21 (c0 at frame 855) and 50 per call this gives 15, 10, 5, 0.

`FUN_00414a70` (verbatim in `Ai/AiTargetClosest_naked.inc`) reads per-car progress `0x008a96e8 + v*0x30c`
(`FUN_00408a50`), the slot table `[0x005f2770] + 0x34 + 4*j` (`FUN_0040e370` / `FUN_0040e470`), the car
positions (`+0x30` / `+0x38` of `FUN_0046d4a0`'s pointer), speeds, the forward row, and `[0x0066d6d8]`
(`FUN_00426bb0`). It reads no `+0xaec` and no contact flag. On `o_tg1` the player's progress goes 30.0 -> 0.013
and its speed 0 -> 1237.9 on the contact call, and `FUN_00414300` starts returning a cand from that call on.

**Hypothesis H:** the producer is `FUN_00414a70` returning 1 (mode 3, look = cand), and the c0/c1/c4/c5 bytes
follow from the existing band rules on the new err. The port has a verbatim body (`Ai/AiTargetClosest.cpp`,
G-P3-REPLAY 5,490/5,490, `verify/d3_modes37_20261009/RESULT_P3.md`). It is default-OFF behind `MASHED_TGT3`,
which needs `MASHED_PATHPROG` (the only exe producer of `0x008a96e8`). That would be why the exe does neither.

## 1. Arms (exe, built in this worktree, `MASHED_NO_DEPLOY=1`)

`run_h4.ps1` recipe (Training, car 1, round 1, rule 4, deterministic, 14400 frames, PLAYERTRACE, AI_STEPDUMP,
U9186_GATES), run inline from the worktree root:

| arm | knobs |
|---|---|
| `U_DEF`, `U_DEF2` | none (current default) |
| `U_PP` | `MASHED_PATHPROG` |
| `U_TGT`, `U_TGT2` | `MASHED_PATHPROG MASHED_TGT3 MASHED_TGT3_TRACE=<arm>.tgt3.csv` |

`run_fk.ps1` recipe (1200 frames, `MASHED_RING_SKIP` = K), K = 0 (A), 1 (B), 26 (C), for the default, `PP`
and `TGT` stacks: `B_DEF_K*`, `B_PP_K*`, `B_TGT_K*`. Scorer `verify/d3_regimes_20261013/score_regimes.py
re/tools/b_envelopes.json verify/d3_regimes_20261013/ORIG_LABELS.txt <csv...>` (the committed rule: A/B 220
cars pooled, C 196 per car).

No new forced inputs: the scenarios are the car2pickup / regimes recipes. No original-side run is planned. If one
is needed it uses the `o_tg1` argv only.

## 2. Gates, statistic, bound, and the decision each outcome triggers

- **G-SRC** (static + committed captures). Statistic: on `o_p2a` car 2 and `o_tg1` car 2, the post-contact rows
  with `c1 == 255` (the first contact run). PASS iff all of them (N/N) have `ret14a70 == 1` and mode 3, all have
  180 < err < 330, and the c0 bytes equal `ftol((200 - 50k) * 0.005 * c0_last)` on N/N rows.
  FAIL -> H is wrong, the producer is elsewhere: stop, report, no code.
- **G-REPLAY.** (i) `MASHED_TGT3_REPLAY=o_tg1.msd.tgt.csv` on this build at PC_24: N/N exact on return and cand
  bits, and in particular 16/16 on car 2's contact run (call 791..806). (ii) the section-0 band rules applied to the
  replayed cand's err give the original's c0/c1/c4/c5 bytes on 16/16 rows. That is the G-SRC model check run on the
  original's own err, since the err geometry is the port's mode-0 path, already exact.
  FAIL -> body defect: fix the body before any live arm is read.
- **G-LIVE** (`U_TGT` vs `o_p2a` / `o_tg1`). Alignment: call 0 = the first exe car-2 call after its contact call
  (the `rec_9e4` drop), against the original's frame 856 / call 791. Per row compare `(c0, c1, c4, c5, mode)`.
  PASS iff the exe's run of `ret == 1` starts at call 0 +- 1, its length is 16 +- 1, `c1` matches on >= 14 of the
  16 aligned rows, and `|c0 diff| <= 5` on >= 14 of 16.
  The regime-A car-1 / car-3 contacts (exe steps 64, 129/130) have no original counterpart (the original's cars
  1 / 3 do not touch inside the window). For those, report only: whether they still occur under `U_TGT`, and
  what the exe's AI emits after them.
  FAIL with G-REPLAY PASS -> the inputs differ (progress, positions, slot table). Name the first differing input,
  leave the knob default-OFF, U-9684 stays open with the cause narrowed.
- **G-CTRL.** `U_DEF` = `U_DEF2` = `991a5fff / be0d47ea / bc75b405`, or this build is not the merged default:
  stop. Expectation: if the producer runs by default, the default trio WILL change. The opt-out arm must then
  reproduce `991a5fff / be0d47ea / bc75b405` exactly.
- **(b) per regime** on `B_TGT_K*` vs `B_DEF_K*` (expected A 1/3, B 2/3, C 0/3). Also `B_PP_K*`, to separate
  PATHPROG's own share from TGT3's.
- **Collateral.** `collateral.py --mode banded` cross side on `s.v2.rec_9e4`, regime A, first 600 driving frames,
  anchor `s.v2.substate >= 6`, as in `RESULT_CAR2PICKUP.md` section 5. A = `o_p2b`, floor `o_p0a`. B = `U_TGT`,
  floor `U_TGT2`. Statistic: paired fields past the floor in some band. Baseline B = `U_DEF`, floor `U_DEF2`
  (car2pickup measured 39 / 94 on `C_DEF`). Fields that move inside / outside the floor are listed.

**Default decision (pre-registered):** default ON (with opt-outs) only if G-REPLAY PASS, G-LIVE PASS, (b) does
not lose a car in any regime against `B_DEF_K*`, and collateral is not worse than the baseline count.
Otherwise default OFF, behind the existing opt-in knobs, and stop with the trade-off reported.

**Prediction (written before any run):** G-SRC PASS. G-REPLAY PASS (the body was exact at `RESULT_P3`). G-LIVE:
since the car2pickup fix the exe's car-2 post-contact speed is 1050.685 against the original's 1050.409 and the
player now receives the impulse, so I predict the car-2 contact run reproduces within the tolerance.
(b): `RESULT_P3` measured PATHPROG alone and PATHPROG + TGT3 at A 0/3, on a build before car2pickup, because
the exe's window starts on the grid with the cars bunched (no countdown, D-11073). I predict (b) does NOT improve
and is worse in at least one regime, which would leave the producer default-OFF. That outcome is not decided here.
The table above decides it.

## 3. AMENDMENT 2026-10-13, written after G-LIVE failed and before the arms below were run

G-LIVE on `U_TGT` FAILED as registered: the exe's car 2 has `FUN_00414a70` = 1 from its first call on the grid
(27 calls), steers off the line and never makes the step-24 contact. First differing input (`S_TGT600.pathprog.csv`
vs `o_tg1.msd.tgt.csv`): the player's progress `0x008a96e8` is 0.000862 (node 0) in the exe, 30.000 in the
original. The AI cars' progress agrees. Per section 2, the knob stays default-OFF and U-9684 stays open, narrowed.

Report-only diagnostic arms, to test whether that input is the only carrier. They do NOT change the section-2
decision rule, and nothing below can flip a default: `ROUND_GRID` is a pending user decision.
- `U_RG` = `MASHED_PATHPROG MASHED_ROUND_GRID MASHED_ROUND_GRID_ORDER`;
  `U_TGT_RG` = the same plus `MASHED_TGT3` (+ trace, + `MASHED_PATHPROG_TRACE`). Both are existing knobs
  (`verify/d3_bulk_20261011/roundgrid`), so no new forced input.
- Read-outs: the player's progress on frame 1, then G-LIVE re-scored with the same bounds, and (b) per regime on
  `B_TGT_RG_K*`.

## 4. AMENDMENT 2, 2026-10-14, written after `U_TGT_RG` and before the arms below were run

Resumed session (claude-opus-5-5). Re-run on a fresh build of `b5ed1601` (exe sha `C9123FB212E3`): G-SRC 16/16 x3,
G-REPLAY 5,490/5,490, `U_DEF` = `U_DEF2` = `991a5fff / be0d47ea / bc75b405`, `U_TGT` = `U_TGT2` and `U_PP`, `U_RG` equal
their WIP hashes. G-LIVE FAILS on `U_TGT` and on `U_TGT_RG`: car 2 returns 1 from its second call (frame 1) in both.

`U_TGT_RG` puts all four cars on the original's frame-0 grid bit-exactly (player x/z `0.50093675 / -1.99903715` both
sides, `o_tg1` row n 1 vs `U_TGT_RG.ptrace.log` f=1). Two player inputs of `FUN_00414a70` still differ from the
original's car-2 call at frame 50 (`o_tg1` row n 4) onward:
- position z: the original's player moves once, -1.99903715 -> -1.99894500 (+0.0000921) between frame 0 and frame 50
  at speed 0, then rests; progress `0x008a96e8` becomes `0x41effff4` = 29.999977. The exe's player never moves
  (z -1.99903715 on every ptrace row), progress stays 0.000000.
- speed `0x00881f84 + i*0xd04` (= rec `+0x9e4`): original player 0.0 until the contact; exe player 0.638046 at rest
  (`v9e4` in the ptrace, vel (0,0,0)).

Report-only counterfactual arms, offline, through the G-REPLAY path (the verified naked body, `MASHED_TGT3_REPLAY`),
no live forced input. Input = the `o_tg1.msd.tgt.csv` rows with frame < 45150 (before the player first moves), with
the player's (car 0) recorded columns replaced by the exe's values:
- `CF_PR`: `pr0` := `0x00000000`; `CF_Z`: `z0` := bits(-1.99903715); `CF_SP`: `sp0` := bits(0.638046);
  `CF_ALL`: all three. `CF_NONE`: unmodified (the control, must give 0 mismatches).
- Statistic per arm: the replay's `ret_mismatch` count, and the car-2 rows whose return flips 0 -> 1.
- Prediction (before the run): `CF_PR` and `CF_ALL` flip car 2 to 1 on its early calls, `CF_SP` and `CF_Z` alone
  do not. Not tested before, so this is a guess, recorded so the result can refute it.
- Decision: none of these can flip a default. They only name which input carries car 2's early return of 1, for the
  U-9684 narrowing. Porting the player's grid settle is a D2-physics change and needs the user's decision.
