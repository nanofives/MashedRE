# PREREG PORT-X87-PC24: the game thread at x87 PC_24 from device creation on (knob MASHED_X87_PC24)

Step PORT-X87-PC24 (ledger D4), Mariano's decision L-103 (a). Lead cmw7xu1960f37p41557tefpnd. Background:
`verify/portstate_20261109/RESULT_PORT-X87-PC24-AUDIT.md` on `orch/PORT-X87-PC24-AUDIT` (`dc8b11eb8`). Committed alone,
before the knob code and before any arm. Branch `orch/PORT-X87-PC24` from `orch/integration` `5873e911e`.

## 0. Facts this rests on (from the audit, not re-measured here)

- The original runs every matched x87 wrapper it reaches after D3D device creation at CW 0x007f (PC_24, round
  nearest, all masked): 74 of 82 reached rows. The 6 rows at 0x027f run before device creation, 2 straddle it.
- The exe runs at 0x027f (PC_53) because `CreateDeviceWatched` (`mashedmod/src/mashed_re/exe_main.cpp:3186`) creates the
  device on a detached `std::thread`, so D3D9's FPU setup lands on that worker and the game thread keeps the CRT's 0x027f.
- 0x007f and 0x027f differ only in the precision-control field (`_MCW_PC`).

## 1. The knob (what changes)

`MASHED_X87_PC24` already exists (2026-10-10, `verify/d2_pc24_20261010`, default OFF, no ledger owner): it forces PC_24
in `PumpOnce` every frame from the first frame, which can be before device creation. This step redefines it:

- **K1.** In `InitD3D9`, on the game thread, right after a `CreateDeviceWatched` attempt returns success (both success
  returns), `_controlfp_s(&t, _PC_24, _MCW_PC)`. This is the point where the original's thread becomes 0x007f
  (its CreateDevice inside SubsystemInit 0x00492270, which the exe stands in for with InitD3D9).
- **K2.** `PumpOnce` no longer forces PC_24 from frame 0. With the knob ON and the device created, it reads the PC
  field once per frame. A value other than PC_24 counts one drift event and is reasserted. `MASHED_LOG_CW` logs the
  count. Prediction: 0 drift events in every arm.
- **K3.** The two pickup callbacks in `Gameplay/PickupTick.cpp` (`PK_Grant` 0x0045c010 stand-in, `PK_Sfx` 0x00465ca0
  stand-in) force PC_53 today to reproduce "the exe's PowerupSystem runs at the thread's PC_53 everywhere else". Under
  the knob the thread baseline is PC_24, so they use PC_24. `MASHED_X87_PC24_PK53=1` keeps PC_53 there (interaction arm
  only).
- Knob OFF: no code path changes. P1 below is the check.

Threads that run game code in `mashed_re.exe` (static census, `CreateThread` / `_beginthread*` / `std::thread` /
timer-queue grep over `mashedmod/src` and `mashedmod/deps`): (1) the game thread (WinMain, the window procedure through
`DispatchMessageA`, sim, AI, physics, render, audio voice model); (2) the `CreateDeviceWatched` worker (only
`IDirect3D9::CreateDevice`, already PC_24 from D3D9, exits); (3) the ShadowTrack watchdog (`Core/ShadowTrack.h:181`, only
with `MASHED_SHADOW_TRACE`, diagnostics, no game float code). Only (1) needs the knob. The arm log records the thread id
the knob ran on and the id of the thread that runs `PumpOnce`; they must be equal.

## 2. Predictions (falsifiable)

- **P1 (OFF unchanged).** Knob unset: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (`merge_train.json`).
- **P2 (ON moves the race).** Knob ON: all three G-CTRL hashes and all three B hashes differ from P1. Cause: 208 TUs are
  `/arch:IA32` (`mashedmod/x87_tus.txt`) plus the qhull lib, among them the RwpSolver* physics TUs, PathNodes,
  CameraEntries, FastSqrt 0x004c3b30 and the tri-normal 0x0046c5f0. Two H_X24 runs are byte-identical (determinism).
- **P3 (CW per row).** Under ON, on the G-CTRL Training recipe (the audit's exe recipe, `x87_cw_probe.py --target exe`):
  every reached row that the original runs at 0x007f after device creation reads 0x007f in the exe. Named rows:
  0x0046c5f0, CameraEntries 0x00441b30 / 0x00441c80 / 0x00442600 / 0x00445aa0, FastSqrt 0x004c3b30, 0x00407a60,
  0x00409290, 0x00414060, 0x0041d910, 0x00458fa0, plus the 4 already at 0x007f (0x0040e470, 0x00442cc0, 0x00458920,
  0x004282a0). The pre-device rows the original runs at 0x027f (0x00498910, 0x00498950) stay 0x027f. 0x00402f50
  (original 0x007f, exe 0x027f today) is [UNCERTAIN]: it stays 0x027f if the exe calls it before InitD3D9, which is a
  call-order fact, not a CW fact. Knob OFF on the same build reproduces the audit's exe table.
- **P4 (D2).** D2_X24H_1..3 stay 3/3 in the O42 band (`verify/d2band42_20261029/band.json`). No direction predicted.
- **P5 (B).** B_X24 arms keep 9/9 per-car envelope PASS (`vsum.py`, `re/tools/b_envelopes.json`) and (e) 3/3 PASS.
  The (e) launch values (`1425.7 / 2052.5 / 2055.0`) may change. No direction predicted.
- **P6 (13 tracks).** `run13.py run ... MASHED_X87_PC24=1`: INSTRUMENT GATES PASS. Table identical to
  `verify/run13p3_20261029/tracks_score_sub_p3.txt` or different only in matched counts. No direction predicted.
- The 10 matched rows the audit lists as "x87 @ PC_53 would not match once called" (0x00427680, 0x00427e00,
  0x00427f00, 0x00428140, 0x00428320, 0x00428610, 0x004288a0, 0x00428a30, 0x00428d30, 0x00495080) are port=body/none
  today: no hash movement is attributed to them. Under ON they would run at the original's CW once called.

## 3. Interaction with the per-row precision knobs (predictions)

Survey of `mashedmod/src` (2026-11-09): `MASHED_ROTATE_PC24`, `MASHED_ROTATE_LUT`, `MASHED_ORTHO_LUT` and
`MASHED_WCS_COMBINE_PC24` no longer exist (D5-KNOBS-E3-GEOM-AI / E2: their ON paths are unconditional). What is left:

| knob / scope | today | prediction under ON | arm |
|---|---|---|---|
| `MASHED_CC_PC24` (seeded ON, CarCarContacts PC_24 scope) | ON | **redundant**: H_X24NCC == H_X24 | H_X24NCC |
| `MASHED_MATMUL_PC24` (default OFF, 0x005cb2a0 scope) | OFF | **redundant**: H_X24MM == H_X24 | H_X24MM |
| unconditional local PC_24 scopes (RwMatrixRotate, BodyOrientationIntegrate, RwV3dNormalize, PathNodes, PathProgStep, WorldRegistry, WarzonePontoon, AiTargetClosest `MASHED_TGT3_PC`, `MASHED_MODE7_PC`, AiLeafTargets, CollisionWorld, `MeasureMenuMessage`) | PC_24 inside | **redundant** by construction (set PC_24 on PC_24, restore to PC_24) | none (static) |
| PickupTick PC_53 scopes (K3) | PC_53 inside | **conflicting** with ON. H_X24PK vs H_X24 measures whether it bites on G-CTRL | H_X24PK |
| `MASHED_COLL_LUT` (default ON, LUT vs std::sqrt) | ON | **still needed**: the LUT is a different algorithm, not a precision: H_X24NCL != H_X24 | H_X24NCL |
| `MASHED_NO_LEAVES1_X87` (x87 leaves default ON) | x87 | **still needed**: the x87 body now runs at the original's PC; H_X24NL1 != H_X24 | H_X24NL1 |
| `MASHED_A5_DRAFT_LUT`, `MASHED_A6B_NORM_LUT` (seeded ON), `MASHED_NO_A3_LUT`, AI LUT mask 7, RW LUT (Rotate/Ortho, unconditional) | ON | still needed (algorithm, not precision). No arm: same class as COLL_LUT | none |
| `MASHED_A6A_LUT`, `MASHED_A6A_DRIVE_NORM_LUT`, `MASHED_A5_SUSP_LUT`, `MASHED_RW_LUT_ALL` | OFF | unaffected (report-only, algorithm) | none |

"Redundant" = byte-identical G-CTRL hashes with and without the knob under ON. "Still needed" = hashes differ.

## 4. Arms

All boots through `mashed_lock.py run` (D2 and run13 with `--solo`), test-slot outside, private root
`D:/MashedWT/PORT-X87-PC24_root`, raw data `D:/MashedWT/PORT-X87-PC24_data`. Runner: `verify/savelink_20261023/run_arms.py`
with stacks added: `X24 = [MASHED_X87_PC24]`, `X24H = X24 + DEFH`, `X24NCC`, `X24MM`, `X24PK`, `X24NCL`, `X24NL1`.

1. **A0 (OFF):** H_DEF, B_DEF_K0, B_DEF_K1, B_DEF_K26. P1.
2. **A1 (ON):** H_X24, H_X24-2, B_X24_K0, B_X24_K1, B_X24_K26. P2, P5.
3. **A2 (D2, solo):** D2_DEFH_1..3 (same-session control) and D2_X24H_1..3. P4 and S2.
4. **A3 (13 tracks, solo):** `run13.py run --exe <build> --root <root13> --out <data>/tracks_x24 MASHED_X87_PC24=1`, scored
   with `verify/rres_20261016/cites_rres.json`. P6 and S4. Reference = `tracks_score_sub_p3.txt` (DEF).
5. **A4 (CW):** `x87_cw_probe.py --target exe` (audit tool, `dc8b11eb8`) on this build, G-CTRL recipe 1200 frames, ON and
   OFF. P3 and S1.
6. **A5 (interactions):** H_X24NCC, H_X24MM, H_X24PK, H_X24NCL, H_X24NL1. Section 3.

A run that crashes on a piz collision (U-9677) or comes out short (D2 motion_diag under 1600 lines) is re-run, not scored.

## 5. Scoring rule: is ON closer to the original?

- **S1 (CW, required).** P3 holds: every reached post-device row at 0x007f, the pre-device rows at 0x027f.
- **S2 (D2).** Required: 3/3 ON arms in band. Distance d = mean over the 3 arms of |drvmed - 1964.41| (O42 mean,
  `band.json`). Noise n = max(half-range of the DEF arms, half-range of the ON arms). ON is **closer** if
  d(DEF) - d(ON) > n, **farther** if d(ON) - d(DEF) > n, else **same**. The slips are reported the same way, not scored.
- **S3 (B).** Required: 9/9 envelope PASS and (e) 3/3. Scored **same** when both hold (9/9 is the ceiling).
- **S4 (13 tracks).** Required: INSTRUMENT GATES PASS. Over the 39 rows: ON is **closer** if the summed `matched` rises
  and no row's `red` count rises, **farther** if the summed `matched` falls or any `red` rises, else **same**.
- **Verdict.** ON is **closer** if S1 holds, every required check holds, and at least one of S2 / S4 is closer with
  neither farther. **Equivalent** if S1 holds, the required checks hold, and S2 and S4 are both same. **Not closer**
  otherwise.
- **Recommendation rule.** Closer: propose the flip. Equivalent: propose the flip on fidelity grounds (S1: the original's
  own state), flagged as a judgment call for Mariano. Not closer: do not propose the flip, list the failing check.

## 6. Out of scope

No default flip, no gate reference change, no tracker edit. Finding (a) of the audit (0x00443300 DDK evaluation order)
is a separate step.
