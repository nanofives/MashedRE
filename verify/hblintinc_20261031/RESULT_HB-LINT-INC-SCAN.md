# RESULT HB-LINT-INC-SCAN

Verdict: GREEN (tool change only; no source bodies, trackers or .ps1/.bat touched).

## Change
`scripts/rva_body_scan.py` (the scanner `scripts/lint_rva_bodies.py` calls) now follows `#include "<x>.inc"`
from every TU in either target. Each body found in the .inc (same passes 1-2 as a .cpp) is attributed to the including TU
and anchored at the include line; nested .inc includes are followed. `scan_tu` was split into `scan_tu` + `_scan_lines`
(passes 1-2) + `_scan_inc` (new pass 4). The MATCH-S02 `re/match/*.c` pass is unchanged.

## Re-run on current integration (orch/integration @ 5cf5cdd63)
| | before | after |
|---|---|---|
| findings | 16 (CROSS-TARGET 16) | 48 (CROSS-TARGET 33, DUP-IN-TARGET 15) |
| allowlisted | 16 | 16 |
| NEW | 0 (exit 0) | **32** (exit 1) |

Logs: `lint_before.txt`, `lint_after.txt` in this folder (`--verbose`).

## New dual copies: 32 (15 DUP-IN-TARGET [exe], 17 CROSS-TARGET)
RVA, files, as printed by the lint (file:line is the include line in the including TU):

- DUP-IN-TARGET 0x00408a50  [exe] two bodies: Ai/AiLeafTargets.cpp:261 (AL_408a50), Ai/AiTargetClosest.cpp:120 (TG_408a50), Frontend/Leaves.cpp:44 (PerCarRaceProgressGet)
- DUP-IN-TARGET 0x0040e370  [exe] two bodies: Ai/AiTargetClosest.cpp:120 (TG_40e370), Match/MatchS02_0040e370.cpp:11 (FUN_0040e370)
- DUP-IN-TARGET 0x0040e470  [exe] two bodies: Ai/AiLeafTargets.cpp:261 (AL_40e470), Ai/AiTargetClosest.cpp:120 (TG_40e470)
- DUP-IN-TARGET 0x004233e0  [exe] two bodies: Ai/AiLeafTargets.cpp:261 (AL_4233e0), Ai/HeadingAtan2.cpp:30 (float __cdecl)
- DUP-IN-TARGET 0x00426090  [exe] two bodies: Frontend/GlobalGetters_s4.cpp:180 (GlobalDat0066ce58Get), Race/PathProgStep.cpp:154 (PP_426090)
- DUP-IN-TARGET 0x00426bb0  [exe] two bodies: Ai/AiLeafTargets.cpp:261 (AL_426bb0), Ai/AiTargetClosest.cpp:120 (TG_426bb0), Race/PathProgStep.cpp:154 (PP_426bb0)
- DUP-IN-TARGET 0x00426cb0  [exe] two bodies: Frontend/MenuNearLeaves_s6.cpp:47 (SlotIndexToPtr), Race/PathProgStep.cpp:154 (PP_426cb0)
- DUP-IN-TARGET 0x00426cf0  [exe] two bodies: Frontend/BatchAA_s1.cpp:104 (), Race/PathProgStep.cpp:154 (PP_426cf0)
- DUP-IN-TARGET 0x00426d00  [exe] two bodies: Frontend/BatchAA_s6.cpp:93 (FrontendArraySlotGet), Race/PathProgStep.cpp:154 (PP_426d00)
- DUP-IN-TARGET 0x0046d4a0  [exe] two bodies: Gameplay/PickupTick.cpp:187 (PK_46d4a0), Vehicle/VehicleRecordPtr.cpp:47 (PtrCompute881ec8)
- DUP-IN-TARGET 0x004c39b0  [exe] two bodies: Ai/AiLeafTargets.cpp:261 (AL_4c39b0), Ai/AiTargetClosest.cpp:227 (TG_4c39b0), Gameplay/PickupTick.cpp:186 (PK_4c39b0), Math/RwV3dNormalize.cpp:109 (), Race/PathProgStep.cpp:154 (PP_4c39b0), Race/WorldRegistry.cpp:116 (RG_4c39b0)
- DUP-IN-TARGET 0x004c3ac0  [exe] two bodies: Ai/AiLeafTargets.cpp:261 (AL_4c3ac0), Ai/AiTargetClosest.cpp:120 (TG_4c3ac0), Gameplay/PickupTick.cpp:186 (PK_4c3ac0)
- DUP-IN-TARGET 0x004c3b30  [exe] two bodies: Ai/AiTargetClosest.cpp:120 (TG_4c3b30), Math/RwLutTables.cpp:52 (LutSqrt)
- DUP-IN-TARGET 0x004c3b90  [exe] two bodies: Collision/CollisionWorld.cpp:233 (BW_4c3b90), Math/RwMatrixOrthoNormalize.cpp:31 (RwMatrixOrthoNormalizeLut), Race/PathProgStep.cpp:154 (PP_4c3b90)
- DUP-IN-TARGET 0x00547450  [exe] two bodies: Collision/CollisionWorld.cpp:233 (BW_547450), Race/PathProgStep.cpp:154 (PP_547450)
- CROSS-TARGET 0x00407580  exe-only Gameplay/PickupTick.cpp  vs  asi-only Gameplay/SubsystemARecordKey.cpp
- CROSS-TARGET 0x00407b00  exe-only Race/PathProgStep.cpp  vs  asi-only Gameplay/RangeTable_ah1.cpp
- CROSS-TARGET 0x00407b70  exe-only Race/PathProgStep.cpp  vs  asi-only Gameplay/RangeTable_ah1.cpp
- CROSS-TARGET 0x00407d70  exe-only Race/PathProgStep.cpp  vs  asi-only Gameplay/RangeTable_ah1.cpp
- CROSS-TARGET 0x00407db0  exe-only Race/PathProgStep.cpp  vs  asi-only Gameplay/RangeTable_ah1.cpp
- CROSS-TARGET 0x00408590  exe-only Race/PathProgStep.cpp  vs  asi-only Util/PromoLoop_sessionB.cpp
- CROSS-TARGET 0x00408af0  exe-only Ai/AiLeafTargets.cpp  vs  asi-only Ai/PromoLoop_round1.cpp
- CROSS-TARGET 0x0040e470  exe-only Ai/AiLeafTargets.cpp, Ai/AiTargetClosest.cpp  vs  asi-only Frontend/RaceResults.cpp
- CROSS-TARGET 0x00426bb0  exe-only Ai/AiLeafTargets.cpp, Ai/AiTargetClosest.cpp, Race/PathProgStep.cpp  vs  asi-only Util/PromoLoop_round31.cpp
- CROSS-TARGET 0x00426cc0  exe-only Race/PathProgStep.cpp  vs  asi-only Vehicle/PromoLoop_round2.cpp
- CROSS-TARGET 0x00442cc0  exe-only Ai/AiTargetClosest.cpp  vs  asi-only Ai/PromoLoop_round1.cpp
- CROSS-TARGET 0x00443300  exe-only Ai/AiLeafTargets.cpp  vs  asi-only Ai/AiSplineHooks.cpp
- CROSS-TARGET 0x004576b0  exe-only Gameplay/PickupTick.cpp  vs  asi-only Util/PromoLoop_sessionB.cpp
- CROSS-TARGET 0x0045c330  exe-only Gameplay/PickupTick.cpp  vs  asi-only Util/PromoLoop_sessionB.cpp
- CROSS-TARGET 0x00484a50  exe-only Race/WorldRegistry.cpp  vs  asi-only Util/PromoLoop_sessionB.cpp
- CROSS-TARGET 0x00484c70  exe-only Ai/AiTargetClosest.cpp  vs  asi-only Util/PromoLoop_round20.cpp
- CROSS-TARGET 0x00484c90  exe-only Race/WorldRegistry.cpp  vs  asi-only Util/PromoLoop_sessionB.cpp

The reported miss is the first-listed class: `0x0040e370` [exe] two bodies: `Ai/AiTargetClosest.cpp:120 (TG_40e370)`
(from `AiTargetClosest_naked.inc:334`) and `Match/MatchS02_0040e370.cpp:11 (FUN_0040e370)`.

## Would the build lint fail on integration?
**Yes.** `mashedmod/build.bat:35-36` runs `lint_rva_bodies.py` and exits 1 on any NEW finding. With this scanner change
on integration it returns rc=1 with 32 NEW findings, so the next build fails until each key is either consolidated to
one body per target or added to `re/tools/dual_copy_allowlist.txt` with a citation. Without the scanner change nothing
changes (rc=0).

## Notes
- The .inc includes are unguarded (no `#if` around them in the six including TUs), so the naked bodies are compiled.
- Whether these are real defects or deliberate verbatim naked copies is not judged here. Each is a follow-up.
- Needs Mariano / lead decision: either land the scanner with 32 allowlist entries (allowlist is a tracker-adjacent file, not touched here) or fix the dual copies first. Merging this branch alone breaks the build lint.
- No .ps1/.bat edits needed.
