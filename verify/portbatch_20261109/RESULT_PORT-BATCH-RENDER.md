# RESULT PORT-BATCH-RENDER (D4, L-107 (a)): 0 rows can be swapped

Branch `orch/PORT-BATCH-FRONTEND` (this RESULT is committed there with the FRONTEND swap; there is no code change
for this subsystem). Brief: L-107 (a), lead cmw7xu1960f37p41557tefpnd. Set: the default-reached closed set of
RESULT_PORT-BATCH-TOOL section 2 (`verify/portbatch_20261109/closed/closed_render.tsv`, groups whose `reached` contains
`default`). Swapped: **0 rows**. Port column: unchanged. Gates: not run (no exe change). Bisect: not needed.

Rule used for every batch (RESULT_PORT-BATCH-GAMEPLAY section 3): an in-place swap is a swap only when the hand body has
the matched prototype and reads the same data. A row whose exe body is a stand-in, takes caller-supplied inputs or reads
a port-side value needs a caller rewrite or a data producer first. "not reached" = the tool's set takes the closure of
every group that has *any* reached member, so it holds heads and callees that no exe path calls; compiling them gives
port `body`, not `called` (their own blocker in brackets).

## Rows by reason

| reason | rows |
|---|---|
| caller-supplied inputs (2) | 0x0040b890 FUN_0040b890; 0x0040b8e0 FUN_0040b8e0 |
| RWGLOBALS (1) | 0x004c3bf0 Vec2Length |
| not reached (13) | 0x0040b420 IndexedTableLookup [CROSS]; 0x0040b930 PlayerScoreThresholdMask [CROSS]; 0x0041ae60 FUN_0041ae60 [none]; 0x0041af00 FUN_0041af00 [none]; 0x0041af50 FUN_0041af50 [none]; 0x0041b7a0 FUN_0041b7a0 [none]; 0x0041c090 FUN_0041c090 [none]; 0x0041c380 FUN_0041c380 [none]; 0x0041cb80 sub_0041cb80 [none]; 0x0041ce00 FUN_0041ce00 [SEED]; 0x0041d830 FUN_0041d830 [none]; 0x004c13e0 FUN_004c13e0 [RWGLOBALS]; 0x004c1480 FUN_004c1480 [RWGLOBALS] |

Cross-subsystem counts and the three cheapest unblocks: RESULT_PORT-BATCH-FRONTEND.md section 4.

## Notes

- Every render group in the default set holds 0x0040b890 / 0x0040b8e0 (caller-supplied inputs) or 0x004c13e0 /
  0x004c1480 / 0x004c3bf0 (RwGlobals). With the threshold getters left out, render-004 (0x0041c380, 0x0041cb80) has
  no exe body for its callee `FUN_0040b890`: the exe's 0x0040b890 is the 3-argument `GridCount8or12Core`.
- None of the render heads (0x0041ae60, 0x0041b7a0, 0x0041c380, 0x0041ce00) has an exe caller: the exe renders the
  race HUD panels through librw (`LibRw/RwRaceFx.cpp`).
- 0x0041ce00 also lacks seed data at 0x005cd118 / 0x005cc8f0 / 0x005f33c4.
