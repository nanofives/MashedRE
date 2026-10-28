# RESULT D4-WSB-RAND-FLIP: MASHED_OWN_RAND_564310 default ON (L-47 (a))

**Verdict: GREEN.** The standalone's own CRT rand for `FUN_00564310` is default ON. `MASHED_NO_OWN_RAND_564310=1`
opts out to the old call through `0x005c229b`. All 6 default gate hashes equal the `merge_train.json` refs. The
opt-out arms give the same 6 hashes. One UNCERTAINTIES row, U-9797, is filed through re-classify for the two open
points L-47 named. No C-level moves.

Decision: `log/orch/DECISIONS.md` L-47, answer (a) (Mariano 2026-10-28). That answer authorizes the default flip.
Base: `orch/D4-WSB-RAND-D11064` @ `b85836c0b` (not yet in `orch/integration`, which was at `f7c41f574` and did
not contain it). Background: `verify/d4wsb_rand_20261028/RESULT_D4-WSB-RAND-D11064.md`.

## 1. Change

- `mashedmod/src/mashed_re/Collision/RwpSolverLeaves1.cpp`, `OwnRand564310()`: the knob now reads
  `MASHED_NO_OWN_RAND_564310`. It is on unless that variable is set to a value other than empty or `0`. This is the same
  form as the other default-ON knobs (`MASHED_NO_AI_CLOCK_LATE`, `Ai/AiStandalone.cpp:2164`). `MASHED_OWN_RAND_564310`
  is now a no-op. The comments name the flip and U-9797. The call site comment at the `_rand()` line now reads
  "default own LCG (L-47)". `.asi`: still compiled out under `#ifdef MASHED_STANDALONE`. The string
  `MASHED_NO_OWN_RAND_564310` occurs 1 time in `mashed_re.exe` and 0 times in `mashed_re_dev.asi`.
- `verify/savelink_20261023/run_arms.py`: two new stacks. `NORAND` = the opt-out plus `MASHED_OWN_RAND_LOG`. `RLOG` =
  only `MASHED_OWN_RAND_LOG` on the new default. The log is opened only when the knob is on, so the presence of the
  `selftest` line tells whether the knob is on.
- Trackers (re-classify): `UNCERTAINTIES.md` U-9797 (new, `Blocks` = nothing). `re/analysis/CHANGELOG.md` gets one line
  under `<!-- ENTRIES -->`. `hooks.csv`, `STUBS.md`, `DEFERRED.md` are not touched. DEFERRED D-11064 stays Active (see section 4).

## 2. Build

`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` from the worktree: `=== Build OK ===`, rva-lint OK (0 NEW),
asm-lint OK, `.asi` not deployed.

- Gate build: exe `a22ae5e0`, asi `db2989a2`. Every arm in section 3 ran on it.
- After the gate build, one comment was added on an existing line (the `[UNCERTAIN U-9797]` marker, no line-count change).
  Rebuilt from the committed tree: exe `f8987c11`, asi `ac1f7e5b`. Per-section SHA-256 against the gate exe: `.text`
  `c0156e47` same, `.data` same, `.fptable` same, `.reloc` same. `.rdata` differs. That section holds the debug
  directory with the link timestamp. The gate hashes therefore apply to the committed code.

## 3. Arms (standalone only, private root `D:\MashedWT\D4-WSB-RAND-FLIP_root`, test slot + `mashed_lock.py run --label D4-WSB-RAND-FLIP`)

All 9 arms `exit=0 self_exited=True` on exe `a22ae5e0`. The lock waited for a merge-train solo window, then ran the arms back to back.

| arm | result | expected |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | merge_train `gctrl.ref` `5943c5f1/364350ae/52052890`, met |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `9fcdbd3a` / `53778664` | merge_train `b.ref` `3a03670c/9fcdbd3a/53778664`, met |
| H_NORAND step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | equal to H_DEF, met |
| B_NORAND_K0 / K1 / K26 | `3a03670c` / `9fcdbd3a` / `53778664` | equal to B_DEF, met |
| B_RLOG_K0 step | `3a03670c` | equal to B_DEF_K0, met |
| B_RLOG_K0 `.rand.txt` | 1 line: `selftest 41 18467 6334 26500 19169 15724 11478 29358`, 0 `call` lines | knob on by default, 0 calls, met |
| 4 x NORAND `.rand.txt` | not created | knob off under the opt-out, met |

The `selftest` line equals a Python LCG run from seed 1 (`*0x343fd`, `+0x269ec3`, `>>16 & 0x7fff`), and it equals the
four knob-on logs of D4-WSB-RAND-D11064 (same SHA-256 `dc84a66c`). The RUNLOG shows `MASHED_NO_OWN_RAND_564310=1` on
the 4 NORAND arms and on no other arm.

**13-track check: not run.** The knob only changes the `_rand()` call inside `FUN_00564310`, and the standalone does
not execute that island: 0 calls here and 0 calls in the 4 knob-on arms of D4-WSB-RAND-D11064. The flip does change
code layout (`.text` differs from base `e9835c0f`), and layout alone has moved rouabout / Storm / Highway before
(memory `sim-trajectory-is-code-layout-sensitive`). The merge train's run13 against
`verify/hbfullcheck_20261028/tracks_score.txt` covers that case at merge.

**Original side: not attempted.** The original MASHED.exe does not boot this run (no audio endpoint, D4-WSB-RAND-D11064
section 4). There is no bit compare of the octant pick against the original. `0x00564310` keeps its `hooks.csv` row
unchanged.

## 4. Open after this step

- U-9797 (filed here): (1) the original's CRT rand state is per thread (`_getptd`, initial 1 at ptd`+0x14`, `0x004a8870`),
  while the port uses one process-wide `s_own_rand_state`. (2) No one has checked whether `KTCScript.lua`, `powerups.lua`,
  `powerups_gold.lua`, `powerups_all.lua`, `LAPDATA.LUA` or `COURSE.LUA` call `random` / `randomseed`. Those calls
  would reach the same state through `0x004c03b4` / `0x004c0502`. Path: an original-side thread-id and call log once
  the original boots, plus a grep of the six scripts extracted with the piz skill.
- DEFERRED D-11064 stays Active. Its re-pickup action ("rebind to the port's own CRT rand() and decide seed-state
  policy") is now done in the default build, with the seed-state question held open as U-9797. The task asked for the
  UNCERTAINTIES row only, so closing or rewriting D-11064 through `deferred_close.py` is left to the lead.

## Raw data (not committed), `D:/MashedWT/D4-WSB-RAND-FLIP_data/`

| file | sha256 |
|---|---|
| gates/H_DEF.step.csv | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| gates/H_DEF.gates.csv | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| gates/H_DEF.ptrace.log | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| gates/B_DEF_K0.step.csv | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| gates/B_DEF_K1.step.csv | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| gates/B_DEF_K26.step.csv | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| gates/H_NORAND.step.csv / gates.csv / ptrace.log | identical to the H_DEF rows |
| gates/B_NORAND_K0 / K1 / K26 .step.csv | identical to the B_DEF rows |
| gates/B_RLOG_K0.step.csv | identical to B_DEF_K0 |
| gates/B_RLOG_K0.rand.txt | `dc84a66c7c9a45e24f23d32d055b99af8f6c5a068dcd8c72f6275688719cdf21` |
| gates/RUNLOG.txt | `3f6143aaa7246f6c750670fe333fc47856a0b01262c5d0d2f2038e5562d0585a` |
| gates_run1.log (run console) | `b63db17bfe4404c94e16b9b82238528626ee075abaa6dc73d16e9d621add47e0` |
| mashed_re.gates.exe (gate build) | `a22ae5e0ab5c3550c3f9d148732219e8bccc3a9a67810269fadd7af86dc9caaa` |
| mashed_re_dev.gates.asi (gate build) | `db2989a291450099ede85d40cbb05e73f45553678a9bc7c301b1827a297cfc58` |
