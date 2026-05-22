---
rva: 0x004fcbca
name_in_ghidra: FUN_004fcbca
size_bytes: 28
confidence: C1
subsystem: shader-compiler
callees_depth1: [FUN_004fcb96, _free]
callers_noted: [FUN_004fcf91, FUN_004fddbc, FUN_004fdf42, FUN_004fe1b0, FUN_004fe9cf]
opened_in_slot: Mashed_pool8
session_date: 2026-05-18
---

## Mechanical description

Standard MSVC "vector-deleting destructor" / "scalar-deleting destructor" thunk, `__thiscall(this, byte flags)`:

1. Calls the body-destructor `FUN_004fcb96()` on `this`.
2. If `(flags & 1)`: calls `_free(this)`.
3. Returns `this`.

Five callers, all inside this bucket — confirms this is the public delete-entry-point for a class whose body-destructor is FUN_004fcb96.

## Constants

| Constant | Address | Note |
|---|---|---|
| 0x01 | flags mask | low bit = "also free heap" |

## Uncertainties

(none)

## Stubs encountered

(none — both callees are local)
