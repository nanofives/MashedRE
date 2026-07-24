# Callee-gate cascade scan — account3 hand-off (2026-07-24)

**Lane:** HARNESS_BACKLOG item 2 ("callee-gate cascade"). Run on **account2** (read-only
analysis; no Ghidra-MCP, no Frida promotion). Machine-readable data:
`re/analysis/plans/callee_gate_cascade.tsv`. Generator (one-off, in scratchpad):
`callee_gate_cascade.py` — a companion to `scripts/promote_frontier.py` that reuses its
capstone `.text` call-graph verbatim but targets **non-leaf** C2 functions instead of leaves.

## Method

For every `hooks.csv` row: `confidence==C2` ∩ first-party ∩ in `.text` ∩ **has a callee**
(non-leaf — leaves are `promote_frontier.py`'s job) ∩ body < 900 B ∩ ≥1 caller at C2+
(reachability gate). Then classify by the **callee gate**: a direct callee *blocks* only if it
is **first-party AND below C2** (C0/C1). Third-party callees (RenderWare, RW-Physics 3.7, CRT,
qhull, libpng) are a **stable library boundary** the standalone links/trampolines into — they
stay C1 by design and satisfy the gate at any confidence. (Initial run mis-scored these as
blockers; every one of the top-25 "cascade keys" was `third-party-library[renderware]`, which is
what surfaced the correction.)

## Result — there is no cascade

| bin | count |
|---|---|
| **CLEARED** (all first-party callees C2+, reachable) | **1416** |
| NEAR-1 (blocked by exactly 1 first-party C0/C1 callee) | **0** |
| NEAR-2 (blocked by exactly 2) | **0** |
| DEEP (3+) | **0** |
| skipped: unreachable (no C2+ caller) | 555 |
| skipped: over 900 B | 110 |

**Root cause (definitive):** excluding comment rows, there are **0 first-party below-C2 rows** in
`hooks.csv` — all 797 C1 rows are `third-party-library[...]`. So the callee gate is *structurally
never* the binding constraint for any first-party function: there are no unreversed first-party
callees left to gate anything. The C0→C1→C2 discovery lanes are effectively drained on first-party
code.

**Implication for account3:** the C2→C3 frontier is gated **entirely by the Frida-diff authoring
work** (and the `arg_type`/harness capability — HARNESS_BACKLOG item 1), **not** by callee
readiness. Waiting for callees to promote will never unblock anything; there is nothing upstream to
wait on. This corroborates the backlog's own thesis that harness capability is the only reliable
C3 lever.

## The actual hand-off: a callee-ready promotion backlog

The 1416 CLEARED rows are **all callee-gate-ready to author+diff right now** (plus the ~19-row leaf
frontier in `promote_frontier.tsv`). This is the standing backlog for `/promote-round` /
`promote-c3-batch` sessions. `callee_gate_cascade.tsv` (CLEARED section, sorted small-first) is the
worklist. Suggested pick order:

**Tier 1 — clean trivial shapes (~290 rows), highest yield/lowest risk:**
- `read_global_u32` (168): render 70, hud 17, boot 17, util/particle/gameplay 11 each, audio 10, …
- `arg_getter` (102): audio 40, render 29, util/gameplay 9 each, vehicle 5, …
- `const_return` (10), `read_global_f32` (6), `const_setter` (2)

**Tier 2 — `other`-shape non-leaf bodies (1125 rows)**, prioritize by subsystem matching the active
slice (R7 full-game systems). CLEARED by subsystem: render 436, audio 287, gameplay 150, util 105,
particle 86, vehicle 67, frontend 57, boot 55, hud 50, input 42, ai 21, track 17, physics 10, …

**Caveats for the promoter:**
- The 15 smallest CLEARED rows (5 B) are tail-jmp `thunk_*` trampolines — verify each is a real
  promotable body, not a 5-byte thunk whose inline-JMP patch would clobber past its boundary (the
  `MIN_BODY` install-crasher class from `promote_frontier.py`). `promote_frontier.py` excludes <5 B
  for exactly this reason; these are ≥5 B tail-jmps and need a per-row look.
- "reachable" here = ≥1 direct caller at C2+; it does **not** assert the caller is *installed*.
  Confirm an install path before spending a canonical-scenario slot.
- The 555 "unreachable" rows have no C2+ caller — deprioritized, not disqualified (a caller
  promotion could add reachability, but per the finding above there is no first-party caller
  promotion pending that would change this).

## Reproduce / refresh

`callee_gate_cascade.py` is a scratchpad one-off (not committed). It imports `scripts/promote_frontier.py`,
so it refreshes for free whenever that does. Re-run after any batch of C3 promotions to confirm the
"no cascade" invariant still holds (it will, until a first-party function is ever discovered below C2
again — e.g. a new `ghidra-sweep` discovery drain landing C0/C1 first-party rows).
