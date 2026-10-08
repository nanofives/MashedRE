# RESULT: D-11073 exit-flag cadence (task 1) + clip-handle liveness (task 3)

Date 2026-10-08. Pre-registration: `PREREG_CADENCE.md` (main table before run 1, addendum
before run 2). READ-ONLY on the game: `ReadProcessMemory` poll, `re/tools/orig_rampwatch.py
--cam` (new opt-in column set; default output unchanged). No build, no port change, no C-level.
Process hygiene: zero MASHED running before either run; one new PID adopted each time
(18184, 35344); neither was killed by this tool.

Raw: `o_cadence.csv` / `.log` (run 1, 4,099 samples), `o_cadence_nopress.csv` / `.log`
(run 2, 4,125 samples). Both at 60 Hz, `--track 0 --mode 10 --cars 4 --car 0
--poke-ctrl-slots --hold 60`; run 2 adds `--statediff-out` (scratch .msd, not kept).

## 0. Verdict first

> **The exit flag HOLDS, and its natural release is the camera-entry DISTANCE test, not input
> and not the clip module.** Run 2, with no injected input: flag = 1 on **628/630** sub-state-3
> samples, for clk **0 -> 32,600 = 652 frames** (50 ticks/frame). It clears on the sample where
> entry 0 is **0.0207** from the target. That is clear site (a), `fVar3 < 0.02f`. All eight input
> bytes read 0 on every sub-state-3 sample.
>
> **`FUN_00445aa0` runs every frame of sub-state 3, but its clears are conditional.** It is not an
> immediate cancel, so the hold survives. **The cheap estimate's END condition was wrong,
> though.** In an unattended run, the hold ends only through site (a). Site (a) needs the
> type-0 entry motion (`FUN_00445aa0`'s flag-set branch) and the target copy in
> `FUN_00446520`. Porting the coordinator plus the flag alone would **hold forever** in a
> no-input deterministic run.
>
> **Task 3: the clip handles are ZERO throughout phase 3 on both runs.** `DAT_00657448`,
> `DAT_00639d70` and `DAT_00639d78` all read 0 on **656/656** sub-state-3 samples. So
> `FUN_00405430` returns 0, `FUN_0040d470` is not gated off, and `FUN_00405460` returns 0. The
> camera-path module plays no part in this scenario's phase 3.

## 1. Static chain (Ghidra, pool clone, read-only)

- **Per-frame route in sub-state 3.** `FUN_004111c0` case 3 calls `FUN_004102f0` and then
  `FUN_0040fc00`. `FUN_0040fc00` calls `FUN_0040d470(0)` unconditionally. `FUN_0040d470`
  returns early only when `DAT_0063ba8c==3 && FUN_00405430()!=0`, which is clip-gated. It then
  calls `FUN_00448220`, which reaches `LAB_004483a3` when sub-state is not 6, 7 or -1.
  That label calls `FUN_00446520(&DAT_00897fe0)`. The site at `0x004468e2..0x004468f9` reaches
  `FUN_004464c0([ebp+8])` when sub-mode != 6. `FUN_004464c0` runs `FUN_00445aa0(entry,
  &DAT_00897fe0)` for each entry with `entry+4 == 0`. Entries live at `0x008964c0`, stride
  0xd8, count `DAT_00898994` (writer `FUN_00442600` at `0x004426b7`).
- **`*param_2` IS the exit flag.** `FUN_00448700` pushes `0x897fe0` at `0x00448706`. Both
  `FUN_004430a0(0)` sites are in the `*param_2 != 0` branch and need `entry+0xa8 == 0`:
  - (a) `fVar3 < _DAT_005ce18c`, which is **0.02f** (`0ad7a33c`). `fVar3` is the horizontal
    distance from entry `+0x3c/+0x44` to `param_2[1]/[3]`, the latter offset by
    `min(d,4)*_DAT_005ccac8` (0.3333).
  - (b) any byte `0x007f1042 + k*0x4c != 0`, k=0..7. That byte is per-player input block
    +0x0a, written by the cook `FUN_00496530` at `0x004965b7`.
- **The target `param_2[1..3]` has a writer the reference count cannot see.**
  `FUN_00446520` at `0x004481c9..0x004481e2` copies `[ebp+8]+0x40..+0x48` into
  `[ebp+8]+4..+0xc`, with `[ebp+8] = &DAT_00897fe0`. Ghidra reports **0 references** to
  `0x00897fe4..ec` (computed base). This is the same addressing-mode trap as the
  standing caution.
- **Order inside `FUN_004102f0`** (`0x004102fc..0x00410313`): `FUN_00448700(0,0)` runs 100
  dispatch ticks with the flag still 0. Then `FUN_004430a0(1)`, `FUN_0040e590`,
  `FUN_0040d470(1)`.

## 2. Gates, as registered

| gate | verdict | evidence |
|---|---|---|
| G-ARM | PASS | 1 new PID per run; sub-state 3 on 26 (run 1) / 630 (run 2) samples; `flag_fe0` readable on every clocked sample |
| G-PATH | PASS | sub-state 3: `n994 = 7` (run 2: 629/630; one sample 0 on the 2->3 edge), `e0_type = 0` on all, `f1a50 = 0` on all, `rule_0fd0 = 0` |
| G-CLIP | **ZERO-ALL** | `h657448`, `d70`, `d78` = 0 on 26/26 + 630/630 sub-state-3 samples. Blind to a write-then-clear inside one frame |
| G-HOLD | **PASS (flag holds)** | run 1: 25/26 = 96%; run 2: 628/630 = 99.7% |
| G-CAUSE (run 1) | **NOT the distance branch** | at the 1->0 sample `in0 = 255` and entry-0 distance = 29.36. Run 1 was cut short by the harness, see §3 |
| G-NOPRESS (run 2) | PASS | `in0..in7 = 0` on 630/630 sub-state-3 samples |
| G-NATURAL (run 2) | **site (a)** | 652 frames; at the 1->0 sample all inputs 0, distance 0.0207 (0.0280 and 0.0368 on the two samples before). Distance falls monotonically, 29.4 -> 0.02, at these sampled points: 684: 27.49, 810: 16.41, 936: 6.23, 1062: 2.14, 1188: 0.63 |

## 3. Harness finding: run 1's phase 3 was truncated by the harness

`scenario_launch.py:3347` calls `E.press(4, 250)` every loop iteration unless `--statediff-out`
is set. The comment says "to skip the start intro". In run 1, `in0` follows a ~14-on / ~20-off
sample square wave from sample 612 onward. The first edge lands on the flag-clear sample, so
run 1's 25-frame phase 3 is **harness-truncated**. **Any phase-3 measurement from a
non-statediff `scenario_launch` run is truncated the same way.**

## 4. What this changes in D-11073's estimate

- **Needed for the HOLD:** the `DAT_005f29b8 = 100000` seed (FUN_004111c0 case 1), the
  172 B `FUN_004102f0` coordinator, and the flag `FUN_004430a0`/`FUN_004430b0`. This part is
  unchanged.
- **Needed for the hold to END without input:** clear site (a). That means
  `FUN_004464c0` dispatch (C2, hook-only file `Util/CameraEntryDispatch.cpp`, which calls the
  original's `0x00445aa0`), `FUN_00445aa0`'s flag-set branch (2,579 B, C2), the type-0 entry
  set-up (`FUN_00442600`, writes `DAT_00898994`), and the target copy at
  `0x004481c9` inside the sub-mode != 6 path of `FUN_00446520`. The port's
  `Race/RaceCamera.cpp` is labelled the "0x00446520 race branch". **[UNCERTAIN]** whether it
  carries that copy and the entry state. That is unread, and it is a source survey.
- **NOT needed:** the camera-path clip module `0x004053d0..0x00405540`. Its handles are 0
  for the whole of phase 3 in this scenario.
- **Length to reproduce:** 652 frames in this scenario, set by the entry-0 approach dynamics
  (29.4 -> 0.02). Not measured: other tracks, and the six other entries (only entry 0 is
  sampled).

## 5. Residuals, named

- Entries 1..6 are not sampled. Site (a) can fire from any type-0 entry. Run 2 attributes the
  clear to site (a) because site (b) is entry-independent and read 0, not specifically to
  entry 0.
- The 60 Hz poll is non-atomic. A 1-frame input pulse between polls is not excluded. No
  injected input existed in run 2.
- `CALLWISE2`'s original anchor `i0 = 683` calls and this 652-frame hold are not the same unit.
  No claim is made that they match.

## 6. Run 3: entry-type census (PREREG addendum 2)

Raw: `o_cadence_types.csv` (2,384 samples, 60 Hz, `--statediff-out`, `--hold 30`, PID 19144, not
killed by this tool). `--cam` now samples entries 0..6.

| gate | verdict | evidence |
|---|---|---|
| G-NOPRESS3 | PASS | `in0..in7` = 0 on 626/626 sub-state-3 samples |
| G-COUNT | `n994 = 7` on 626/626 | all seven entries are dispatched |
| G-TYPES | **types 1 AND 2 are LIVE** | constant across all 626 sub-state-3 samples (and all 1,765 rows where read): e0 = **0**, e1/e2/e3 = **1**, e4/e5 = **2**, e6 = **3** (raw; no `FUN_004464c0` arm dispatches 3) |
| G-CLEARER | **entry 0** | at the 1->0 sample (1244) entry 0 is 0.0279 from the raw target; entries 1..6 are 4.45-31.30 away. Entry 0 is the ONLY type-0 entry, so it is the only entry that can reach `FUN_00445aa0` |

Replication: phase 3 = **625** flag-1 samples, clk 0 -> 32,550 (run 2: 628, 0 -> 32,600). One frame
apart.

**What this means for D-11073.** `FUN_004430a0` has exactly two callers (`FUN_004102f0` sets,
`FUN_00445aa0` clears), so the **hold and its end depend only on entry 0 and type 0**. The type-1
(`FUN_00441d40`) and type-2 (`FUN_00442440`) arms run on every phase-3 frame. They are needed for
the fly-in's **visual** parity, not for its length or its release. Static basis: `FUN_00445aa0`
reads only its own entry, `param_2` (the `0x00897fe0` struct), the input bytes, and
`DAT_007f1008` / `DAT_007f101c`. **Checked, not uncertain:** neither arm writes the `0x00897fe0` struct. `FUN_00441d40` (1,730 B) touches `param_2` only twice, both reads: it passes it to `FUN_00441c80` (line 47 of the decomp), which only reads `+0x1c` / `+0x20`, and it reads `+0x20` at line 190. `FUN_00442440` (391 B) never references its `param_2`. The rest of both arms' callees are RW math on entry-local pointers, plus `FUN_00408a50` / `FUN_0040dc90` / `FUN_0046d4a0`, and none of them is passed the struct.
