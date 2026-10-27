# PREREG D4-FU-MENUSPEED: menu animation speed default 1.0, one body for 0x004325c0

Decision L-35 = 3 (Mariano, 2026-10-27, `log/orch/DECISIONS.md`): the exe default menu animation speed becomes 1.0
(the original's), `MASHED_MENU_ANIM_SPEED=6` restores the old fast menus, and `0x004325c0` becomes one body
shared by the exe and the `.asi`. A decided flip, so the default-OFF-knob rule is waived for this step.
Base `orch/integration` @ `ebc7b0c36`. Written and committed alone, before any build or arm.

## 1. Original, from the disassembly (unpatched `MASHED.exe`, capstone, 2026-10-27)

`ESI` starts at `0x00898ad0` = record base `0x00898ac0` + 0x10, stride 0x34, loop while `< 0x008990e8` (30 records).
So `[esi-0x10]` = tag +0x00, `[esi-0xc]` = type +0x04, `[esi-8]` = float +0x08, `[esi]` = slide +0x10,
`[esi+0xc]` = **+0x1c**, `[esi+0x10]` = row +0x20, `[esi+0x20]` = screen pointer **+0x30**.

- `0x004325c8` `cmp [0x0067eca4],4` / `jl` picks base 2.0 (`0x005cc574`) or 1.0 (`0x005cc320`), held in ST1.
- `0x004325dd` `mov ebp,[0x0067ece0]`: item total read once.
- Settle `0x004326b8..0x00432734`: `mov eax,ebp; mov ecx,0x1e; call 0x0042ac50` (count in **EAX**, spacing in **ECX**,
  result EAX: `0x0042ac51 mov ebx,eax`, `0x0042ac6b imul ebx,ecx`), `fstp [esi+0xc]` (+0x1c); then
  `cmp ebx(=[esi+0x20]),0x5f7370` recompute with 0x1c and `fsub [0x5cd900]`; then `cmp ebx,0x5f72a0` `fsub [0x5cd900]`.

## 2. Change

- New shared body `MenuAnimTickBody` in `Frontend/MenuAnimTickTwin.cpp`, which joins `exe_sources.rsp` (it is already in
  `asi_sources.rsp`). The `.asi` export `MenuAnimTickTwin` + `RH_ScopedInstall` stay in that TU under `#ifndef MASHED_STANDALONE`
  and pass the live globals; the exe `Anim_Tick` (`MenuNavSM.cpp`) passes `g_records` / `g_frame_dt` / `g_anim_phase` /
  `g_anim_gate` / `slot.item_count` and its screen id. `kMenuAnimSpeedup` is deleted.
- Speed: `base = (phase >= 4 ? 2.0f : 1.0f) * MenuAnimSpeed()`, `MASHED_MENU_ANIM_SPEED` (float > 0, else 1.0), default 1.0.
  `x * 1.0f` is exact, so at the default the body computes the original's products.
- Inputs the two targets supply differently, with no second copy of the logic: `FUN_0042ac50` (exe: the port
  `Q_ListBaseY`; `.asi`: a call helper loading EAX/ECX and calling the original) and the screen match (`.asi`: record +0x30
  against `0x005f7370` / `0x005f72a0`; exe: `slot.screen_id` against 24 / 18, as today).
- Truncation knob `MASHED_NO_ANIM_TRUNC` (L-32) moves into the shared body, so the exe now honours it too (opt-out only;
  the default truncates on both targets, as both did before).

Side effect on the `.asi` (twin bugs this exposes, both on the type-0 settle path at `0x004326b8`):
1. The twin's `Rec` puts `y` at +0x0c, so it writes the settle Y into +0x0c. The original writes +0x1c (`0x004326f2`).
2. The twin calls `0x0042ac50` as `cdecl(int,int)`; the original passes EAX/ECX, so the twin's base Y is computed from stale registers.
The shared body uses the exe's `MenuRecord` layout (+0x1c) and the register helper, so both are fixed. The 2026-06-10 and
2026-10-26 arms never reached this path: `menu_anim_diff.py` reloads the table (`FUN_0043d2a0(scr,2)`, every slide 0x1ff) and
runs one tick, and one tick of -2000 * dt never reaches 0 at dt <= 2 ticks.

## 3. Arms and predictions

Build `MASHED_NO_DEPLOY=1`. Private root `D:\MashedWT\D4-FU-MENUSPEED_root` (robocopy of `original\`), new `.asi` copied
there only. Every boot under `mashed_lock.py run --label D4-FU-MENUSPEED` + test-slot.

### 3a. Exe timing, deterministic (`navsm_test`, Piece 3: push screen 8, `FrameClockUpdate(50)` per frame = 1 tick, dt `0x3c888888`)

Per tick at base 1: item -2000 -> `trunc(-33.33)` = -33, count-up +1000 -> 16; slide class -800 -> -13, +500 -> 8.
x6: `0.016666666 * 6 = 0.099999996`, so item -199 / +99, slide class -79 / +49 (`trunc`, not -200 / -80).
Back row (`0xff000000`, slide class): base 1 = 40 down + 50 up = 90 ticks, x6 = 7 + 9 = 16 ticks.

| Arm | Build | Env | Prediction (frames to `settled`) |
|---|---|---|---|
| N0 | base `ebc7b0c36` (x6) | none | 17 |
| N1 | new | none | 91 |
| N2 | new | `MASHED_MENU_ANIM_SPEED=6` | 17 (= N0) |

`rec1 slide == 0` and `OK` on all three. If N0 is not 17 the per-tick model above is wrong, and N1/N2 are read against N0 instead.

### 3b. `.asi` shared body against the original (`re/frida/menu_anim_diff.py`, 20 samples each)

| Arm | dt | Env | Prediction |
|---|---|---|---|
| A0 | game's own | none | GREEN 20/20 |
| A1 | `0.016666666` (1 tick) | none | GREEN 20/20 |
| A2 | `0.03333333` (2 ticks) | none | GREEN 20/20 |
| A3 | `0.3` (`0x3e99999a`) | none | GREEN 20/20, and the settle path runs: every 0xff040000 type-0 record goes 0 -> 1 in one tick (`511 - trunc(600.0) < 0`) |
| A3c | `0.3` | none, **old `.asi`** (the main install's `original\mashed_re_dev.asi`, read-only copy) | RED: first differing dword at +0x0c or +0x1c of the first settling 0xff040000 record |
| A4 | 1 tick | `MASHED_MENU_ANIM_SPEED=6` | RED 0/20 (twin slide step -79 / -199 against the original's -13 / -33): the knob reaches the shared body |

A3 needs a harness field counting records whose type went 0 -> 1 in the original's result (`settle`), added to
`menu_anim_diff.py` (Python harness, defaults unchanged). If A3 shows `settle = 0` on every sample, the screen in the
harness has no 0xff040000 type-0 record and A3/A3c are reported as not exercising the path, not as GREEN evidence.
Piz-collision crashes (U-9677) are re-runs.

### 3c. Default gates (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`)

Refs `re/orchestrator/merge_train.json`: G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664`.
Prediction: **unchanged**. The arms boot straight into the race (`MASHED_TRACK_VIEW=Training`, no menu screen), so
`Anim_Tick` either does not run or runs before any race state exists. A move would be code layout
(memory "sim trajectory is code-layout sensitive", which G-CTRL TRAINING has not shown). If they move, the new
hashes are recorded as the decided flip's expected refs and the step merges alone.

### 3d. Menu parity

- `nav_coverage.py`: unchanged output against base (no table or push-map edit).
- Settled frame: standalone `MASHED_GOTO=1 MASHED_DBG_DRAWSTREAM=400 MASHED_SETTINGS_DEFAULTS=1`, default against
  `MASHED_MENU_ANIM_SPEED=6`. At frame 400 (~6.7 s, 133 ticks at 60 fps) both are frozen (90 ticks back row), so
  prediction: `drawlist_diff.py` between the two standalone streams GREEN (identical settled draws). Against an
  original `menu_draw_burst.py --screen 1` capture (if Frida spawn works on this boot): same divergence classes and
  counts on both speeds.
- Mid-animation (frame 60, 20 ticks): the two speeds differ (x6 has item rows settled, default does not). Recorded, not gated.

### 3e. Original-side timing (if cheap)

`verify/menuanim_20261027/orig_anim_timing.py` against the private root. Prediction from the model
(`settle_model.json`): item rows 800 ms, slide class 2000 ms at 60 fps. Reported as measured or as not obtained.

## 4. Verdict rule

GREEN = N1 91 and N2 = N0, A0-A3 GREEN with A3 exercising the settle path, A4 RED, gates unchanged (or moved and recorded),
settled parity GREEN, `lint_rva_bodies.py` NEW = 0 with `0x004325c0` reported as one body. Tracker: `0x004325c0` stays C3
(A0-A3 re-earn it on the new body); `exe_file` becomes `Frontend/MenuAnimTickTwin.cpp` through `re-classify`. No C4 claim.
