# RESULT D4-WSB-L38-DEMOTE

Verdict: GREEN (text-only tracker leg, no builds, no game runs).

Decision: L-38 option (b), Mariano 2026-10-27, `log/orch/DECISIONS.md`.

## What changed (via re-classify)
- `hooks.csv`: `0055fea0`, `0055ff90`, `00564040`, `00565260` C4/verified -> C3/impl. Each notes cell gets a
  demotion segment: the write-set shadow leg compared 0 heap pages (`o:0,n:0` in 24/24/24/20 samples,
  `MASHED_SHADOW_PRIVATE` unset), so only the caller 4 KB stack window and the return were compared.
  The racediff legs (C4-CLEAN) are unaffected.
- `re/analysis/CHANGELOG.md`: 4 `C4->C3` entries under `<!-- ENTRIES -->`.
- `UNCERTAINTIES.md`: U-9792 filed (heap stores unmeasured, path to resolution = heap-tracked write-set leg).
- `STUBS.md`: no change. The S-3725 / S-3747 rows are existing callee passthroughs, not stubs in these bodies.

## Evidence
`verify/wsbbvhfu_20261027/RESULT_D4-WSB-BVH-FU.md` section 4.

## Re-promotion
Only after a CLEAN heap-tracked write-set leg (D4-WSB-SHADOW-PRIVATE, another worker). Not done here.
