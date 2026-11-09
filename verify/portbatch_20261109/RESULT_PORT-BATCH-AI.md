# RESULT PORT-BATCH-AI (D4, L-107 (a)): 0 rows can be swapped

Branch `orch/PORT-BATCH-FRONTEND` (this RESULT is committed there with the FRONTEND swap; there is no code change
for this subsystem). Brief: L-107 (a), lead cmw7xu1960f37p41557tefpnd. Set: the default-reached closed set of
RESULT_PORT-BATCH-TOOL section 2 (`verify/portbatch_20261109/closed/closed_ai.tsv`, groups whose `reached` contains
`default`). Swapped: **0 rows**. Port column: unchanged. Gates: not run (no exe change). Bisect: not needed.

Rule used for every batch (RESULT_PORT-BATCH-GAMEPLAY section 3): an in-place swap is a swap only when the hand body has
the matched prototype and reads the same data. A row whose exe body is a stand-in, takes caller-supplied inputs or reads
a port-side value needs a caller rewrite or a data producer first. "not reached" = the tool's set takes the closure of
every group that has *any* reached member, so it holds heads and callees that no exe path calls; compiling them gives
port `body`, not `called` (their own blocker in brackets).

## Rows by reason

| reason | rows |
|---|---|
| port-side value (6) | 0x00417cf0 FUN_00417cf0; 0x0046c730 Table882198Get; 0x0046c750 Table882194Get; 0x0046d4a0 PtrCompute881ec8; 0x0046d780 LaunchRevRelease; 0x0046d7f0 LaunchRevChargeTick |
| naked x87 (4) | 0x00414300 FUN_00414300; 0x00414490 FUN_00414490; 0x00414a70 FUN_00414a70; 0x004233e0 HeadingAtan2ToGameAngle |
| RWGLOBALS (2) | 0x004c3b30 FastSqrt; 0x004c3bf0 Vec2Length |
| not reached (4) | 0x00415d00 FUN_00415d00 [none]; 0x00443d10 FUN_00443d10 [none]; 0x0046d6d0 VehTbl881f84Get1 [CROSS]; 0x004c3ac0 Vec3Magnitude [RWGLOBALS] |

Cross-subsystem counts and the three cheapest unblocks: RESULT_PORT-BATCH-FRONTEND.md section 4.

## Notes

- `port-side value`: 0x0046c730 / 0x0046c750 / 0x0046d4a0 / 0x0046d780 / 0x0046d7f0 are MATCH-S11-FU-VEHBASE rows
  (rebased vehicle record). 0x00417cf0 calls 0x0046d4a0 and its matched file (`re/match/s03/h2_00417cf0.c`) also
  defines 0x00417730, which the exe has as `VehicleRaceAngleGet`, so it would add a second body.
- `naked x87`: 0x004233e0 (`Ai/HeadingAtan2.cpp`) is a verbatim FPATAN transcription kept for bit identity; the matched
  C calls `atan`, which VS2022 does not compile to the same x87 sequence. 0x00414300 / 0x00414a70 / 0x00414490 are
  verbatim naked transcriptions (`Ai/AiTargetClosest_naked.inc`, mode-7 M4 `AiModeSeven_naked.inc`) bound to `TG_`
  host callbacks and replay hooks. A swap here is a precision change, not a swap.
- `RWGLOBALS`: FastSqrt 0x004c3b30 and Vec2Length 0x004c3bf0 (hot math leaves) read through RwGlobals in the matched
  source.
