---
name: roadmap-loop
description: L0 conductor for the Mashed roadmap loop (re/orchestrator/ROADMAP_LOOP_PLAN.md). Walks the open roadmap steps in the ledger, spawns claude2 section leads that spawn claude3 step workers, runs the claude3 merge train into orch/integration, files user decisions, and stops with a kickoff. Event-driven on child notices with a 1800 s heartbeat. Triggers on "roadmap loop", "run the conductor", "L0 conductor", "next roadmap run", "/roadmap-loop".
---

# roadmap-loop: the L0 conductor playbook

You are L0. You schedule; you never implement, never merge, never read a RESULT file, plate,
log or transcript beyond a child's REPORT block. Your context grows by `roadmap.ps1 status`
(<=2 KB) and REPORT blocks (<=10 lines) only. That is the whole point of the design.

Paths: `ORCH = C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration`.
Ledger: `pwsh -NoProfile -File $ORCH\re\orchestrator\roadmap.ps1 <cmd>` (live dir `log\orch`).
Rules every child reads: `$ORCH\re\orchestrator\WORKER_RULES.md`. Decisions: `log\orch\DECISIONS.md`.

## Start of a run
1. `roadmap.ps1 status` and `mcp__happy__list_children`. Adopt nothing you did not spawn this
   run: a live row whose owner is not one of your children and whose `updated` is older than
   2 h goes back to `open` with `set <id> open "orphaned by run N-1"` (its branch stays).
2. Count open rows in the `## Loop decisions` and `## Phase 0` tables of DECISIONS.md. If any
   were answered since the last run, re-open their rows (`set <id> open "<answer>"`).
3. Plan the wave: group the `ready` rows by `section`, lowest wave first. One lead per section.

## Caps (decision 5)
- 3 live leads, 4 live workers in total, 8 workers per run. Leads get at most 2 workers each.
- A `solo` row starts only when no other worker is live. Tell its lead so.
- Exactly one merge train child at a time.

## Spawning (decisions 1 and 2): always the auto-permission pattern
1. `spawn_child` with `account`, `model`, `effort` and the holding prompt
   `"Roadmap loop <role> for <section>. Reply with the single word READY and stop. Your real
   task arrives in the next message. Do not message any other session yourself."`
2. Then `send_to_session ... yolo: true` with the task. **Every** later message also carries
   `yolo: true`.

| role | account | model | effort |
|---|---|---|---|
| L1 section lead | claude2 | sonnet | medium |
| L2 worker, RE / port / Ghidra / Frida | claude3 | claude-opus-5-5 | high |
| L2 worker, text-only / tracker leg | claude3 | claude-sonnet-5-5 | medium |
| L1 merge train | claude3 | claude-sonnet-5-5 | medium |

Check the child's real model with `read_session` (limit 3) once after spawning; if it was
clamped down, respawn with the alias.

### Lead task (send with yolo)
```
You are the L1 lead for section <section>, run <N>. Read <ORCH>\re\orchestrator\WORKER_RULES.md first.
Steps (ledger ids): <ids>. Ledger: pwsh -NoProfile -File <ORCH>\re\orchestrator\roadmap.ps1 get <id>.
You never implement and never merge. For each step, in dependency order, at most 2 workers live:
claim it for the worker you spawn (roadmap.ps1 claim <id> <worker-session-id> -Parent <your-session-id>),
spawn the worker on claude3 with the holding prompt, then send its task with yolo:true:
"<step task: the row's title + next + evidence, the worktree/branch name orch/<id>, WORKER_RULES.md>".
Model: claude-opus-5-5 high for RE/port work, claude-sonnet-5-5 medium for text-only steps.
When a worker reports: GREEN -> read its RESULT file, accept it, roadmap.ps1 enqueue <id> -Branch orch/<id> -Sha <sha>.
RED/BLOCKED -> set the row blocked with the reason. DECISION -> roadmap.ps1 decision <id> "<question/options/evidence/recommendation>".
Answer a worker's question yourself when it follows from the task or a project rule (send with yolo:true); a
genuine user decision goes to roadmap.ps1 decision. Close every worker you spawned. Never message me yourself.
Final message: one REPORT block, <step-id> = <section>, next: = one line per step id with its stage.
```

### Merge train task (send with yolo)
```
You are the merge train, run <N>. Read <ORCH>\re\orchestrator\WORKER_RULES.md. Run, with the sandbox disabled and a
600000 ms timeout: pwsh -NoProfile -File <ORCH>\re\orchestrator\merge_train.ps1
Repeat until it prints "branch: (nothing queued)" or a RED. Do not fix anything. Final message: the last REPORT block it printed.
```

## The event loop
Each child notice ("Child session ... finished its turn ... Its final reply: ...") is a hint,
not a fact. Notices lag and duplicate; ignore notices from closed or already-collected children.
Per notice:
1. `roadmap.ps1 status`.
2. Read the REPORT block in the notice. Do not open what it points at.
3. Act, one of:
   - lead REPORT -> `close_child` it; spawn the next lead if a section has ready rows and caps allow.
   - merge train REPORT -> close it; if the queue is not empty, spawn a new one.
   - queue non-empty and no merge train live -> spawn the merge train.
   - a child idle with no REPORT -> classify with `read_session` limit 2 (ORCHESTRATOR.md
     "Driving child sessions"): ASKING -> answer (yolo) or file a decision; WEDGED (frozen >=2
     polls, idle >5 min, last message a partial action) -> `stop_child` and respawn the same task;
     DONE without a REPORT -> send "Reply with your REPORT block only." (yolo).
   - a new `decision` row -> batch it; one `PushNotification` per batch, never per row.
4. `ScheduleWakeup` 1800 s as the heartbeat only (wedged children). Never poll faster.

## Stop conditions (section 11)
Stop when any holds: every section dry or parked on decisions; 8 workers spent; two event
rounds with no ledger transition (`events.jsonl` unchanged); your context past ~60%.
On stop: close every child (`list_children`, then `close_child` each), `roadmap.ps1 status`,
`roadmap.ps1 snapshot "orch: ledger snapshot run <N>"`, then write the kickoff below.
`race/first-frame-parity` moves only when Mariano says. Never push. Never redeploy the shared .asi.

## Kickoff template (end of every run)
```
/loop Roadmap loop run <N+1>. Read C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/.claude/skills/roadmap-loop/SKILL.md and act as the L0 conductor it describes.
Last run: <merged ids> merged into orch/integration @ <sha>; <blocked/red ids>; decisions open: <ids>.
Start with roadmap.ps1 status. Sections to lead first: <sections with ready rows>. Caps 3 leads / 4 workers / 8 per run.
```
