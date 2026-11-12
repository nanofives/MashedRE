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
0. `roadmap.ps1 run <N>` (the run number of your kickoff). Every claim / set / enqueue / decision / add from then on
   stamps the row with it (`run`, `runs`), so `roadmap.ps1 list -Run <N> [-Stage merged]` counts what a run moved.
   Children inherit it through the ledger and pass nothing.
1. `roadmap.ps1 status` and `mcp__happy__list_children`. Adopt nothing you did not spawn this
   run: a live row whose owner is not one of your children and whose `updated` is older than
   2 h goes back to `open` with `set <id> open "orphaned by run N-1"` (its branch stays).
2. Count open rows in the `## Loop decisions` and `## Phase 0` tables of DECISIONS.md. If any
   were answered since the last run, re-open their rows (`set <id> open "<answer>"`).
3. Plan the wave: group the `ready` rows by `section`, lowest wave first. One lead per section. Skip rows whose
   note starts "FROZEN by ROADMAP v5" (hand-path work, L-127): they are not planned until Mariano names one.

## Row kinds (ROADMAP v5, L-127, 2026-11-11)
Tell each lead the kind of every row it gets, and put the kind in the worker task. Full table: WORKER_RULES.md
"Row kinds and what the worker runs".
- **Top-down row** (touches only PortTop wrappers, bindings, `re/match` sources or other files the default path does
  not execute): the worker runs both builds (the default VS2022 build and the object build, `set MASHED_DDKOBJ=1`),
  `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` (IDENTICAL for every substituted 100% source; a refused unit keeps
  its VS2022 body and the RESULT names the reason), the knob boot, its self-test, and the menu walk when menu code
  changed. **No G-CTRL, no B, no 12 captures x 4 in the worker**: those hashes are computed with the knob off and
  cannot see the change. The merge train runs the default gates once per batch.
- **Race-path row**, once `RO-ORACLE-GATE` exists: the above plus the replay oracle on the object build (L-125).
- **Default-path row**: as before, default gates unchanged on the worker's exe, PREREG for a behaviour change.
- **Match-only, tracker and text rows**: no build, no gates, a text-only train pass.
- **RenderWare (L-124)**: on the top-down path the route is the matched RW core on d3d9. A brief that adds a new librw
  binding there is wrong: rewrite it before spawning.
- **Unmatched bodies (L-126)**: `re/tools/match/equiv.py` decides. EQUIV in default mode, or EQUIV-IF-CALLERS-IGNORE
  with every caller scanned as ignoring (exposed-frame rule A1' included), is admitted with no decision row and
  recorded in `ADMITTED.tsv`. A NOT-PROVEN source may replace a hand stand-in on the knob path only as a listed
  provisional body (`PROVISIONAL.tsv`), never counted toward the port DoD. Neither status is C4 (L-128: C4 only by
  a route of `re/CONFIDENCE.md`). Still a decision row: a conditional verdict with a caller that reads or is
  unknown, any proof that needs `--relax-icall`, any x87 instruction-order admission.
- **Standing classes (L-127)**: P1, a flip on the knob path whose captures and logs are unchanged ON against OFF on
  OFF / ON / OFF / ON passes, and P2, a refile of a function to another subsystem on identity evidence, land
  without a decision row. Do not file `roadmap.ps1 decision` for them: append one line to `log\orch\DECISIONS.md`
  ("auto, class P1" or "auto, class P2", row id, evidence path) so Mariano can reverse it. A flip on the default
  path, and anything that moves a gate hash, is still a decision row.

## Caps (decision 5)
- 4 live leads, 6 live workers in total, 12 workers per run. Leads get at most 3 workers each
  (Mariano 2026-10-29, raised from 3 / 4 / 8 / 2).
- Test slots are machine-wide (LIVING-PC: 8 since 2026-11-09, shared with SimGolf / SimCity, with the memory gate
  as the real limit: each run is capped at 8 GB and waits for free memory before it starts; workspace `CLAUDE.md`).
  Text-only and static legs take no slot; prefer them when `test-slot --status` shows the slots full. A train's D2 arms still need the machine alone.
- Keep the pool fed: when the ready rows cannot fill the live-worker cap, spawn a scoping lead that cuts new
  standalone or text-only steps into the ledger (roadmap.ps1 add) from ROADMAP.md, DEFERRED.md and the open
  RESULT follow-ups. It ports nothing itself.
- A `solo` row starts only when no other worker is live. Tell its lead so.
- Exactly one merge train child at a time.

## MATCH section: C4 by byte match (Mariano 2026-10-31; C4 meaning changed by L-93, 2026-11-07)
- Its own cap on top of the caps above: one MATCH lead, **+3 live / +8 workers per run**. MATCH workers do not count
  against the 6 / 12. Static legs (select, g2s, match, permute) take no game lock and no test slot, but each holds one
  Ghidra pool slot and runs compiles. Port-swap legs build and go through the train like any source row.
- Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 in `tools/vc7ddk` (gitignored). Game code flags `/O2 /GS /Ob1`, as
  C, x87 (`/G5` breaks game code). Lua 4.0 `/O2 /Ob1 /G5`, qhull 2002.1 `/O2 /G5`. Tools in `re/tools/match/`.
- **C4 by match certifies the reverse engineering, not the port (L-93, Mariano 2026-11-07).** A row is C4 by
  `match` when its reconstructed source is byte-identical under `re/tools/match/match.py` with that compiler and the
  recorded flags, AND `relocheck.py` reports OK, AND it is not a naked inline-asm transcription nor a body that is only
  inline `__asm` inside a C frame (SimGolf G26). Nothing else: no exe body, no exe caller (L-68 / L-84 / L-89), no gate
  run. Library identity (`libid`) is C4 when the function is identical to one library function (libmatch, one
  candidate). Before L-93 both routes also required the port to use the source and the gates to stay unchanged (see
  `re/CONFIDENCE.md`, "What C4 meant before 2026-11-07").
- Promotion goes through `py -3.12 re/tools/c4_reclass.py batch` (dry run, then `--apply --result <RESULT>`): it
  recompiles each source, re-runs relocheck and writes all passing rows in one transaction. No per-slice reclass
  scripts.
- Game-code slices (`MATCH-Snn`) are contiguous RVA ranges of about 100-150 functions: MSVC lays functions out per
  object file and Ghidra's register-passed args come from earlier static callees in the same file. One slice per
  worker: select, g2s, match, bounded permute, relocheck, re-classify C4 with the batch tool. A match-only slice needs
  no build and no gates.
- Swapping a matched body into `mashed_re.exe` is **port-axis** work (the `hooks.csv` `port` column, none / body /
  called, `scripts/backfill_port.py`): it builds, runs the gates and goes through the train like any source row. A body
  swap that moves a gate hash is a real port divergence: file it as an L-n decision, not a failure.

## Spawning (decisions 1 and 2): always the auto-permission pattern
1. `spawn_child` with `account`, `model`, `effort` and the holding prompt
   `"Roadmap loop <role> for <section>. Reply with the single word READY and stop. Your real
   task arrives in the next message. Do not message any other session yourself."`
2. Then `send_to_session ... yolo: true` with the task, **only after the child's READY turn
   has gone idle** (its READY notice arrived, or `read_session` limit 1 shows it idle).
   **Every** later message also carries `yolo: true`.
3. **Pickup check.** Within 5 minutes of sending the task, confirm the child picked it up:
   its `latestSeq` climbs past the READY turn (`read_session` limit 1 or 2), or its ledger row
   is `claimed` / `working`. If not, re-send the same task once with `yolo: true`. If it still
   has not started, `stop_child` and respawn. (Run 1, 2026-10-24: the merge train answered
   READY, sat idle 23 min with its task unprocessed, and the queue stalled.)

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
"<step task: the row's title + next + evidence, its row kind (top-down / race-path / default-path / text) and what
that kind runs per WORKER_RULES.md "Row kinds", the worktree/branch name orch/<id>, WORKER_RULES.md>".
Row kinds: <id: kind, one per step>. Never claim a row whose note starts "FROZEN by ROADMAP v5".
Model: claude-opus-5-5 high for RE/port work, claude-sonnet-5-5 medium for text-only steps.
When a worker reports: GREEN -> read its RESULT file, accept it, roadmap.ps1 enqueue <id> -Branch orch/<id> -Sha <sha>.
RED/BLOCKED -> set the row blocked with the reason. DECISION -> roadmap.ps1 decision <id> "<question/options/evidence/recommendation>".
Answer a worker's question yourself when it follows from the task or a project rule (send with yolo:true); a
genuine user decision goes to roadmap.ps1 decision. Close every worker you spawned. Never message me yourself.
Final message: one REPORT block, <step-id> = <section>, next: = one line per step id with its stage.
```

### Merge train task (send with yolo)
```
You are the merge train, run <N>. Read <ORCH>\re\orchestrator\WORKER_RULES.md. Run, with the sandbox disabled,
run_in_background and a 7200000 ms timeout (a -Full pass runs past 30 min, and a foreground or default background
timeout kills it): pwsh -NoProfile -File <ORCH>\re\orchestrator\merge_train.ps1 [-Full]
Repeat until it prints "branch: (nothing queued)", a RED, or an INFRA (display asleep or locked, D3D9 hr=0x8876086A:
nothing bounced, the rows stay queued, not a finding). Do not fix anything. Final message: the last REPORT block it printed.
```
On an INFRA report (HB-TRAIN-INFRA-PRECHECK), re-run the train once the display is awake. Do not re-run it in a loop.
`re/orchestrator/INTEGRATION_LOG.md` gets one line per pass that merged or bounced a row (HB-V5-SKILL, 2026-11-11): a
mixed pass names its bounced rows on the merged line, and a pass that bounced rows and merged nothing commits a
"RED, nothing merged" line on orch/integration by itself (a ledger-only tip the next pass folds). An INFRA pass that
bounced nothing writes no line.

**Drain mode (HB-TRAIN-DRAIN, 2026-11-10).** `merge_train.ps1 -Drain` runs the passes itself, back to back, while the
queue is non-empty, and after each GREEN pass whose gate hashes equal the `merge_train.json` refs (or a text-only pass)
it does the autopilot step: `git merge --ff-only orch/integration` in the main checkout (race/first-frame-parity, retried
on index.lock), `git update-ref refs/heads/main <new> <old>`, a check that both equal orch/integration, then
`release_due.py` and on DUE `publish.sh`, waited for before the next pass. It stops with one `REPORT merge-train-drain`
on RED, INFRA, HASH (a hash or a ref moved), FLIP (a decided-flip row in the batch: nothing merged, run it alone with
`-Full` per the flip protocol) or ERROR. A drain that went to the end but set rows aside on a merge conflict (a
conflict-only RED pass, which does not stop it) reports verdict PARTIAL and exits 2, with the set-aside ids at the end
of `next:` (HB-DRAIN-VERDICT, 2026-11-12): hand-merge and re-queue them. It never pushes. Each pass keeps its own priority hold; the driver takes no
lock (wrap it in test-slot if wanted: slot outside, mashed_lock inside). A drain run with `-Full` passes `-Full` to
every pass. Driver log and `pass<N>.result.json` files: `D:/MashedWT/mtrain_data/drain_<ts>/`.

When to pass `-Full` (run 20): a queue that touches `mashedmod/src` or another build input, and every decided
flip, runs WITH `-Full` (a default can live outside `full_set_paths`). A text-only queue (trackers, docs, `verify/`
evidence, `re/` tools that no build reads) runs WITHOUT it. `-Full` on a text-only row runs D2 x3 and the 13 tracks
for nothing and holds `mashed_lock` past an hour. Queue the two kinds in separate passes. Known gap: the train's own
text-only check counts any `.py` under `re/tools/` or `verify/` as a build input, so such a pass still builds and
runs the default gates (kept on purpose: gate scorers live there).
Since HB-TRAIN-MATCH-TEXTONLY (2026-11-10) the train applies this itself: `-Full` counts only when the batch holds a
decided-flip row (id `-FLIP`/`FLIP-` or "decided flip" in the title) and is dropped with a log line otherwise
(`full_set_paths` still bring the full set in); a text-only batch skips build and gates even under `-Full`; and a
`re/match/**` file is text-only unless the build reads it (an `.rsp` entry or an `#include` from mashedmod, about 790 of
4035 files on 2026-11-10). `-ForceFull` keeps the old `-Full`. So passing `-Full` on a mixed queue costs nothing.

### Decided default flips (MERGE_TRAIN.md "Decided default flips")
A row that flips a default on a user decision changes the gate hashes. It records its expected new
G-CTRL/B hashes, merges alone, and L0 commits those hashes to `merge_train.json` on orch/integration
(no train live) and spawns the train for it right away: refs change + flip merge are one step, nothing
merges in between (dequeue every other row first, re-enqueue after the flip lands). GREEN = the merged
tree reproduces exactly the new hashes. The flip's train pass always runs with `-Full`, so D2 x3 and the
13 tracks run even when the default lives outside `full_set_paths`.
(Run 2: D3-CF-U9779 bounced RED with exactly its expected hashes because the refs were stale.)

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

**Never call `wait_for_children` or `read_session` without `limit` at L0.** Both echo the child's
transcript (and your own task text) into your context. In the Phase 0 dry run one
`wait_for_children` on the lead returned ~6 KB, 20x its REPORT block. End your turn and let the
automatic notice wake you. If you must block inside a turn, poll the ledger row instead
(`roadmap.ps1 get <id>` in a loop, or a small poller that prints stage changes only), then
`read_session` with `limit: 1` for the REPORT.

## Stop conditions (section 11)
Stop when any holds: every section dry or parked on decisions; 8 workers spent; two event
rounds with no ledger transition (`events.jsonl` unchanged); your context past ~60%.
On stop: close every child (`list_children`, then `close_child` each), `roadmap.ps1 status`,
`roadmap.ps1 list -Run <N>` (its last line is the run's count by stage, for the kickoff),
`roadmap.ps1 snapshot "orch: ledger snapshot run <N>"`, then write the kickoff below.
`race/first-frame-parity` moves only when Mariano says. Never push. Never redeploy the shared .asi.

**LAN release (Mariano 2026-10-29).** Right after `race/first-frame-parity` is fast-forwarded, whoever did it runs
`py -3.12 scripts/lan_release/release_due.py`. On DUE (3 commits changing `mashedmod/` since the published build,
or 1 once it is 24 h old), run `bash scripts/lan_release/publish.sh` with `run_in_background` and a 7200000 ms
timeout. It builds the ref from a clean `git archive` with `MASHED_NO_DEPLOY=1`, uploads only the changed files to
http://mariano-server.local:8088/mashed/ and deletes the server's old copies. Name the release (label + sha) in the
REPORT. On NOT_DUE do nothing. On ERROR or a failed publish, put it in the REPORT and do not retry in the same run.
Detail: CLAUDE.md "LAN release of the standalone".

## Kickoff template (end of every run)
```
/loop Roadmap loop run <N+1>. Read C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/.claude/skills/roadmap-loop/SKILL.md and act as the L0 conductor it describes.
Last run: <merged ids> merged into orch/integration @ <sha>; <blocked/red ids>; decisions open: <ids>; rows moved: <list -Run N summary line>.
Start with roadmap.ps1 run <N+1>, then roadmap.ps1 status. Sections to lead first: <sections with ready rows>. Caps 4 leads / 6 workers / 12 per run, 3 per lead.
```
