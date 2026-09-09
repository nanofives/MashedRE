# TT-2 C2 pre-screen (2026-09-09)

Third run of the operand-correspondence check, this time over the **C2 pool**, to test the
idea that it can pre-screen a reimplementation before harness time is spent promoting it.

    py -3.12 re/tools/matchdiff_sweep.py --conf C2 --out re/parity/matchdiff_sweep_c2.csv
    py -3.12 re/tools/matchdiff_triage.py --conf C2 --out re/parity/matchdiff_triage_c2.csv

## Result

| | |
|---|---:|
| C2 rows with a registration | 192 |
| comparable | 185 |
| **PASS** | **176** |
| **FAIL** | **9** |
| SKIP | 7 |
| `.data` PASS rate | **95.1%** |

Triage of the 9: **CANDIDATE 3, WINDOW 3, WRAPPER 3.** The three candidates are
`0x00410860 ScoreThresholdStateCheck` (−1 addr), `0x00495350 HardwareShowIntroVideo` (+1),
and `0x004b302f StricmpThunk` (−1, 5 B against 105 B).

The C2 pass rate (95.1%) is **higher** than C3/C4 (85.8%). That is not a paradox: the
registered C2 population is dominated by small leaves, which have fewer places to diverge
in shape. It is not evidence that C2 code is better.

## Verdict on the idea: useful, but narrow

**Worth keeping** as a gate in front of a C3 promotion batch — it is seconds of compute, it
runs offline, and 9 flagged rows is a tractable review. Wire it into `promote-c3-batch`
candidate selection.

**The coverage limit is the point though:** only **185 of 2,536** first-party C2 rows are
reachable, because the check needs a compiled reimplementation and most C2 rows do not have
one. The pre-screen covers exactly the population that would enter a promotion batch next,
and says nothing about the rest. **Authoring remains the bottleneck; this does not move it.**

## Correction to an earlier claim in this series

`re/TOOLING_TODO.md` opened with "only 14 of the 2,536 C2 rows have a reimplementation file at
all". That number came from counting `.cpp` values in `hooks.csv`'s `file` column, and **the
column does not mean what I assumed**. For C2 rows it usually holds the **analysis-note path**,
not the reimplementation:

    rva=00403050  hooks.csv file='re/analysis/loading_screen/0x00403050.md'   reimpl=SkeletonAndScatter_t6.cpp
    rva=00407600  hooks.csv file='re/analysis/profile_career_d4/REPORT.md'    reimpl=SlotObjectAccessors.cpp
    rva=0040cf40  hooks.csv file='re/analysis/bucket_gameplay_.../0040cf40.md' reimpl=Thresholds_ah4.cpp

50 of the 62 first-party comparable C2 rows are in that state. So the `file` column is
mixed-purpose and **undercounts reimplementations**; 14 was wrong as a count of ported C2
functions. The framing conclusion (authoring is the bottleneck) survives — 185 registered of
2,536 is still a small fraction — but the specific figure should not be requoted.

## Finding that bears on D-11067 (points miscount)

**D-11067 states the port "skips the finish-order resolver `FUN_0040d590`".** It does not skip
it: a transcribed reimplementation exists as `RaceRankThreePlayers` in
`Frontend/MenuLeaves_af4.cpp:225` (documented `0x0040d590..0x0040d8e0`, "three-way position
ranking, score deltas"), it compiles, and it **PASSES** the `.data`-address check. Its
`RH_ScopedInstall` is **commented out** at `MenuLeaves_af4.cpp:337`.

So the row's shape changes from "port it" to "wire it up and verify". Stated precisely: this
is static evidence of existence and operand correspondence only — **nothing was executed**, and
the two-sided witness D-11067 asks for (does `FUN_0040d590` actually fire in a scored
single-player race) is still required before installing it.

## New column: `installed`

**66 of 1,336 registrations are commented out** — the reimplementation is compiled and
comparable, only the install is disabled. The sweep now records `installed=yes|COMMENTED` per
row rather than filtering those out, because "a reimpl exists but is not hooked" is exactly the
state D-11067 was mis-reading, and it is worth seeing.

## Wired into `promote-c3-batch` (2026-09-09)

The naive reading of "wire it into candidate selection" is impossible and the skill now says
so: **a fresh C3 candidate has no reimplementation yet**, so there is nothing to check at
selection time. It is wired at the two points where it can actually work.

**(a) Per-function, after authoring, before the Frida boot** — `SKILL.md` per-function
workflow step 6:

    py -3.12 re/tools/matchdiff_sweep.py --symbol <Name>
    # exit 0 = PASS, 1 = FAIL, 2 = symbol not found

New single-hook mode (`--symbol` / `--rva`, repeatable) resolves the object and the original
size itself, prints one verdict line per hook, and exits nonzero on FAIL so it can gate a
workflow step. This is the cheapest possible moment to catch the U-9085 / U-9086 class: seconds
of offline compute instead of a boot plus a harness slot spent on a diff that was going to be RED.

**(b) Batch generation, on the filter-3 "stale-impl / tracker-drift" rows** — those DO have a
compiled reimpl, so `--conf C2` applies. A drift row that FAILs is not a no-op re-classify; its
reimplementation disagrees with the original about which globals it touches.

Also added to the skill: a `(v5)` row in the pre-flight viability matrix, and two anti-patterns —
**treating a PASS as promotion evidence** (it is a detector, not a gate; it can justify a re-read
or a demotion, never a C-level), and skipping the check because the hook "looks right".

