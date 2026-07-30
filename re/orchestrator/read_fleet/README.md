# claude2 read-fleet supervisor

A supervisor that loops a queue of **read-only** analysis units, each run as a
detached `delegate.ps1` worker on the Accenture worker account (account2), with
bounded concurrency, per-unit timeout + retry, and a cost/status manifest.

This is the **wide, off-quota, read-only** tier of the two-tier orchestration
design (`re/analysis/plans/methods_efficiency_scorecard_2026-07-29.md`). It
feeds briefs to the narrow claude3 execution pipeline (Ghidra/Frida/build/git),
which is machine-bound and must stay serialized/coordinated.

## Why delegate.ps1, not spawn_child

Both can run a claude2 child, but for an autonomous loop `delegate.ps1` wins:

| | `delegate.ps1` (this fleet) | `spawn_child` (MCP) |
|---|---|---|
| timeout | **hard `-TimeoutSec`** → returns TIMEOUT | none → ~50% **silent wedge** |
| scriptable loop | yes (shell) | no (MCP tools, orchestrator-driven only) |
| survives teardown | yes (detached `Start-Process`) | session-bound |
| steerable mid-run | no | yes (`send_to_session`) |

The ~50% silent-wedge on `spawn_child` (memory `feedback-claude2-fleet-spawn-lessons`)
is what makes it unsuitable for unattended looping; the hard timeout here means a
stuck worker is re-queued, never swallowed.

## Usage

```powershell
pwsh -File re/orchestrator/read_fleet.ps1 -Queue re/orchestrator/read_fleet/queue.json `
     [-MaxConcurrent 3] [-MaxRetries 1] [-OutDir <dir>] [-StaggerSec 2] [-PollSec 5]
```

Outputs to `re/orchestrator/read_fleet/runs/<timestamp>/`:
- `<id>.md` — the worker's returned brief (the deliverable).
- `<id>.console.txt` — the delegate status line (account, status, cost).
- `manifest.json` — per-unit status/cost/attempts + totals.

For true unattended autonomy (survives closing this session), launch the
supervisor itself detached:
`Start-Process pwsh -WindowStyle Hidden -ArgumentList '-NoProfile','-File','...read_fleet.ps1','-Queue','...'`.

## Queue format

`queue.json`: `{ description, defaults:{repo,model,timeoutSec}, units:[{id, prompt, model?, repo?, timeoutSec?}] }`.
One unit per read-only lane; the `prompt` must be self-contained.

## Preflight: read-only enforced up front

A claude2 child that hits a permission prompt (any write/build/install/network/
MCP action) **hangs** — bypass-permissions is disabled by managed policy, so no
one can approve it headless. The supervisor therefore runs a **read-only
preflight** (`re/orchestrator/preflight.ps1`, `Test-ReadOnlyPrompt`) on every
unit's prompt BEFORE launching it. A flagged unit is **rejected up front**
(status `REJECTED-NONREADONLY` in the manifest), never launched, so one
mis-authored unit can't stall the fleet.

It is tuned for **few false positives**: it strips negated action clauses first
(so "Do NOT write any file" is a *good* signal), then flags only high-confidence
signals — command invocations (`py -3.12 …`, `pwsh -…`), `git commit/push/…`,
`pip/npm install`, `curl/wget`, `run/boot the batch/game/…`, `decompile`, and
explicit writes to a source/tracker file (`.cpp`, `hooks.csv`, `build.bat`, …).
Mentioning a file *named* `run_diff_*.py` is fine; *running* it is not.

Escape hatches: per-unit `"skip_preflight": true`, or the `-NoPreflight` switch
(not recommended). Validated: 0 false positives on the shipped queues, all
bad-prompt shapes flagged, rejected units never launched.

## The ONE rule: read-only only

Every unit is pure **Read/Grep/Glob + reasoning**. account2 prompt-gates (and
**hangs on**) writes, builds, `py`/bare-script runs, MCP (Ghidra/Frida), and web
(which it *fabricates*). Never put any of those in a unit — route them to the
claude3 execution lane. Good unit shapes:

- `arg_type` inference over an RVA bucket (cite `re/frida/ARG_TYPES.md`; never invent a name).
- Interpreting a committed capture (e.g. a Stalker `*.json`) into a routing table.
- Reading plates and returning mechanical descriptions (no intent-guessing).
- Tracker/CHANGELOG READS and summaries; prior-art cross-reference.

Each unit should tell the worker to **return a compact brief as text** — that
brief is what lands in `<id>.md` and what the orchestrator consumes; keep raw
file contents out of it.

## Validated

2026-07-29: 3-unit queue, `MaxConcurrent=3`, all read-only → **3/3 green in
3m11s** (vs ~9min serial), off personal quota. Briefs were high quality
(arg_type TSV with NEEDS_NEW_HANDLER shapes; mutator global-vs-heap routing;
mechanical plate descriptions).
