# STEP 2 PRE-REGISTRATION — the modes-3/7 hypothesis, tested on the RUNNING ORIGINAL first. **UNRUN.**

Written and committed **before** any capture in this directory exists, and before any
line of game code is edited. HEAD at write time `409cd086` (STEP 1 result).

## What STEP 1 narrowed, and why it changes what must be tested

STEP 1 scored (b) as **13 failing bands across 3 cars, and ALL of them are STEER bands**:
`c0_distinct`, `c1_distinct`, `steer_distinct`, `c1_median`, `abs_steer_median`.
`accel_distinct`, `accel_median`, `brake_distinct`, `brake_median` and `c0_median`
**pass on all three cars**.

That matters for the user's stated closure path, and it has to be said before the test:

- **Mode 3 has NO output tail at all** (`AiControlStep.cpp:143-223` sets it; the dispatch
  at `:318-349` has arms for 7, 5, 9, 2 only).
- **Mode 7's only tail is `ctrl[4] = 0x40`** (`0x00416896`), i.e. the **accel** byte —
  a band that already passes.

So **"port modes 3/7 → their tails fix the steer bands" is false on its face.** If modes
3/7 carry (b), it must be by a different route, and that route has to be named and
measured before any code is written. The candidate route:

> **H-LINK.** The curvature multiplier at `0x0041665c` is gated on **`mode == 0`**:
> `if (mode == 0 && k20f < curv) m = m * (curv * kSteerExtra);`
> (`mashedmod/src/mashed_re/Ai/AiStandalone.cpp:876` and `:899`,
> `kSteerExtra = 0.05f` = `_DAT_005cc9a0` at `:569`, `k20f = 20.0f` = `_DAT_005ccd6c`
> at `:578`). With `curv > 20`, `curv * 0.05 > 1`, so the multiplier **amplifies** steer.
> The shipping exe pins **`int mode = 0;`** at `AiStandalone.cpp:844`, so the `mode == 0`
> conjunct is **always true** and the multiplier fires on **220/220** window calls. The
> original is in mode 0 on only part of its window. The excess steer that (b) measures is
> therefore the multiplier firing on the calls where the original is in mode 3 or 7.

**This unifies the two hypotheses the kickoff names as separate.** `AiStandalone.cpp:844`
`int mode = 0;` is simultaneously (i) the dual-copy divergence that demoted `0x00416250`
C3→C2 on 2026-09-29 (`hooks.csv` row `00416250`: *"`int mode = 0;` at
Ai/AiStandalone.cpp:844 makes the whole targeting chain unreachable"*) and (ii) the reason
modes 3 and 7 are unreachable in `mashed_re.exe`. They are **one defect**, not two
competing explanations, and the test below measures the single mechanism.

A separate, **different** `rate1` defect exists at `AiStandalone.cpp:983` / `:1102`
(`const float rate1 = 0.0f;` → `if (rate1 <= k20f) mag *= kSteerExtra;` always true), but
it is inside `ControlStepM49` (`0x00416a30`) and `0x00417da0`, **not** the main
`ControlStep` (`0x00416250`) that this recipe runs. It is recorded here and **excluded
from this step's scope** unless the window is shown to run M49.

## Where the world objects come from (static, already measured, no guess)

`py -3.12 re/tools/decomp_pc.py 0x006e70d8 0x006dccb8 --datarefs` (read-only pool clone
`Mashed_pool0`):

- `DAT_006e70d8` (the count `FUN_00484c70` returns through `param_1`): **written by
  `FUN_00484c90` at `0x00484cd4`** and **`FUN_00485070` at `0x0048508a`**; read only at
  `0x00484c74`, inside `FUN_00484c70`.
- `DAT_006dccb8` (the array base `FUN_00484c70` returns): written by `FUN_00484c90` at
  `0x00484ca4`; the registrar `FUN_00484cf0` indexes it at `0x00484d2d`/`0x00484d35`.

Callers (`--callers --no-decomp`): `FUN_00484c90` ← `FUN_0040cfd0`, `FUN_004111c0`.
`FUN_00485070` ← `FUN_0040fc00`. **`FUN_00484cf0` ← 13 sites**, and most are the power-up
range already ported: `0x0045bba0` (the dispatcher), `0x004532f0`, `0x00454350`,
`0x004570a0`, `0x00458080`, `0x00459000`, `0x0045a530`, plus `0x00419a00`, `0x0041f710`,
`0x0044bbc0`, `0x0044c490`, `0x00481a30`, `0x00486830`.

**Consequence that governs scope:** porting `FUN_00484c70` alone is **inert** in the
standalone — it reads two `.bss` globals nothing writes, so it returns count 0 and no mode
is ever set (memory `zeroed-granule-vs-minus-one-sentinel`). A working port needs a
**producer**. That is a measured fact about scope, stated before the work, not a reason to
skip it.

## STEP 2A — the live test on the RUNNING ORIGINAL. No code is edited.

Capture, one run, all three AI cars (`--statediff-aistep` covers every AI car):

```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/d3_modes37_20261002/o1.msd \
    --statediff-car 1 --statediff-aistep --hold 60
```

Window: the same 220 calls from the first `c4 != 0` that `ai_ctrl_window.py` uses, so
every number below is on (b)'s own population. Frida **entry hooks only**. The game is
launched muted with `MASHED_TITLE` set; the PID is tracked and only it is killed.

### Gates, registered before the capture exists

- **G2-MODE** — the original's per-car `ai_mode` histogram over the window is
  non-degenerate and contains **mode 3 and/or mode 7**. The 2026-09-27 record
  (`D3_AI_RESIDUE_2026-09-27.md:107`) is car 1 `{0:105, 7:115}`, car 2 `{0:176, 3:44}`,
  car 3 `{0:207, 3:13}`.
  **If today's original is mode 0 on 220/220, the premise of the user's closure decision
  is refuted on the current binary and I STOP and report that**, rather than porting
  against a premise the running original does not show.
- **G2-JOINT** — the "the original brakes and lifts where the port holds throttle"
  hypothesis, stated so it can fail: **every** window call with `c4 != 255` or `c5 != 0`
  on the original is a `ai_mode != 0` call. Reported as a count, per car, both ways
  (non-full-throttle ∧ mode 0, and non-full-throttle ∧ mode≠0).
- **G2-STEER** — the mechanism's own signature. On the original, split the window by
  `ai_mode == 0` vs `!= 0` and report, per car and per group: `n`, `abs_steer` median,
  `c1` distinct, `curv` median, and **the fraction of calls with `curv > 20`**. H-LINK
  predicts the multiplier is available on the mode-0 group and withheld on the other.
  Median **call index** is printed for each group, because two groups inside one window
  are two different moments (memory `band-on-speed-compares-different-moments`).
- **G2-SIM** — the counterfactual, on `re/tools/ai_band_sim.py`, whose own validation
  gate (*"must reproduce the standalone's OBSERVED c0/c1 and c4/c5 on >= 95% of window
  calls before any counterfactual is believed"*, `ai_band_sim.py:32-33`) is **kept, not
  relaxed**. The tool is extended by one argument — a **per-call mode sequence** replacing
  the current scalar `mode=0` at `ai_band_sim.py:72` — and nothing else. Run: the port's
  own window (`verify/d3_rebase_20261002/r1.csv`) with the mode sequence taken from the
  ORIGINAL's same-index calls.

  **Decision rule, registered now:**
  - **SUPPORTED** → `abs_steer_median` and `c1_distinct` both move **toward** their bands
    on **all three cars**, and at least one of them **enters** its band on at least one
    car. The port leg proceeds.
  - **REFUTED** → either statistic fails to move toward its band on any car, or no
    statistic enters a band on any car. Then **modes 3/7 are not the dominant carrier of
    (b)**, I report that prominently as a finding against the user's stated closure path,
    and I do **not** write the port on a refuted premise.
  - If the tool's own ≥95% validation gate fails, **G2-SIM is VOID** — not re-thresholded
    — and I say so.

  **Stated limit of G2-SIM, before it runs:** splicing the original's mode sequence onto
  the port's own `(err, curv, speed)` is a **bound on the mechanism's contribution**, not
  a forecast of the post-port score. A real port computes its own modes and its trajectory
  diverges from both inputs. It is registered as a go/no-go on the mechanism, and it will
  not be quoted as a predicted (b) result.

## STEP 2B — the port. Runs ONLY if G2-SIM returns SUPPORTED.

Scope, in this order, each with the promotion leg:
`FUN_00484c70` (the query, 17 bytes) → its producer chain → `FUN_00414c30` (704 bytes,
`0x00414c30..0x00414ef0`) and the callees it needs → un-pin `AiStandalone.cpp:844`.

Promotion leg per RVA, non-negotiable: one faithful body at the RVA, in a TU listed in
**both** `exe_sources.rsp` and `asi_sources.rsp`; duplicate implementations checked **by
hand** as well as by `py -3.12 scripts/lint_rva_bodies.py` (the lint is a text scanner and
**cannot see a port with no RVA comment** — `DUAL_COPY_FIX_2026-09-29.md:337-339` — which
is exactly the shape `AiStandalone.cpp` has, so the lint alone is not sufficient here);
`NEW=0` on the lint; Frida path1 (`run_diff.py`) + path2 (`run_verify_hook.py`), or
`re/CONFIDENCE.md`'s non-repeatable-function clause invoked **by name**; then
`re-classify` with only what was earned.

**Scope control, registered now:** if the producer chain needed to make `FUN_00484c70`
return a non-zero count exceeds what can be ported with its own evidence in this session,
I port what is complete, say exactly what is left, and do **not** fabricate a producer or
seed the globals to make the modes appear. A seeded global is not a port.

## STEP 3 — re-score (b) and (e) against the UNCHANGED bands, with `n`, median speed and median frame index next to every number, plus the collateral review and both guards.

## D2 WATCH

No D2 code is changed in this session. Anything found that touches `+0x4a4`, the contact
collector / `FUN_00538c80`, grip-clamp #6, the substep/chunk loop, `ReassertContacts` or
player-car speed on Training is reported as a **"D2 REOPEN CANDIDATE"** row with evidence.

## Rules

Game launched muted, `MASHED_TITLE` set, `MASHED_WIN_POS=primary-bl`, `--poke-ctrl-slots`
on race captures, **never** `MASHED_NAV_DEMO`. PIDs tracked, only mine killed. Frida
**entry hooks only**. Ghidra pool clones `-readOnly`; the master project is never written.
`original/MASHED.exe` is the diffing reference and gets no `unlock_*` patch.
