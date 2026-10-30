# RESULT D5-DEFERRED-SWEEP (text-only, nothing edited)

Verdict: GREEN. Scope: the 15 unstruck rows of the `## Active` table in `DEFERRED.md` (lines 15-19, 29-32, 197-202 of
`orch/integration` @ 1611794e7). Every other Active row is already struck. Ledger and merge facts read from
`roadmap.ps1 get/status` and `git log orch/integration`. No tracker, `.ps1` or `DEFERRED.md` edit was made.
Closing is `py -3.12 re/tools/deferred_close.py close --reason "<why>" --evidence "<path>" D-xxxx` (rewrite: `deferred_close.py rewrite --id D-xxxx ...`), run through re-classify.

## A. Proposed actions (condition names only merged items)

| Row (DEFERRED.md line) | Re-pickup condition (text) | Merged item that satisfies it | Proposed action |
|---|---|---|---|
| D-11076 (l.15) | "Before ship: ledger D5-ICONCUBE-SPIN (cube pose and skin) and D5-ICONCUBE-GLOW (glow waits on rfx emitters). Condition met 2026-10-28: pickup pool tick is default ON." | D5-ICONCUBE-SPIN merged (c8cfcdc11; follow-up D5-PREGO-CUBE-FLIP 1b70ca47a made `MASHED_ICONCUBE` and `MASHED_PICKUPS_PREGO` default ON). D5-ICONCUBE-GLOW merged (2f7a1bfdb, GREEN, behind `MASHED_RFX_EMITTERS`, default OFF) on D5-RFX-EMITTERS (c4291ecfc). Evidence: verify/iconcube_20261028/RESULT_D5-ICONCUBE-GLOW.md l.8; GLOW RESULT l.126-129 lists the candidate re-classify of D-11076. | **Close** via deferred_close.py. Reason: cube half default ON, glow half ported on the rfx emitters. Caveat for the closer: the glow knob is still default OFF, so the flip (and the A6 original-side parity "pending, no audio endpoint") must live in a ledger or decision row, not in this DEFERRED row. |
| D-11077 (l.16) | "Before ship: ledger D5-PARTMORPH (part 0x12 morph, part 0x14 wobble). Previously tagged D1-residue, which closed 2026-10-24." | D5-PARTMORPH merged (9db830bea, GREEN on G1..G5); D5-PARTMORPH-PRODUCER merged (18ddec8b5, state 0 producer behind `MASHED_CAR_STATE0`, default OFF). Evidence: verify/partmorph_20261028/RESULT_D5-PARTMORPH.md l.3, l.80, l.105 (row "left for re-classify at the lead's call"). | **Close** via deferred_close.py, reason "law ported behind MASHED_PARTMORPH, producer ported behind MASHED_CAR_STATE0". Caveat: both knobs default OFF and the RESULT says the flip changes nothing visible until state 0 has a producer. The flip is a decision row (ledger D5-PARTMORPH-FLIPPREP is `open`), so do not carry it in DEFERRED. |

## B. Not proposed (reason)

| Row (line) | Condition (short) | Why not proposed |
|---|---|---|
| D-11062 (l.31) | "Before ship: ledger D5-INTROMOVIES" | D5-INTROMOVIES is merged (e29fc115d) but its RESULT is RED on PREREG row I1 (movie 1 time band; run 19: I1b PASS at 35% CPU, I1c FAIL at 97-100% CPU, RESULT l.9-25). Ledger D5-INTROMOVIES-I1B is `open`. Named item is merged, condition not honestly met. Needs user/lead call: keep, or close once I1B resolves. |
| D-11073 (l.19) | "Only the MASHED_SUBSTATE default flip remains (ledger D3-SUBSTATE-FLIP, user-decided, hash-changing, needs a gate re-score)" | D3-SUBSTATE-FLIP is `open` (not merged). Needs the flip pass. |
| D-11078 (l.17) | two-car race needed by a parity/D-phase gate | Event-driven, no merged item. Needs user call if any. |
| D-11079 (l.18) | D2 or Dump-row lane open, or dual-copy allowlist drained | Event-driven, needs a user decision on the default. |
| D-11060 (l.29) | M1 "playable whole game" pass | Needs an interactive playthrough session. |
| D-11061 (l.30) | next full-loop verification with desktop unlocked | Needs an unlocked-desktop recording session. |
| D-11063 (l.32) | v1.0 scope wants split-screen, or U-1908 resolved | Scope decision, user call. |
| D-11075 (l.197) | user asks for QoL, or QoL item gains a port consumer | User-triggered. |
| D-11080..D-11084 (l.198-202) | wider census reaches the row, or static caller walk proves no caller in the executed image | Not a merged-item condition. D4-WSB-PORT-BROAD-1 (9bd91e873), BVH (dc414cdd7), SOLV (merged) ported some rows from the MAPPED-DISP census, but these rows are the NOT-EXEC groups (BROAD 26, BVH 19, SOLV 14, NARROW 9, TAIL 2 rows) and need a census or caller walk, not a merge. Possible later action: per-row close as not applicable after a static caller walk (a text-only task). |

Counts: 2 proposed close, 0 proposed re-pickup, 13 needs-user-call (D-11062, D-11073, D-11078, D-11079, D-11060, D-11061, D-11063, D-11075, D-11080, D-11081, D-11082, D-11083, D-11084).
