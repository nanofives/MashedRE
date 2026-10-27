# RESULT D4-FU-MENUCLOCK-FLIP: faithful menu clock default ON

Verdict: **GREEN.** `MASHED_MENU_CLOCK_FAITHFUL` is now default ON, and `MASHED_NO_MENU_CLOCK_FAITHFUL=1` opts out. Default
G-CTRL and B hashes **did not move** (G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664`, equal to
`merge_train.json`), so this is a normal merge with no new refs. The default exe menus now take one anim tick per frame,
like the original.

Decision L-36 = 1 (Mariano 2026-10-27). Base `orch/integration` @ `725474f9c`. PREREG `PREREG_D4-FU-MENUCLOCK-FLIP.md`
(`c7d9744f2`, committed alone before any arm). Code `1bc83c9b2`. Build `MASHED_NO_DEPLOY=1`, exe
`f1e34064fe5cc823...457a`, `.asi` `fa17e561...8456` (not deployed). Private root `D:\MashedWT\D4-FU-MENUCLOCK-FLIP_root`
(robocopy of `original\`, `MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E`). Every boot ran under `test-slot` +
`mashed_lock.py run --label D4-FU-MENUCLOCK-FLIP`. The harnesses killed only their own PIDs, and all six had exited at the end.
Evidence base: `verify/menuspeed_20261027/RESULT_D4-FU-MENUSPEED.md` section 3.

## 1. Change

`mashedmod/src/mashed_re/exe_main.cpp`, the frame-clock block before `Nav_AnimTick()`: `s_clock_faithful` is now
`GetEnvironmentVariableA("MASHED_NO_MENU_CLOCK_FAITHFUL") == 0`, so by default the exe passes 0x32 to
`Nav_FrameClockUpdate` on every frame. That matches the original: `FUN_00493390` stores 0x32 into `DAT_007f1000` at
`0x004933d5`, and `FUN_00493480` reads it at `0x00493485`. The opt-out restores the pre-flip input, elapsed ms clamped to 50.
`MASHED_MENU_CLOCK_FAITHFUL` is no longer read. No tracker change: no function's C-level moves.

## 2. Default gates (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`)

| Arm | Lines | sha8 | Ref |
|---|---|---|---|
| H_DEF step / gates / ptrace | 14659 / 56873 / 28436 | `5943c5f1` / `364350ae` / `52052890` | equal |
| B_DEF_K0 | 1459 | `3a03670c` | equal |
| B_DEF_K1 | 1825 | `e85ea6fb` | equal |
| B_DEF_K26 | 1411 | `53778664` | equal |

All four exited on their own with exit 0 (H_DEF 482 s, B 40-42 s). The flipped clock changed no gated byte. Unchanged, so the merge is normal and needs no ref update. `gates.txt`, `gates_RUNLOG.txt`.

## 3. Flip check, menu timing (`run_exe_timing.py`, `MASHED_GOTO=8`, `MASHED_MENU_ANIM_LOG`)

| Arm | Env | dt nonzero | Items settle (call / ticks / ms) | Back row settle | All frozen | Prediction |
|---|---|---|---|---|---|---|
| F0 | default | 680 of 680 | 16 / 16 / 230 ms | 40 / 40 / 630 ms | 90 / 90 / 1463 ms | = E1, met |
| F1 | `MASHED_NO_MENU_CLOCK_FAITHFUL=1` | 226 of 684 | 52 / 16 / 821 ms | 123 / 40 / 2005 ms | 273 / 90 / 4505 ms | = E0, met (E0 items call 51) |

F0's call counts equal the original's 16 / 40 / 90 (`orig_anim_timing.py`, 270 / 781 / 1632 ms, measured in
D4-FU-MENUSPEED). The measured original timings supersede L-35's modeled 800 / 2000 ms. The wall-ms gap (0.85 / 0.81 / 0.90
of the original) comes from frame pacing, as in E1: the exe's first calls after the push run about 5 ms apart, the
original's about 17 ms. Wall ms were reported, not gated. `timing_summary.json`.

## 4. Raw data (not committed), `D:/MashedWT/D4-FU-MENUCLOCK-FLIP_data/`

SHA-256 of every cited raw file and of the tested binaries: `raw_sha256.txt` (gates `*.step.csv`, `H_DEF.gates.csv`,
`H_DEF.ptrace.log`, `exe_timing/F0.animlog.txt`, `F1.animlog.txt`, `summary.json`, `mashed_re.exe`, `mashed_re_dev.asi`).
