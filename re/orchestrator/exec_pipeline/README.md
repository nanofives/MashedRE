# claude3 execution-tier pipeline (narrow / serialized)

The machine-bound half of the two-tier orchestration
(`re/analysis/plans/methods_efficiency_scorecard_2026-07-29.md`). It runs a
queue of build/verify jobs **serialized under a machine lock**, parses verdicts,
and emits a promotion-ready candidate list.

## Why narrow (the opposite of the read-fleet)

The read-fleet fans WIDE because read-only children share nothing. This tier is
the reverse: every job touches a **shared machine resource** —

- the **one GPU + game process** (concurrent boots contend — measured 1.12x for
  2 — and simultaneous `frida.spawn` collides),
- the single **build tree / `.asi`**,
- the **git working tree** (one writer),
- **Ghidra pool slots** (one session per slot).

Fan-out doesn't help and actively breaks things, so jobs run **one at a time**
under `exec_pipeline/.machine.lock`. The lock also stops an exec run from
colliding with a manual Frida/Ghidra run: acquire fails if another live PID holds
it (`-Force` breaks a stale lock whose owner is dead).

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
