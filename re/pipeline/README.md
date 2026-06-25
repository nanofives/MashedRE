# Autonomous C2→C3 promotion pipeline

A self-driving promotion loop. One **Workflow orchestrator** runs each round; the
**worker agents are ephemeral** (their Frida/Ghidra transcripts never enter the
orchestrator's context); all **state lives in flat files** (`hooks.csv`,
`PROMOTION_QUEUE.md`, `LEDGER.tsv`). That split is the whole point: long promotion
sessions get fresh, disposable context windows, and the orchestrator stays tiny.

```
ROUND (one Workflow run, re/pipeline/promote_pipeline.workflow.js):

  Preflight  1 agent   anchor + d3d9 shim + clean baseline build. Fails closed.
  Plan       1 agent   promote_frontier.py + promote_enrich.py + pipeline_plan.py
                       → re/pipeline/rounds/round_<N>_plan.json  (NO MASHED)
  Execute    N workers  one worktree-isolated agent per candidate: read decomp →
                       confirm signature → author .cpp+registry → build → run_diff
                       → verdict {promoted|queued|failed}. Concurrency capped by
                       frida_pool + runtime. Workers DO NOT touch hooks.csv.
  Collect    1 agent   (autoMerge) merge GREEN branches behind the INTEGRATION-DIFF
                       GATE: rebuild canonical .asi, run_diff_parallel over every
                       landed hook. RED or broken build → HARD ABORT (git reset to
                       pre-round main). Only GREEN lands; re-classify C2→C3 centrally,
                       move PROMOTION_QUEUE rows Queued→Merged, append LEDGER.tsv.
```

## Tier ↔ primitive map (what each "session" you asked for actually is)

| Role you described | Implemented as | Why |
|---|---|---|
| Plans the roadmap / picks targets | `scripts/pipeline_plan.py` + Plan-phase agent | deterministic file analysis; cheap; runs every round |
| Creates the next batches | `pipeline_plan.py` → `round_<N>_plan.json` | N sessions × K candidates, pre-filtered (drift/queued/score) |
| Executes the batches | Execute-phase worker agents (`isolation:'worktree'`) | one isolated context per candidate = context-window management |
| Receives promotion results | `re/PROMOTION_QUEUE.md` + worker verdicts | **file**, not a live session (a session holding state burns tokens) |
| Status of each promotion session | `re/pipeline/LEDGER.tsv` + `re/analysis/CHANGELOG.md` | **file** round-history ledger |

## Run it

```bash
# 1. PLAN ONLY (safe, no MASHED) — inspect what a round would schedule:
py -3.12 scripts/promote_frontier.py
py -3.12 scripts/promote_enrich.py
py -3.12 scripts/pipeline_plan.py            # → re/pipeline/rounds/round_<N>_plan.json

# 2. ONE OBSERVED ROUND, NO MERGE (validate executors end-to-end, zero main-tree risk):
#    Workflow({ scriptPath: 're/pipeline/promote_pipeline.workflow.js',
#               args: { rounds: 1, dryExecute: true } })

# 3. FULLY AUTONOMOUS (user-selected): self-merges GREEN to main behind the
#    integration-diff gate. Spawns MASHED windows each round.
#    Workflow({ scriptPath: 're/pipeline/promote_pipeline.workflow.js',
#               args: { rounds: 8, autoMerge: true } })
```

To make it *repeat unattended*, wrap step 3 in the `loop` skill (self-paced) or a
`CronCreate` routine. Each firing is a fresh Workflow → fresh orchestrator context;
the ledger carries state across firings.

## Args (`promote_pipeline.workflow.js`)

| arg | default | meaning |
|---|---|---|
| `rounds` | 1 | plan→execute→collect rounds back-to-back |
| `sessions` | 6 | worker sessions per round |
| `perSession` | 5 | candidates per session (30/round) |
| `autoMerge` | true | merge GREEN to main behind the integration-diff gate |
| `dryExecute` | false | author+diff+queue only; never merge (validation mode) |
| `model` | `claude-opus-4-8[1m]` | worker model |

## Safety rails (baked in)

- **Preflight fails closed** — no anchor / no shim / dirty build ⇒ round skipped.
- **Confirmed-arg_type-only** — a worker that can't match an existing arg_type
  returns `queued`, never invents one (the integration sweep RED's on invented
  arg_types that pass locally).
- **Integration-diff gate** — a RED integration diff or a broken canonical build
  triggers a HARD ABORT: `git reset --hard` to pre-round main. A red build never
  survives on main.
- **Loop stops** on preflight fail, supply drained, merge abort, or budget < 80k.
- **PID hygiene** — workers kill only the MASHED PIDs they spawn (multi-session rule).

## The yield ceiling (read before scaling up)

Throughput is bounded by **arg_type coverage, not parallelism** (memory
`project_c2c3_throughput_ceiling`). The planner reports the top `argtype_gaps`
each round: as of round 1, building `cdecl nargs=1 ret=int` would move **195**
functions from `needs_argtype` (1440) into `runnable_today`. The highest-leverage
*meta* action is a periodic round that builds the top-gap arg_type in
`re/frida/diff_template.js` — that, not more workers, is what breaks the ceiling.
