---
rva: 0x004fceec
name_in_ghidra: FUN_004fceec
size_bytes: 165
confidence: C1
subsystem: shader-compiler
callees_depth1: [FUN_004ff913, FUN_004ff958, FUN_004aec1d, __strdup, __controlfp]
callers_noted: [FUN_004fbf76]
opened_in_slot: Mashed_pool8
session_date: 2026-05-18
---

## Mechanical description

Top-level shader-compiler-context constructor (`__fastcall(this=param_1)`). Establishes compile-environment defaults and saves prior CRT FPU state.

1. Call sibling initializers: `FUN_004ff913()`, `FUN_004ff958()` (in-bucket).
2. Initialize a block of 14 dword flag/count slots on `this`:
   - `+0x24 = 0`, `+0x28 = 1`, `+0x2c = 0`, `+0x30 = 0`
   - `+0x34 = 1`, `+0x38 = 1`, `+0x3c = 1`
   - `+0x40 = 0`, `+0x44 = 0`, `+0x48 = 0`
   - `+0x4c = 0`, `+0x50 = 0`, `+0x54 = 0`
   - `+0x80 = 1`
3. **Locale capture/override:**
   - `pcVar1 = FUN_004aec1d(4, 0)` — read current category-4 locale. `FUN_004aec1d` matches `setlocale(category, NULL)` signature (read) and `setlocale(category, name)` (write).
   - `pcVar1 = __strdup(pcVar1)` — copy returned locale string.
   - Store at `*(this+0x84)`.
   - If non-NULL: byte-compare first 2 bytes against literal `"C"`. If they match (i.e., current locale already `"C"`), skip the override.
   - Else (or if dup failed): call `FUN_004aec1d(4, &DAT_005cfee4)` — force locale (where `&DAT_005cfee4` is the override string; almost certainly the literal `"C"` or `"C\0"`).
4. **FPU control-word capture & override:**
   - `uVar2 = __controlfp(0, 0)` — read current x87 control word.
   - Store at `*(this+0x88)`.
   - `__controlfp(0x10000, 0x30000)` — set bits in mask 0x30000 to value 0x10000.
     - 0x30000 = `_MCW_RC` (rounding-control mask) in MSVC CRT.
     - 0x10000 = `_RC_DOWN` (rounding-mode = round-toward-negative-infinity).
   - I.e., force x87 to round-down rounding mode for the lifetime of this compiler context.

Returns `this`.

## Constants

| Constant | Address | Note |
|---|---|---|
| 4 | `FUN_004aec1d` arg 1 | locale category id (matches `LC_NUMERIC` index in MSVC CRT) |
| `&DAT_005cfee4` | locale override | string pointer, likely `"C"` |
| `"C"` | literal | comparison target |
| 0x10000, 0x30000 | __controlfp args | _RC_DOWN bit, _MCW_RC mask |
| Flag block 0x24..0x54 | this offsets | preprocessor/option defaults |
| +0x80 | this offset | post-init mode flag (set to 1) |
| +0x84 | this offset | saved locale string (heap, free'd in dtor) |
| +0x88 | this offset | saved FPU control word |

## Uncertainties

- [UNCERTAIN] Identity of `FUN_004aec1d` — strong shape-match to MSVC `_setlocale` but not FidDB-attested. Could also be a thin wrapper.
- [UNCERTAIN] Whether the round-down FPU mode is for bit-exact constant-folding (typical of shader compilers) vs. legacy quirk.
- [UNCERTAIN] Meaning of each flag in the 14-dword init block — order suggests preprocessor/parse-options.

## Stubs encountered

- [STUB] `FUN_004ff913` — 6-byte util in bucket.
- [STUB] `FUN_004ff958` — 32-byte util in bucket.
- [STUB] `FUN_004aec1d` — likely MSVC `_setlocale`; outside bucket, in CRT band.
- [STUB] `&DAT_005cfee4` — string data, not plated.
