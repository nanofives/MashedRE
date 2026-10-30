# RESULT D5-KNOBS-B1-RENDERFX (2026-10-30)

Verdict: **GREEN**. 8 of the 10 named knobs deleted (render FX), exe builds, G-CTRL and B unchanged, rva-lint 0 NEW. 2 knobs left out (section 1).

Step: first batch of class (b) knob deletions from `verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md`,
render FX only. Branch `orch/D5-KNOBS-B1-RENDERFX` from `orch/integration` @ `9d3e2421c`. No default flipped: every
deleted read was an opt-out (`=0`) of a path that runs when the variable is unset, so the default build executes the
same code before and after.

## 1. Audit, one line per knob (read site at `9d3e2421c`)

`EnvOff(name)` (`LibRw/RwRaceFx.cpp:39`, `LibRw/RwCarFx.cpp:24`) is true only for the exact value `"0"`, so an
unset variable gives ON. `Core/FaithDefaults.cpp` seeds none of these names (its table, lines 30-40, has no
`CAR_*`, `HEADLIGHT`, `LENSFLARE`, `OBJ_SHADOW`, `ICONCUBE`, `PICKUP_GLOW` or `RFX_EMITTERS` entry).
Script check = `grep -rnw MASHED_<name>` over `scripts/`, `re/tools/`, `re/frida/`, `verify/` (`.py .ps1 .sh .bat .js .cmd .json`).

| Knob | Read site | Default | Set by a live script | In batch |
|---|---|---|---|---|
| MASHED_CAR_BEAM | `LibRw/RwRaceFx.cpp:191` `!EnvOff` | ON | none | yes |
| MASHED_CAR_DIRT | `LibRw/RwCarFx.cpp:61` `!EnvOff` | ON | none (named in prose only: `verify/rtex_20261016/tracker_tx.py:50`, `verify/carlod_20261016/tracker_tx.py:41`) | yes |
| MASHED_CAR_SHADOW | `LibRw/RwRaceFx.cpp:190` `!EnvOff` | ON | none | yes |
| MASHED_CAR_SHADOW_RT | `LibRw/RwRaceFx.cpp:2103` `!EnvOff` | ON | none (named in `cites_used_diff.json` notes only) | yes |
| MASHED_CAR_VIS | `LibRw/RwCarFx.cpp:66` `!EnvOff` | ON | only the closed carlod runner `verify/carlod_20261016/run_all.py:43` (`visoff4` control, 2026-10-16); no run_arms sets it | yes (see 3) |
| MASHED_HEADLIGHT | `LibRw/RwRaceFx.cpp:1066` `!EnvOff` | ON | none (prose: `verify/rfx2_20261016/tracker_tx.py:15`) | yes |
| MASHED_LENSFLARE | `LibRw/RwRaceFx.cpp:992` `!EnvOff` | ON | none (prose: `verify/rfx2_20261016/tracker_tx.py:16`) | yes |
| MASHED_OBJ_SHADOW | `LibRw/RwRaceFx.cpp:1072` `!EnvOff` | ON | none (prose: `verify/rfx2_20261016/tracker_tx.py:14`) | yes |
| MASHED_NO_PICKUP_GLOW | `Gameplay/PickupGlow.cpp:50` | **OFF path**: `On()` is `MASHED_RFX_EMITTERS == "1" && !NO_PICKUP_GLOW` (`PickupGlow.cpp:47-52`); `MASHED_RFX_EMITTERS` is default OFF and not in FaithDefaults (`PickupGlow.h:15`, `RwRaceFx.cpp:2301`) | none | **no**: opt-out of a default-OFF path, not class (b) |
| MASHED_NO_ICONCUBE | `D3d9Render/PickupIconCube.cpp:64` (with `MASHED_ICONCUBE=0` at `:65`) | ON (`IconCube_Enabled`, L-56 flip) | **yes**: `verify/savelink_20261023/run_arms.py:446` `STACK['NOPGIC']` (the gate runner itself) and its copy `verify/d5partmorphflip_20261030/run_arms.py:446`, plus `verify/pregocubeflip_20261029/run_arms.py:435` / `run_ctl.sh` | **no**: still set by the gate runner's NOPGIC stack |

## 2. Change (render files only)

- `LibRw/RwRaceFx.cpp` / `.h`: removed `RaceFx_CarShadowEnabled`, `RaceFx_BeamEnabled`, `RaceFx_LensEnabled`,
  `RaceFx_HeadlightEnabled`, `RaceFx_ObjShadowEnabled`, `RaceFx_ShadowRtEnabled` and every test of them.
  `RaceFx_DrawCarFx` loses `want_shadow` / `want_beam` and the `(!want_shadow && !want_beam)` early-out (with both
  ON it never fired; the `!any` early-out keeps its effect, `g_have` is already all false there).
- `LibRw/RwCarFx.cpp` / `.h`: removed `CarDirtEnabled`, `CarVisEnabled`. `CarEnvEnabled` (knob `MASHED_CAR_ENV`,
  not in this batch) returned `CarVisEnabled()` when unset and had a `=1` force-on for the `CAR_VIS=0` case; with
  CAR_VIS gone the `=1` branch is dead, so it is now `!EnvOff("MASHED_CAR_ENV")` (unset ON, `=0` OFF, unchanged).
- `D3d9Render/TrackRenderer.cpp`: the dirt load (`RegisterCarModel`, line 4235) and the `carvis` lambda (line 10260)
  drop their `CarDirtEnabled()` / `CarVisEnabled()` tests; two comments that named `MASHED_CAR_VIS=0` now name the
  remaining null case (no part-19 model).
- 5 files, 18 insertions, 59 deletions. `exe_main.cpp` and `stage_lan_release.py` untouched (D5-ICON-ASSET).
- Built exe check: none of the 8 names occurs as a string in the new `mashed_re.exe` (`grep -a`); `MASHED_CAR_ENV`,
  `MASHED_NO_PICKUP_GLOW` and `MASHED_NO_ICONCUBE` still do.

## 3. Note on the historical carlod runner

`verify/carlod_20261016/run_all.py:43` ran a one-off `visoff4` arm with `MASHED_CAR_VIS=0` as a no-op control for
PREREG_carlod (closed 2026-10-16). It is not a gate runner and nothing calls it. Re-running it on this branch would
make `visoff4` identical to the default arm. Left unedited (frozen evidence).

## 4. Gates

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` = `=== Build OK ===` (shared `.asi` not deployed).
rva-lint: `scripts/lint_rva_bodies.py` rc 0, `16 finding(s): CROSS-TARGET=16 | allowlisted=16 NEW=0`.

Arms: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, private root
`D:/MashedWT/D5-KNOBS-B1-RENDERFX_root`, under `mashed_lock.py run` and `test-slot`.
Refs from `re/orchestrator/merge_train.json`: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9 x3`.

| Gate | This exe (`mashed_re.exe` sha256 `9ce7c63a`) | Ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | `8284be46` / `ba407a16` / `31fe043e` | `8284be46` / `ba407a16` / `31fe043e` | same |
| B_DEF_K0 / K1 / K26 step | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | `1d5bf4f9` x3 | same |

First attempt stopped in H_DEF's copy step (`FileNotFoundError`, the runner does not create `A4_OUT`); re-run after
creating the folder, no game result involved. The 13-track table and D2 (`-Full`) are left to the merge train.

## 5. Raw files (not committed)

Folder `D:/MashedWT/D5-KNOBS-B1-RENDERFX_data/` (sha256):

- `arms/H_DEF.step.csv` 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198
- `arms/H_DEF.gates.csv` ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb
- `arms/H_DEF.ptrace.log` 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3
- `arms/B_DEF_K0.step.csv`, `arms/B_DEF_K1.step.csv`, `arms/B_DEF_K26.step.csv` 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457
- `build.log`, `lint.txt`, `arms_run.log`
- exe `mashedmod/build/mashed_re.exe` 9ce7c63ab063cef3f681426bea7d83dfae407520363cccbb05ad0790723ee045

## 6. For the lead

- No `.ps1` edit needed.
- Second batch candidates left out: `MASHED_NO_ICONCUBE` (and `MASHED_ICONCUBE=0`) needs the gate runner's NOPGIC
  stack retired first (that stack is report-only; dropping it is a run_arms change the lead owns).
  `MASHED_NO_PICKUP_GLOW` belongs with `MASHED_RFX_EMITTERS` (class c, L-52), not class (b).
