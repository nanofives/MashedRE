# FUN_004cfe20 — render pool init (FreeList alloc)

**RVA:** 0x004cfe20  
**Session:** batch-render-5-s4  
**U-id:** U-5177  
**Confidence promoted:** C1 → C2

## Signature
```c
void FUN_004cfe20(void);
```
Body: 0x004cfe20 .. 0x004cfe38 (24 bytes)

## Decomp (verbatim)
```c
void FUN_004cfe20(void)
{
  DAT_007d46e4 = FUN_004cc7f0(8, 0x7f, 4, 0x30411);
  return;
}
```

## Mechanical analysis

Single call: `FUN_004cc7f0(8, 0x7f, 4, 0x30411)`.
- This matches existing C1 note: "127×8-byte pool" — args decode as: entry_size=8, count=0x7f (127), align=4, flags=0x30411.
- Return value (the FreeList handle) stored to DAT_007d46e4 (0x007d46e4).
- FUN_004cc7f0 is the FreeList create function (RW allocator).

## Key globals
| Address | Role |
|---------|------|
| 0x007d46e4 | FreeList handle for render node pool (127 × 8-byte entries) |

## Callers/Callees
- Callees: FUN_004cc7f0 (FreeList alloc).
- Callers: FUN_004c7a70 (case 2 after device creation) at 0x004c7a70..0x004c856a.
- Counterpart teardown: FUN_004cfe40 (C2 confirmed).

## Evidence for C2
- Existing C1 plate `re/analysis/render_c0_promote_c/0x004cfe20.md` already analyzed correctly.
- Decompiler output is unambiguous: trivial one-liner, no uncertainty.
- Args and global confirmed consistent with existing notes.
