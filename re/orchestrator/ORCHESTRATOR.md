# Fable-5 orchestrator playbook

You are the orchestrator. You run on **Fable 5** because the job is judgment:
decide what each tier should do, read what the children produced, author from
briefs, and decide what gets promoted. The two tiers do the heavy lifting; you
do the thinking and hold the state.

**Run me iteratively.** Each invocation does ONE bounded batch, persists every
step immediately, and ends with a resume kickoff. Running out of tokens never
loses work — the ledger (`state.json`) is always current because every
transition is written the moment it happens.

## The two tiers you command

| tier | account | script | shape | for |
|---|---|---|---|---|
| **read-fleet** | claude2 (off-quota) | `re/orchestrator/read_fleet.ps1` | WIDE, parallel | briefs: arg_type, mutator routing, plate reads — pure Read/Grep |
| **exec-pipeline** | claude3 (this machine) | `re/orchestrator/exec_pipeline.ps1` | NARROW, serialized under a machine lock | build + STATE-batch/Stalker verify |

Read-only work → read-fleet (never spends your quota). Machine-bound work →
exec-pipeline (one at a time; it holds a lock so it can't collide with a manual
Frida run). **Never** send writes/builds/MCP to the read-fleet — account2 hangs
on them.

## Per-iteration algorithm

Spend tokens on steps 2 and 5 (judgment). Steps 1, 3 are dispatch; step 0/6 are
cheap bookkeeping via `orch.ps1` — do NOT read raw child files into your context
when the helper or a manifest already summarizes them (token economy).

0. **Orient** — `pwsh -File re/orchestrator/orch.ps1 status` (and `next`). This is
   your whole situational picture; don't re-read source/plates to reconstruct it.
1. **Brief** (do FIRST, it's off-quota and frees wall-clock) — for `candidate`
   items needing analysis, write a read-fleet `queue.json` (buckets of ~12) and
   launch `read_fleet.ps1` in the background. Launch it BEFORE any long local
   run. When briefs land in `runs/<ts>/<id>.md`, read the BRIEF (not the raw
   files) and `orch.ps1 set <id> briefed`.
2. **Author** (your main token spend) — from a brief, write the `.cpp` +
   `hooks_registry.py` entry + `build.bat`/`asi_sources.rsp` lines. Cite every
   RVA (NO-GUESSING). Pick an `arg_type` that already exists in
   `re/frida/ARG_TYPES.md`; author a new handler only if the brief says
   NEEDS_NEW_HANDLER. `orch.ps1 set <id> authored`.
3. **Verify** — add `authored` items to `exec_pipeline/exec_queue.json` and run
   `exec_pipeline.ps1`. Read only the manifest's `promotion_candidates`.
4. **Classify verdicts** — GREEN → `set <id> verified`; INCONCLUSIVE-both-errored
   → `set <id> blocked "gate not live this run"` (NOT a defect — re-verify when
   the state_gate resolves); RED → `set <id> blocked "<fault>"` and investigate.
5. **Promote** (deliberate, your judgment) — for `verified` items, run the
   `re-classify` skill (it gates on evidence) and commit. `set <id> promoted`.
6. **Budget stop** — when you have completed ONE full brief→author→verify cycle,
   OR your context is getting heavy, STOP. Run `orch.ps1 status`, then write a
   ready-to-paste kickoff naming exactly what is pending and what the next
   iteration should pick up. Do not push past this to "finish everything" —
   iterating is the design.

## How to read each tier's output (don't over-read)

- **read-fleet**: the `<id>.md` brief is the deliverable + `manifest.json` has
  status/cost. A TIMEOUT/ER​ROR unit was already retried once; if still failing,
  `set` its item `blocked` and move on — don't debug the worker.
- **exec-pipeline**: `manifest.json` → `promotion_candidates` is the list that
  matters. `green_tally` X/Y tells you the yield; INCONCLUSIVE ≠ RED.
- **Both are serialized-safe**: the read-fleet detaches its workers; the
  exec-pipeline holds a machine lock. You can launch a read-fleet batch and,
  while it runs, do authoring — but never launch two exec-pipeline runs.

## Parallelism — spawn_child fan-out + the MASHED run queue

You MAY fan work out across parallel child sessions when the backlog and budget
justify it. Decide K yourself: scale it to the number of independent lanes, keep
it small (2–4), and bias hard toward the READ tier.

**Two ways to run parallel work, in order of preference:**
1. **read-fleet (`-MaxConcurrent N`)** — the default for read-only fan-out.
   Off-quota, detached, hard-timeout+retry, no wedge. Widen `MaxConcurrent` when
   the candidate backlog is large. This is where parallelism actually pays.
2. **`mcp__happy__spawn_child`** — full persistent sessions when you need a lane
   to do more than read-and-distill (e.g. a claude3 execution lane, or a
   long-running analysis you want visible/steerable in the app). Rules, learned
   the hard way (do not relearn them):
   - **ALWAYS pass `account` explicitly** (`claude2` or `claude3`). Omitting it
     dies in ~2 min with OAuth-expired.
   - **Smoke-test the transport first**: spawn a 1-line child (e.g. `git
     rev-parse --short HEAD`) and confirm it returns before sending an expensive
     prompt.
   - **Wedge-watchdog**: poll `read_child_output` and compare `latestSeq` across
     polls. The tell is **`latestSeq` FROZEN + idle >5 min** → wedged →
     `stop_child` + re-spawn the same prompt. **Ignore the `(N text, 0 tool)`
     counter** — it reads 0 for perfectly healthy children too. claude2 wedges
     ~50% of the time; that's why read-only work prefers the read-fleet.
   - **Never send a write/build/MCP leg to a claude2 child** — it hangs on the
     first permission prompt.

**The MASHED run queue makes game-bound fan-out SAFE (but not fast).** Every
script that boots MASHED (`run_diff_scenario_batch.py`, `stalker_write_surface*`)
acquires the machine-wide game lock (`re/orchestrator/mashed_lock.py`, mirrored
by `MachineLock.ps1`) before `frida.spawn` and releases after it kills its pid.
So if you spawn several children that each run the game, they **automatically
take turns** — child B's boot blocks until child A finishes, no collision on the
one GPU. Check/clear it with `py -3.12 re/orchestrator/mashed_lock.py status |
break`. Consequence: parallel *game* children mostly overlap only their non-game
phases (each still pays a full serialized boot), so the big parallel win is the
READ tier; treat game-lane fan-out as "safe to launch, not a throughput
multiplier."

## Budget discipline (the whole point)

- One bounded batch per iteration (≈4–8 hooks, or one read-fleet bucket + its
  authoring). Persist after every `set`.
- Delegate every readable leg to the read-fleet — it costs your quota nothing
  and is the #1 cost lever.
- Prefer the manifest/`orch.ps1 status` over raw files; a 3 KB summary in your
  context is re-billed cheaply, 75 KB of plates is not.
- End with a kickoff, not with exhaustion.

## Kickoff template (paste at end of every iteration)

```
Orchestrator iteration N+1. Read re/orchestrator/ORCHESTRATOR.md, run
`pwsh -File re/orchestrator/orch.ps1 status`, then:
- PROMOTE: <verified ids> via re-classify + commit.
- VERIFY: <authored ids> via exec_pipeline.
- AUTHOR: <briefed ids> from runs/<ts>/<id>.md.
- BRIEF: next read-fleet bucket = <candidate ids / next 12 RVAs>.
Do one bounded batch, persist each step with orch.ps1 set, then stop + kickoff.
```
