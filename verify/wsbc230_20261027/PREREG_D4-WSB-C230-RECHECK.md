# PREREG D4-WSB-C230-RECHECK: 0x0055c230 C4 recheck (L-40 item 2)

Committed alone, before any arm. Source of the row: `log/orch/DECISIONS.md` L-40, decided 2026-10-27: "0055c230: keep
C4 meanwhile, add a run-16 row to re-run its write-set leg on a window every original boot reproduces; demote to C3
only if that fails". Also binding: L-38 (a C4 needs a write-set leg that compared more than 0 heap pages).

## Why

BROAD-A (`verify/wsbbroada_20261027/RESULT_D4-WSB-C4-BROAD-A.md` section 5) promoted `0x0055c230` on Z2 C4-CLEAN, with
Z1 CONTROL-SAME. Over the 4 original boots that reached that window, 3 gave the set `{c55cdd0, c57b420}` and 1 gave
`{c55c680, c55cdd0}` (v8 OFF boot 18028). The tool keeps the first 6 calls after A4 call 300 (`const SAMPLES = 6`), and
the first token of every signature is the target of `call dword ptr [edx+0x18]` at `0x0055c2a8`, which follows the object
the sampled call receives. The body part after that call was identical on all 6 boots. BROAD-A's out-memory leg was leg A
(no store outside the own frame), so no heap-tracked write-set leg exists for this row.

## Build

Branch `orch/D4-WSB-C230-RECHECK` off `orch/integration` @ `37383a4bb`. One source change, harness only: the
`0x0055c230` shadow wrapper (`RwpSolverGlue5.cpp`) goes from `ShadowAB::Run` to `ShadowAB::RunTracked(...,
SHADOW_STACK_WINDOW(), ...)`, the same form as `0x0055ab30` and `0x0055bd80` in that file. It only acts with
`MASHED_SHADOW_AB=1`. Built with `MASHED_NO_DEPLOY=1`, `.asi` copied to the private root `D:/MashedWT/D4-WSB-C230-RECHECK_root`
only. Every boot under the test slot and `mashed_lock.py run --label D4-WSB-C230-RECHECK`.

## Leg 1: default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe. Must equal `merge_train.json`:
G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664`. A mismatch makes the step RED and no tracker is
touched.

## Leg 2: heap-tracked write set (`run_shadow_c230.py`, copy of BROAD-WS `run_shadow3.py`)

`MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, census (`--track 0 --mode 10 --cars 4 --hold 60
--hooks 0x0055c230`), 24 samples. Read-outs as BROAD-WS round 2:

- **HEAP-CLEAN**: `PATCHBYTE ... A/B-IS-REAL installed=e9`, ndiff 0, every sample `stack=0 noise=0 ovf=0 ret=0`, and heap
  pages compared (sum over the counted samples of the `heap=o:` count) > 0.
- **CLEAN-NO-HEAP** (ndiff 0, heap pages 0): one arm with `MASHED_SHADOW_NEED_PAGES=1` (`SHADOW_TAG=need`). HEAP-CLEAN
  there counts as HEAP-CLEAN. Otherwise the leg fails L-38.
- **DIVERGENT** (ndiff > 0): the leg fails.

This leg compares the original and the port on the same call in the same boot, so its result does not depend on which
objects a boot samples.

## Leg 3: racediff on a window every original boot reproduces (`c4_racediff_v11.py`, `run_c230.py`)

v11 = BROAD-A's v10 plus one opt-in knob, `C4_SAMPLES` (per-target sample budget, default 6 = v10). Every arm uses
BROAD-A Z2's settings unchanged: census, `C4_HOLD=400`, `C4_WINDOW="300 30000"`,
`C4_TRANSP_SYMS=__init_thread_header,__init_thread_footer,_getenv`, no `C4_DEREF`, no `C4_ARGW`. Only the budget changes.

Window W1 = `C4_SAMPLES=48`.

1. **C1, C2**: `C4_CONTROL=1`, two arms, 4 original boots. Each boot's signature set is read from
   `c4_racediff_samples.json` (both slots). W1 **reproduces 4/4** iff the 4 sets are equal and non-empty.
2. **T** (only if W1 reproduces 4/4): hook installed alone vs original. **PASS** iff verdict C4-CLEAN (ON byte `0xE9`)
   and its OFF set equals the control set (5/5 original boots).

If W1 does not reproduce 4/4: window W2 = `C4_SAMPLES=200`, the same steps with 4 new control boots (C3, C4), then T2.
No further window. If W2 does not reproduce 4/4 either, leg 3 fails (no window found that every original boot
reproduces).

Re-run only for both-sides-empty, `could not attach`, or a U-9677 crash signature (`MASHED.exe+0x1509b4`, `+0xe66db`).

## Outcome

- Leg 1 holds, leg 2 HEAP-CLEAN, leg 3 PASS: `0x0055c230` **stays C4**. `re-classify` records the new evidence in its
  `hooks.csv` notes / `frida_diff` and one CHANGELOG line. Verdict GREEN.
- Leg 1 holds, leg 3 fails: **demote to C3** through `re-classify`, notes name the failed leg. Verdict GREEN (the rule
  was applied).
- Leg 1 holds, leg 2 fails (DIVERGENT, or 0 heap pages after the NEED arm): **demote to C3** through `re-classify` with a
  blocker note (L-38). Verdict GREEN.
- Leg 1 does not hold: RED, no tracker change.
