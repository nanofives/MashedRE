# Fleet kickoff — hardened per-subsystem child spawn

How to spin up the area-loop child fleet safely, with the concurrency hazards from the
2026-09-01 run baked in. Companion to [[AREA_LOOP.md]]; enforcement guard: `scripts/kill_mine.ps1`.

## Parent (orchestrator) protocol

1. **Sweep first, then spawn.** Base every child on the POST-sweep integrated tip (the branch
   where the last sweep landed the C3s + shared arg_types). Branching from a pre-sweep base
   re-creates the base-divergence + compounding merges (hit frontend/track on 2026-09-01).
2. **Spawn ONE AT A TIME.** A 5-way burst 500'd the spawn endpoint and left one dead session.
   `mcp__happy__spawn_child` one child, then confirm it ACTIVATED (`wait_for_children` /
   `read_child_output` shows real tool calls, not just the queued prompt) BEFORE the next.
3. **Own the verification lane.** Children queue `NEEDS-BOOTED-RACE`; the PARENT runs booted-race
   + path2 serially against ONE canonical `.asi` (shared `original\mashed_re_dev.asi` can't be
   raced across children). Approve account2 prompts, promote on GREEN.
4. Own `CROSS_AREA_BUS.md` + `AREA_LEDGER.md`; run `frida-sweep` between round-batches.
5. Concurrency is real for decode/author (~3-5 children); the build/verify half serializes on one
   machine — 5 concurrent `build.bat` runs starve each other (a parent build timed out this way).

## Enforcement: PID-safe MASHED kills

- Sanctioned wrapper `scripts/kill_mine.ps1 <PID>` — accepts a single numeric PID, rejects any
  name/wildcard at binding, verifies the PID is MASHED before killing. Physically cannot blanket-kill.
- **account2 caveat:** custom PreToolUse hooks are disabled (`allowManagedHooksOnly`), so a raw
  `taskkill /im` cannot be blocked at the harness level — the prompt FORBIDS it and mandates the
  wrapper. **On account3**, add a PreToolUse hook that denies `taskkill /im|Stop-Process -Name|pkill`
  matched against MASHED for a true physical block (see `.claude/settings.json`).

## Paste-ready child prompt (substitute {AREA} = render|hud|ai|track|frontend|vehicle|...)

> You are the **{AREA}** child in the Mashed per-system-area RE loop (star topology). Read
> `re/AREA_LOOP.md` (CHILD section) and your `re/AREA_LEDGER.md` {AREA} section before acting.
>
> BOOTSTRAP (before any RE work): you spawned in the parent's folder and SHARE it with the parent
> and siblings — do NOT work in the shared checkout. Create your OWN worktree + Ghidra pool slot via
> the `worktree` skill (name `area-{AREA}`); if pool-acquire fails, STOP and tell the parent. NEVER
> remove/force-remove any worktree — `git worktree remove --force` has wiped the real game install
> (WORKTREE-SYMLINK-WIPE); teardown is the parent's job only.
>
> BUILD: use PowerShell `& "mashedmod\build.bat"` — NEVER `cmd /c` from Git Bash (it silently no-ops
> and false-passes). After building, VERIFY `original\mashed_re_dev.asi` mtime is < 120s old (epoch
> check) before trusting it.
>
> DECODE: analyzeHeadless + DecompPC.java against YOUR pool slot. NEVER Ghidra MCP (blocked on account2).
>
> TESTING POLICY (concurrency-safe):
> - You MAY run synthetic path1 yourself (`run_diff.py <hook>`) — it loads your own build/ .asi and
>   spawns its own MASHED with its own PID.
> - You may NOT run booted-race or path2 (`run_verify_hook.py`) yourself — they rely on the fleet-shared
>   `original\mashed_re_dev.asi` and strain the one machine. Author + queue them to `re/PROMOTION_QUEUE.md`
>   tagged NEEDS-BOOTED-RACE; the PARENT runs them serially.
>
> MASHED / PID HYGIENE (mandatory): let `run_diff.py`/`scenario_launch.py` spawn AND tear down their
> own MASHED (they hold the PID). If you must kill a MASHED, use ONLY `pwsh scripts/kill_mine.ps1 <PID>`
> with the PID your tool printed. NEVER run `taskkill /im`, `Stop-Process -Name`, `pkill`, or any
> name/wildcard kill of MASHED — it terminates OTHER sessions' games (incident 2026-06-17). Forbidden.
>
> CROSS-AREA findings (shared struct offset / global / dispatcher / an RVA another area reads): report
> to the PARENT via `list_peers` (the Happy session in the Mashed dir that is not you) + `send_to_session`.
> NEVER edit `re/CROSS_AREA_BUS.md` yourself.
>
> Loop `/area-round {AREA}`; record each round in the ledger; 2 consecutive dry rounds -> mark MINED-OUT
> and tell the parent. First message: post "{AREA} child up" to the parent.
