# Roadmap loop: nested orchestrator plan (DRAFT 2026-10-24, not built)

Goal: one conductor session walks every open roadmap step in a loop. Children solve and
implement the steps in parallel, and the conductor's context stays thin because it only
ever reads the ledger and short fixed-format reports.

Status: **plan only.** Nothing below exists yet except the seed table
(`roadmap_seed_20261024.md`) and the existing hook-level orchestrator (`orch.ps1`,
`read_fleet.ps1`, `exec_pipeline.ps1`, `mashed_lock.py`), which this plan reuses.
Building it needs the decisions in section 10.

## 1. Feasibility, verified 2026-10-24

- **Nesting works.** Smoke test: a claude2 child loaded `mcp__happy__spawn_child`, spawned a
  claude2 grandchild, waited on it with `wait_for_children`, read it with `read_session` and
  closed it with `close_child`. All four calls succeeded two levels down.
- **The loop can be event-driven.** When a child finishes a turn, its parent receives a user
  turn ("Child session ... finished its turn ... Its final reply: ..."). The conductor does
  not have to poll. A `ScheduleWakeup` heartbeat (1800 s) only catches wedged children.
- **Any session can read a child by id**, including a grandchild it did not spawn (the
  conductor read the grandchild's transcript directly). Messaging a non-own child by id was
  not tested. If it works, a fresh conductor can adopt a running fleet from the ids in the
  ledger. Phase 0 tests it.
- **Auto permissions work, on both accounts (probe 2026-10-24).** `spawn_child` has no
  permission-mode field. The working pattern: spawn with a holding prompt ("reply READY"),
  then send the real task with `send_to_session ... yolo: true`. `yolo` applies per message,
  so **every** message to a child (task, answers, follow-ups) carries `yolo: true`, and a lead
  spawning workers must use the same pattern. Evidence, checked from the parent's side, not
  from the children's claims:
  - claude3 and claude2 haiku children each ran a `py -3.12` command with the sandbox
    disabled and wrote a fresh UUID plus `mashed_lock.py status` to a file in the parent's
    scratchpad.
  - Each then ran `git -C <throwaway repo> merge --no-ff` and left a real merge commit.
  - Caveat for claude2: its managed policy disables bypass mode, so these may have run
    because `py -3.12` is allow-listed and because `git -C ... merge` does not start with the
    `git merge` prefix the managed ask rule matches, not because `yolo` bypassed anything.
    So claude2 gets no git mutation, build or game leg (consistent with ORCHESTRATOR.md).
- **Finish notifications lag and duplicate.** In the 2026-10-24 probe, seven notifications
  arrived in one burst after both children were already verified and closed, and the claude3
  child's READY came twice (once by its own `send_to_session`, once as the automatic notice).
  So the conductor treats a notification as a hint only: it re-reads `roadmap.ps1 status`,
  ignores notices from closed or already-collected children, and tells children not to
  message the parent themselves (the automatic notice is enough).
- Superseded: the first smoke test's "NESTED FAIL" was about the grandchild's shell call and
  rested on the `0 tool` counter, which ORCHESTRATOR.md says reads 0 for healthy children.

## 2. What the loop can finish

From the seed (about 45 open rows):

| bucket | rows | loop can drive it? |
|---|---|---|
| Concrete and unblocked (AudioRws 19 fns, D3 carried-forward box-state / R_FLAME / U-9779 / U-9782 / U-9780 / U-9775, D1-R1, D1-R2, HB-1..HB-6, D5-FIRSTRUN, D5-PDOD) | ~20 | yes |
| Parked on a Mariano decision (job twins, savelink2 x4, math-pair flips x3, deliberate splits x2, racepct opt-out, FX tuning, D1-R2 in-race review) | ~13 | **answered 2026-10-24** (`DECISIONS.md`); now concrete steps, and the flips they produce come back as new decision rows |
| Scope not defined (WS-B, WS-F, WS-H, WS-J, D4-UNC-PHYS) | ~5 | needs a scoping step first |
| The D4 gate itself: every subsystem S-DONE, ~1,788 undiscovered race-closure RVAs | 1, very large | yes, as a capped breadth section that runs the existing `/orchestrate` lane |
| D5 items that wait on D4 (D5-SHIM, D5-KNOBS) | 2 | later waves |

One run will not finish the roadmap. A realistic run clears 4-8 concrete steps and leaves a
kickoff. The decision bucket is the biggest lever: answering it up front roughly doubles the
unblocked work.

## 3. Topology

```
L0 conductor (claude3, opus-5-5 high, this kind of session)
 |  reads: roadmap.ps1 status (~2 KB) + child REPORT blocks (<=10 lines each)
 |  owns: ledger, decision queue, wave scheduling, concurrency caps
 |
 +-- L1 section lead (one per roadmap section, <=3 live)
 |    |  decomposes the section into steps, orders by deps, reviews RESULT files,
 |    |  answers or escalates child questions, enqueues reviewed branches to the merge train
 |    |  never implements, never merges
 |    |
 |    +-- L2 step worker (one per step, <=2 per lead, <=4 live in total)
 |         own worktree + private root (D:/MashedWT/<step>_root) + Ghidra pool slot
 |         brief -> author -> path1/path2 -> gates on its own exe -> RESULT file -> commit
 |         read-only legs go to account2 via delegate.ps1 / read_fleet.ps1 (off-quota, cannot wedge)
 |
 +-- L1 merge train (exactly one, claude3)
      merge_queue.json FIFO -> temp worktree -> merge -> build -> lint -> gates -> ff orch/integration
```

| tier | account | model / effort | why |
|---|---|---|---|
| L0 conductor | claude3 | opus-5-5 high | judgment, decisions, scheduling |
| L1 section lead | claude2 (decision 1) | sonnet, medium | coordination is mechanical |
| L2 step worker, RE / port | claude3 | opus-5-5 high | needs Ghidra MCP, builds, Frida, boots |
| L2 step worker, mechanical (tracker leg, text-only D5 rows) | claude3 | sonnet-5-5 medium or haiku-5-5 low | |
| L1 merge train | claude3 | sonnet-5-5 medium | runs scripts, reads verdicts |
| read legs | account2 worker | opus-5-5 (pinned in accounts.json) | off this quota |

The account column conflicts with the workspace rule "every spawn_child runs on claude2"
(Proyectos/CLAUDE.md, 17 Oct). claude2 has no Ghidra MCP (`re/ACCOUNT2_CAPABILITIES.md:23`)
and cannot bypass permission prompts, so L2 work there would stop at the first build or
game launch. Decision 1 in section 10.

## 4. State

- **`re/orchestrator/roadmap.json`** (new) with **`roadmap.ps1`** (new: `status`, `next`,
  `claim <id> <session>`, `set <id> <stage> "<note>"`, `fleet`). Separate from `state.json`,
  which stays the per-hook promotion ledger that `/orchestrate` drives.
- Row fields: `id, section, title, stage, owner, parent, branch, worktree, evidence, deps,
  needs, solo, decision, note, updated`.
- Stages: `open -> claimed -> working -> review -> merge-queued -> merged -> done`, plus
  `blocked`, `decision`, `red-merge`, `handoff`.
- `claim` is atomic (file lock), so two leads never take the same step.
- Children write their own transitions. The conductor never reconstructs state from files.

## 5. Contracts

**REPORT block** (the only thing a child's final message may contain):
```
REPORT <step-id>
verdict: GREEN | RED | BLOCKED | DECISION | HANDOFF
branch: orch/<step-id> @ <sha>
evidence: verify/<lane>_<yyyymmdd>/RESULT_<step>.md
gates: G-CTRL <h1/h2/h3>  B <h1/h2/h3>   (or "not run: <why>")
decision: <one line, or none>
next: <one line>
```

**`re/orchestrator/WORKER_RULES.md`** (new, ~2 KB): the guardrails every child reads instead
of the 235 KB NEXT_SESSION.md. Kill only own PIDs; `diag.py wt-remove` only; never touch
`original/`; never merge into the integration branch; never flip a default (behaviour
changes ship as a default-OFF knob + committed PREREG); never redeploy
`original/mashed_re_dev.asi`; trackers only through `re-classify`; no C4 without
canonical-scenario evidence with the hook installed; piz-collision crashes (U-9677) are
re-runs, not findings; game launches need the sandbox disabled; `git -c merge.autoStash=false`;
heavy context -> write the handoff into the RESULT file and report `HANDOFF`.

**L2 step worker:** claims its row, works in its own worktree, ends with REPORT. Does not
close itself (an ASKING child that self-archives loses its question). Its lead closes it.

**L1 section lead:** spawns at most 2 workers, classifies idle children with the existing
ASKING / DONE / WEDGED table (`ORCHESTRATOR.md` "Driving child sessions"), auto-answers what
follows from the task, writes genuine user decisions into the decision queue, enqueues
GREEN branches, closes every child it collected. Ends with one REPORT for the section.

**L0 conductor:** per event: `roadmap.ps1 status`, then one of spawn lead / answer /
respawn wedged / enqueue / park. Never opens a RESULT file, plate, log or transcript beyond
the REPORT block.

## 6. Serialization (what parallelism cannot touch)

- **Game boots** already queue on `mashed_lock.py`. Parallel workers overlap only their
  non-game phases, so the cap of 4 live workers is about CPU and quota, not the GPU.
- **Solo steps** (D2 arms, anything wall-clock bound, memory `d2-arms-are-wall-clock-bound`):
  ledger flag `solo: true`. Needs a new exclusive mode in `mashed_lock.py`, which today has
  only the FIFO lock. The conductor starts a solo step only when no other worker is live.
- **Merge train, gate-before-ff.** Merge 1-3 queued branches in a temp worktree, build both
  targets, `lint_rva_bodies.py` 0 NEW, then G-CTRL + B on the merged exe. The merge criterion
  is **default hashes unchanged** (behaviour changes are knob-OFF by rule). Full set ((b) 9/9,
  (e) 3/3, D2 3/3, 13 tracks) when a branch touches `Vehicle/`, `Ai/` or `Collision/`, run
  solo. RED on a batch: re-merge one at a time to find the branch, bounce it as `red-merge`.
  Layout-sensitive track flips follow memory `sim-trajectory-is-code-layout-sensitive`.
- **Integration branch** `orch/integration`, cut from `race/first-frame-parity` at loop start.
  `race/first-frame-parity` moves only on Mariano's word (current rule, NEXT_SESSION.md:64-65).
- **Single writers:** master Ghidra (ghidra-sweep only, scheduled as its own step),
  `xtwin.py`, trackers through `re-classify` on the lane branch with hooks.csv conflicts
  resolved by the merge train using the frida-sweep recipe.
- **Conflict magnets:** lanes never edit `ROADMAP.md` or `NEXT_SESSION.md`. The merge train
  writes one status line and one START HERE block per GREEN fast-forward, from the RESULT files.

## 7. Decision queue

- `re/orchestrator/DECISIONS.md`: id, question, options, evidence path, what it unblocks,
  recommendation. The step goes to stage `decision` and the conductor schedules other work.
- One `PushNotification` per batch of new decisions, not one per decision.
- Mariano answers in the conductor chat or in the file. The conductor re-opens the unblocked rows.

## 8. Sections and wave order

| wave | section (L1) | steps |
|---|---|---|
| 0 | Decision sweep | **DONE 2026-10-24**, all answers in `DECISIONS.md` (W0-1..W0-16) |
| 1 | Audio link | D4-AUDIO-RWS (pool/list family first; AudioContextLookup after 0x005ac930), D4-AUDIO-REST survey, W0-1 BankPath shared core, W0-2 VolToDs split + knob |
| 1 | D3 carry-forward | D3-CF-BOX, D3-CF-RFLAME, D3-CF-U9779, D3-CF-U9782, D3-CF-U9780; D3-CF-U9775 solo (D2 arms) |
| 1 | Harness | HB-1 (unblocks HB-6), HB-2, HB-3, HB-CHUNK |
| 1 | Save | W0-3 option B with the declared guard; W0-5 hooks.csv note (re-classify) |
| 2 | Dual-copy | W0-10 0x004282a0 and W0-11 0x00410510 consolidations (P4), W0-12 delete the racepct opt-out; W0-7 / W0-8 / W0-9 math-pair knobs + PREREG + re-measure (solo, D2 arms); D4-DC-77b0 after its input producers |
| 2 | D1-residue | W0-14 D1-R2 wire behind a knob, then Mariano's in-race review; D1-R1 frame-synced capture |
| 2 | Collision FX | W0-13 D4-FX-GATES: derive the skid gates from the original |
| 2 | Scoping | WS-B / WS-F / WS-H / WS-J: one text-only worker each, output = concrete step rows |
| 3+ | Breadth (S-DoD) | lead runs `/orchestrate N` per wave; HB-4 discovery drains; capped per run |
| late | D5 | D5-PDOD and D5-FIRSTRUN any time; D5-SHIM, D5-KNOBS, D5-DEPS after D4 |

## 9. Phase 0: build list (one claude3 session)

1. Verify every seed row against the files (account2 worker), then write `roadmap.json`.
2. `roadmap.ps1` (status / next / claim / set / fleet).
3. `WORKER_RULES.md` and the REPORT format.
4. `merge_train.ps1` (reuse frida-sweep's conflict recipe and the existing gate scripts).
5. Exclusive mode in `mashed_lock.py` + `MachineLock.ps1`.
6. Probe: a claude3 child under `yolo` runs `build.bat` in a worktree and one game launch.
   (Shell, sandbox-off and `git merge` already passed on 2026-10-24, section 1.)
7. Skill `.claude/skills/roadmap-loop/SKILL.md` (the L0 playbook). New skill, needs approval.
8. Dry run of the whole chain on one cheap text-only step (D5-PDOD): L0 -> L1 -> L2 ->
   merge train -> ff. Check that the conductor's context grew by REPORT blocks only.

### 9a. Phase 0 design notes (constraints found while planning)

- **Happy forces every child into the conductor's folder** (`spawn_child`: "the child is
  forced to run in the SAME folder as this orchestrator session"), which is the main checkout
  on `race/first-frame-parity`. Children must `cd` into their own worktree before any write,
  and nobody switches the main checkout's branch (other sessions poll its index).
- **`orch/integration` lives in its own worktree** at a fixed path (created with the worktree
  skill, removed only with `diag.py wt-remove`). Phase 0 commits the scaffolding there, so
  step worktrees cut from `orch/integration` contain the scripts.
- **The ledger is one file at one absolute path** that every tier and every worktree uses,
  not a tracked copy per branch (per-worktree copies would diverge). Same for
  `DECISIONS.md` and `merge_queue.json`. The conductor commits snapshots.
- Local only: no push of any branch (W0-16). No redeploy of `original/mashed_re_dev.asi` (W0-15).

## 10. Decisions (settled 2026-10-24)

All five **DECIDED 2026-10-24 (Mariano)**:

1. **Accounts: split by job.** L2 step workers and the merge train on claude3. L1 section
   leads on claude2 (their legs are MCP, reads and `py`/`pwsh` ledger scripts, which ran
   unattended in the probe). Deliberate exception to the workspace "every child on claude2"
   rule for Mashed: claude2 has no Ghidra MCP and cannot bypass prompts.
2. **Permissions: children always open with auto permissions** (holding prompt +
   `yolo: true` on every message, section 1).
3. **Merge authority:** GREEN branches merge into `orch/integration` without asking, after
   build + lint + unchanged default gates. `race/first-frame-parity` moves only when Mariano says.
4. **Wave 0: ask the parked decisions now**, before Phase 0, four at a time with options and
   evidence. Answers go in `DECISIONS.md` and re-open the rows they unblock.
5. **Run caps: 3 live leads, 4 live workers, 8 workers per run**, then a kickoff.

## 11. Stop conditions (per run)

All sections dry or parked on decisions; run budget spent; two event rounds with no ledger
transition; conductor context past ~60%. On any of these: close every child
(`list_children`), `roadmap.ps1 status`, write the kickoff. Every transition is already in the
ledger, so a stop never loses work.
