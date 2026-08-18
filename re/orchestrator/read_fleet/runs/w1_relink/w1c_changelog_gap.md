I have everything needed. The key discriminator was `git log --since=2026-08-14 -- re/analysis/CHANGELOG.md` (26 of 37 commits touched the CHANGELOG), plus the fact that c16eb4cc's *only* CHANGELOG line is the unrelated U-9040 entry, and that a6d8be70 dates the D-S3-BANK closure to 2026-08-04 (8dfed24c), before this window.

## (1) Commit table ÔÇö 2026-08-14 ÔåÆ HEAD (37 commits, newest first)

| Commit | Subject (short) | Verdict | Reason |
|---|---|---|---|
| b898b66f | prune ~500 MB captures | OUT-OF-SCOPE | Housekeeping; prunes regenerable captures, no tracker-state move. |
| **c16eb4cc** | world mirrored fixed + FX bloom + lens | **ENTRY-MISSING** | Reverts decision **D-S3-4**, corrects handedness + 3 same-session claims (sky-colour, level-horizon, 90px offset), FX-bloom + lens (inventedÔåÆmeasured). Its **only** CHANGELOG line is the *unrelated* U-9040 filing. |
| 3e35711c | refresh project-info | OUT-OF-SCOPE | Chore. |
| cf1859fb | MASHED_WIN_POS | OUT-OF-SCOPE | New dev feature; no claim/confidence/uncertainty move. |
| a2ece2dd | CAPMODE at capture sink | ENTRY-EXISTS | 2026-08-16 "CAPMODE moved to the capture SINK ÔÇª RESULTS-EXCLUSION REASONING IS OVERTURNED". |
| b7804907 | A/B to measured-InRace | ENTRY-EXISTS | 2026-08-16 "ÔÇªrestricted to MEASURED-InRace shots ÔÇª AN EARLIER READING IS CORRECTED". |
| 4afc8eff | drawstream GameFlow tag | ENTRY-EXISTS | 2026-08-16 "DrawStreamDump gains a GameFlow-mode tag ÔÇª question SETTLED". |
| ecace4fe | withdraw "refuted" claim | ENTRY-EXISTS | 2026-08-16 "D1 drawstream refutation WEAKENED ÔÇª 'REFUTED' WAS OVERSTATED". |
| dcf6baf6 | drawstream proves world drawn | ENTRY-EXISTS | 2026-08-16 "WORLD-COVERAGE HYPOTHESIS REFUTED". |
| 34db7559 | U-9039 resolved | ENTRY-EXISTS | 2026-08-16 "U-9039 RESOLVED ÔÇª NO COORD-FRAME MISMATCH; TWO REAL DEFECTS FIXED". |
| 92912a60 | campose reader +0x4c | ENTRY-EXISTS | 2026-08-16 "race_draw_burst.py campose reader ÔÇª +0x4c READER BUG FIXED". |
| 4ad479cb | R/G gain ÔåÆ coverage | ENTRY-EXISTS | 2026-08-16 "D1 R/G hunt ÔÇª IT IS A WORLD-COVERAGE FAILURE". |
| 6aadfd86 | D1 bisect | ENTRY-EXISTS | 2026-08-15 "D1 bisect ÔÇª COORD-FRAME CLAIM RETRACTED". |
| 0be413f9 | U-9039 filed | ENTRY-EXISTS | 2026-08-15 "U-9039 FILED ÔÇª POSE NOT USABLE" (also corrects 53855ee1's campose claim). |
| 703ed66f | renderer inversion | ENTRY-EXISTS | 2026-08-15 "D1 renderer inversion ÔÇª INVERSION BLOCKED". |
| 45ab2c5d | close R10b | ENTRY-EXISTS | 2026-08-15 "R10b residual ÔÇª CLOSED ÔÇö CAPTURE GATE NOW ZERO-NOISE 16/16". |
| 92ade5eb | fill 113 Blocks cells | ENTRY-EXISTS | 2026-08-15 "113 rows ÔÇª BLOCKS CELLS FILLED ÔåÆ 640 FINAL". |
| 02e065da | D0.2+D0.3+D0.7 | ENTRY-EXISTS | 2026-08-15 "ÔÇª LINKAGE CLAIM CORRECTED". |
| ab189c8d | D0.4+D0.5 | ENTRY-EXISTS | 2026-08-15 "STUBS CENSUS RECOUNTED + CHANGELOG SCOPE DECLARED". |
| b70f31e2 | re-verify 14 C4 rows | ENTRY-EXISTS | 2026-08-15 "14 rows ÔÇª C4 RE-VERIFIED, 0 DEMOTIONS" (the sole hooks.csv-touching commit; change is documented ÔÇö frida_diff repointed). |
| 6d493c72 | D0.8 output rerouting | ENTRY-EXISTS | 2026-08-15 "D0.8 ÔÇª COMMITTED-EVIDENCE OVERWRITE FIXED". |
| df3794a8 | D0.1 audit refresh | ENTRY-EXISTS | 2026-08-15 "D0.1 ÔÇª NON-LINKAGE IS THE BIGGER GAP". |
| b4c3220a | per-slot FX counters | ENTRY-EXISTS | 2026-08-15 "Collision FX ÔÇª NO DEFECT". |
| 81da2c4c | collision FX in race | ENTRY-EXISTS | 2026-08-15 "Collision FX ÔÇª RACE-CAPTURE VERIFICATION". |
| f1807a7e | refresh project-info | OUT-OF-SCOPE | Chore. |
| ba8f0ff1 | settle 3 decisions | ENTRY-EXISTS | 2026-08-15 "D-11063 + ROADMAP v3 + branch retirement ÔÇª DECISIONS 1-3 SETTLED". |
| 53855ee1 | port 3 promote-c4 features | OUT-OF-SCOPE | Feature port; its campose claim entered via ba8f0ff1 and was corrected in 0be413f9. |
| f5e53237 | roadmap v3 | ENTRY-EXISTS | 2026-08-15 "ROADMAP v3 ÔÇª PHASE MODEL REPLACED". |
| a6d8be70 | commit D-S3-BANK evidence | OUT-OF-SCOPE | Commits previously-untracked evidence for a closure dated **2026-08-04 (8dfed24c)** ÔÇö no new tracker move (see note 3). |
| a09ed599 | U-9038 resolved | ENTRY-EXISTS | 2026-08-14 "U-9038 ÔÇª EmitCarFx UNCERTAINTY RESOLVED same-day". |
| 56196060 | D-11063 upheld | ENTRY-EXISTS | 2026-08-14 "D-11063 ÔÇª DEFERRAL UPHELD, JUSTIFICATION CORRECTED". |
| 9d4e9d6c | rescue --on-chunk idea | OUT-OF-SCOPE | Backlog note; no tracker move. |
| 0a1cbf65 | shim 3D draw counters | OUT-OF-SCOPE | Tooling port; referenced inside the ba8f0ff1 entry. |
| 5159e446 | restore 478 CHANGELOG entries | ENTRY-EXISTS | The loss+recovery is itself recorded in the CHANGELOG header and the D0.4/D0.5 entry (ab189c8d). |
| f037f1f5 | branch teardown mandatory | OUT-OF-SCOPE | Skill-doc/process fix; no tracker state. |
| 1b883e9f | merge fog/S3-bank | OUT-OF-SCOPE (flagged) | Reconciliation merge; D-S3-BANK closure predates window (08-04) and its P6/P7 fog + D-S3-PROP closures live in the per-lane doc LIBRW_SIZING_2026-08.md, which the scope rule assigns to per-lane docs. See note 3. |
| 599868a1 | land orchestrator/ghidra artifacts | OUT-OF-SCOPE | Housekeeping. |

## (2) Drafted CHANGELOG entry for the one ENTRY-MISSING (c16eb4cc)

Single line, prepend directly below the `<!-- ENTRIES -->` marker (above the current top U-9040 line):

```
2026-08-16  D1 mirror fix + FX particle bloom + measured lens (standalone renderer, no RVA moved, no ladder move)  THE STANDALONE RENDERED THE WHOLE WORLD MIRRORED; DECISION D-S3-4 REVERTED AND THREE SAME-SESSION CLAIMS OF MINE CORRECTED  With pose, basis, FOV and clips all matched the standalone still drew the same landmarks on the OPPOSITE side (verify/d1_basis, verify/d1_mirrorfix). MASHED_CAM_POSE extended to a 12-float basis -- the 6-float form assumes up=+Y and cannot express the ~26deg roll the original measurably has -- and MatViewFromBasis added; measured verbatim basis 89.68%, right-axis-negated 33.79%, beating a post-hoc pixel flip 44.38%, so the right axis was negated in both view builders. That blew the D3D9-vs-librw A/B from <=1.01% to 23-57% because librw had NOT followed, which localised the defect: D-S3-4 had negated `right` to cancel librw's built-in view-space X negation -- its measurement sound, its conclusion wrong, because it tuned librw to agree with a D3D9 path that was ITSELF mirrored; librw is a RenderWare implementation and its negation IS the original's convention. D-S3-4 REVERTED, A/B restored to max 1.01% matching the old baseline to the decimal -- a shared reflection cancels out of a D3D9-vs-librw comparison, which is why it survived months of clean A/Bs and needed an original-side reference to catch (zero-cost tell: sponsor banner text read BACKWARDS in every standalone capture ever taken, verify/d1_evidence/mirror_prefix_banner_REVERSED.png). FX PARTICLE BLOOM (verify/d1_fxcut): ParticleSystem.cpp exempted kind==2 from the near-camera fade, so SpawnBurst at a spin-out (TrackRenderer.cpp:2796) placed 36 fully-opaque billboards of half-extent up to track_radius_*0.025 ~1.3u ahead of the chase rig, each quad subtending more than the viewport -- FX class alone 87-99% on four shots vs pickup orbs 0.00-0.04%, split out by new gates MASHED_PARTS_KINDS / MASHED_NO_PICKUPS; a projection-derived screen-coverage guard was added and the class CUT from the default build (admitted scaffold with no reversed counterpart, MASHED_PARTS_KINDS=7 restores it). LENS MEASURED (verify/d1_lens): the 60deg FOV was self-declared invented; read live from MASHED.exe at *(DAT_00897fe0+0x84)+0x6c = viewWindow (0.6000,0.4500) -> fovy 48.46deg, near 0.1, far 360, projType 1, cross-checked against Camera::SetupFOV 0x00441700, FAR=360 identified as COURSE.LUA Setup_Fog's 2nd arg (training Setup_Fog(20,360)) and wired to fog_end_. THREE SAME-SESSION SELF-CORRECTIONS recorded in place: (1) the 'sky-colour divergence' does not exist -- a fixed build was compared against a stale librw baseline still carrying the bloom; (2) the camera-frame hypothesis was refuted on a 'level horizon' that was actually rolled ~26deg; (3) the ~90px car-projection offset blamed on capture timing was this reflection. Behaviour-preserving cleanup: the two opposite 'track right' conventions in TrackRenderer.cpp unified at :2254 to match :3133/:2770, verified 16/16 bit-identical. The AI steer-sign contradiction surfaced here is filed SEPARATELY as U-9040 (its own line). hooks.csv UNTOUCHED, no confidence levels moved; DECISION D-S3-4 REVERTED. Evidence verify/d1_mirrorfix/AUDIT.md; NB all pre-2026-08-16 verify/ stills are horizontally flipped vs current output.
```

## (3) hooks.csv movement + the D-S3-BANK nuance

**hooks.csv (the harder defect) ÔÇö none found.** `git log --since=2026-08-14 -- hooks.csv` returns exactly **one** commit, **b70f31e2**, and that commit *does* have its CHANGELOG entry (2026-08-15, "14 rows ÔÇª C4 RE-VERIFIED, 0 DEMOTIONS", which explicitly documents the frida_diff repointing). So no commit moved hooks.csv without a matching entry.

**The D-S3-BANK / 1b883e9f nuance (why I did not flag it ENTRY-MISSING).** The merge subject reads "D-S3-BANK closed at floor + fog term closed," which looks like an in-window decision closure. But a6d8be70's own message pins that closure to **2026-08-04, landed on main as 8dfed24c** ÔÇö i.e. before this window. D-S3-BANK appears exactly **once** in the entire CHANGELOG (line 38, 2026-08-15, a mid-body reference calling it "already on record"), and that "record" is the 08-04 event and the per-lane doc `LIBRW_SIZING_2026-08.md` (which the merge grows by +349 lines), not a dedicated CHANGELOG entry. The merge's genuinely-new bullets (P6/P7 fog-colour R/B swap; D-S3-PROP props-term isolation) are librw-parity-lane characterisations, which the CHANGELOG's own scope rule assigns to "per-lane docs plus git log." I therefore classified both a6d8be70 and 1b883e9f OUT-OF-SCOPE ÔÇö but I am flagging it: if the project treats the D-S3-* family as decisions requiring audit-trail entries (it clearly does for the sibling D-S3-4), then the **2026-08-04 D-S3-BANK closure itself has no dedicated CHANGELOG entry** ÔÇö a pre-window gap, outside the 08-14ÔåÆHEAD mandate, worth a human glance but not something I will assert as an in-window ENTRY-MISSING without inventing the closure's mechanics.

**Bottom line:** one in-window defect ÔÇö **c16eb4cc**, whose substantive corrections (D-S3-4 reversal, handedness/mirror, FX bloom, inventedÔåÆmeasured lens, three self-corrections) were never recorded; its single CHANGELOG line covers only the tangential U-9040 filing.
