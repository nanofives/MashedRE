# PRE-REGISTRATION — D2 attempt 19, STEP 1: the PORT's `T_post` split, per PRODUCER and per SUBSTEP

Committed **before the run**. Base `025e1642`. Branch `race/first-frame-parity`.

U-9178 names the blocker: at `d = 222..250` the port's `T_post` is **-27.50393** against the
original's **-0.00464** (n = 29 each, median speed O 790.9 / P 633.2,
`verify/d2_budget_20261002/RESULT.md`), and **~90 %** of the port's side has no identified
producer. Attempt 18 showed clamp #6 accounts for 7.2038e-03 of a measured 7.4031e-02 through
the port's own measured `l_60` = 676.82. This step measures the rest **directly**, at every
producer's own phase, per substep.

`d` = frames from release. PORT release **R = 1** (`a18_budget.py`'s own rule, reused, not
restated). ORIG release is re-derived from the capture's own `+0xbf8` marker — attempt 18's
`orig_lb18.msd` gave **883**, and that capture is reused unchanged.

---

## 1 What `T_post` is, and therefore what the split must telescope to

From `a18_budget.py`'s docstring, reused verbatim:

```
s_mid(f)  = +0x9e4(f)          written at 0x004686cc, after W1, BEFORE clamp #6
s_post(f) = |+0x9b0..b8|(f)    the render-tick snapshot (port: motion_diag.log's vel=[..])
T_post    = s_post(f) - s_mid(f)
```

So the interval is `[the +0x9e4 store, the render tick]`. On the PORT, with
`MASHED_A8_A4_FIRST` at its default 1 (`VehiclePhysicsRun.cpp:845-851`), the code in that
interval is, **in execution order**:

| # | producer | port file:line | original RVA |
|---|---|---|---|
| 1 | grip-clamp #6 (A6a tail) | `Integrate2.cpp:665-757` | `0x004687f0..0x0046897b` |
| 2 | A6b `Vehicle_AeroStabilize` | `VehicleControl.cpp:276` | `0x00468980` |
| 3 | A4's tail parked damp, gate `+0x9f0 == 2` | `VehicleControl.cpp:278-282` | `0x00470948` |
| 4 | the substep loop, **N** iterations | `VehiclePhysicsRun.cpp:913-1067` | `0x00471106..0x00471141` |
| 4a | — position + orientation | `:950-978` | `0x0046e9e0` |
| 4b | — `SolveWheelContacts` | `:999` | `0x0046f6c0` |
| 4c | — fixup, bounce add | `ContactFixup.cpp:261-265` | `0x0046f52c` |
| 4d | — fixup, last-contact damp | `ContactFixup.cpp:307-315` | `0x0046f5ba`/`0x0046f5c0` |
| 4e | — fixup, all-grounded slide term | `ContactFixup.cpp:326-331` | `0x0046f5f3` |
| 5 | anything else between A4's exit and the render tick | — | — |

**RVA CORRECTION, recorded before the run.** Attempt 18's standing fact, and
`NEXT_SESSION.md`, cite **`0x00469ad4 mov ebx,2`** as the original's fixed substep count.
That citation is **WRONG** and is corrected here. `0x00469ad4` is `0x34` bytes inside
`FUN_00469aa0`, the contact-history scan, and `D2_REOPEN_2026-09-29.md` §19.2 already reads it
correctly as *"`0x00469aa0`'s history-shift loop runs 2 iterations"*; §20.9 then re-used the
same RVA for *"the frame contains two substeps"*. Both cannot be true of one `mov`.

Disassembled this session from `original/MASHED.exe.unpatched`
(`py -3.12 re/tools/disasm_fn.py`), **the original's substep loop is
`0x00471106..0x00471141`, inside the dispatcher `FUN_00470c70`**:

```
00471106  mov  edi,[esp+0x10]     ; rem = the frame chunk, an INTEGER ms count
0047110a  test edi,edi
0047110c  jbe  0x471143           ; rem <= 0 -> no substep at all
00471110  cmp  edi,0x19           ; LOOP TOP: 25
00471113  mov  esi,edi
00471115  jbe  0x47111c
00471117  mov  esi,0x19           ; sub = min(rem, 25)
0047111c  test esi,esi
00471122  mov  [esp+0x20],esi
00471126  fild [esp+0x20]         ; (float)sub   <- INTEGER-sourced, no residue
0047112b  jge  0x471133
0047112d  fadd [0x5cc94c]         ; unsigned fixup, sub < 0 only
00471134  fstp [esp]              ; arg0 = (float)sub
00471137  call 0x4709a0           ; THE SUBSTEP
0047113f  sub  edi,esi            ; rem -= sub
00471141  jne  0x471110           ; while (rem != 0)
```

and the chunk that feeds it is `chunk = min(remaining, 0x32)`, also an integer
(`0x00470f50..0x00470f61`), with the per-frame budget pinned to `0x32` = 50
(`VehiclePhysicsRun.cpp:739-746`'s already-recorded decode of `DAT_007f1000`). `50 -> 25, 25`
is **exactly 2** substeps, by integer arithmetic.

`FUN_004709a0`'s own `0x00470ab0 cmp ebp,2` is the **retry** cap, not a time loop — the port
already models it as `for (pass = 0; pass < 2; ++pass)` at `VehiclePhysicsRun.cpp:932`. There
is **no** `0x19` chunk inside `FUN_004709a0`; `VehiclePhysicsRun.cpp:119`'s comment
`kMaxSubstep = 25; // 0x19 (FUN_004709a0 inner chunk)` misattributes the constant's home. The
constant 25 is right; its RVA is `0x00471110`/`0x00471117`.

This correction does **not** disturb attempt 18's live measurement
(`4666/2333 = 2.0000` substeps per frame on the original, gate CV) — that is a count of hits
on `0x004709a0`, independent of which `mov` sets the bound.

---

## 2 The instrument: `MASHED_D2SINK`, default-OFF, labelled DIAGNOSTIC

One new TU, `mashedmod/src/mashed_re/Vehicle/D2SinkProbe.cpp` (+ `.h`), added to
`mashedmod/exe_sources.rsp` only — the four TUs that hold this interval
(`VehiclePhysicsRun.cpp`, `VehicleControl.cpp`, `Integrate2.cpp`, `ContactFixup.cpp`) are
**exe-only**; none appears in `asi_sources.rsp`, checked this session. So there is no `.asi`
copy of any of this and no dual-copy drift to introduce.

`MASHED_D2SINK=<absolute path>` arms it. Unset -> every call site is a single `if (g_on)`
test on a `static const bool` and nothing is written. **Slot 0 only.** Line cap 400 000.

Each `Mark` line carries: `f=` frame ordinal, `tag=`, `sub=` substep index (`-1` outside the
loop), `pass=`, `v=(x,y,z)`, `mag=` `|v|`, `r9e4=`, `r9e0=`, `r9f0=`, `r9ec=`, `key0=`
(`+0x4ac`), `chunk=`, `rem=`, `fx=` fixups so far this frame.

Tags, in order, one per site in §1's table: `w1`, `a6a_out`, `a6b_out`, `a4_out`, then per
substep `sub_top`, `sub_orient`, `sub_wheel`, `fx_bounce`, `fx_damp`, `fx_slide`, `sub_end`,
then `snap` emitted from the **same statement block** as `motion_diag.log`'s line
(`VehiclePhysicsRun.cpp:1199-1203`) so `snap` and `s_post` are the same phase **by
construction**, not by argument.

### 2.1 The split, defined before the run

Per frame, with `m(tag)` = the `mag=` of that tag:

```
D_clamp6 = m(a6a_out) - m(w1)
D_a6b    = m(a6b_out) - m(a6a_out)
D_damp   = m(a4_out)  - m(a6b_out)
D_gap    = m(sub_top,0) - m(a4_out)
D_sub[s] = m(sub_end,s) - m(sub_top,s)          for each substep s = 0..N-1
   D_orient[s] = m(sub_orient,s) - m(sub_top,s)
   D_wheel[s]  = m(sub_wheel,s)  - m(sub_orient,s)
   D_fixup[s]  = m(sub_end,s)    - m(sub_wheel,s)     split further by fx_* when present
D_tail   = m(snap) - m(sub_end,N-1)
```

Reported as medians over the window, plus each one's share of the median `T_post`.

### 2.2 Known-answer checks — BLOCKING

| id | asks | bar |
|---|---|---|
| **KA1** | `m(w1) == r9e4(w1)` as an exact float compare (same store, same statement) | **100.00 %** of frames |
| **KA2** | the telescoping sum of every `D_*` above equals `m(snap) - m(w1)` | `<= 1e-5` relative on **>= 99 %** of frames |
| **KA3** | `m(snap)` equals the same frame's `motion_diag.log` `\|vel=[..]\|` | `<= 1e-6` relative on **>= 99 %** of matched frames |
| **CV** | tags `w1`, `a6a_out`, `a6b_out`, `a4_out`, `snap` appear **exactly once** per frame; every `sub_top` has a matching `sub_end` or a retry `continue`; substep count per frame reported as a histogram | 0 frames missing any of the five; 0 orphan `sub_top` |
| **CH** | **channel control.** An UNARMED run (no `MASHED_D2SINK`) writes no sink file, and its `motion_diag.log` is **byte-identical** to the armed run's | both must hold |

**If CH fails the probe is not inert and the split is NOT readable — STOP and report.**
**If KA2 fails the split does not account for the interval — STOP and report.**

### 2.3 Regime gate — EV

`d = 222..250`, **n >= 25** matching frames; `gnd == 4.0` on **>= 90 %**; `|ctrl_xz| > 0` on
**>= 90 %**; median speed reported and required within **25 %** of attempt 18's port figure
for the same window (633.2 from `budget.csv`). Median frame index reported per §26.10. If EV
fails, the window is off-regime and no row is read.

---

## 3 The decision rule — registered before the run

Let `Tp` = the median port `T_post` over `d = 222..250` from `a18_budget.py` on the **same
run** (not attempt 18's number), and let each producer's share be
`median(D_x) / Tp`.

1. **CARRIER = the producer with the largest `|median(D_x)|`**, and it is only called the
   carrier if its share is **>= 50 %**.
2. **SUBSTEP-BUDGET TEST.** Let `S2plus = sum over s >= 2 of median(D_sub[s])` — the substeps
   the original does not run. If `|S2plus| / |Tp| >= 50 %`, the carrier is **the substep
   budget (U-9160)** and STEP 2 ports `0x00471106..0x00471141` faithfully.
3. If rule 1 names a single producer at **>= 50 %** and rule 2 does **not** fire, STEP 2
   confirms that producer live on the original with a pre-registered test, then fixes **that
   body at its RVA**.
4. **If neither rule fires** (no producer at >= 50 % and `S2plus` under 50 %): **report the
   split and STOP.** No speculative fix. No knob, no clamp, no fitted constant.

Also reported, whatever fires: the per-substep fixup **count** on the port against the
original's measured **zero** velocity writes across all three substep members (§22.2:
2945/2945, 2945/2945, 2932/2932), so "the port's substeps write velocity where the original's
do not" is scored rather than assumed.

---

## 4 The original side

**Reused unchanged**: `verify/d2_budget_20261002/orig_lb18.msd` (+ `.latbracket.csv`), release
**883**. No new original-side capture, no new original-side probe — every channel this step
needs on the original is already in that capture (`T_post` total, `+0x9f0 == 0` 29/29,
`+0x9ec == 0` 29/29, `+0x9e0 == 4` 2333/2333, substeps 4666/2333). **Entry hooks only** if
that changes, and it would be re-registered first.

The original's `T_post` is **-0.00464** at `d = 222..250` and is quoted as a **total**, not
decomposed. §3.1's 51x phase bias is an open `[UNCERTAIN]` on the original and this step does
not close it.

---

## 5 What this step does NOT do

- No `mashedmod/src` change beyond the probe. `NEW = 0` for behaviour: every edit is inside
  `if (g_on)` or is the probe TU itself.
- No tracker mutation. No C-level change.
- No knob that alters ported behaviour. `MASHED_D2SINK` only writes a file.
- AI slots 1+ (`VehiclePhysicsRun.cpp:702`) untouched — D3.

## 6 Run plan

```
# armed
py -3.12 re/tools/statediff/a8_run_port.py verify/d2_sink_20261002/p_armed 90 \
    MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0 \
    MASHED_TITLE=d2-a19-armed MASHED_D2SINK=<abs>/verify/d2_sink_20261002/p_armed/sink.log
# unarmed control (CH)
py -3.12 re/tools/statediff/a8_run_port.py verify/d2_sink_20261002/p_ctrl 90 \
    MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0 \
    MASHED_TITLE=d2-a19-ctrl
```

Muted (`MASHED_MUTE=1` is in the recipe), `MASHED_WIN_POS=primary-bl` (in the recipe),
`MASHED_TITLE` set on both, PIDs tracked and only those killed by `a8_run_port.py`.
Reducer: `re/tools/statediff/a19_split.py`, written before the run and committed with this
file.
