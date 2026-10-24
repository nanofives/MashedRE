# RESULT D5-PDOD (text-only, no build, no game run)

Branch: orch/D5-PDOD (from orch/integration 5f9a1b182)

## What changed (ROADMAP.md only)
- Line 154 (end of the P-DoD block, lines 152-154): appended
  "Multiplayer (WS-I) is out of v1.0 scope (DEFERRED.md D-11063)."
- Line 2977 (D5 shipping checklist): the item "Multiplayer (WS-I) is out of v1.0 scope per D-11063,
  and this is stated in the P-DoD." now ends with "**DONE 2026-10-24** (P-DoD, line 154)."
  The checklist has no checkbox syntax, so DONE is marked inline.

## D-11063 citation check
`grep -n "D-11063" DEFERRED.md` -> one hit, line 32: "D-11063 | Gate D3 — split-screen local
multiplayer OUT for v1.0 ...". The ID exists and matches the claim.
Note: the row scopes D-11063 to split-screen local multiplayer (render + second local input).
The sentence uses the roadmap's own "Multiplayer (WS-I)" wording as the ledger row prescribed.

## Gates
not run: text-only
