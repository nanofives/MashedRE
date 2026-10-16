# RESULT d2close: fix W and fix K default ON, the stand-in deleted, D2 closed

Branch `work/d2close`, rebased onto `race/first-frame-parity` `f76ae153b` (built first on `07526b4ea`, then rebased when
gridyaw + d2lateral merged; every control below was re-run on the rebased tip). Commits `b61e14a49` (1/2, flip) and
`f3163e6a6` (2/2, stand-in deleted), then one docs + evidence commit. Model claude-sonnet-5-5. User decision (Mariano,
2026-10-16): turn the terrain walk on by default and close D2.

| exe | source | base | role |
|---|---|---|---|
| `52e91fd8` | `aa1a1cc18` (pre-rebase 1/2) | `07526b4ea` | flip commit, stand-in and `MASHED_NO_D2BSP` still present: the opt-out chain |
| `d738362e` | `73fbc535e` (pre-rebase 2/2) | `07526b4ea` | stand-in deleted: first full measurement |
| `ce47e706` | `f3163e6a6` (rebased 2/2, tip) | `f76ae153b` | final: every number cited as "the default build" |

## 1. Verdict

All three D2 metrics are inside the unchanged `d81a8df6` bounds on 3/3 runs of the plain default build (no knob), launch
and recovery H1 pass, G-CTRL is byte-identical before and after the stand-in deletion and after the rebase, D3 (b) and
collateral are unchanged. No STOP condition was hit. D2 is closed (ROADMAP.md section D2, `DEFERRED.md` D-11071,
U-9180). Two things are accepted open and filed (section 7): the countdown-hold G-WHEEL-F miss (U-9712) and the walk
compared with the original on Training only.

## 2. What changed (source)

| fix | default | opt-out |
|---|---|---|
| W: RW BSP sphere walk `FUN_00538c80` case 3 (`FUN_00539ec0` + `FUN_00547450` + `FUN_004c3b90` verbatim) + collector `FUN_00468b80` | ON, the ONLY terrain query | none after `f3163e6a6`; `MASHED_NO_D2BSP=1` exists in builds of `b61e14a49` and earlier |
| K: classifier `0x0046cc40` z-first summation (`0x0046ccfd..0x0046ce13`) | ON | `MASHED_NO_D2CLS=1` |
| L: outer chunk loop `0x00470f50..0x00471151` | ON (since d2bsp) | `MASHED_NO_D2CHUNK=1` |

Commit 1/2 flips W (`CollisionWorld.cpp` `D2BspOn()` = `!MASHED_NO_D2BSP`, was `MASHED_D2BSP && !MASHED_NO_D2BSP`) and K
(`CarWorldContacts.cpp` `s_clsOrder` = `!MASHED_NO_D2CLS`, was `MASHED_D2CLS=1`), with the stand-in still compiled so the
opt-out chain can be measured. Commit 2/2 deletes what only the stand-in used:

- `ProduceTerrainBatch` and its 256-entry storage (`ContactProducer.cpp`), `MASHED_D2_BATCHMODE`.
- `Collision::g_worldTris` / `g_worldTriCount` (`WheelContactSolver.cpp`, `ContactSolvers.h`, the two assignments in
  `VehiclePhysics_SetWorld`), `PerfBatchTestCounter` / `f_batchTests` (`phys_prof.log` loses its `batch_tests` column).
- The stand-in half of the `MASHED_BSP_REPLAY` diagnostic (csv is now `seq,nW,entW,clsW`) and `D2BspOn()`.
- `b4_chain_selftest.cpp` (it only drove the stand-in). `contact_selftest.cpp` now fills its batch entry with
  `FillBatchEntry`.
- Kept, because something else reads them: `g_worldTriStore` (ProbeGround, `VehiclePhysicsRun.cpp`), `g_terrainPassCount`
  (the `wcs_cls` diagnostic column, now equal to the entry count).
- A track whose replica fails to build (logged `[d2bsp] collision replica FAILED`) now produces an empty batch. Section 6
  shows 13/13 build.

`rva-lint`: 22 known findings, NEW=0. `asm-lint`: OK, 634 files. Both targets build (`Build OK`), `MASHED_NO_DEPLOY=1`.

## 3. G-CTRL (14400 frames, step / gates / ptrace; `run_arms.py H_*`, private roots)

| exe | arm | knobs | step | gates | ptrace | repeat |
|---|---|---|---|---|---|---|
| `52e91fd8` | `H_DEF` | none (W + K + L) | d3b90ee0 | 80f01f35 | 56d62ae7 | x2 identical |
| `52e91fd8` | `H_NOK` | `MASHED_NO_D2CLS` | d3b90ee0 | 80f01f35 | 56d62ae7 | = `H_DEF` (K moves no G-CTRL hash) |
| `52e91fd8` | `H_NOW` | `MASHED_NO_D2BSP` | 2b4d47bb | 356821c8 | f7a155e1 | = the 2026-10-16 d2bsp default |
| `52e91fd8` | `H_NOWK` | `MASHED_NO_D2BSP MASHED_NO_D2CLS` | 2b4d47bb | 356821c8 | f7a155e1 | x2 identical |
| `52e91fd8` | `H_NOWKL` | the three opt-outs | 716cb2fd | 2a2afda1 | ba90ed88 | x2 identical, = the pre-d2bsp default |
| `d738362e` | `H_DEF` | none, stand-in deleted | d3b90ee0 | 80f01f35 | 56d62ae7 | x2 identical |
| `ce47e706` | `H_DEF` | none, rebased tip | d3b90ee0 | 80f01f35 | 56d62ae7 | x2 identical |

**New default G-CTRL `d3b90ee0 / 80f01f35 / 56d62ae7`** (= d2bsp's `H_W`). Opt-out chain: `MASHED_NO_D2BSP=1` back to
`2b4d47bb / 356821c8 / f7a155e1`, plus `MASHED_NO_D2CHUNK=1` back to `716cb2fd / 2a2afda1 / ba90ed88`. The chain was
measured on `52e91fd8` (pre-rebase flip commit); the tip has no W opt-out. Line counts 18671 / 55433 / 27716 (default),
18956 / 55433 / 27716 (W off), 18664 / 56153 / 28076 (all off). The deletion changed nothing the default build runs:
`d738362e` and `ce47e706` reproduce `52e91fd8`'s default trio.

## 4. D2 (bounds `d81a8df6` unchanged, `score_d2.py`, `a8_launch.py`), each arm alone

Rebased tip `ce47e706`, plain default, `rebased/SCORE_D2_rebased.txt`:

| run | log lines | slip 1500-2000 [0.18855, 0.19635] | slip 2000-2600 [0.24488, 0.25487] | driving-median [1904.70, 1982.44] (n) | launch | H1 |
|---|---|---|---|---|---|---|
| `D2_DEF_1` | 1635 | 0.1890 in | 0.2483 in | 1914.84 in (1425) | L = 0 at 0.19 % | 398/400, 1362.7 |
| `D2_DEF_2` | 1634 | 0.1890 in | 0.2482 in | 1914.84 in (1424) | L = 0 at 0.19 % | 398/400, 1362.7 |
| `D2_DEF_3` | 1633 | 0.1890 in | 0.2482 in | 1913.06 in (1423) | L = 0 at 0.19 % | 398/400, 1362.7 |

Same arms on `d738362e` (pre-rebase, `final/SCORE_D2_final.txt`): slip 1500-2000 0.1890 x3, slip 2000-2600 0.2482 /
0.2480 / 0.2483, driving-median 1913.06 / 1912.04 / 1914.84 (n 1423 / 1421 / 1425), 1631..1635 lines, L = 0, H1 398/400.
Smallest margin on any of the six runs: driving-median 1912.04, +7.34 above its floor. The original reads
`+0xb14` engages at d=15, peak 1832.40, and 398/400 / median 1333.9; the port reads engages at d=15, peak 1835.50.
All six runs are above the 1600-line floor (memory `d2-arms-are-wall-clock-bound`), none re-run. The Training wall-pin
signature (driving n ~54, slips NA) is absent on every run.

## 5. D3 (b), collateral, watch list

- D3 (b) per regime (`score_regimes.py`, `re/tools/b_envelopes.json`): **A 1/3, B 2/3, C 0/3** on `d738362e` and on the
  tip. The three K dumps are byte-identical between those builds and to d2bsp's `B_DEF` (`17d07c90`, `062a3c98`,
  `b8124a63`). No loss.
- Collateral (`coll.py`, A = `o_p2b`, floor `o_p0a`, banded): **39 of 94** paired fields over their floor, on the tip and
  on `d738362e`; d2bsp's `H_BASE` / `H_DEF` / `H_W` read 39 of 94. No change.
- Not measured: `wcs_drift`, airborne drift and the `+0x9e0` distribution (they need `MASHED_D2SINK` arms; none run
  here, none registered). No watch-list hit reported.

## 6. All 13 tracks: replica build and non-empty batches (new, not in d2bsp)

With the stand-in gone, a track whose `COLLI*.BSP` replica fails to build has no terrain at all, and d2bsp compared the
walk with the original on Training only. `tracks_check.py` boots the D2 recipe on each `MASHED_TRACK_SEL` 0..12 for 45 s
with `MASHED_BATCH_DUMP` and reads the replica log line and the player's solver queries. Run on `d738362e` (walk) and,
for contrast, `52e91fd8` with `MASHED_NO_D2BSP=1` (stand-in). The two are separate boots, so the medians are not a
per-query comparison.

| sel | track | replica planes/sectors/trees/materials | walk queries | non-empty | walk median / max | stand-in median / max (queries) |
|---|---|---|---|---|---|---|
| 0 | Arctic | 5/6/5/5 | 3679 | 3679 (100.0 %) | 5 / 5 | 8 / 8 (3671) |
| 1 | Egypt | 3/4/3/5 | 3270 | 3270 (100.0 %) | 2 / 19 | 5 / 21 (3268) |
| 2 | City | 5/6/4/9 | 3277 | 3277 (100.0 %) | 3 / 11 | 4 / 14 (3271) |
| 3 | Forest | 9/10/6/8 | 3476 | 3476 (100.0 %) | 7 / 7 | 12 / 16 (3387) |
| 4 | Highway | 9/10/5/3 | 3647 | 3647 (100.0 %) | 5 / 5 | 8 / 8 (3640) |
| 5 | Neustein | 9/10/6/8 | 3450 | 3450 (100.0 %) | 4 / 10 | 5 / 12 (3298) |
| 6 | Storm | 2/3/3/10 | 3616 | 3616 (100.0 %) | 4 / 4 | 6 / 6 (3613) |
| 7 | SuperG | 3/4/2/6 | 3455 | 3455 (100.0 %) | 3 / 11 | 2 / 20 (3268) |
| 8 | Warzone | 25/26/10/25 | 3632 | 3632 (100.0 %) | 8 / 11 | 12 / 17 (3632) |
| 9 | rouabout | 8/9/5/16 | 3260 | 3260 (100.0 %) | 3 / 8 | 4 / 11 (3260) |
| 10 | sands | 5/6/4/12 | 3597 | 3597 (100.0 %) | 3 / 3 | 7 / 7 (3546) |
| 11 | dump | 4/5/3/5 | 3610 | 3610 (100.0 %) | 11 / 11 | 25 / 25 (3618) |
| 12 | training | 3/4/2/5 | 3299 | 3299 (100.0 %) | 5 / 18 | 8 / 36 (3300) |

**13/13 replicas build ("collision replica FAILED" appears on none), and the walk returns a non-empty batch for 100 % of
the player's solver queries on all 13** (3260..3679 queries per track). This is a build-and-non-empty check. It is not
a comparison with the original: that exists for Training only (G-WALK 4764/4764, d2bsp). The track column is the
`verify/car_bright_fix_20260930` directory name for the same index; only sel 12 = Training is established here (the
D2 recipe), the other index-to-name pairs were not checked. Not re-run on the rebased tip (the rebase pulled in `RoundGrid`, `RwLutTables`, `TrackRenderer` and `VehiclePhysicsRun` changes for opt-in
knobs and no change to `CollisionWorld`, `ContactProducer` or `WheelContactSolver`).

## 7. Accepted open at closure (user accepted the walk despite these)

- **U-9712 (open).** The G-WHEEL-F gate (d2bsp, registered 99 %) is not met: 76.61 % for W alone, 81.42 % (15512/19052)
  with K. The remainder is the 3540 countdown-hold wheel-calls: on frames < 1000 the next solver entry of the original
  holds fv 0, normal (0, 1, 0), state 1 and none equals the classifier output (d2bsp amendment 2 attributes it to the
  per-hold-tick reset `FUN_0046baa0`). Whether the port's own solver entry carries the same reset is not measured; there
  is no port-side capture of the solver-entry wheel fields. U-9712 already was this row (d2bsp filed it), so it was
  updated in place instead of filing a duplicate at the next free id (U-9721). Path to resolution is in the row.
- **Walk vs original on one track.** G-WALK 4764/4764 bit-identical is Training only. Section 6 is the only evidence for
  the other 12.
- **driving-median margin.** +7.34 (pre-rebase) / +8.36 (tip) above the floor, from runs that are wall-clock bound.
- **W has no opt-out.** See section 9.

## 8. Tracker changes (through the `re-classify` rules; no function changed C-level, so `hooks.csv` and `STUBS.md` are untouched)

- `DEFERRED.md` D-11071: struck, RESOLVED text (all four residuals).
- `UNCERTAINTIES.md` U-9180: type struck + RESOLVED with the final numbers; the carrier is named at the fix level (L +23.64,
  walk +28.13..+30.93, d2bsp's arms) and the sub-fix decomposition is stated as not run. U-9181 was already RESOLVED by
  d2bsp (row at the foot of the file). U-9712: Blocks cell and a 2026-10-16 note.
- `re/analysis/CHANGELOG.md`: one line below the marker.
- `ROADMAP.md`: State 2026-10-16 line, and a CLOSED block at the top of the D2 section (PARKED / REOPENED kept as history).
- `re/NEXT_SESSION.md`: START HERE block (D2 closed, what remains for D3).

## 9. User decisions needed

1. **W's opt-out.** The brief asked for an opt-out knob on W and for the stand-in to be deleted. With the stand-in gone,
   an opt-out has nothing to select (an empty batch is not a mode anyone wants), so `f3163e6a6` has none; `b61e14a49` is
   the last commit that has `MASHED_NO_D2BSP=1`. If an empty-batch diagnostic knob is wanted, it is one line in
   `ProduceTerrainBatchBsp`.
2. U-9712 stays the single open row for the hold-tick miss (no duplicate row). Say if a separate row is wanted.

## 10. Follow-ups

- U-9712 resolution path: a port-side dump of the solver-entry wheel fields beside `MASHED_BATCH_DUMP`, compare frames < 1000.
- A second-track `--batch-probe` capture of the original to extend G-WALK beyond Training.
- `MASHED_D2SINK` arms for the unmeasured watch-list items if a later change touches contacts again.

## 11. Evidence (`verify/d2close_20261016/`)

`c1/` chain arms on `52e91fd8`; `final/` pre-rebase tip arms (`d738362e`); `rebased/` tip arms (`ce47e706`); `tracks/`
section 6 (`TRACKS_final.txt`, `TRACKS_standin.txt`, per-track logs). `c1/`, `final/` and `rebased/` each have `RUNLOG.txt` (one provenance line per
run: exe sha8, PID, knobs, sha8 and line count of every output), `SCORE_*.txt`, the `*.motion_diag.log` D2 logs and the
`*.ptrace.log` G-CTRL logs. The 11 MB `*.step.csv` / `*.gates.csv` / track `*.batch.csv` dumps are local only (sha8 and
line counts are in `RUNLOG.txt`; the runner regenerates them). Scripts: `run_arms.py`, `score_all.py`, `coll.py`
(adapted from `verify/d2bsp_20261016/`, same recipes, `D2C_*` env instead of `D2B_*`), `tracks_check.py`. Runs used
private `MASHED_ROOT` copies that contain `original\`; every PID was tracked and only own children were killed.
