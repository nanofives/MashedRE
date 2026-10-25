# RESULT D4-DC-8ad0: delete the MASHED_NO_RACEPCT_SLOT opt-out (W0-12)

**Verdict: GREEN.** The opt-out and the gate-count stand-in are gone, 0x00408ad0 leaves the dual-copy
allowlist (8 findings to 7, NEW=0), and both default gates are byte-identical to the rpct default.

Base: `orch/integration` @ `b5c84688e`. Branch `orch/D4-DC-8ad0`. User decision W0-12
(`re/orchestrator/DECISIONS.md:48`): delete the opt-out, old trio stays in git history only.

## Change

`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`

1. `RacePctSlotEnabled()` deleted (was :168-173, the only reader of `MASHED_NO_RACEPCT_SLOT`).
2. `TrackRenderer::RacePct` (now :7410-7417): returns `RaceScoreFloatGetBySlot(car)` for `0 <= car < kRaceCars`,
   0 otherwise. On the default path that is exactly what it returned before (slot load in range, the
   stand-in's own range guard returned 0 out of range). The `fmod(progress, n) / n * 100` stand-in is deleted.
   The `// 0x00408ad0` anchor above it is replaced by a prose header naming the one body
   (`Frontend/SmallLeaves_t1.cpp:RaceScoreFloatGetBySlot`), so the RVA scanner no longer binds a second body.
3. `FillRaceCamCars`: `c.race_pct = RaceScoreFloatGetBySlot(i)`, the ternary and its stand-in arm deleted.

`re/tools/dual_copy_allowlist.txt`: line 26 (`DUP-IN-TARGET:00408ad0:...`) removed.

## Gates (exe `fa099e3c`, built with `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\D4-DC-8ad0_root`, under `mashed_lock.py`)

Harness: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` (`A4_OUT` / `A4_ROOT` pointed at
`D:/MashedWT/D4-DC-8ad0_data`). Raw output: `gates.txt`, provenance: `RUNLOG.txt`.

| Arm | Expected | Got | |
|---|---|---|---|
| H_DEF step / gates / ptrace | bacfc747 / 8dd1c004 / 6f9932b6 | bacfc747 / 8dd1c004 / 6f9932b6 | same |
| B_DEF_K0 / K1 / K26 | f9deb9b9 / d3601fcc / 9da9a26a | f9deb9b9 / d3601fcc / 9da9a26a | same |

All four runs self-exited, exit 0.

## Lint

`scripts/lint_rva_bodies.py`: base 8 findings (DUP-IN-TARGET=5), branch 7 (DUP-IN-TARGET=4), NEW=0 both
(`lint.txt`).

## Dead mentions left on purpose

- `verify/{rpct,dcstream,kfdot}_20261022`, `verify/savelink_20261023`, `verify/{bankpath,audiovoltods,u9782}_20261024`
  `run_arms.py` define `STACK['NRS'] = ['MASHED_NO_RACEPCT_SLOT']`. These are committed evidence copies of
  past runs, not edited. NRS is now a no-op knob in any copy made from them.
- `re/NEXT_SESSION.md:233, 286, 290, 317` describe the opt-out. Not edited (WORKER_RULES: that file is written by
  the merge train / its owner). The lines to retire: 233 and 317 (0x00408ad0 "stays allowlisted"), 290 (opt-out trio).
- `re/orchestrator/DECISIONS.md`, `roadmap_seed_20261024.md`, `roadmap_verify_20261024.md`, ledger json: records of the
  decision itself, left as written.

No tracker row changed (no C-level move: the shared body `RaceScoreFloatGetBySlot` is untouched).
