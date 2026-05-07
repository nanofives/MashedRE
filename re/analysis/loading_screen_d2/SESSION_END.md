---
session: loading_screen_d2
session_id: loading_screen_d2-20260507
slot: Mashed_pool14
date: 2026-05-07
halt_reason: no_deferred
---

## Halt: <5 DEFERRED

The assigned ID range D-8200..8259 contains zero entries. Current DEFERRED.md high-water
mark is D-7966; the loading_screen_d2 range was never populated.

### Why there is no work

The parent session (`loading_screen`, 2026-05-03) analyzed `FUN_00403050` (0x00403050)
and found all depth-1 callees already at C1:

- `FUN_00402fb0` (0x00402fb0) — C1, mapped
- `FUN_00428760` (0x00428760) — C1, mapped

No stub rows were emitted, no uncertainties were filed, and no functions were deferred
into a `loading_screen_d2-cont1` bucket. The parent session closed cleanly with the
note: "Stubs encountered: None — all callees already at C1."

### Pre-flight results (recorded for completeness)

| Check | Result |
|---|---|
| MASHED.exe SHA-256 | BDCAE093…EFD3C0E ✓ |
| Pool slot | Mashed_pool14 — free (no lock file) |
| Batch 20 G6 lock | Mashed_pool15 (different slot — no conflict) |
| Ghidra health | pong / ok |
| D-8200..8259 in DEFERRED.md | 0 entries |
| U-2767..2786 in UNCERTAINTIES.md | 0 entries |
| S-2760..2779 in STUBS.md | 0 entries |

### Actions taken

- None. No Ghidra project opened (no analysis needed).
- No tracker mutations.
- No SCRIBE_QUEUE entry (nothing to scribe).

### Recommendation

The `loading_screen` subsystem is complete at depth-1. No further depth passes are
warranted unless a future session discovers new callers that reference `0x00403050`
or `0x00403160` (the in-race variant) with un-mapped callees.
