# Shadow A/B mass lane — first sweep (2026-09-10)

Session goal: turn the in-process shadow A/B (TT-11, `Core/ShadowAB.h`) from 7 hand-written
sites into a lane that can absorb the whole "C2 with a compiled, installed, operand-check-PASS
port" pool. Tooling built, one full sweep run, results below. **Nothing in this file is a
C-level change**; promotions go through `re-classify` with this note as evidence.

## Tooling (all new, `re/tools/`)

| tool | what | state |
|---|---|---|
| `shadow_gen.py` | rewrites an installed port to `Name_impl` + a `ShadowAB::Run` / `RunRegion` wrapper; adds the include; writes `re/parity/shadow_sites.tsv` | 79 return-value sites + 15 region sites generated; 55 void ports still `NEEDS_REGION`; 4 skipped (naked / line-broken signature / unnamed params) |
| `shadow_batch.py` | boots groups of sites through `scenario_launch.py --hooks`, retries harness VOIDs once, bisects CRASHes to single sites, merges into `re/parity/shadow_results.tsv`, keeps every raw log + launcher transcript under `log/shadow_ab/` | 4 runs, 18 boots |
| `shadow_ab_report.py` | `shadow_ab.log` (+ the B5c `phys_c4_*_selftest.log`s) -> per-function verdict: CLEAN / DIVERGENT / DIVERGENT_FLOAT10 / UNPROVEN / SKIPPED / NO_SAMPLES | |

Candidate source: `re/parity/matchdiff_sweep_c2.csv` rows `verdict=PASS, installed=yes` (153).

## Results — 79 return-value sites

| verdict | n | meaning |
|---|---|---|
| CLEAN | 44 | 48 samples each, 0 divergent, PATCHBYTE proof `A/B-IS-REAL` (installed E9 -> original prologue byte) |
| DIVERGENT | 9 | return differs on at least one sample; **hand review required** (see below) |
| DIVERGENT_FLOAT10 | 5 | x87 80-bit return compared through MSVC 64-bit `long double`; lane limit, not a port verdict |
| NO_SAMPLES | 21 | never reached in `--track 0 --mode 10` with 1 car (30 s) nor 4 cars (60 s) |

Scenario coverage mattered: with the default 1-car race, 48 sites never fired (boots 4-5 of run 1
wrote **no log at all**). 4 cars + 60 s reached 27 of them.

### The 9 DIVERGENT rows are NOT nine wrong ports

`Run()` executes the function twice on the same inputs and compares only the return. A
non-idempotent function returns two different values legitimately:

- `0x00564310 FUN_00564310` (20/48): calls CRT `rand()` (`RwpSolverLeaves1.cpp`, `s_rand_orig()`
  first statement). Original consumes one draw, the port the next. **Non-idempotent, not a defect.**
- Worker review 1 (`log/shadow_ab/worker_divergent_review.txt`, account2, evidence at file:line):
  - `0x00574ad0` NON_IDEMPOTENT: contact record `param_6+0xac` read+incremented by callee `FUN_00575120`;
    a point the first call appended is rejected (`return 0`) by the second.
  - `0x005752b0` NON_IDEMPOTENT: pair-record separating-axis field `param_1+0x100` read as input,
    overwritten by callee `FUN_00576880`.
  - `0x00575560` NON_IDEMPOTENT: same `+0x100` axis via `FUN_005752b0` -> `FUN_00576880`.
  - `0x00576880` NON_IDEMPOTENT: reads its `param_7` axis buffer, overwrites it with the normalised normal.
  - `0x005667c0` IDEMPOTENT, mismatch = float10 precision (64-bit MSVC `long double` vs x87 80-bit);
    already labelled DIVERGENT_FLOAT10.
- Worker review 2 (`log/shadow_ab/worker_divergent_review2.txt`):
  - `0x00575120` NON_IDEMPOTENT: contact count `+0xac` and the contact slot are read then written
    (first call appends -> `1`, second call finds the point present -> `0`; 46/48 exactly that).
  - `0x00578ff0` NON_IDEMPOTENT: the merge loop zeroes the manifolds' `+0xac` counts, then sums them
    for the return (`ours = orig-1`, 5/48).
  - `0x00577be0`, `0x00577cb0`: body IDEMPOTENT in isolation; divergence requires the caller to
    alias an output buffer with an input (`orig=1, ours=0`, 38/48 and 24/48, `cb0` propagates `be0`).
    **UNRESOLVED**: the aliasing is a hypothesis about the call site, not shown by the file. To
    settle: convert to `RunRegion` over the output buffer (restores it between the two calls).

Net: of 9 DIVERGENT rows, **0 are established port defects**; 7 are proven non-idempotent
(state named at file:line in the two reviews), 2 are unresolved pending a RunRegion re-test.
A `Run()` DIVERGENT on a function that reads state it (or a callee) writes is uninformative —
those rows stay C2 with this reason, not as refusals on evidence.

Rule going forward: a DIVERGENT from `Run()` on a function that mutates state it also reads is
uninformative. The fix is either a `RunRegion` over that state or a snapshot/restore hook for
the specific global (CRT `rand` seed for `0x00564310`). Filed in `re/HARNESS_BACKLOG.md`? — no,
kept here + TT-11 until a second instance justifies a generic mechanism.

### CRASH note (unresolved)

Run 2 (4 cars) had three boots die at ~28 s (phase 2, before the race) with groups containing
`0x00574ad0` + `0x00575120`; bisection stopped at the pair. Run 3 booted both in 8-site groups
and reached the race twice with no crash. Not reproduced; recorded so it is not forgotten.
Launcher transcripts: `log/shadow_ab/launch_20260910_0018*.txt`, `..._0020*.txt`.

## Region (void) sites — 15 generated, verified mechanically

The worker (account2) surveyed the 70 void ports' write sets
(`log/shadow_ab/worker_void_regions.txt`): 19 SINGLE_REGION, 12 MULTI_REGION, 18 INDIRECT,
7 GLOBAL_WRITES, 12 CALLS_UNKNOWN, 2 unclassified. **One SINGLE_REGION claim was wrong**
(`0x0056c8e0`: write base is `*(int*)(*param_7+i*4)*0x20`, not `param_4*0x20`), so every spec
was re-checked by scanning each body's assignment LHSs (`log/shadow_ab/region_verify.txt`):
15 VERIFIED, 1 REJECT (`0x00566830`, variable indices), 3 COMPLEX (data-dependent offsets).
Only the 15 were applied. Run 4 (4 cars, 60 s): **2 CLEAN** (`0x00546c50`, `0x00565200`, 48/48
region-identical), **1 DIVERGENT** (`0x0055b750`, 13/48 on the vec3 at `param_3+4/+8`; RunRegion
restores the region between the two calls so non-idempotency does not explain it — this one is a
genuine defect-or-precision candidate, unreviewed), 5 NO_SAMPLES, and the group
`0x0056bb80 0x0056bce0 0x0056cf90 0x0056d350 0x0056ed60 0x0056fad0 0x005735f0` **crashes the
game ~10 s into boot** (three times, Frida "script has been destroyed"; `0x0056ed60` had logged
7 identical samples before the crash). Run 5 (one site per boot, `batch_run5.txt`) isolated it: **`0x0056bce0` alone crashes the game
at ~10 s, deterministically**; the other six boot fine — `0x0056cf90`, `0x0056ed60`,
`0x0056fad0` **CLEAN 48/48**, `0x0056bb80`/`0x0056d350`/`0x005735f0` NO_SAMPLES. Region lane
total: 5 CLEAN, 1 DIVERGENT (`0x0055b750`), 1 CRASH, 8 NO_SAMPLES. The `0x0056bce0` crash is
a claim about the shadow SPEC or double-execution at init (the port itself is installed in every
default `.asi` run and boots), not yet about the port; see the crash note below.

## C3 gate check (Ghidra, slot Mashed_pool14, read-only)

`log/shadow_ab/c3_gate_check.tsv`: callers/callees of every CLEAN RVA with their `hooks.csv`
confidence. All 44: **32 PASS, 12 LEAF** (zero callees, every caller C2), **0 FAIL**; every
neighbour resolved in `hooks.csv` at C2 or above (no C0/C1, no unknown). `0x00575fe0` is
self-recursive and passes on its other neighbours.

## What this changes about throughput

- Cost per site: one `shadow_gen.py --apply` + a share of one boot. 18 boots covered 94 sites.
- The lane's ceiling is now **scenario reach** (21 NO_SAMPLES) and **idempotency** (9 rows), not
  boilerplate. Both are attackable: richer scenarios (contacts, powerups, damage) and a
  per-site snapshot of the mutable state.
- The 55 remaining void ports need regions from their analysis notes; the worker survey gives
  the write-set class for each, so MULTI_REGION (12) is the next slice once `RunRegion` takes
  a second span.
