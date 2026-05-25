# FUN_0043aee0 — Gated Mutator (FUN_004307a0 + DAT_0067ed6c Guard)

## Signature

```c
void FUN_0043aee0(void)
```

Address: `0x0043aee0`  Body: `0x0043aee0`..`0x0043af07`

## Callers

| RVA | Name |
|-----|------|
| 0x00410860 | FUN_00410860 |

## Callees

| RVA | Name |
|-----|------|
| 0x004307a0 | FUN_004307a0 |

## Body Summary

Calls `FUN_004307a0()` and checks its return value. If non-zero AND `DAT_0067ed6c != 0`:
- Writes 1 to `(&DAT_007f0a40)[DAT_0067f17c * 0xc]`.

Otherwise does nothing. Returns void.

The write index is `DAT_0067f17c * 0xc` (a stride-12-int offset from `0x007f0a40`), selecting a per-player (or per-slot) entry.

## Cited Constants / Offsets

| Value | Where | Note |
|-------|-------|------|
| `DAT_0067ed6c` | `0x0043aee9` | Gate flag (must be non-zero) |
| `DAT_007f0a40` | `0x0043aef7` | Base of stride-0xc array written |
| `DAT_0067f17c` | `0x0043aef1` | Index into that array |
| `0xc` (12) | `0x0043aef4` | Stride in int units |

## Uncertainties

- [U-4261] [semantic] `0x0043aee0`: `FUN_004307a0()` gate function — its exact semantics (what condition it tests) are not established from this function alone. Evidence missing: decomp of `FUN_004307a0`. Path to resolution: `mcp__ghidra__decomp_function` at `0x004307a0`.
- [U-4262] [semantic] `0x0043aee0`: `DAT_007f0a40[DAT_0067f17c * 0xc]` — the purpose of writing 1 to this array slot is unknown. Evidence missing: context of `0x007f0a40` array (player flags? network sync?). Path to resolution: cross-reference to `0x007f0a40` write/read sites.
