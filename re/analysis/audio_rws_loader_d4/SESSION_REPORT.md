---
session: audio_rws_loader_d4
session_id: audio_rws_loader_d4-20260507 (first attempt); BB6-audio_rws_loader_d4-20260508 (re-check)
date: 2026-05-07 (first attempt); 2026-05-08 (BB6 re-check)
slot_requested: Mashed_pool15
slot_used: none
halt_reason: SHALLOW_PARENT
---

## Halt — shallow parent

Pre-flight check found the parent bucket (audio_rws_loader_d3, session KKKK) left only
**1 DEFERRED function** (D-5080 / `0x005aa1e0`). The minimum to open a d4 session is ≥5.

No analysis was performed. No pool slot was acquired. No Ghidra reads were issued.

**BB6 re-check (2026-05-08)**: Same halt confirmed. D-5080 no longer present in DEFERRED.md
(tracker inconsistency — hooks.csv still shows `005aa1e0` as C0/stub but no matching DEFERRED row).
Regardless, parent d3 DEFERRED count is 0–1, still below ≥5 threshold. Halt stands.
D-5080 tracker inconsistency should be resolved via `re-classify` when `audio_rws_loader_d3-cont1` runs.

## Pre-flight checklist

| Check | Result |
|---|---|
| SHA-256 MASHED.exe | BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓ |
| Pool15 exists | Pool has only Slot 0 (pool reorganized since d1 attempt); no lock files present ✓ |
| X6 lock (batch 24) | No lock files in pool — clear ✓ |
| Parent DEFERRED count | 0 in DEFERRED.md (D-5080 missing — tracker drift); hooks.csv shows 1 C0/stub — below threshold of 5 → HALT |

## Outstanding from d3

The single outstanding function from d3 is:

| D-ID | RVA | Description |
|---|---|---|
| D-5080 (missing from DEFERRED.md) | 0x005aa1e0 | Inline callback `LAB_005aa1e0`; tree-predicate for FUN_005aa060 (DSound context search?); requires `listing_disassemble_seed 0x005aa1e0` then `decomp_function` |

**Pickup note**: D-5080 should be resolved as part of `audio_rws_loader_d3-cont1`.
If additional callees are discovered at that time and accumulate ≥5 DEFERRED entries,
a d4 session may then be justified.

## Tracker changes

None. No rows added to hooks.csv, STUBS.md, DEFERRED.md, or UNCERTAINTIES.md.
