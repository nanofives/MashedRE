# RESULT D4-WSB-D11064-CLOSE

**Verdict: DECISION (no tracker change made).** Text-only step, no game runs.

## Finding
- D-11064 is not closable. U-9797 (filed by D4-WSB-RAND-FLIP, `verify/d4wsb_randflip_20261028/RESULT_D4-WSB-RAND-FLIP.md`) holds two unresolved points: (1) original rand state is per thread (`_getptd`, `0x004a8870`) vs the port's one process-wide `s_own_rand_state`, (2) shipped Lua scripts' `random`/`randomseed` use is unchecked (Lua callers `0x004c03b4`, `0x004c0502`). The standalone's own rand is default ON and the island runs 0 times (`verify/d4wsb_rand_20261028/RESULT_D4-WSB-RAND-D11064.md`, flip RESULT). The `.asi` path still calls `0x005c229b`.
- Right action is a rewrite of D-11064 to those two points, not a close.

## Blocker
`re/tools/deferred_close.py rewrite --id D-11064` refuses: `REFUSED: D-11064 has no unstruck row in the Active table`. The row is at DEFERRED.md line 1243, inside the section `## camera_follow-20260502-2132 depth-2 callees (D-1540..D-1599)` (starts line 494). The tool only touches `## Active` (lines 11-203). Hand-editing is barred by the tracker rule, so DEFERRED.md is unchanged.

## Proposed rewrite (for a lead/user to apply once a tool path exists)
- Why deferred: own CRT rand default ON since D4-WSB-RAND-FLIP (L-47 (a)), standalone executes the island 0 times, `.asi` still calls `0x005c229b`. Open: U-9797 (1) per-thread original state, (2) Lua `random`/`randomseed` binding.
- Re-pickup when: U-9797 resolved (thread id of original `0x005c229b` calls once the original boots again, plus a scan of the six shipped `.lua` scripts).

## Options
(a) Extend `deferred_close.py` to search all sections (a tool change, needs approval), then rerun the rewrite. (b) Move the row into `## Active` through re-classify, then rewrite. (c) Leave D-11064 as is, U-9797 already carries the open points.
Recommendation: (c) now, since U-9797 holds the same content with `Blocks` = nothing.

Gates: not run: text-only.
