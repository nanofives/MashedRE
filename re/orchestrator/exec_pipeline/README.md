# claude3 execution-tier pipeline (narrow / serialized)

The machine-bound half of the two-tier orchestration
(`re/analysis/plans/methods_efficiency_scorecard_2026-07-29.md`). It runs a
queue of build/verify jobs, parses verdicts, and emits a promotion-ready
candidate list.

## Why narrow (the opposite of the read-fleet)

The read-fleet fans WIDE because read-only children share nothing. This tier is
the reverse: every job touches a **shared machine resource** —

- the **one GPU + game process** (concurrent boots contend — measured 1.12x for
  2 — and simultaneous `frida.spawn` collides),
- the single **build tree / `.asi`**,
- the **git working tree** (one writer),
- **Ghidra pool slots** (one session per slot).

## Two locks, held at the right level (no nesting, no deadlock)

- **GAME lock** (`mashed_machine`, in `$env:TEMP`): held by the MASHED-spawning
  scripts THEMSELVES (`run_diff_scenario_batch.py` / `stalker_write_surface*.py`
  via `re/orchestrator/mashed_lock.py`), around `frida.spawn`..kill. So a
  state_batch/stalker job queues on the game automatically — and so does an
  INDEPENDENT child session or a manual Frida run, because they call the same
  scripts. This is the machine-wide MASHED run queue: instances take turns.
- **BUILD lock** (`mashed_build`): taken by this pipeline ONLY around
  `build.bat`, so two concurrent builds can't corrupt the shared `.asi`.

The pipeline does NOT hold the game lock itself (its Python children do), so
there is no lock nesting and no deadlock. `MachineLock.ps1` (PowerShell) and
`mashed_lock.py` (Python) share the same lock files + JSON schema, so PS and
Python spawners queue against each other. Inspect/clear:
`py -3.12 re/orchestrator/mashed_lock.py status` / `... break`.

## What it automates vs what stays human

| step | who | why |
|---|---|---|
| build.bat, STATE batch, Stalker sweep | **pipeline** (scripted) | pure shell, no judgment |
| brief -> `.cpp` authoring | human/claude3 | RE judgment |
| re-classify + commit | human + skill | evidence-gated, deliberate |

The pipeline produces the **evidence + a GREEN candidate list**; promotion is a
separate, deliberate step (never auto-committed).

## Usage

```powershell
pwsh -File re/orchestrator/exec_pipeline.ps1 -Queue re/orchestrator/exec_pipeline/exec_queue.json [-Force]
```

Output → `re/orchestrator/exec_pipeline/runs/<timestamp>/`: per-job `.log` +
`manifest.json` (jobs, per-job verdicts, `promotion_candidates`).

## Job types

- `{ "type": "build" }` — rebuilds both targets; a failed build HALTS the pipeline
  (later verify jobs need the fresh `.asi`).
- `{ "type": "state_batch", "id", "hooks":[...], "scenario", "dwell", "sentinel", "round"?, "repeat_first"? }`
  — one boot, many hooks; parses `GREEN X/Y` + per-hook verdicts into
  `promotion_candidates` (the GREEN hook names).
- `{ "type": "stalker_batch", "id", "targets_file" | "targets":[...], "dwell" }`
  — arm-all Stalker sweep; parses `captured` / `reachable-now`.

## The full loop (both tiers)

1. **read-fleet** (account2, wide) → briefs: arg_type, mutator routing, port drafts.
2. **human/claude3** → author `.cpp` + registry entry from the briefs.
3. **exec-pipeline** (account3, narrow) → build + verify → GREEN candidate list.
4. **human + re-classify skill** → promote + commit the verified hooks.

Steps 1 and 3 are automated and off/on the right account; 2 and 4 are the
judgment points that stay with the human.
