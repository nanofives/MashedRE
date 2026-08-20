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

## Second pass (n=18, stdout retained) — the truncated runs ARE failures, and the classifier was wrong

`verify/wedge_rate2_20260820/`. Same config, stdout kept per run. Result:
**USABLE 7, FROZEN 5, EMPTY 6 — failures 11/18 = 61.1%, Wilson 95% [38.6%, 79.7%]**.

The question this pass was run to settle — are the short captures legitimate
race endings? — is answered **no**, and the discriminator is not frame count at
all. It is the number of **distinct payloads** in the capture:

| Run class | n | Frames | Distinct payloads |
|---|---:|---|---|
| USABLE | 7 | 1084–1100 | **268–271** |
| FROZEN | 5 | 8, 8, 236, 446, 735 | **exactly 4** |
| EMPTY | 6 | 0 | 0 |

Categorical, with **zero overlap**. A FROZEN run reaches phase 3, survives to
the `+18s` tick, exits cleanly with `rc=0`, and reports a healthy-looking frame
count — while the captured vehicle state takes only four distinct values for the
whole run. Frame count cannot see this: run 02 captured **735 frames with 4
distinct payloads**. The EMPTY six are the documented mechanism, all six showing
`TIMEOUT waiting for race running (phase 3) (last phase=2)` at ~48 s.

### This invalidates the "5/6 healthy" basis of the 1/6 figure

Re-scoring the archived captures with the same measure:

```
flake_2.msd   532 frames    4 distinct      flake_5.msd   536 frames    4 distinct
flake_3.msd   481 frames    4 distinct      flake_6.msd   536 frames    4 distinct
flake_4.msd   519 frames    4 distinct
fix_ring_alone.msd  533 frames    4 distinct
```

**All five captures that constitute the "5/6 boots healthy" claim have exactly 4
distinct payloads.** By the criterion established here they are FROZEN, not
healthy — so the 1/6 figure does not measure what it was taken to measure, and
the full-set configuration produced **zero** usable captures that day.

The scenario the flake runs used is not recorded, so this is checked against
both baselines and holds either way:

| Capture | Frames | Distinct | distinct/frame |
|---|---:|---:|---:|
| `drive_stock_a.msd` (drive baseline) | 2731 | 1798 | 0.658 |
| `run_01` today (drive, usable) | 1100 | 268 | 0.244 |
| `stock_a.msd` (**idle** baseline) | 1249 | 34 | 0.027 |
| `flake_2..6` | 481–536 | 4 | **0.008** |

4 distinct is far below even the *idle* stock baseline of 34, so the flake
captures are degenerate whichever scenario they used.

**`fix_ring_alone.msd` is also frozen** (533 frames, 4 distinct). That is the
capture cited as verifying the U-6701 ring fix ("culprit-alone boot healthy,
533 frames"). The fix itself is separately supported by the decompiled
ABI defect, but its runtime verification is weaker than recorded.

Two archived pairs are **not** affected: `stock_a/b.msd` at 34 distinct are the
idle noise-floor pair, where low state change is the expected condition and is
the point of the measurement; `drive_stock_a/b.msd` (1798/1861 distinct) and
`drive_hooked_phys.msd` (1282) are richly evolving.

### The real risk this exposes

A FROZEN capture passes every check the protocol had — non-empty, plausible
frame count — and would diff **GREEN against another FROZEN capture**, because
neither side has state to disagree about. Nothing in `statediff.py` or the
README requires a minimum state-evolution count. So the harness can currently
manufacture false GREENs, and a GREEN is only as strong as the distinctness of
the two captures behind it.

`wedge_rate.py` now computes distinct payloads directly from the `.msd`
(`msd_distinct`) and classifies FROZEN on it, defaulting to `--min-distinct 100`.

### Pooled rate

Pass 1 (5/18, frame-count classifier only) and pass 2 (11/18, full classifier)
are not measuring the same predicate, so they are not averaged as a rate. Pass 1
re-scored by frame-count bands gives 9/18 with a full-length capture; pass 2
gives 7/18 usable. Pooling only the usable/unusable split: **20/36 = 55.6%
failures, Wilson 95% [39.6%, 70.5%]**. Against the recorded ~17%, the
conservative reading is that **more than half of full-set boots do not produce a
usable capture.**

## CORRECTION (same day) — "FROZEN" was the wrong mechanism, and the car does move

Chasing the `vel=[0,0,0]` observation overturned my own framing above. Both
corrections are recorded rather than edited away.

### 1. The car moves. `vel=[0,0,0]` was a sampling artifact.

Velocity lives at record `+0x9b0/+0x9b4/+0x9b8` (`scenario_launch.py:569`). In a
usable capture it is **not** static:

| Field | Distinct | Range |
|---|---:|---|
| `vel.x` `+0x9b0` | 203 | −55.26 … +9.12 |
| `vel.y` `+0x9b4` | 28 | 0 … +160.38 |
| `vel.z` `+0x9b8` | 203 | −4054.74 … 0 |
| `yawrate` `+0x9c0` | 188 | −0.0058 … +0.0561 |
| `fwd.z` `+0x9dc` | 112 | 0 … −1.0000 |

Motion (`|vel.z| > 1`) begins at **frame 898** (hooked, today) and **897**
(stock, 2026-07-31). The launcher prints status at +4 s / +9 s / +13 s / +18 s,
and a usable run captures ~1097 frames ≈ 18.3 s, so nearly every printed sample
lands in the stationary pre-countdown phase. The console line is misleading, not
the data.

The large magnitudes are also **not** a hook defect: stock `drive_stock_a.msd`
reaches `|vel.z|` 4341 with 992 frames over 200, first at frame 918. Hooked
today peaks at 4054. I checked stock before calling these absurd.

### 2. The short runs are truncations, not state freezes

The claim above that FROZEN runs have "static state" is **wrong**. Slicing the
known-good 1097-frame capture down to the short runs' lengths:

```
first 236 frames -> 4 distinct      first 783 frames (pre-anchor) ->   5 distinct
first 446 frames -> 4 distinct      frames 783..1097 (the window) -> 263 distinct
first 735 frames -> 4 distinct      whole capture                 -> 268 distinct
```

**The stationary pre-countdown phase genuinely has 4–5 distinct states.** So
"4 distinct" is the *correct* value for a capture that ended early, not evidence
that anything froze. `distinct` cannot distinguish a truncated healthy run from
a defective one, and using it as a usability test — as the version of
`wedge_rate.py` committed in `a1861251` did — was a mistake.

### 3. The right test is whether the capture reaches the countdown anchor

`NOISE_MASK.md:33-45` anchors the drive comparison on `+0xBF4` going non-zero
(countdown start) and declares verdicts valid only inside `--until 314`. So
usability = does the capture contain `[anchor, anchor+314]`. Re-scored:

| Capture | Frames | Anchor | Window | Verdict |
|---|---:|---:|---:|---|
| `wedge_probe_00` (today) | 1097 | 783 | 314 | usable |
| `drive_stock_a` | 2731 | 782 | 314 | usable |
| `drive_hooked_phys` | 2099 | 771 | 314 | usable |
| `flake_2` … `flake_6` | 481–536 | **none** | — | **zero comparable frames** |
| `fix_ring_alone` | 533 | **none** | — | **zero comparable frames** |

The anchor is reproducible to within one frame (783 vs 782) across 20 days and
different builds, so this is a stable test, not a threshold guess.

**The conclusion about the 1/6 basis survives, and is stronger for the right
reason.** `flake_2..6` and `fix_ring_alone` do not merely have low
distinctness — they **never reach the countdown at all**, so they contain zero
frames inside the only window in which a drive verdict is defined. The "5/6
boots healthy" tally counted captures that cannot support a drive A/B claim.

Note also that a usable run's ~1097 frames only just covers `anchor+314` (783 +
314 = 1097). Runs at 1084–1095 frames cover 301–312 of the 314, so most
"usable" captures are a few frames short of the full documented window.
`wedge_rate.py` now classifies on the anchor window (`msd_anchor`,
`--min-window`, default 200) and `msd_distinct` is demoted to a descriptive
statistic with the trap documented in its docstring.

### What this does NOT change

The failure counts stand: EMPTY 6/18 in pass 2 (all six with
`TIMEOUT waiting for race running (phase 3) (last phase=2)`), and the short runs
are still unusable for statediff. What changes is the mechanism label — early
termination, not state freeze — and therefore the direction of any follow-up:
the question is why a run ends before frame ~783, not why state stops evolving.

## Control arm: the phase-2 hang is caused by the harness, not the hooks

`verify/wedge_nodrive_20260820/` — n=12, `--hooks all --no-drive`, i.e. identical
hook set with the **cook injector not armed**.

`--statediff-drive` does one thing beyond forcing input: it
`Interceptor.attach`es to `COOK_RVA = 0x00496530` (`scenario_launch.py:93-104`),
and the argparse help says it arms **"BEFORE the phase poke"**
(`scenario_launch.py:654-658`) — so the instrumentation is live during phase 2
(track load + car spawn), which is exactly where the documented wedge occurs.
`0x00496530` runs per player per frame, and today's U-3558 watchpoint data
showed its inner `0x00496568` firing 38 times in 40 hits, i.e. hot. CLAUDE.md's
standing rule is that Frida `Interceptor` on hot paths destabilises Mashed.

| Measure | with `--drive` | without | Fisher 1-tailed |
|---|---:|---:|---:|
| Phase-2 hangs (EMPTY) | **6/18 = 33.3%** | **0/12 = 0%** | **p = 0.031** |
| Total failures | 11/18 = 61.1% | 2/12 = 16.7% | — |

**Zero phase-2 hangs in 12 boots without the injector**, against 6 in 18 with
it. The idle runs are also strikingly consistent: 10 of 12 gave 1254–1256
frames with 34 distinct payloads — a two-frame spread — and that **reproduces
the archived idle baseline exactly** (`stock_a.msd` 1249 frames / 34 distinct,
`stock_b.msd` 1255 / 34, both 2026-07-31), 20 days later with the full hook set.
The hooks are not breaking the idle path.

So the "residual wedge" that has blocked D2 since 2026-07-31 is substantially an
artifact of the measuring apparatus, not a defect in the ported hooks. The
planned index bisection would have been hunting a culprit hook that does not
exist.

### Caveat — this is not a fully controlled comparison

Dropping `--statediff-drive` removes **two** things at once: the hot-path
`Interceptor`, and the race actually starting. So this pins the phase-2 hang on
the drive configuration but does **not** separate "instrumentation overhead"
from "the race starting at all" as the mechanism. [UNCERTAIN]

The clean follow-up isolates them: keep `--statediff-drive` but (a) arm the
injector with a no-op `onLeave`, or (b) arm it **after** the phase poke rather
than before. Either keeps the race and drops the phase-2 instrumentation. If
the hangs stay away, overhead is confirmed; if they return, the race start is
implicated instead.

### A residual remains, and it is small

2 of 12 idle runs still failed — one 164-frame run and one `NOFILE` at `rc=3`,
21.0 s (the same third signature seen in pass 1). So a failure mode independent
of the injector does exist, at roughly 17% rather than 61%. That is the real
residual, and it is a different and much smaller problem than the one on record.

### Classifier correction

The raw run printed `failure rate 12/12 = 100.0%`, which was **my classifier
misfiring, not a result**. The anchor test is drive-specific: with no
acceleration the countdown witness `+0xBF4` never fires, so `anchor=None` for
every healthy idle run. `wedge_rate.py` now applies the anchor test only when
`--drive` is set and judges idle runs on frame count (`--idle-min-frames`,
default 1000). Re-scored: **USABLE 10, SHORT 1, NOFILE 1**.

## Isolation arm — dose-response, but the split is NOT resolved

`verify/wedge_noopcook_20260820/` — n=12, new `--statediff-noop-cook`
(`scenario_launch.py`): the **same** `Interceptor.attach` at `0x00496530`, armed
at the **same moment** (before the phase poke), with an **empty callback** and
**no forced input**. Idle plus instrumentation, nothing else.

| Arm | Phase-2 hangs | Render collapse | Total | Rate |
|---|---:|---:|---:|---:|
| no-drive (no attach, no drive) | 0/12 | 1/12 | 1/12 | **8.3%** |
| noop-cook (attach only) | 2/12 | 2/12 | 4/12 | **33.3%** |
| drive (attach + drive) | 6/18 | 5/18 | 11/18 | **61.1%** |

Monotone across all three arms, on both failure families. But the pairwise tests
do not support declaring the mechanism:

```
phase-2 hangs   drive vs no-drive   p = 0.031   *
                noop  vs no-drive   p = 0.239   ns
                drive vs noop       p = 0.282   ns
total failures  drive vs no-drive   p = 0.020   *
                noop  vs no-drive   p = 0.320   ns
```

**Only the extreme comparison is significant.** Attachment alone is *not* proven
to cause the hang: 2/12 against 0/12 is p = 0.24. The first no-op boot did
reproduce a phase-2 hang immediately, which is what made this look decisive at
n=1, and the batch shows why n=1 was not.

So the honest state is: the full drive configuration is significantly worse than
no drive (that stands, p = 0.031/0.020), and the ordering across three arms is
consistent with *both* instrumentation and the race start contributing — but
each individual step is underpowered at n=12. Separating them properly needs
roughly n≈40–50 per arm to detect a ~17-point difference at 80% power, i.e.
about 45 minutes of boots per arm at ~31 s each. **That has not been paid for,
and the split should not be asserted until it is.**

### The decision does not actually depend on resolving the split

Both candidate mechanisms point at the same fix: **do not instrument
`0x00496530` during phase 2.** `--statediff-drive` arms before the phase poke
only so that frame 0 lines up with the first phase-3 tick
(`scenario_launch.py:760`). Arming *after* phase 3 is reached keeps the drive
and removes the instrumentation from track load entirely, which avoids both
candidate mechanisms without needing to know which dominates.

That is the experiment to run next (variant B): `--statediff-drive` with the
cook armed post-phase-3. If it restores the ~8% baseline rate **while keeping a
usable driving capture**, the harness fix is proven end-to-end and D2's "wedge
rate zero" gate becomes a property of how statediff is invoked. The cost is that
frame 0 no longer coincides with the phase-3 anchor, so the alignment would move
to the `+0xBF4` countdown witness — which is already the documented drive anchor
and is deterministic to one frame, so this is likely free.

## [UNCERTAIN] — the gap that must close before concluding

RESOLVED by the second pass above — the short runs are failures (frozen state),
not legitimate race endings. What remains open:

**Whether FROZEN and EMPTY share one mechanism.** They present differently:
EMPTY never leaves phase 2 and times out at ~48 s with `rc=1`; FROZEN reaches
phase 3, runs the full hold, exits `rc=0`, and simply never evolves the vehicle
record. Whether one race produces both outcomes depending on when it lands, or
these are two defects, is not established. Missing evidence: a writer-level
trace of the vehicle record on a FROZEN run — is nothing writing it, or is the
capture hook (`FUN_004c1be0`) not firing?

**Why `vel=[0, 0, 0]` on every run, including USABLE ones.** The launcher
reports zero velocity at every tick in all 18 runs even with `--statediff-drive`
(full accel). So the car may not be moving in any run, and the 268-distinct
"usable" captures may be evolving something other than motion. This weakens the
drive scenario as a physics-evidence vehicle and is not noted anywhere.

**Whether the rate changed with today's merges.** The `.asi` was rebuilt after
`0e6e0834` / `218b6598`, but Slice B left the `.asi` branch of
`GameSaveBuffer.cpp` untouched by construction and the D1 flag is read in the
exe target, so neither should touch this path. No pre-merge measurement at this
n exists to compare against.

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
