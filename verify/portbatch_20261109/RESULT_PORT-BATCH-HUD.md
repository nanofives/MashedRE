# RESULT PORT-BATCH-HUD (D4, L-107 (a)): 0 rows can be swapped

Branch `orch/PORT-BATCH-FRONTEND` (this RESULT is committed there with the FRONTEND swap; there is no code change
for this subsystem). Brief: L-107 (a), lead cmw7xu1960f37p41557tefpnd. Set: the default-reached closed set of
RESULT_PORT-BATCH-TOOL section 2 (`verify/portbatch_20261109/closed/closed_hud.tsv`, groups whose `reached` contains
`default`). Swapped: **0 rows**. Port column: unchanged. Gates: not run (no exe change). Bisect: not needed.

Rule used for every batch (RESULT_PORT-BATCH-GAMEPLAY section 3): an in-place swap is a swap only when the hand body has
the matched prototype and reads the same data. A row whose exe body is a stand-in, takes caller-supplied inputs or reads
a port-side value needs a caller rewrite or a data producer first. "not reached" = the tool's set takes the closure of
every group that has *any* reached member, so it holds heads and callees that no exe path calls; compiling them gives
port `body`, not `called` (their own blocker in brackets).

## Rows by reason

| reason | rows |
|---|---|
| switched (4) | 0x0041d410 FUN_0041d410; 0x0041d870 sub_0041d870; 0x0041e630 FUN_0041e630; 0x0041e850 sub_0041e850 |
| stand-in (2) | 0x0041db80 Sub0041db80_HudThresholdDispatch; 0x005554d0 FUN_005554d0 |
| caller-supplied inputs (2) | 0x0040b890 FUN_0040b890; 0x0040b8e0 FUN_0040b8e0 |
| port-side value (1) | 0x0046c7b0 VehicleSlotGetter |
| RWGLOBALS (1) | 0x00403160 Sub00403160_SubMode0BViewport |
| not reached (13) | 0x00402fb0 FUN_00402fb0 [none]; 0x0040b420 IndexedTableLookup [CROSS]; 0x0040b930 PlayerScoreThresholdMask [CROSS]; 0x0040b970 Bool0Out8a94e0 [CROSS]; 0x0040b9a0 MaxScoreFlags40b9a0 [CROSS]; 0x0040ba00 BoolMinOut8a94e0 [CROSS]; 0x0041c410 HudStandingsRowUpdate [SEED]; 0x0041cc50 HudSlotUpdateCc50 [DUP]; 0x004c0e50 FUN_004c0e50 [RWGLOBALS]; 0x004c13e0 FUN_004c13e0 [RWGLOBALS]; 0x004c1480 FUN_004c1480 [RWGLOBALS]; 0x004c1c80 FUN_004c1c80 [CROSS]; 0x005c4d30 CondGet5c4d30 [CROSS] |

Cross-subsystem counts and the three cheapest unblocks: RESULT_PORT-BATCH-FRONTEND.md section 4.

## Notes

- `switched`: the PORT-BATCH-TOOL bodies of 0x0041d870 / 0x0041e850 (with static callees 0x0041d410 / 0x0041e630) are
  in the exe behind `MASHED_BATCH_HUD`, OFF per L-106 (b). L-107 (a) does not override L-106 (b), so they stay.
- `stand-in`: 0x0041db80's exe forwarder `MatchS02_0040dfc0_Call0041db80` calls `LibRw::RaceFx_HudBindLights()`
  (`Match/MatchS02_0040dfc0.cpp:52`, librw start lights). 0x005554d0 is bound to `MenuBa_ExeMeasure`
  (`Match/MatchS06_004282a0.cpp:26`, the exe's glyph-sum measure). The matched bodies draw / measure through the RW
  and font calls the exe does not route.
- `RWGLOBALS`: 0x00403160 reads `DAT_007d3ff8` (RwGlobals), 0 in the exe pad; its exe forwarder is a no-op today.
- hud-002 (head 0x0041c410) counts as reached only because its callees 0x0040b890 / 0x0040b8e0 / 0x0046c7b0 are: the
  head itself has no exe caller (the exe draws the standings as a 2D overlay, `exe_main.cpp`).
