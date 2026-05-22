---
rva: 0x004fccbc
name_in_ghidra: FUN_004fccbc
size_bytes: 28
confidence: C1
subsystem: shader-compiler
callees_depth1: [FUN_004fcc67, _free]
callers_noted: [FUN_004fcf91, FUN_004fd4da, FUN_004ff5b6]
opened_in_slot: Mashed_pool8
session_date: 2026-05-18
---

## Mechanical description

Vector/scalar-deleting destructor thunk for the FUN_004fcc34/FUN_004fcc67 large-class:
1. `FUN_004fcc67()` (body destructor)
2. If `(flags & 1)`: `_free(this)`
3. Return `this`.

Three callers, all inside this bucket — confirms FUN_004fccbc is the public delete-entry-point for the large shader-context class.

## Constants

| Constant | Address | Note |
|---|---|---|
| 0x01 | flags mask | low bit = "free heap" |

## Uncertainties

(none)

## Stubs encountered

(none)
