---
name: orchestrate
description: Run the two-tier Mashed promotion orchestrator (read-fleet + exec-pipeline) as the Fable-5 judgment layer, cycling until the cycle budget is spent (default 4 brief→author→verify→promote cycles, not one). With NO arguments it resumes from the ledger (re/orchestrator/state.json) — picks up exactly where the last run left off. A bare number sets the cycle budget (`/orchestrate 8`); other arguments are the directive. Triggers on "orchestrate", "run the orchestrator", "orchestrator iteration", "next iteration", "pick up where it left off", "continue orchestration", "/orchestrate".
---

# orchestrate — Fable-5 iterative promotion orchestrator

You are the orchestrator. The two tiers do the heavy lifting; you do the
judgment and hold the state. Run **cycles until the cycle budget is spent**,
persisting every step, and end with a resume kickoff.
(`re/orchestrator/ORCHESTRATOR.md` is the full playbook — read it if you have
not already this session; do not duplicate its detail into your head, just
follow it.)

## Cycle budget (read this before you plan the run)

A **cycle** = one pass of Step 1 + Step 2: take the highest-priority non-empty
ledger group, work it, persist the transition. Completing one cycle is **not** a
reason to stop — go straight back to Step 1 and start the next.

- **Budget = 4 cycles** by default. A bare number in the arguments overrides it
  (`/orchestrate 8` → 8 cycles); `until-dry` means run until the ledger has no
  actionable item left.
- **Announce the budget** in your first message of the run ("cycle budget 4"),
  and label each cycle as you enter it ("cycle 2/4: verify argtype_render_backlog").
- **Stop early ONLY on a hard stop** (below). Running low on things you *feel
  like* doing is not one; an empty ledger is — see "Refill" in Step 1.

**Hard stops — end the run, write the kickoff, do not start another cycle:**
1. Context is genuinely heavy (you are compacting, or past ~60% of the window).
2. A `stop-and-ask` trigger fired (CLAUDE.md: architecture fork, missing
   evidence for a promotion, destructive action, tracker conflict).
3. Two consecutive cycles produced **no** ledger transition — you are spinning;
   say so explicitly in the kickoff rather than trying a third.
4. The ledger is dry *and* refilling it needs a decision you cannot make alone.

A cycle that ends in `blocked` still counts as a completed cycle — record it and
move to the next item.

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
  "brief the mutator sweep", "re-verify the blocked getters"). Work it across
  the cycle budget — one cycle per bounded batch — not in a single batch.
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

**Refill (do this the moment the ledger has no actionable item).** A thin ledger
is the #1 reason a run ends after one cycle — the priority ladder drains in a
single pass and there is nothing left to take. Refilling is part of the job, not
a separate task: pick the next bucket from `re/PROMOTION_QUEUE.md` /
`re/analysis/plans/` / the `candidate` notes already in the ledger, add the RVAs
as `candidate` items with `orch.ps1 set`, and spend the cycle briefing them via
the read-fleet (off-quota — widen `-MaxConcurrent` and load **≥12 RVAs**, not 3).
Only if no bucket is obvious does hard-stop #4 apply.

## Step 2 — execute this cycle's batch (per the playbook)

- Launch the off-quota **read-fleet** brief first (frees wall-clock), then do
  authoring while it runs.
- **read-only → read-fleet** (`re/orchestrator/read_fleet.ps1`, account2). Give
  it EXPLICIT RVA lists, never "derive rows N-M from a TSV" (that timed out).
- **machine-bound → exec-pipeline** (`re/orchestrator/exec_pipeline.ps1`,
  account3); read only the manifest's `promotion_candidates`.
- **Promotion is deliberate**: only via `re-classify` (it gates on evidence).
  Check the leaf caller-gate — a GREEN leaf whose callers are all C1 is QUEUED
  in `re/PROMOTION_QUEUE.md`, not promoted.

### Deciding parallelism (you choose K)

- **Read tier — widen freely.** If several candidate buckets are pending and
  budget is healthy, run the read-fleet with `-MaxConcurrent 2..4`. It's
  off-quota and touches no machine resource, so this is where parallelism pays.
- **spawn_child — when a lane needs more than reading.** You MAY
  `mcp__happy__spawn_child` (ALWAYS with an explicit `account`; smoke-test a
  1-line child first). Never give a claude2 child a write/build/MCP leg.
- **Classify an idle child before acting** (idle ≠ wedged ≠ done). Read its
  transcript tail:
  - **ASKING** (ends on a question/options block) → decide and answer via
    `send_to_session` when the choice follows from the task/your directive/a
    convention; **escalate to the human** (surface it, don't guess) for genuine
    user-decisions (architecture forks, destructive/irreversible actions, scope
    changes, evidence-missing promotions). Log what you answered in the ledger.
  - **DONE** (ends on a summary/result) → collect it.
  - **WEDGED** (`latestSeq` frozen ≥2 polls, idle >5 min, last message a partial
    action — NOT a question or summary) → `stop_child` + re-spawn same prompt.
  Only WEDGED gets respawned — killing an ASKING child loses its work. Ignore
  the `(N text,0 tool)` counter. Permission prompts ≠ options: `yolo` bypasses
  them only if pre-authorized, and is disabled on account2 (a claude2 child just
  hangs → escalate). Full detail in `ORCHESTRATOR.md` → "Driving child sessions".
- **Game runs auto-queue.** Every MASHED-spawning script takes the machine-wide
  game lock (`re/orchestrator/mashed_lock.py`), so multiple game-bound children
  take turns instead of colliding — safe to launch, but each still pays a full
  serialized boot, so don't expect a throughput multiplier there. Check/clear
  the queue: `py -3.12 re/orchestrator/mashed_lock.py status | break`.
  Full detail in `ORCHESTRATOR.md` → "Parallelism".

## Step 3 — persist, then LOOP (do not stop here)

- After EVERY state change: `pwsh -File re/orchestrator/orch.ps1 set <id> <stage> "<note>"`.
  This is what makes running out of tokens safe — and it is why looping is safe:
  a run killed mid-cycle 3 keeps everything cycles 1–2 earned.
- Decrement the cycle budget. **If cycles remain and no hard stop fired, go back
  to Step 1 immediately** — same message, no check-in, no "shall I continue?".
  The user asked for the whole budget; asking to continue spends their turn for
  nothing. (A genuine `stop-and-ask` trigger is different — that is hard stop #2.)
- Keep the per-cycle report to ~3 lines (item, verdict, transition). The
  end-of-run summary is the deliverable, not a running narration.

## Step 4 — end of run (budget spent or hard stop)

- Print `orch.ps1 status`.
- Summarise the run: cycles completed, transitions made, anything blocked.
- Write the ready-to-paste kickoff for the next run, naming exactly what is
  pending and which hard stop (if any) ended this one.

## Guardrails (from the memories — do not relearn them)

- Never `git add -A`; targeted adds only. Commit is deliberate; do not auto-push.
- Never send a write/build/MCP leg to the read-fleet (account2 hangs on them).
- Never touch `original/`; worktree removal only via `diag.py wt-remove`.
- Kill only PIDs you spawned; the exec-pipeline already handles this + its lock.
- NO-GUESSING: cite every RVA/const when authoring; mark `[UNCERTAIN]` otherwise.
