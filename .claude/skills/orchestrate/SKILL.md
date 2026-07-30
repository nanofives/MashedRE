---
name: orchestrate
description: Run one bounded iteration of the two-tier Mashed promotion orchestrator (read-fleet + exec-pipeline) as the Fable-5 judgment layer. With NO arguments it resumes from the ledger (re/orchestrator/state.json) — picks up exactly where the last iteration left off. With arguments, treats them as the directive for this iteration. Triggers on "orchestrate", "run the orchestrator", "orchestrator iteration", "next iteration", "pick up where it left off", "continue orchestration", "/orchestrate".
---

# orchestrate — Fable-5 iterative promotion orchestrator

You are the orchestrator. The two tiers do the heavy lifting; you do the
judgment and hold the state. Run **one bounded iteration**, persist every step,
and end with a resume kickoff — never try to "finish everything." Iterating is
the design (`re/orchestrator/ORCHESTRATOR.md` is the full playbook — read it if
you have not already this session; do not duplicate its detail into your head,
just follow it).

## Step 0 — orient (cheap; do NOT read raw files to reconstruct state)

```
pwsh -File re/orchestrator/orch.ps1 status
pwsh -File re/orchestrator/orch.ps1 next
```

The ledger (`re/orchestrator/state.json`) is the single source of truth and is
always current, because every transition was persisted with `orch.ps1 set`. A
prior run cut off for budget lost nothing — resume from what `status` shows.

## Step 1 — decide this iteration's focus

- **Arguments given** → treat them as the directive (e.g. specific hooks/RVAs,
  "brief the mutator sweep", "re-verify the blocked getters"). Do that, within
  one bounded batch.
- **No arguments (the default)** → **resume from the ledger**. Work the
  pipeline in priority order, taking the first non-empty group:
  1. `verified` → **promote** via the `re-classify` skill (+ commit), then
     `orch.ps1 set <id> promoted`.
  2. `authored` → **verify** via the exec-pipeline (build + STATE batch);
     GREEN → `set <id> verified`, INCONCLUSIVE/RED → `set <id> blocked "<why>"`.
  3. `briefed` → **author** the `.cpp` + registry from the brief (your main
     token spend; cite every RVA), then `set <id> authored`.
  4. `candidate` → **brief** via the read-fleet (off-quota; launch FIRST so it
     runs while you work), then read the brief and `set <id> briefed`.
  Also read any `blocked` notes — many just need a longer dwell or a caller at
  C2, not a fix.

## Step 2 — execute ONE bounded batch (per the playbook)

- Launch the off-quota **read-fleet** brief first (frees wall-clock), then do
  authoring while it runs.
- **read-only → read-fleet** (`re/orchestrator/read_fleet.ps1`, account2). Give
  it EXPLICIT RVA lists, never "derive rows N-M from a TSV" (that timed out).
- **machine-bound → exec-pipeline** (`re/orchestrator/exec_pipeline.ps1`,
  account3, serialized under its machine lock). Never launch two exec runs at
  once; read only the manifest's `promotion_candidates`.
- **Promotion is deliberate**: only via `re-classify` (it gates on evidence).
  Check the leaf caller-gate — a GREEN leaf whose callers are all C1 is QUEUED
  in `re/PROMOTION_QUEUE.md`, not promoted.

## Step 3 — persist + budget-stop + kickoff

- After EVERY state change: `pwsh -File re/orchestrator/orch.ps1 set <id> <stage> "<note>"`.
  This is what makes running out of tokens safe.
- Stop when you have completed one full brief→author→verify cycle OR your
  context is getting heavy. Do not push further.
- Finish by printing `orch.ps1 status` and a ready-to-paste kickoff for the next
  iteration (naming exactly what is pending) — this IS the deliverable.

## Guardrails (from the memories — do not relearn them)

- Never `git add -A`; targeted adds only. Commit is deliberate; do not auto-push.
- Never send a write/build/MCP leg to the read-fleet (account2 hangs on them).
- Never touch `original/`; worktree removal only via `diag.py wt-remove`.
- Kill only PIDs you spawned; the exec-pipeline already handles this + its lock.
- NO-GUESSING: cite every RVA/const when authoring; mark `[UNCERTAIN]` otherwise.
