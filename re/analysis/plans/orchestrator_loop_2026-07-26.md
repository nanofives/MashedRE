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
| 1 | 1 | x87 ST0 handler `st0_ret_global` + proof `0x00431b50` | `cms1aq31…ebf0` | **DONE** | `log/diff_sin_global_product_431b50.csv` 10/10 · commit `cec43000` · promo `e72191d9` | **C2→C3 ×1** |
| 2 | 2 | wave1-A render `0x004d9360/9a60/9ee0` | `cms1aqtz…l3ye` | **DONE** | 3× `log/diff_palette_*.csv` 6/6 · `425f246d` · merge `0004ec9e` · promo `521141ec` | **C2→C3 ×3** |
| 3 | 2 | wave1-B gameplay `0x00456eb0/0045ac40/0045c550` | `cms1aren…a6zf` | **DONE** | no commits (branch clean) | **3× BLOCKED — no existing handler fits** (honest negative) |
| 4 | 2 | cheap lane: sin siblings `0x00431b20`/`0x00431b60` | `cms1bomk…73cz` (sonnet) | running | — | — |
| 5 | 2 | wave2: the 4 existing-handler rows `0x004c1a70 0x004c3910 0x004233e0 0x005aed20` | `cms1bpiu…w9jc` | running | — | — |

**Verified promotions so far: 4.  C3 836 → 840, C2 4045 → 4041.**

### REVISED YIELD MODEL (the loop's most important finding)

The frontier metadata claims these rows are "gated ONLY on Frida-diff authoring". **That is
misleading.** Measured over 15 triaged rows: roughly **⅓ have an existing `arg_type` handler; ⅔ need a
NEW handler authored.**

- wave1-B: **3 of 3** gameplay rows blocked, each needing a different new handler
  (`0x00456eb0` EAX-implicit-ptr void writer; `0x0045ac40` ECX+EAX+stack void sorter; `0x0045c550`
  two-arg dual-global-gated predicate needing a live global pointer — the last independently
  corroborated by an account2 second opinion).
- wave2 account2 triage of 12 rows: **4 fit** (`0x004c1a70` `structptr_seeded_array`, `0x004c3910`
  + `0x005aed20` `vec3_normalize`, `0x004233e0` `float3_scalar_ret`), **8 need new handlers**.
- Both successful units had to **author a handler** (`st0_ret_global`, `ptr_seed_observe`).

**Consequence:** the unit of value is the **handler**, not the row — a handler unlocks N siblings
(`st0_ret_global` unlocked 3 sin-getters for one authoring cost). Prioritise handler authoring by
how many rows each unlocks. `shape_hint` in `promote_frontier.tsv` is NOT a reliable predictor of
handler availability and should not be used for batch sizing.

### SAFETY: rows that must never get a naive diff

- **`0x00431b80`** — `hooks.csv` row 730 already records `ESI=0 → infinite loop` at quiescent state
  (U-1655). A naive `run_diff` **hangs the harness**. Needs a seeded car-select scenario and ESI=±1.
  Caught by the account2 prep leg before any child touched it.

### x87 porting law (confirmed twice, independently)

The `.asi` builds `/arch:SSE2`. A plain-C reimpl of an x87 function carries only 32-bit intermediates
and diverges by ULPs → RED. Both GREEN units used `__declspec(naked)` **verbatim** transcriptions
(`0x004d9a60` FISTP-truncates, so a sub-ULP error flips the stored byte). Treat verbatim-naked as the
default for anything touching the x87 stack.

### Merge protocol (validated on the first real conflict)

`ARG_TYPES.md` is **generated** — when two branches both add handlers it is the only conflicting file,
and the resolution is `py -3.12 scripts\gen_arg_types_index.py`, never a hand-merge. `diff_template.js`
/ `run_diff.py` / `hooks_registry.py` auto-merge cleanly because handler additions are purely additive.
After merging, **re-diff every previously-promoted hook** (integration gate) — 4/4 stayed GREEN.

### Tracker hygiene defects found

- **DOC-1** `launch.exe` SHA-256 in CLAUDE.md/memory ≠ on-disk (size matches; never modified by us).
- **DOC-2** `hooks.csv` cited `U-5401` / `U-5404` / `U-5407` for the palette rows; **none was ever
  filed** in `UNCERTAINTIES.md` (dangling refs, now dropped). Suggests other stale U-refs exist —
  worth a sweep comparing every U-id cited in `hooks.csv` against the filed set.

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

## SESSION CLOSED 2026-07-26 — final state and resume point

### Delivered
- **6 verified C2→C3 promotions** (C3 836 → 842): `0x00431b50` `0x00431b20` `0x00431b60` (sine getters),
  `0x004d9360` `0x004d9a60` `0x004d9ee0` (palette quantizer). **CAVEAT: these were race wins** — see below.
- **2 new arg_type handlers**: `st0_ret_global` (x87 80-bit ST0 float return — closes HARNESS_BACKLOG #1)
  and `ptr_seed_observe` (general N-pointer differ). 112 → 114, 0 orphans.
- **4 rows ported, deliberately NOT promoted** (`0x004c1a70` `0x004c3910` `0x004233e0` `0x005aed20`) —
  no Frida evidence, left C2/`BLOCKED-ON-ENV`.
- **pc=0x44 ROOT-CAUSED after ~2 months blocked.** 3 crashers found and fixed, each proven by a
  single-hook before/after:

  | RVA | defect | before → after |
  |---|---|---|
  | `0x005ab380` `AudioRwsChunkHeaderRead` | 12 bytes read into a 4-byte local | AV 4.6 s → BOOTS 18 s |
  | `0x005ab410` `AudioRwsChunkTypeSeek` | 20-byte struct written via `&local_14` | AV → BOOTS 18 s |
  | `0x0042ac00` `MenuGroupCount` | `__cdecl` reimpl of a `__fastcall` original | 4/4 AV → 0/3 |

- **2 reusable harnesses**: `scripts/bisect_hook_index.ps1` (index-level, no rebuilds — **use this**) and
  `scripts/bisect_asi_boot.ps1` (TU-level; refuses to verdict on a link failure).

### THE CAVEAT THAT MATTERS MOST
A single-hook `run_diff` completes in **~4.2 s**; the AV fired at **~4.6 s**. The 6 promotions above were
**winning a race by ~0.4 s**. Their bit-identity evidence is real (genuine distinct per-case fingerprints,
independently re-run), but a GREEN obtained while a boot-AV stands is a race win, not a sound harness.
**Re-verify all 6 once the remaining crashers are fixed.**

### Open: at least one crasher remains — full hook set still AVs at ~6.5 s
**Next target `0x004b40c0` `RenderElemArrayCopy`** — confirmed 3/3 alone (bisect index 434) but it is a
**DIFFERENT defect**: `eip=0x004b52ac`, `params=['0x1','0x4a467c']` = a **WRITE** violation into MASHED's
own code/`.rdata`, not the near-null `+0x44` dispatch. Its C++ body reads faithful against the cited ASM,
so the cause is not visible by inspection and was deliberately **not guessed**. Needs Ghidra on
`0x004b40c0` + adjustor `0x004b4130` + wrapper `0x004b4140`. Dump + parse in `verify/menu_crash_pc44/`.

Also unverified for the same stack-smash class: `Boot/BootLowRvaCluster.cpp:123` and
`Boot/SubsystemInit.cpp:290/339/409` pass a local's address as an out-param (benign iff the callee
writes ≤ 4 bytes).

### Method that works (no rebuilds)
`MASHED_HOOK_LO`/`HI` install only indices `[LO,HI)`; `MASHED_HOOK_ONLY`/`SKIP` confirm a single suspect;
`MASHED_HOOK_HI=0` is a clean 18 s baseline. Expect **multiple independent crashers** — fixing one just
moves the crash later (4.3 → 6.5 s). Do NOT bisect by TU: rsp subsets under ~170 files fail to **link**.

### Three traps that cost hours (all now encoded in the scripts + memory)
1. `Start-Process -PassThru` + `HasExited` reports `ExitCode = -1` (0xFFFFFFFF) for this AV — CLAUDE.md
   documents that as the *force-kill* signature, so it reads as a clean exit. **`WaitForExit()` first.**
2. `LoadDevAsi()` falls back to `..\mashedmod\build\mashed_re_dev.asi`, so parking only the deployed copy
   does **not** unload the hooks. Park both, or `MASHED_RE_NO_AUTO_HOOK=1`.
3. "No WER dump" was the `%LOCALAPPDATA%\CrashDumps` 10-dump retention cap. Archive it and a dump appears.

### Tracker conclusions this session corrected
- 2026-07-25 #4 "subset installs do not dodge it → static initializer" — **FALSE**; `MASHED_HOOK_HI=0`
  boots, so the trigger is hook *installation*.
- I twice wrongly "disproved" that the `.asi` was the cause (traps 1 and 2 above) and retracted both.
- `hooks_registry.py` recorded the `MenuGroupCount` fastcall fix on **2026-06-01** but **the source change
  never landed** — registry and code disagreed for ~8 weeks while the row read `GREEN (11/11)`.

### Housekeeping at close
Working tree clean; both targets build clean; deployed `.asi` byte-identical to `build\`; no stray MASHED;
all worktrees removed via `diag.py wt-remove`; `original/` intact (45 entries, TOASTART present). The
stale `mashed_re_dev.asi.pristine` bisect backup was **deleted** — it predated the fixes, so restoring it
would have silently reinstalled broken code.

## Halt conditions

Halt and ask the user on: any RED implying real semantic divergence; any C4 claim (needs
canonical scenario with the hook live); destructive ops; `original/` touched; architecture
forks; `pc=0x44` proving attributable to a hook cluster needing redesign; refill yield
under ~30% (the documented `promote-c3-batch` gate).

## Open defects found by this loop

- **DOC-1** (2026-07-26) `CLAUDE.md` / memory `project_version_anchor` record a `launch.exe`
  SHA-256 that does not match the on-disk file (size matches). Harmless to RVA validity;
  should be re-anchored or the anchor dropped.
