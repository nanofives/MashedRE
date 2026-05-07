# Session End Report — video_mci_d2-20260503

**Date:** 2026-05-03  
**Slot:** Mashed_pool1 (stale lock cleared — lock was 9.5 hours old)  
**Parent bucket:** video_mci-cont1 (D-1000..D-1002)

## Work completed

### D-1000..D-1002 drained — 4 units promoted C0→C1

| D-ID | RVA | Description |
|------|-----|-------------|
| D-1000 | 0x0049ec10 FUN_0049ec10 | `__thiscall` ctor; installs vtable[0]+7 slots on object ≥0x128 bytes; calls FUN_0049dd60+FUN_0049cfb0 |
| D-1001 | 0x004a3b84 FUN_004a3b84 | `vsnprintf` via MSVC FILE-trick (_flag=0x42); delegates to FUN_004a504f; null-terminates |
| D-1002a | 0x00493ac0 LAB_00493ac0 | pre-NT5 code-page: `GetThreadLocale`→`GetLocaleInfoA(0x1004)`→atoi; fallback `GetACP` |
| D-1002b | 0x00493b40 LAB_00493b40 | NT5+ code-page: `MOV EAX,3` (CP_THREAD_ACP); `RET` |

Note: D-1002 covered two code labels (not Ghidra functions) in one analysis file.

### Depth-3 DEFERRED filed — D-4060 (1 entry)

| D-ID | RVA | Reason |
|------|-----|--------|
| D-4060 | 0x0049dd60 FUN_0049dd60 | callee of FUN_0049ec10 with 4 pass-through args; likely base/ancestor ctor; not recursed |

## Tracker mutations

| Tracker | Change |
|---------|--------|
| hooks.csv | +4 rows (0049ec10, 004a3b84, 00493ac0, 00493b40) all C1 util |
| STUBS.md | S-0373/S-0374/S-0375/S-0376 moved to Resolved; +S-1380 (FUN_0049dd60) |
| UNCERTAINTIES.md | +U-1387 (FUN_0049ec10 struct layout) |
| DEFERRED.md | D-1000/D-1001/D-1002 struck-through; +D-4060 |
| CHANGELOG | +4 C0→C1 entries |

## S/U IDs used

- S range: S-1380 (1 of S-1380..1399 used)
- U range: U-1387 (1 of U-1387..1406 used)
- D range: D-4060 (1 of D-4060..4119 used)

## cap_count

- cap_count = 0 at session end (no cap hit; all 3 parent D entries processed)

## MCP observations

- `project_program_open_existing` fails when `program_name="MASHED.exe"` (adds double slash); fixed by using `program_path="MASHED.exe"` (no leading slash).
- `function_at` on 0x00493ac0 and 0x00493b40: "no function found" confirmed — Ghidra has them as code labels only; used `listing_code_units_list` to read disassembly directly.
- All three decompilations succeeded on first attempt.

## Continuation queue

**video_mci_d2-cont1**: process D-4060 (0x0049dd60 FUN_0049dd60). Depth-3; 1 function. Recommend combined with other cont1 sweeps if queue consolidation desired.
