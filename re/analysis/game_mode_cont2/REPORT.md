# REPORT — game_mode_cont2 session (2026-05-12)

## Parent: FUN_0043dfd0 (0x0043dfd0)

The game-state dispatcher. 10,969 bytes; 63 callees. Too large for direct
decompilation read (C1 with D-3282 deferral). Not directly analyzed this
session.

## Caller: FUN_00492d30 (D-4868, 0x00492d30)

Already C2 from util_c0_promote-20260512. Key findings:

7-case state machine dispatch on `DAT_00771968`. **FUN_0043dfd0 is called from
cases 1, 3, 4, and default of FUN_00492d30's switch.** Case 4 calls FUN_0043dfd0
then checks FUN_0042b930 result against 0x20 (32). Cases 1/3/default fall through
to a shared tail that also calls FUN_0043dfd0 then FUN_0043d7c0.

Semantic meaning of `DAT_00771968` states 1–7 is [UNCERTAIN U-0686] from prior
session.

## Dispatch table of FUN_0043dfd0 — NOT directly observable

FUN_0043dfd0 is 10,969 bytes with a decomp output that exceeds 60,000 characters.
The switch-table layout (which game-mode value → which handler RVA) cannot be
determined from this session without a dedicated large-function analysis (see D-3282).

**What is observable from the callees:**

The 28 child functions in D-4841..D-4867 cluster into functional groups based on
the data they access:

### Loading-state setters (util subsystem)
- 0x004098b0 → DAT_008a9584=1 (loading state-1 entry)
- 0x00409900 → DAT_008a9584=2 + wprintf("Load start\n")
- 0x00409930 → DAT_008a9584=3

These three form a sequence for the loading pipeline, called from FUN_0043dfd0
(and FUN_0040ab40 for state-3). They establish that FUN_0043dfd0 handles loading
phase transitions.

### Car-select state management
- 0x00431d00 → resets 7 globals at 0x67ea74..0x67eaac + calls cursor mover x3
- 0x00492340 → initializes per-player entry (stride-0x4c struct at 0x7f1058)
- 0x0042ae10/aeb0 → readiness checks on stride-0x4c arrays
- 0x0042aa00/ac90/b960 → cursor advance, linked-group walker, first-valid scanner

Car-select cluster data lives in range 0x7f1042..0x7f1a14.

### Indexed entity write
- 0x0046dc00 → write to struct array at 0x8815a8 with stride 0xd04 (=entity struct)

### Snapshot / timer capture
- 0x00494f30 → DAT_00771a50 = DAT_00771a54 = FUN_00493fc0() → DAT_00771a18

### Float-transform wrapper
- 0x00495080 → calls FUN_00494fd0(param_1*2 + const@0x5cc32c, param_2)

### Other mapped callees (already resolved in prior sessions)
- 0x004298c0: zeroes 4 globals (menu state clear)
- 0x0040acd0: state-machine goto-linked body; reads DAT_008a9584 (states 1..6)
- 0x00414120: zeroes 0x89a37c/78/80; copies 0x9c bytes from 0x5f2a70
- 0x0040e480: indexed store *(PTR_PTR_005f2770+param_1*4+0x34)=param_2
- 0x0042f6b0, 0x004307a0, 0x0042f020, 0x0042ef40, 0x00430a10/a60/ab0/b30,
  0x00430910, 0x004309b0, 0x00430b60, 0x004323c0, 0x00422b30, 0x0040b810,
  0x00429aa0, 0x0042af50, 0x0040b6c0, 0x0040ad20: see respective analysis notes

## D-row outcomes

All 29 D-rows D-4840..D-4868 fully resolved in this session (either by
prior sessions or by this analysis):

| D-IDs | Count | Disposition |
|---|---|---|
| D-4840, D-4843..D-4845, D-4848, D-4850..D-4852, D-4854, D-4856, D-4863 | 11 | Already resolved by frontend_c0_promote-20260512 |
| D-4841, D-4842, D-4846, D-4847, D-4849 | 5 | Already resolved by frontend_promote_menus_a (C2) |
| D-4857, D-4858, D-4859, D-4868 | 4 | Already resolved by util_c0_promote-20260512 (C2) |
| D-4860, D-4861, D-4862, D-4867 | 4 | Already resolved by c0_promotion_frontend_a (C1) |
| D-4853, D-4855, D-4864, D-4865, D-4866 | 5 | Resolved this session (C2 new plates) |

No rows need re-deferral to game_mode_cont3; all 29 closed.
