# Pipeline — resume here (saved 2026-06-26)

## State at log-off
- `main` clean at commit `ebfebe15` (clean-getter gate). **862 C3** in hooks.csv.
- All pipeline infra committed: planner (`scripts/pipeline_plan.py`), orchestrator
  (`re/pipeline/promote_pipeline.workflow.js`), `ptr_arg_int_get` arg_type
  (`re/frida/diff_template.js` + `scripts/promote_enrich.py`), ledger
  (`re/pipeline/LEDGER.tsv`), doc (`re/pipeline/README.md`).
- No background tasks running (the autonomous run `wfopd0vbc`/`wf_5125d613` was stopped).
- All worker worktrees pruned; no half-merge; working tree has only pre-existing scratch.

## What happened this session
- Built + hardened the autonomous C2→C3 pipeline through 4 runs (fixed: thunk/teardown
  selection, args-not-propagating, prior-round re-burn, abort-on-one-flake).
- Landed **+8 C3** (854→862): RainSetCameraScale, RainLineWidthRangeSet,
  AudioAtomicExchangeStore, RaceArmReset, + 4 more.
- Built the `ptr_arg_int_get` meta-action for deref'd-pointer getters; **validated GREEN**
  on `0x004dfaa0` (RwRasterU16Get). Honest yield: clean subset ~23, not the raw 263.

## To resume tomorrow — one command
```
Workflow({scriptPath: "re/pipeline/promote_pipeline.workflow.js"})
```
Defaults: 8 rounds (self-drains ~round 3-4), Sonnet workers, autoMerge behind the
integration-diff gate, flake-tolerant collector. Preflight needs: anchor backup
(`original/MASHED.exe.unpatched`), d3d9 shim (`original/d3d9.dll`), clean build — all
present at log-off. It re-plans from current `hooks.csv` so it auto-excludes the 862
already-C3 and the ~120 prior-tried; ~97 fresh diffable candidates remain.

First candidate it will re-promote: **`0x004dfaa0` RwRasterU16Get** — already validated
GREEN (14B pure-leaf LE-u16 getter: `MOV EAX,[ESP+4]; XOR ECX,ECX; MOV CH,[EAX+1]; MOV
CL,[EAX]; MOV EAX,ECX; RET`). Its worktree work was discarded for a clean log-off; the
pipeline re-authors it in minutes.

## Strategic note for tomorrow (decide before a big spend)
The cheap "leaf + scalar arg_type" lane is ~mined out (~862 C3 + this run's ~10-25). The
`ptr_arg_int_get` win is real but small (~23 clean getters) — the 1440-deep
`needs_argtype` pool is INFLATED by indirect/vtable/jump-table/multi-arg functions no
scalar/ptr arg_type can diff. The remaining C2→C3 ceiling needs **reimpl-first /
scenario-attach** (reverse a whole cluster as one module + control its state), not more
scalar arg_types. Options: (a) run the pipeline once more to drain the 97 clean
candidates, then (b) pivot to reimpl-first clusters. See memory
`project_autonomous_promotion_pipeline` + `project_reimpl_first_d3d9_state_cache`.
