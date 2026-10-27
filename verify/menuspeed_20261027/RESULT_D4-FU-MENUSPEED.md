# RESULT D4-FU-MENUSPEED: menu animation speed default 1.0, one body for 0x004325c0

Verdict: **GREEN for L-35 = 3 as decided, plus one DECISION row.** `0x004325c0` is one body for both targets. The exe
default speed is 1.0 and `MASHED_MENU_ANIM_SPEED=6` gives the old menus. Default G-CTRL and B hashes **did not move**, so
this is an ordinary merge. New finding: the original takes one anim tick **per frame**. The exe's frame-clock input is
wall time, so at speed 1.0 the exe menus run 3x slower than the original at 60 fps. Fix shipped default OFF
(`MASHED_MENU_CLOCK_FAITHFUL`), flip filed as a decision.

Base `orch/integration` @ `ebc7b0c36`. PREREG `PREREG_D4-FU-MENUSPEED.md` (`de2d16169`, alone, before any build or arm).
Amendment 1 `PREREG_D4-FU-MENUSPEED_amend1.md` (`612b94b8a`, alone, before its arms). Code `48798c3ae` (one body + speed),
`be3429252` (clock knob + debug log). Harness `25b6a9451` (`menu_anim_diff.py`: `settle` count, `MENU_ANIM_ASI`; defaults
unchanged), `74ec31d91` (`build_navsm_test.bat` links again), drivers `run_anim_arms.py`, `run_navsm_arms.py`,
`run_parity.py`, `run_exe_timing.py`. Builds `MASHED_NO_DEPLOY=1`. Private root `D:\MashedWT\D4-FU-MENUSPEED_root`
(robocopy of `original\`, `MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E`); the new `.asi` was copied there only. Every
boot under test-slot + `mashed_lock.py run --label D4-FU-MENUSPEED`; harnesses killed only their own PIDs.

## 1. What changed

| Item | Before | After |
|---|---|---|
| `0x004325c0` | two bodies: `.asi` `MenuAnimTickTwin.cpp` (asi-only) and exe `MenuNavSM.cpp` `Anim_Tick` (x6) | one body `MenuAnimTickBody` in `Frontend/MenuAnimTickTwin.cpp`, now in both rsp lists. `.asi` export passes live globals, exe `Anim_Tick` passes `g_records` / clock / nav slot (`MenuAnimTick.h`) |
| exe speed | `kMenuAnimSpeedup = 6.0f` | `MASHED_MENU_ANIM_SPEED`, default 1.0 (`x * 1.0f` exact) |
| `.asi` settle path (`0x004326b8`) | Y written to +0x0c, `FUN_0042ac50` called as cdecl | Y at +0x1c (`0x004326f2 fstp [esi+0xc]`, ESI = rec+0x10), `FUN_0042ac50` with EAX/ECX via a call helper |
| `MASHED_NO_ANIM_TRUNC` | `.asi` only | shared body, both targets (opt-out only) |
| exe frame-clock input | elapsed ms | unchanged by default. `MASHED_MENU_CLOCK_FAITHFUL=1` feeds 50 per frame (`0x004933d5`). Debug `MASHED_MENU_ANIM_LOG` |

`scripts/lint_rva_bodies.py`: `CROSS-TARGET=1 | allowlisted=1 NEW=0`, unchanged (the lint never listed `0x004325c0`).

## 2. Arms and predictions

### 2a. navsm_test Piece 3 (push screen 8, one tick per frame)

`build_navsm_test.bat` had not linked since D4-WSH-EXEFILE run 11 (`PromptStripTag` / `PromptStripResolve` unresolved). Fix:
add `PromptStripTwin.cpp` + `MenuNearLeaves_s6.cpp` and `/DMASHED_STANDALONE`, applied to both the base snapshot and the branch.

| Arm | Build | Result | Prediction |
|---|---|---|---|
| N0 | base `ebc7b0c36` (x6), `git archive` snapshot | GREEN, settled after **17** frames | 17, met |
| N1 | branch default | GREEN, **91** frames | 91, met |
| N2 | branch, `MASHED_MENU_ANIM_SPEED=6` | GREEN, **17** frames | 17 (= N0), met |

### 2b. `.asi` shared body against the original (`menu_anim_diff.py`, 20 samples, `.asi` `052b7132...`)

| Arm | dt | Result | Prediction |
|---|---|---|---|
| A0 | game's own (`0x3c888888` on all 20) | GREEN 20/20 | met |
| A1 | 1 tick | GREEN 20/20 | met |
| A2 | 2 ticks | GREEN 20/20 | met |
| A3 | 0.3 (`0x3e99999a`) | GREEN 20/20, `settle` = 100 (5 item rows per sample went type 0 -> 1) | met: the `0x004326b8` Y path ran on both sides |
| A3c | 0.3, old `.asi` `906F8601...` | RED 0/20: rec 1 tag `0xff040000` +0x0c, original `-1` (untouched), old twin `0x43700000` (240.0f) | met (+0x0c) |
| A4 | 1 tick, `MASHED_MENU_ANIM_SPEED=6` | RED 0/20: rec 0 tag `0xff000000` +0x10, v0 511, original 498 (-13), twin 432 (-79) | met |

### 2c. Default gates (`run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`)

| Exe | G-CTRL | B | Refs (`merge_train.json`) |
|---|---|---|---|
| `48798c3ae` build `d485dacf...` | `5943c5f1/364350ae/52052890` | `3a03670c/e85ea6fb/53778664` | equal |
| `be3429252` build `703f1a03...` (clock knob OFF) | `5943c5f1/364350ae/52052890` | `3a03670c/e85ea6fb/53778664` | equal |

Prediction (unchanged) met: the hashes did not move, so no new refs.

### 2d. Menu parity, screen 1

| Diff | Result |
|---|---|
| standalone default vs x6, frame 60 | GREEN 238/238 per frame (476 total) |
| standalone default vs x6, frame 400 | GREEN 238/238 per frame |
| original burst (settle 6 s) vs default, frame 400, `--scale-b 1` | GREEN 117/117 per frame (234) |
| original vs x6, frame 400, `--scale-b 1` | GREEN 117/117 per frame |

At `--scale-b 0.8` (the scenarios.json default for an 800x600 standalone) both speeds give the same RED, 0 matched / 225
missing / 225 extra, every row offset by the scale. The standalone in this root draws at 640x480, so 1 is the like-for-like
factor. Frame 60 does not separate the speeds on screen 1: it is the depth-0 root, whose rows do not slide in. The
mid-animation check is section 3 (screen 8). `nav_coverage.py` not run: no table or push-map edit.

## 3. Original-side timing, and the frame-clock finding

`verify/menuanim_20261027/orig_anim_timing.py` (observe-only attach, push screen 1): 230 calls over 4041 ms, median gap
17 ms, `DAT_007f1004` = `0.016666666` on **all 230**, phase 3. Item rows settle (type 0 -> 1) at call 16 = **270 ms**, the
back row at call 40 = **781 ms**, all frozen at call 90 = **1632 ms**.

Prediction (D4-FU-MENUANIM model: 800 ms / 2000 ms) **not met**. The call counts match the per-tick model (16 / 40 / 90,
the navsm N1 lifecycle). The model's wall clock does not: it assumed one tick per 50 ms of wall time.

Cause (disassembly): `FUN_00493390` stores constant `0x32` into `DAT_007f1000` at `0x004933d5` on every call.
`FUN_00493480` calls it (`0x00493480`) and reads `DAT_007f1000` back (`0x00493485`), so its input is always 50: one tick
per frame, `DAT_007f1004` = 50 * `0x005cc948` (`0x0049351f..0x0049352b`). Wall time from `FUN_004950b0` (`0x00493392`)
only reaches `DAT_007f0ffc` / `DAT_007f1010` / `DAT_007f1030`. The exe feeds elapsed ms (clamped to 50) instead, which at
~17 ms frames is one tick every third frame.

Exe timing arms (amendment 1, `MASHED_GOTO=8`, `MASHED_MENU_ANIM_LOG`):

| Arm | Env | dt nonzero | Items settle (call / ticks / ms) | Back row settle | All frozen | Prediction |
|---|---|---|---|---|---|---|
| E0 | default | 222 of 669 calls | 51 / 16 / 816 ms | 123 / 40 / 2016 ms | 273 / 90 / 4517 ms | met (~800 / ~2000 ms) |
| E1 | `MASHED_MENU_CLOCK_FAITHFUL=1` | 674 of 674 | 16 / 16 / 235 ms | 40 / 40 / 634 ms | 90 / 90 / 1501 ms | met: calls equal the original's 16 / 40 / 90, ms at 0.87 / 0.81 / 0.92 of 270 / 781 / 1632 (back row at the 20% edge) |
| E2 | knob + `MASHED_MENU_ANIM_SPEED=6` | 673 of 673 | 3 / 3 / 21 ms | 7 / 7 / 88 ms | 16 / 16 / 238 ms | met (3 / 7) |

Rows are logged after the tick, the original's before it, so "call" here is the line index + 1. The wall-ms gap
under E1 comes from frame pacing, not ticks: the exe's first calls after the push are 5 ms apart, the original's 17 ms.

So the default this step ships (speed 1.0, wall-clock input) is per-tick faithful but 3x slower than the original
on the wall clock at 60 fps. The pre-step exe (x6, wall input) was about 1.8x faster than the original
(items 3 ticks = 150 ms against 270 ms). Faithful in both senses = speed 1.0 + `MASHED_MENU_CLOCK_FAITHFUL=1`.
Frida spawn of `MASHED.exe` worked on this boot (A-arms, `menu_draw_burst.py`, `orig_anim_timing.py`), so the
D4-FU-MENUANIM ~3.5 s spawn death did not reproduce here.

## 4. Decision filed

**L-36** (`roadmap.ps1 decision`): flip `MASHED_MENU_CLOCK_FAITHFUL` to default ON? Options: (1) ON,
`MASHED_NO_MENU_CLOCK_FAITHFUL` opts out. Then exe menus match the original per frame (E1). (2) keep OFF. Exe menus stay 3x
slower than the original at 60 fps (E0). Menu-only, so G-CTRL / B do not run it. Recommendation (1).
If Mariano chose L-35 = 3 to get the original's feel, (1) is what delivers it. The 800 / 2000 ms in the L-35 question were
the exe's timing at speed 1.0, not the original's.

## 5. Tracker

Through `re-classify`: `0x004325c0` C3 -> C3. `exe_file` := `mashedmod/src/mashed_re/Frontend/MenuAnimTickTwin.cpp`.
Note appended (one body, speed knob, the two `.asi` settle-path fixes, A0-A4). CHANGELOG line under `<!-- ENTRIES -->`.
C3 is re-earned on the shared body by A0-A3, and A3 is the first arm to run the settle path. C4 still needs a canonical-scenario run.
No STUBS / UNCERTAINTIES / DEFERRED change (U-4251 structural, blocks none, unchanged). The A-arms ran on `.asi`
`052b7132...` (`48798c3ae`). `be3429252` only adds a `MenuNavSM.h` declaration on the `.asi` side (rebuilt `.asi` `3e217e6c...`,
body text unchanged) and was not re-diffed.

## 6. Raw data (not committed), `D:/MashedWT/D4-FU-MENUSPEED_data/`

| File | SHA-256 |
|---|---|
| `N0.log` / `N1.log` / `N2.log` | `6d71b652...1522` / `aabe26fd...4cca` / `3f89ef64...026dee` |
| `arms/A0.json` (= `A1.json`, same bytes) | `7ff561078831af716a352b816331fccc49fd386574b6c14f54d7f3a77af73f4c` |
| `arms/A2.json` | `c493415a9e49a4e530aad7bb8adff214d09bb45275fd63e944b4156c93a8fdcc` |
| `arms/A3.json` | `2b59f96924c1496fffa63a871fff478791b3a531e973362acd6adca488a360ab` |
| `arms/A3c.json` | `2be36c929f053e2c140ddb5f484d1a87138ad54aa9a02b3f690e4e969efcb8ca` |
| `arms/A4.json` | `974b40cdad7eab8ac6ff911bc068383f6a0a5ad9c267ec37103cd27c98d9004f` |
| `arms/summary.json` | `66c9f73554e02c77e636c19b09d35537d1ed3b3d833dc534b79cfbaa43595f7f` |
| `old_mashed_re_dev.asi` (A3c control, main install copy) | `906f8601201f69b4670bfef68b438b719e515e0e4e523f892d92c9f095b8a8ad` |
| `gates/` and `gates2/` `H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/` and `gates2/` `H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/` and `gates2/` `H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/` and `gates2/` `B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/` and `gates2/` `B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/` and `gates2/` `B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `orig_timing.json` | `0eceb4f5f181a5022e67ae0d340039012ad96b9f1ebe80d9e8025462b2214334` |
| `parity/orig_s1.json` | `950565027044fd2f6db2ba3b509a876fb6d92a8d56e2e13d98042b487c48ac10` |
| `parity/re_def_f60.json` / `re_def_f400.json` | `cbe32705...022c` / `638a89a6...4430` |
| `parity/re_x6_f60.json` / `re_x6_f400.json` | `69204ef1...0ccb` / `c39001e4...c9c8` |
| `parity/diff_orig_vs_def_f400_s1.txt` / `diff_orig_vs_x6_f400_s1.txt` | `c3205119...6269` / `bdc3881a...4bb7` |
| `exe_timing/E0.animlog.txt` | `a67d6c49ee4545424fb6e89cde96d7a7f8c4813ed3153e9c7cdbe3159bdb9957` |
| `exe_timing/E1.animlog.txt` | `e12ff758a79cf8b144076c76082415b49766319030ad7cb909e81d5d805118a3` |
| `exe_timing/E2.animlog.txt` | `45176d0fb49a557ea230dee96d4e7d35e54283c9c8141277a6e15ad8bde50e68` |
| `exe_timing/summary.json` | `97b06ff80afcd1b6351c86eb431654d43adc3e39e176fd51bcb7665e02d3b9e0` |
| tested exe (`be3429252`) | `703f1a036a8c05936f5f3b54bdcf013710cb36c5d363a7f6bc4fe87997c8634f` |
| tested exe (`48798c3ae`) / `.asi` | `d485dacfd35fe0a25f482414115e7d36b74c99015490374ca562c4e6156a14f5` / `052b71329afe2823f402c88ce8d3e854c04e6d4d74759b2caa1a9b48c47db74d` |
