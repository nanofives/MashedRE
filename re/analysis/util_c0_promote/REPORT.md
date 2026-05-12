---
session_id: util_c0_promote-20260512
date: 2026-05-12
pool_slot: Mashed_pool2
model: claude-sonnet-4-6
rva_budget: 20
rvas_used: 20
---

# Session 35 — util_c0_promote REPORT

## Mission

Promote all 11 util/C0 rows in hooks.csv to ≥C1, then spend remaining 9 slots on unmapped depth-1 callees.

## Results summary

| Phase | Count | Outcome |
|---|---|---|
| Util C0 → promoted | 11 | 10 to C2, 1 to C1 |
| New depth-1 callee plates | 9 | All C2 |
| **Total RVAs worked** | **20** | At budget cap |

After this session: **util has 0 C0 rows remaining** (all 11 promoted).

## The 11 util C0 functions

| RVA | Name | Bytes | New confidence | Notes |
|---|---|---|---|---|
| 0x004af31a | LAB_004af31a | 19 | C2 | Exception filter installer; orphan block (no Ghidra fn entity); listing-based analysis |
| 0x0043dfd0 | FUN_0043dfd0 | 10969 | C1 | Too large (30839-token decomp); C1 via caller-chain anchor (FUN_00492d30) |
| 0x00409930 | FUN_00409930 | 30 | C2 | Loading state-3 setter: DAT_008a9584=3, DAT_008a9590=1 |
| 0x00409900 | FUN_00409900 | 43 | C2 | Loading state-2 setter + wprintf("Load start\n") |
| 0x004098b0 | FUN_004098b0 | 27 | C2 | Loading state-1 setter: DAT_008a9584=1, DAT_008a9588=1 |
| 0x00492d30 | FUN_00492d30 | 265 | C2 | 7-case game-state tick; dispatches on DAT_00771968; QPC timing |
| 0x004295a0 | FUN_004295a0 | ~124 | C2 | HUD label renderer: 2×FUN_0040dc80+FUN_00427e00 |
| 0x00429bd0 | FUN_00429bd0 | ~574 | C2 | HUD full/simple renderer: vtable dispatch + FUN_00472c60 world-pos |
| 0x00424eb0 | FUN_00424eb0 | 1038 | C2 | Name-label timeout loop; 3-pass; respawn on DAT_007f0fd0 modes 4/7/8/9 |
| 0x00442440 | FUN_00442440 | ~390 | C2 | Matrix transform: 16f copy from FUN_0046d4a0; 3×RwMatrixTranslate |
| 0x00412f30 | FUN_00412f30 | 1744 | C2 | Per-player distance/attenuation + screen-proj; events 0x19-0x3a; two render arrays |

## 9 new depth-1 callee plates

| RVA | Name | Subsystem | Bytes | Notes |
|---|---|---|---|---|
| 0x0041f1c0 | FUN_0041f1c0 | util | 23 | Event table lookup: DAT_0063d9e0[(v*0xab+event)*4] |
| 0x0041f1e0 | FUN_0041f1e0 | util | 50 | Event record matrix reader: copies 16f via FUN_004c0ed0 |
| 0x0041f090 | FUN_0041f090 | util | 53 | Per-player bit-flag extractor: bits 4+5 from DAT_0063dc74[v*0x2ac] |
| 0x0046d400 | FUN_0046d400 | render | 152 | World-to-screen Y projection via FUN_00538c80; vertical segment method |
| 0x00442c80 | FUN_00442c80 | util | 62 | Mode-gated check: FUN_0040e350==6 + DAT_007f0fd0 exclusions + per-player float threshold |
| 0x0046cbe0 | FUN_0046cbe0 | vehicle | 43 | Spinout-state setter (companion to FUN_0046cbb0 C2) |
| 0x0046c570 | FUN_0046c570 | vehicle | 74 | Velocity damper: 3 floats at DAT_00881f50/54/58 *= _DAT_005ce264 |
| 0x0046c7d0 | FUN_0046c7d0 | vehicle | 851 | Steering-angle updater: heading vs camera, RwMatrix_SetRotAxisAngle on vehicle |
| 0x00412cf0 | FUN_00412cf0 | hud | 312 | Label-trail record appender: lerp pos + dist-alpha + color bytes, cap 0x7f |

## Key structures discovered

- **Event table** at DAT_0063d9e0: stride 0xab entries × player; 16-float matrix per event (e.g. 0x19-0x1c = flags/animations, 0x37-0x3a = indicators, 0x2a = mode-2 specific).
- **Per-vehicle block** at DAT_00881ec8, stride 0x341 (repeated): velocity at +0x74/78/7c, spinout state at +0x50 from base, matrix at offset computed from DAT_00881f48[v*0x341]*0x10.
- **Loading state machine** globals: DAT_008a9584 (state 1/2/3), DAT_008a9588/58c/590 (per-state enable flags), DAT_008a95b0/ac (counters cleared on entry).
- **Render output arrays**: DAT_0089bea0 and DAT_0089a8a0 with stride 0x2c; write indices DAT_0063bc58/5c.

## Uncertainties filed

All uncertainties are inline in the .md files. No new U-IDs assigned (session did not claim IDs from the counter — these are all C2 plates; U-IDs are required for explicit tracker entries, not inline file notes).

Significant open questions:
- FUN_0043dfd0 (D-3282) remains the central loading/game-state dispatcher; its full analysis is prerequisite for naming the state-1/2/3 loading states.
- FUN_00538c80 (world-to-screen / RW camera projection) is an unmapped callee of FUN_0046d400.
- FUN_004726f0 (angle/quaternion delta) is an unmapped callee of FUN_00412cf0.
- FUN_004c0ed0 (RW iterator step) is an unmapped callee of FUN_0041f1e0.

## No STOP-AND-ASK conditions triggered

All 11 functions are consistent with util subsystem tagging. FUN_0046c7d0, FUN_0046cbe0, FUN_0046c570 (new callees) were tagged vehicle; FUN_0046d400 tagged render; FUN_00412cf0 tagged hud — these are new additions, not reclassifications of existing util rows.

## Scribe queue

Added to SCRIBE_QUEUE.md: `util_c0_promote-20260512` with all 20 RVAs. Pool: Mashed_pool2.
