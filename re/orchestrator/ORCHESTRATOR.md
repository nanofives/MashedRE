# Fable-5 orchestrator playbook

You are the orchestrator. You run on **Fable 5** because the job is judgment:
decide what each tier should do, read what the children produced, author from
briefs, and decide what gets promoted. The two tiers do the heavy lifting; you
do the thinking and hold the state.

**Run me in cycles.** Each invocation works a **cycle budget** (default 4 cycles
— see the `orchestrate` skill, which is authoritative on when to stop), persists
every step immediately, and ends with a resume kickoff. One cycle = one pass of
the priority ladder below. Completing a cycle is not a reason to stop; only a
hard stop is. Running out of tokens never loses work — the ledger
(`state.json`) is always current because every transition is written the moment
it happens.

## The two tiers you command

| tier | account | script | shape | for |
|---|---|---|---|---|
| **read-fleet** | claude2 (off-quota) | `re/orchestrator/read_fleet.ps1` | WIDE, parallel | briefs: arg_type, mutator routing, plate reads — pure Read/Grep |
| **exec-pipeline** | claude3 (this machine) | `re/orchestrator/exec_pipeline.ps1` | NARROW, serialized under a machine lock | build + STATE-batch/Stalker verify |

Read-only work → read-fleet (never spends your quota). Machine-bound work →
exec-pipeline (one at a time; it holds a lock so it can't collide with a manual
Frida run). **Never** send writes/builds/MCP to the read-fleet — account2 hangs
on them.

## Per-cycle algorithm

Spend tokens on steps 2 and 5 (judgment). Steps 1, 3 are dispatch; step 0/6 are
cheap bookkeeping via `orch.ps1` — do NOT read raw child files into your context
when the helper or a manifest already summarizes them (token economy).

Steps 0–6 are ONE cycle. Step 6 loops back to step 0 until the budget is spent.

0. **Orient** — `pwsh -File re/orchestrator/orch.ps1 status` (and `next`). This is
   your whole situational picture; don't re-read source/plates to reconstruct it.
0.5 **Refill if the ladder is empty** — a thin ledger is the #1 reason a run ends
   after one cycle: 12 items, 10 already `promoted`, and the ladder drains in a
   single pass. Pull the next bucket from `re/PROMOTION_QUEUE.md`,
   `re/analysis/plans/`, or an existing `candidate` note, and `orch.ps1 set` the
   RVAs in as `candidate` items. Load buckets of **≥12** — the read-fleet is
   off-quota, so under-loading it costs throughput and saves nothing.
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
6. **Next cycle, or stop** — decrement the cycle budget. Cycles left and no hard
   stop? Go back to step 1 now, without checking in. Budget spent or hard stop
   (context heavy / stop-and-ask fired / two cycles with no transition / ledger
   dry and unrefillable)? Run `orch.ps1 status`, then write a ready-to-paste
   kickoff naming what is pending and which hard stop ended the run.
   If the ladder is empty, **refill it** (step 0.5 below) — that is a cycle's
   work, not a reason to stop.

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
   - **Idle ≠ wedged ≠ done — CLASSIFY before you act** (see "Driving child
     sessions" below). A child waiting on an options prompt is ALSO idle with a
     frozen `latestSeq`; killing it as "wedged" throws away its work. Only
     re-spawn a child that is frozen with **no pending question**.
   - **Never send a write/build/MCP leg to a claude2 child** — it hangs on the
     first permission prompt.

## Driving child sessions — classify before you act

When `wait_for_children until=idle` returns a child (or a poll shows its
`latestSeq` stopped climbing), do NOT assume it's stuck. Read its transcript
tail with `read_child_output` and put it in ONE of three states:

| state | how to tell (read the LAST message) | action |
|---|---|---|
| **ASKING** | ends with a question / an options block, awaiting input | decide → answer, or escalate (below) |
| **DONE** | ends with a completion summary / the result you asked for | collect its output; if `shutdownWhenDone` it self-prunes |
| **WEDGED** | `latestSeq` frozen across ≥2 polls AND idle >5 min AND the last message is a *partial action* — neither a question nor a summary | `stop_child` + re-spawn the SAME prompt |

Ignore the `(N text, 0 tool)` counter for all of this — it reads 0 for healthy
children too. Only WEDGED gets re-spawned; misreading ASKING or DONE as WEDGED
destroys work. (claude2 wedges ~50% of the time — the reason read-only work
prefers the read-fleet, which can't wedge.)

### Answering an ASKING child — decide vs escalate

- **Auto-answer** when the choice follows from the task, the directive you gave
  the child, or a project convention — e.g. "which existing `arg_type` fits?"
  (the brief implies it), "proceed with the obvious next step?", "which of these
  equivalent layouts?". Send the **exact option text** (or a clear directive)
  via `send_to_session`, and record what you chose in the ledger note so it's
  auditable.
- **Escalate to the human** — surface it in your iteration output, do NOT guess
  — when it's a genuine user-decision: an architecture fork, a destructive or
  irreversible action, a scope change, a confidence promotion missing evidence,
  or any choice where two answers have materially different consequences you
  can't adjudicate from the task. This is the project's stop-and-ask rule
  applied to a child's question, not just your own.

### Permission prompts are NOT options questions

A child blocked on a *permission* prompt ("allow this Bash/tool call?") is a
different thing. `send_to_session ... yolo:true` bypasses permission checks, but
only use `yolo` if the user pre-authorized it. On **account2 it is disabled**
(`disableBypassPermissionsMode`) — a claude2 child just hangs on a permission
prompt, so treat that as blocked and escalate/kill, don't wait it out. Prevent
it: keep claude2 children read-only (they never prompt); give claude3 execution
children a permissive mode or pre-authorized `yolo` so they don't stall.

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

- One bounded batch per **cycle** (≈4–8 hooks, or one read-fleet bucket + its
  authoring); several cycles per run. Persist after every `set` — that is what
  makes a multi-cycle run safe to have killed at any point.
- Delegate every readable leg to the read-fleet — it costs your quota nothing
  and is the #1 cost lever.
- Prefer the manifest/`orch.ps1 status` over raw files; a 3 KB summary in your
  context is re-billed cheaply, 75 KB of plates is not.
- End with a kickoff, not with exhaustion.

## Kickoff template (paste at end of every iteration)

```
Orchestrator run N+1, cycle budget 4. Read re/orchestrator/ORCHESTRATOR.md, run
`pwsh -File re/orchestrator/orch.ps1 status`, then:
- PROMOTE: <verified ids> via re-classify + commit.
- VERIFY: <authored ids> via exec_pipeline.
- AUTHOR: <briefed ids> from runs/<ts>/<id>.md.
- BRIEF: next read-fleet bucket = <candidate ids / next 12 RVAs>.
- REFILL from <queue/plan file> if the ladder empties.
Persist each step with orch.ps1 set. Keep cycling until the budget is spent or a
hard stop fires; then status + kickoff.
```
