# thunk_FUN_004b67a0 (0x004b6590) — C1→C2 plate

**RVA:** 0x004b6590  
**Body:** 0x004b6590–0x004b6594  
**Ghidra name:** thunk_FUN_004b67a0  
**Batch:** batch-render-3-s5  
**U-id:** U-4808  
**Date:** 2026-05-26  

## Signature (Ghidra)

```c
void thunk_FUN_004b67a0(void)
```

## Mechanical description

5-byte PE JMP trampoline that jumps unconditionally to `FUN_004b67a0` (0x004b67a0).

Ghidra decompiles this as inlining the body of `FUN_004b67a0`:
- Calls `FUN_004b6770()` — close file handle or fclose.
- Sets `DAT_007d3e4c = 0`.
- Calls `FUN_00550790(DAT_007d3e58)` — free buffer.
- Sets `DAT_007d3e58 = 0`, `DAT_007d3e48 = 0`.
- Calls `FUN_004b65e0()` — clear three global counters.

This is a compiler-generated JMP thunk; the actual logic lives in FUN_004b67a0.

## Key constants / addresses

| Address | Value/Role |
|---------|-----------|
| `FUN_004b67a0` (0x004b67a0) | PizShutdown — composite piz close |

## Callers

- `FUN_004952f0` (0x004952f0) — ClosePizFile wrapper

## Callees

- JMP to `FUN_004b67a0` only (thunk)

## Line count

~2 lines — within 300-line cap.

## C2 promotion rationale

Trivial JMP thunk to FUN_004b67a0. Body is fully described by destination function.
