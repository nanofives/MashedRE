# Harness capability backlog — standing between-slices work

Created 2026-07-06 (foundation reset). Companion to `re/analysis/RE_MASTER_PLAN_2026-07.md` §6/§7.

**Why this list exists:** the 2026-06-09 audit found that *harness capability work is the only
reliably high-yield C3 lever* (harness-ext sessions 8/9, scenario-attach 8/8) after the flat
C2→C3 batch lane mined out. When a milestone slice stalls (blocked on a decision gate, a locked
desktop, another session's capture window, or plain fatigue), pull ONE item from here instead of
idling or reviving dead batch lanes.

**Rules**
- Pull one item, finish it, log the outcome in `re/analysis/CHANGELOG.md`, return here.
- Items are ordered by measured ROI (AUDIT_2026-06-09 + the c2c3 pipeline lessons).
- Refresh this list at milestone boundaries, same cadence as the master plan §1 counts.

## Backlog (ROI order)

1. **New `arg_type` handlers** — the #1 measured lever. Extend `re/frida/diff_template.js` for
   the bespoke-arg-shape long tail that blocked the last C2→C3 rounds; regenerate the index after
   every addition (`py -3.12 scripts\gen_arg_types_index.py`; lookups go through
   `re/frida/ARG_TYPES.md`, never the 232 KB template). Candidate source: rows whose promotion
   blocker was recorded as missing/unsupported arg shape (`promote_frontier.tsv`, c2c3 lessons).
2. **Callee-gate cascade** — when a leaf lands C3, re-check its C2 callers whose only failing C3
   gate was "callee below C2+": each new C3 leaf can unlock a caller chain. Mechanical scan;
   route to a cheap model.
3. **Scenario-harness extensions** — extend `re/frida/scenario_launch.py` (warp / `--oracle` /
   `--poke-lap`) to race states it can't reach yet, and generalize the A/B snapshot/restore
   orchestrator lane (`AiControllerAB.cpp` pattern: window snapshot + RNG-ring restore +
   retry-once) to other live-state orchestrator families. This is what made the WS-R6 AI chain
   drain possible.
4. **Discovery drains** — demand map §4 lists **1,788 undiscovered RVAs** inside the race slice's
   static call-closure (no hooks.csv row). Run `ghidra-sweep` SCRIBE_QUEUE drains (11 queued as
   of 2026-07-03) and demand-map discovery sessions; budget discovery alongside promotion.
5. **Parity-harness coverage** — keep `nav_coverage.py` green on any newly wired screen; extend
   `drawlist_diff.py` / `imgdiff.py` recipes where new visual surfaces land (see
   `re/analysis/parity_tooling.md` "Known asymmetries" before trusting a RED).

## Done

- **T1 delegation-reach test — DONE 2026-07-06: SHELL-BLOCKED.** The account2 worker has no shell
  tool at all (Read/Grep/Glob only); the master plan §6 experimental lane is closed — verification
  and execution stay on account3. Verdict recorded in RE_MASTER_PLAN_2026-07.md §6/§7.
