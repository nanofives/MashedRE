# Orchestrator loop — autonomous frontier drain (opened 2026-07-26)

Orchestrator session drives units; children do the work; **the orchestrator verifies every
claim against on-disk artifacts** before a row is accepted. This ledger is the resume point
if the session ends or context is lost.

## Confirmation contract (applied per promoted row, by the orchestrator)

1. Diff artifact exists at `log/diff_<hook>.csv` and the driver exited **0 / GREEN**.
   `INCONCLUSIVE-DEGENERATE` (exit 5) is **NOT evidence** — degenerate rows go back to the
   child for a scenario-attach re-run, they are never promoted.
2. `hooks.csv` row actually moved to the claimed confidence.
3. Both targets build clean (`mashedmod\build.bat`).
4. The RVA is present in the `.asi` export manifest — guards the `asi_sources.rsp` trap
   (a TU added only to `build.bat` silently unregisters its hooks).

## Driver selection (learned Phase 0 — do not get this wrong)

`re/frida/ARG_TYPES.md` contains **two** tables:

| table | handlers | col 2 means | driver | evidence tag |
|---|---|---|---|---|
| before the "Dispatched by …" line (~130) | 112 | line number in `diff_template.js` | `run_diff.py <hook>` | normal |
| after it | 184 | registry uses | `early_window_leaf_diff.py <hook>` | `green-earlywindow-rN` |

Zero overlap between the two sets. Picking the wrong driver produces a FATAL
"no handler in diff_template.js" pre-flight abort, **not** a wrong pass.

## Phase 0 — harness gate: **PASSED** 2026-07-26

| check | result |
|---|---|
| `MASHED.exe.unpatched` SHA-256 | `BDCAE093…` — **matches anchor** ✓ (live exe patched, expected) |
| `launch.exe` SHA-256 | `01506209…` vs anchor `694AA949…` — **MISMATCH**, size equal. Dated 2025-09-17, never touched by us, no `.unpatched`, only referenced by `scripts/sanity_check.sh`. No RVA work derives from it → **not a gate**. Filed as CLAUDE.md doc defect. |
| `diag.py doctor` | all green; healed one stale frida_pool lock + tracked zombie MASHED |
| registry health | 1043 entries; 8 orphan arg_types, 7 = deliberate `harness_limited` marker + 1 `register_abi_record` |
| control `dot_408590` (C4) | FATAL no-handler — **wrong driver** (early-window lane), not a defect |
| control `menu_readiness_check_a` (C4, `none`) | exit 5 degenerate |
| **control `vehicle_slot_getter` (C4, `int_scalar`)** | **GREEN exit 0, 11/11 bit-identical, 4 s** |
| control `sprite_lookup_table_a` (C4, `int_scalar`) | exit 5 degenerate |
| process hygiene | no stray MASHED; `game_procs` OK |

**Verdict:** harness produces valid evidence today. A single-hook diff completes in ~4 s,
far inside the `pc=0x44` ~10 s window → **the crash does not block the C3 lane.** It still
blocks full-`.asi`-install / C4 / KV standalone-truth.

**Key intel:** 2 of 3 substantive controls were **degenerate at menu state**. Expect a
significant share of frontier rows to need `scenario_launch.py` warp for populated inputs.
Degeneracy is the default failure mode of this loop, not RED.

## Unit ledger

| # | phase | unit | child | state | evidence | verdict |
|---|---|---|---|---|---|---|
| 0 | 0 | harness gate | — (in-session) | done | this file | **PASS** |
| 1 | 1 | x87 ST0 float-return handler + proof row `0x00431b50` | `cms1aq31…ebf0` | running | — | — |
| 2 | 2 | wave1-A render `0x004d9360/9a60/9ee0` | `cms1aqtz…l3ye` | running | — | — |
| 3 | 2 | wave1-B gameplay `0x00456eb0/0045ac40/0045c550` | `cms1aren…a6zf` | running | — | — |

### Transport incident (resolved) — always pass `account` to spawn_child

First attempt at units 1-3 spawned **without** the `account` parameter: all three died in ~2 min with
`Failed to authenticate: OAuth session expired and could not be refreshed` at `latestSeq=4`. The default
spawn profile's OAuth had expired. Re-spawned identically with `account: "claude3"` → fine. A ~40 s
one-liner smoke-test child validated the transport before re-issuing the expensive prompts; do that first
next time. Auth-death (child emits an error, low seq) is a *different* failure from the known claude2
wedge (child emits nothing, seq frozen).

## SCOPE FINDING (2026-07-26) — frontier rows are NOT pre-implemented

**27 of the 28 frontier rows are `status=mapped` or `new`, not `impl`.** They have analysis
plates but **no `.cpp`, no `hooks_registry.py` entry**. Only `0x00443300` is already `impl`
(and already C3). So each frontier row is a *full* promote-c3 unit — author `.cpp` → register
in **both** `build.bat` and `asi_sources.rsp` → registry entry → build → diff — not a quick
diff of existing code. Unit sizing and throughput estimates must assume this.

## Account routing (USER STANDING ORDER 2026-07-26)

"Prioritize claude2 sessions whenever possible." Every unit is **split**, not assigned whole:

| leg | account | transport |
|---|---|---|
| read/distill plates, shortlist an existing `arg_type` + driver + test-vector shape, degeneracy-risk call, port spec | **claude2** | `delegate.ps1 -Repo Mashed -PromptFile … -Save …` |
| Ghidra MCP confirm, author `.cpp`, `build.bat` + `asi_sources.rsp`, registry entry, build, `run_diff.py`, commit | claude3 | `spawn_child(account:"claude3")` |

Two mechanisms enforce it:
1. **`-Save` keeps briefs out of the orchestrator's context** — `delegate.ps1` writes the result text to a
   file locally, so the brief is never relayed through this session and never re-billed per turn.
2. **Every child prompt orders the child to delegate its OWN reading** to `delegate.ps1`. Sent to the three
   in-flight wave-1 children mid-run via `send_to_session`.

Never send a write/build/MCP leg to claude2 — it is Read/Grep/Glob only and hangs on the write prompt.

**Trap re-tripped 2026-07-26 (documented, still bit me):** launching the delegate legs as `&` jobs inside
one backgrounded Bash call killed all 4 when the launcher exited — tell was **0-byte `-Save` files**, and the
launcher's own "completed exit 0" notification referred only to the launcher. Fix = detached
`Start-Process pwsh -WindowStyle Hidden -ArgumentList @('-NoProfile','-File',delegate.ps1,…)`.
Relaunched that way → **all 4 briefs written (15.1 / 15.0 / 14.7 / 17.1 KB), processes exited clean.**

Brief directory (scratchpad, regenerable): `…/scratchpad/portprep/brief_g{1..4}.md`
- g1 render `004c1a70 004c3910 004c4eb0 004cbbd0`
- g2 render `0044c740 0045de80 00475ab0`
- g3 util/frontend/ai `004b4550 00431b80 004277a0 004233e0`
- g4 misc `005aed20 00482ae0 00488320 0048b650`

## Wave assignment (26 remaining non-x87 rows + 6 x87)

Wave 1 (running): render `004d9360 004d9a60 004d9ee0` | gameplay `00456eb0 0045ac40 0045c550`

Unassigned, grouped by bucket for future waves:
- render `004c1a70 004c3910 004c4eb0 004cbbd0`
- render misc `0044c740 0045de80 00475ab0`
- util/frontend/ai `004b4550 00431b80 004277a0 004233e0`
- audio `005aed20` | vehicle `00482ae0` | sky `00488320` | particle `0048b650` (read_global_f32 — may need the Phase-1 handler)
- x87 (blocked on unit 1): `00431b20 00431b60 004c4270 004c42d0 004c4360` (`00431b50` is unit 1's proof row)

`0x00443300` (ai, already C3 + impl) is on the frontier list but needs no work.

## Frontier pool (28 rows, gated only on Frida-diff authoring)

Source: `re/analysis/plans/promote_frontier.tsv`. Subsystems: render 13, util 3, gameplay 3,
ai 2, frontend 2, audio 2, vehicle 1, sky 1, particle 1.
Shape hints: `other` 19, `arg_getter` 5, `read_global_f32` 4.

**x87-blocked (6, need Phase 1):** sin-getters `0x00431b20` / `0x00431b50` / `0x00431b60`;
RwV3d bbox accessors `0x004c4270` / `0x004c42d0` / `0x004c4360`.

## Halt conditions

Halt and ask the user on: any RED implying real semantic divergence; any C4 claim (needs
canonical scenario with the hook live); destructive ops; `original/` touched; architecture
forks; `pc=0x44` proving attributable to a hook cluster needing redesign; refill yield
under ~30% (the documented `promote-c3-batch` gate).

## Open defects found by this loop

- **DOC-1** (2026-07-26) `CLAUDE.md` / memory `project_version_anchor` record a `launch.exe`
  SHA-256 that does not match the on-disk file (size matches). Harmless to RVA validity;
  should be re-anchored or the anchor dropped.
