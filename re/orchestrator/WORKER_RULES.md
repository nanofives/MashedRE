# Roadmap-loop worker rules (read this, not NEXT_SESSION.md)

Every L1 lead and L2 worker reads this file. Mashed `CLAUDE.md` still applies in full.
Ledger: `pwsh -NoProfile -File <orch-wt>\re\orchestrator\roadmap.ps1 <cmd>` where
`<orch-wt>` = `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration`.
The live ledger is one file for all tiers (`log\orch\roadmap.json`), never a branch copy.

## Sessions and permissions
- Spawning a child: `spawn_child` with a holding prompt ("reply READY and stop"), then the
  task with `send_to_session ... yolo: true`. **Every** later message to it carries
  `yolo: true` too. Leads on `claude2` (model sonnet, effort medium), workers on `claude3`.
- Pickup check (every tier): send the task only after the child's READY turn has gone idle.
  Within 5 minutes confirm pickup: its `latestSeq` climbs past the READY turn
  (`read_session` with limit 1 or 2), or its ledger row is `claimed` / `working`. If not,
  re-send the same task once with `yolo: true`. If it still has not started, `stop_child` and
  respawn it with the same task.
- Never message your parent yourself. Your final message is delivered automatically.
- Do not close yourself. Your parent closes you after it collects your REPORT.

## Where you work
- Happy starts you in the main checkout (`race/first-frame-parity`). **Never switch its
  branch, never commit there.** Create your worktree from `orch/integration` and `cd` into it
  before any write:
  `git -C C:\Users\maria\Desktop\Proyectos\Mashed worktree add .worktrees\<step> -b orch/<step> orch/integration`
- Remove a worktree only with `py -3.12 scripts/diag.py wt-remove <path>`. Never
  `git worktree remove --force`. Never touch `original\`.
- Commit only on `orch/<step>`, targeted `git add <paths>` (never `-A`),
  `git -c merge.autoStash=false` on any merge. No push of any branch.
- Never merge into `orch/integration`. Enqueue for the merge train instead.
- **Evidence: commit summaries, not raw data** (Mariano, 2026-10-25). Commit the RESULT,
  the PREREG, scored tables, hash lists and small extracts (keep each file under ~1 MB). Raw
  per-frame arm data (`*.step.csv`, `*.gates.csv`, `*.ptrace.log`, captures, `.msd`) stays in
  `D:/MashedWT/<step>_data`, and the RESULT lists each cited raw file's path and SHA-256.
  Run 4 committed ~400 MB of raw arm folders. Do not repeat it.

## Builds and game runs
- 12-capture checks (nav-demo walk_01..12) run with `MASHED_DET_MPG_PIN=0`, so the frontend.mpg backdrop freezes on
  frame 0 and the captures do not depend on machine load (PORT-TOP-KNOBON-CAPFLAKE). `knob_boot.py` sets it on
  nav-demo runs unless the caller passes it. Harness only, not the deterministic-mode default.
- Build from your worktree: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`.
  Never redeploy `original\mashed_re_dev.asi` (file a decision row if one is needed).
- Game runs: private root `D:\MashedWT\<step>_root` (robocopy of `original\` with
  `original\` inside), `MASHED_ROOT` pointing at it, sandbox disabled, and wrap the boot in
  the lock: `py -3.12 <orch-wt>\re\orchestrator\mashed_lock.py run --label <step> -- <cmd>`
  (`run --solo` for a wall-clock-bound step such as D2 arms; the window expires after 1 h unless you
  pass `--ttl <s>`, and a new solo open waits while the merge train has a priority claim). Kill only PIDs you spawned.
- **Every** launch of `MASHED.exe` or `mashed_re.exe` goes through `mashed_lock.py run`, not only gate
  arms: ad-hoc boots, knob-ON boots, nav-demo captures, state-log runs. With the test slot, test-slot
  OUTSIDE and mashed_lock INSIDE. And no `build.bat` while `mashed_lock.py status` shows a merge-train
  PRIORITY-HOLD or SOLO: do read/edit work and wait. Reason: on 2026-11-08 two full-set passes bounced on
  D2 line count alone under load from concurrent game runs and builds (PORT-SAVE-01 at 13:10, the
  HB-TRAIN-STACK / PORT-TOP-04A / PORT-STATE-03 batch at 18:29), about an hour of gates each.
- In PowerShell call `py.exe`, not `py` (a `~/bin/py` sh shim hangs pwsh).
- Piz-collision crashes (`MASHED.exe+0x1509b4`, `+0xe66db`, U-9677) are re-runs, not findings.

## What a change may do
- Behaviour changes ship as a **default-OFF knob + a committed PREREG** (PREREG committed alone,
  before its arms). Never flip a default. The flip is a decision row.
- Default gates must stay unchanged on your exe: the `gctrl.ref` and `b.ref` values in
  `re/orchestrator/merge_train.json` (since the D3-CF-U9779 flip, 2026-10-25: G-CTRL
  `ef5f2e10/57f7ff83/b4f4b55a`, B `c37d4c0c/0a8f9806/7013671d`; read the file, it wins over this line)
  (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`).
- Trackers only through `re-classify`. C4 has three routes (L-93, 2026-11-07, `re/CONFIDENCE.md`): a
  canonical-scenario run with the hook installed (frida), a byte match (`match.py` byte-identical with the recorded
  DDK compiler and flags, `relocheck.py` OK, not naked or asm-only), or a library identity (`libid`). The static
  routes certify the RE only, need no exe body, caller or gate, and are applied with `re/tools/c4_reclass.py batch`.
  Master Ghidra only through ghidra-sweep.
- Tracker edits go through the re-classify skill. A script is allowed only when it is the skill's
  own documented operation (e.g. re/tools/deferred_close.py). Ad-hoc scripts are not.
- Do not edit `ROADMAP.md` or `re/NEXT_SESSION.md` unless the step's deliverable is that text
  (example: D5-PDOD). The merge train writes the status line.

## Ledger transitions (you write your own)
`claim <id> <session-id> -Parent <lead-id>` then `set <id> working "<note>" -Branch orch/<id> -Worktree <path>`,
then `set <id> review` with `-Evidence verify/<lane>_<yyyymmdd>/RESULT_<step>.md`.
A lead that accepts the RESULT runs `enqueue <id> -Branch orch/<id> -Sha <sha>`.
A user decision: `decision <id> "<question, options, evidence path, recommendation>"`.
Heavy context: write the handoff into the RESULT file, `set <id> handoff`, report HANDOFF.

## Final message: only this block
```
REPORT <step-id>
verdict: GREEN | RED | BLOCKED | DECISION | HANDOFF
branch: orch/<step-id> @ <sha>
evidence: verify/<lane>_<yyyymmdd>/RESULT_<step>.md
gates: G-CTRL <h1/h2/h3>  B <h1/h2/h3>   (or "not run: <why>")
decision: <one line, or none>
next: <one line>
```
A lead's REPORT uses its section name as `<step-id>` and lists its steps on `next:`.
