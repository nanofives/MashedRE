# D2 statediff wedge — re-measured, and it is not a 1/6 binary wedge

Date: 2026-08-20
Driver: `re/tools/statediff/wedge_rate.py` (new). Data: `verify/wedge_rate_20260820/REPORT.json`.
Config: `scenario_launch.py --hooks all --statediff-drive`, argparse defaults
(`--track 0 --mode 10 --cars 1 --car 0 --hold 20 --fps 60`), n = 18 boots.
Binary: patched `original/MASHED.exe` + the dev `.asi` rebuilt today after the
D1 and Save merges (`0e6e0834`, `218b6598`).

## Why re-measure instead of bisecting

`re/tools/statediff/README.md:80-86` and CHANGELOG 2026-07-31 record the residual
as "~1/6 full-set boots still wedge at phase 2", unbisected, with the stated
method being "majority-vote bisection ~3 boots/verdict". Two problems with
acting on that directly:

1. **The 1/6 figure rests on six trials** (`flake_1..6.msd`: one 0-frame, five
   481–536 frames). 1/6 is consistent with a true failure rate anywhere from
   ~3% to ~64% at 95% confidence. That is not a basis for planning a ~4 h
   bisection whose cost scales with the rate.
2. **A same-day post-fix degenerate capture is not in the tally.**
   `fix_ring_all.msd` (15:38, after the 15:37 ring fix) is 6,688 B = **2 frames**
   — a failure by any reading, excluded because the tally was binary
   healthy-vs-empty. `drive_hooked_all.msd` / `all2.msd` are 5 frames each
   (pre-fix), `diag_all_drive.msd` / `idle_hooked_all.msd` are 0 frames.

Cost was also mis-estimated: a boot is **~29 s**, not the ~20 s + unknown the
notes imply, so n = 18 costs about 10 minutes. Re-measuring was cheaper than
reasoning about whether to.

## Result

| Outcome | Runs | Share |
|---|---:|---:|
| HEALTHY (>= 100 frames) | 13 | 72.2% |
| DEGENERATE (1–99 frames) | 2 | 11.1% |
| EMPTY (0 frames) | 2 | 11.1% |
| NOFILE (no .msd written) | 1 | 5.6% |

**Failure rate 5/18 = 27.8%, Wilson 95% [12.5%, 50.9%].** The recorded ~17%
(1/6) sits at the bottom of that interval. The true rate is materially worse
than documented, and could be as bad as one run in two.

### The binary healthy/wedged split hides the real shape

HEALTHY frame counts, sorted:

```
160, 247, 300, 486, 1084, 1084, 1086, 1088, 1089, 1089, 1092, 1094, 1097
```

Two populations, not one: **9 runs cluster tightly at 1084–1097** (mean 1089,
total spread 13 frames = survived the full 20 s hold), and **4 runs sit at
160–486** (mean 298). Median 1086, min/max spread 6.9x.

Counting a full-length capture as the only real success:

```
FULL       (~1089 frames)   9/18 = 50.0%
TRUNCATED  (160-486)        4/18 = 22.2%
DEGENERATE (5, 89)          2/18 = 11.1%
EMPTY      (0)              2/18 = 11.1%
NOFILE                      1/18 =  5.6%
```

**Only half of boots produce a full-length capture.** This matters for evidence
strength, not just throughput: statediff aligns by `frame_idx` and diffs the
common range, so a GREEN against a 160-frame capture is a far weaker claim than
GREEN against 1089 frames, and nothing in the current protocol records which
one a verdict came from.

### Three distinct failure signatures, currently all called "the wedge"

| Signature | rc | Duration | Frames |
|---|---:|---:|---:|
| EMPTY | 1 | 48.8 s, 48.4 s | 0 |
| DEGENERATE | 0 | 49.5 s / 30.1 s | 5 / 89 |
| NOFILE | 3 | 22.1 s | none |

The EMPTY pair is consistent with the documented phase-2 hang: both ran ~48 s,
i.e. they sat through the launcher's `wait_phase(3, 40, ...)` timeout
(`scenario_launch.py:771`) and exited non-zero. The NOFILE run is a **different
failure** — it exited at 22 s with `rc=3`, before the phase-3 wait could time
out, and never wrote a file at all. That is not the recorded mechanism and has
no entry anywhere.

## What this says about the bisection plan

The distribution favours the README's own alternative hypothesis over the one
the plan is built on. `README.md:82` allows that the residual "may be a
warp-timing race rather than a specific hook". A specific-culprit hook predicts
a roughly bimodal wedge/no-wedge outcome; what is observed is **one tight
cluster of survivors plus a scatter of early terminations at 486, 300, 247, 160,
89, 5, 0** — the signature of a run being killed at a roughly arbitrary point,
not of one hook deterministically wedging phase 2.

Index bisection (`MASHED_HOOK_LO`/`HI`) assumes a specific culprit and, per
`scripts/catch_wedge.py:7-9`, "only ever returns a name, never a mechanism".
Against a 27.8% flaky predicate it also needs materially more than the assumed
~3 boots per verdict. On this evidence a bisection is the wrong next spend.

## [UNCERTAIN] — the gap that must close before concluding

**Whether the TRUNCATED and DEGENERATE runs are failures at all.** The scenario
is `--mode 10` (QuickRace) with `--hold 20`; a capture also stops if the race
legitimately ends — AI wins, the car is destroyed, the round completes. So the
160–486 frame runs may be correct behaviour rather than a defect, and the 5- and
89-frame runs may not share a mechanism with the 0-frame ones.

This n = 18 pass **cannot** separate those, because it discarded the launcher's
stdout. Missing evidence: the phase/verdict line at termination for each
non-full run. `wedge_rate.py` now writes `run_NN.stdout.txt` for every
non-timeout run, so a repeat pass answers it directly. Until then the honest
claim is bounded: **EMPTY 2/18 and NOFILE 1/18 are unambiguous failures (16.7%
combined); the other 4 short and 2 degenerate runs are unclassified.**

Also unverified: whether the rate changed as a result of today's merges. The
`.asi` was rebuilt, but Slice B left the `#else` (.asi) branch of
`GameSaveBuffer.cpp` untouched by construction, and the D1 flag inversion is
read in the exe target, so neither should affect this path. No pre-merge
measurement at this n exists to compare against.

## Second D2 blocker, not recorded in ROADMAP §D2

Separate from the wedge, and arguably the real critical path: with
`MASHED_REAL_PHYSICS=1` the standalone's dynamics are unfaithful.
`re/analysis/WS_A8_REALPHYS_2026-07-01.md` §2 records forward speed unbounded,
saturating `kSafetyInternal = 1500` (`VehiclePhysicsRun.cpp:480`) in ~1.5 s, and
steering producing no yaw change (`car_yaw` frozen at 1.5498 under
`steer=+0.50`). Root cause per `COLLISION_GATE_BRIEF_D1_2026-07.md:56`: the
original's stability comes from a two-body closed loop (PD gain
`_DAT_005ccd6c = 20.0` @ `0x005ccd6c`, 0.06 s lookahead, proxy body integrated
by the vendor solver) that the standalone's single-body reduction loses.
Re-confirmed 2026-07-14. **[UNCERTAIN] no re-measure exists after the B5e
solver-island merge (`021a9f38`, 2026-07-20)**, so whether the port now closes
that loop is unknown. ROADMAP §D2 names only the wedge as the blocker.

Also on record and relevant to sequencing: physics is **5/5 C4** (A3/A4/A5/A6a/A6b,
reached 2026-07-01) and the flag's original exit condition — "stays gated OFF
until A6a/A6b reach C4" — was met **six weeks** before D2 was written.
CHANGELOG 2026-08-15 records that gap as process, not technical.
