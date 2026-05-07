---
session_id: exception_filter_d2-20260503
bucket: exception_filter_d2
parent_session: KK (exception_filter)
slot: Mashed(master,read-only)
slot_note: All pool slots (Mashed_pool0..Mashed_pool15) have stale lock~ files; opened master Mashed.gpr read-only as fallback. No writes to Ghidra project were made.
anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (ok)
date: 2026-05-03
outcome: early-finish/cap_count=0
---

## Summary

Depth-2 session for the `exception_filter` subsystem. Scanned DEFERRED rows citing the `exception_filter` bucket.

**Result: no work performed.** Only one DEFERRED row cites this bucket (D-1960), and its pickup condition is not met.

## DEFERRED rows reviewed

| ID | RVAs | Pickup condition | Action |
|---|---|---|---|
| D-1960 | 0x004a92de (`terminate`), 0x004af400 (`_ValidateExecute`) | "pick up only if CRT reimplementation is required" | **Left DEFERRED.** Greenfield game port does not require CRT reimplementation at this stage. |

## Tracker state (unchanged from parent KK session)

| hooks.csv RVA | Name | Confidence | Stubs | Uncertainties |
|---|---|---|---|---|
| 0x004af2d4 | FUN_004af2d4 | C1 | S-0680, S-0681 | — |
| 0x004af31a | LAB_004af31a | C0 | — | U-0687 |
| 0x004af32d | FUN_004af32d | C1 | — | — |

## ID ranges consumed

- U: none (reserved U=1147..1166 — unused)
- S: none (reserved S=1140..1159 — unused)
- D: none (reserved D=3340..3399 — unused)

## Depth-3 DEFERRED rows queued

None. D-1960 stays in its current DEFERRED state; no new depth-3 rows were generated.

## Infrastructure note

All pool slots have stale `.lock~` files left by prior sessions (no Ghidra/Java process was running). Root cause: Ghidra on Windows leaves lock files on abnormal exit. Resolution: use `bash -c "rm -f ..."` to clear stale locks before next session that needs a pool slot write.

## MCP failures

None.
