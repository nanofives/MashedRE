# RESULT MATCH-RWC-NEAR-C: compiler shaping for the two RWC near misses left by MATCH-RWC-NEAR

Row kind: match-only (no build, no gates, no game lock). Branch `orch/MATCH-RWC-NEAR-C` from `orch/integration`
a616fcc3b. RVAs: 0x004d1e30 (99.1%) and 0x00534b60 (98.5%), the two that MATCH-RWC-NEAR part A left
(`verify/match_20261112/RESULT_MATCH-RWC-NEAR.md`). Scored with `re/tools/match/match.py` under each source's
`// TOOLCHAIN` and `// FLAGS` (`/O2 /Ob1 /G5`). No Ghidra pool slot was needed: match.py reads the original bytes itself.

## Verdict

GREEN. 2 of 2 to 100%, BYTE-IDENTICAL, relocheck OK, plain C (no inline asm in either body).
Promotion file `re/promotions/MATCH-RWC-NEAR-C.tsv` (route match), not applied on the branch.

`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWC-NEAR-C.tsv` (dry run):
```
[1/2] 0x004d1e30 match PASS byte-identical 1296 B, relocheck OK (59/1)
[2/2] 0x00534b60 match PASS byte-identical 402 B, relocheck OK (2/0)
batch: 2 pass, 0 refused, 0 skipped (already C4), 2 rows  [by route: match=2]
```

## Per RVA

| RVA | toolchain | before | after | source |
|---|---|---|---|---|
| 0x004d1e30 | ddk (no TOOLCHAIN line; rw9044 also 100%) | 99.1 | **100**, BYTE-IDENTICAL 1296 B, relocheck OK | `re/match/s20/004d1e30.c` |
| 0x00534b60 | rw9044 | 98.5 | **100**, BYTE-IDENTICAL 402 B, relocheck OK 2/0 | `re/match/rwl03/00534b60.c` |

### 0x004d1e30 (100%)

Residual was the case-5 `levels` computation. The original builds the format in edx (`xor edx, edx; mov dh,
[esi+0x23]`), copies it to edi first and runs `not / shr 0xf / and 1` on edi after the argument pushes. Ours ran the
three ops on edx and copied after, which pinned them before `push ebp`.

Fix: the format goes through a 16-bit local and is widened before the `~`:
```c
unsigned short fmt = (unsigned short)RwRasterGetFormat(raster);
unsigned int levels = (~(unsigned int)fmt >> 15) & 1;
```
The ushort-to-uint conversion is the `mov edi, edx` copy, so the ops land on edi and the scheduler moves them between
the pushes as in the original.

Variants tried this row (32 forms, 40 compiles, scratchpad harness):

| form | score |
|---|---|
| `unsigned short fmt` local, `(~(unsigned int)fmt >> 15) & 1` (kept) | 100 |
| same local, `((unsigned int)~fmt >> 15) & 1` | 100 |
| same local, `levels = fmt; levels = (~levels >> 15) & 1` | 100 |
| `(~(unsigned int)(unsigned short)RwRasterGetFormat(raster) >> 15) & 1`, no local | 100 |
| `short fmt` local, both cast placements | 100 |
| `((unsigned short)~RwRasterGetFormat(raster) >> 15) & 1` (cast after the `~`) | 88.7 |
| two- and three-statement in-place forms, `~` first, shift first, mask-then-shift `(~f & 0x8000) >> 15` | 99.1 |
| `__forceinline` helper taking the raster or the format | 99.1 |
| `int` format local, `raster->cFormat << 8` forms, `(unsigned int)cFormat << 8` | 99.1 |
| `?:`, `if/else`, `^ 0x8000`, enum or int format with `?:`, helper with `?:` | 88.4 |
| `unsigned short` / `unsigned char` levels | 88.4 / 83.1 |
| `levels = 1; if (fmt & MIPMAP) levels = 0;` | 72.8 |
| `cFormat * 256u` | 89.2 |

Both toolchains (ddk and rw9044) score every form the same.

### 0x00534b60 (100%)

Residual was the tail `if (f8 == 0 && f1 == 0) v |= 1;`. The original tests f8 then f1 there, with no jump threading.
With f8 first cl threaded the f8 == 1 arm (the end of the block and its f20 failure) straight past the tail
(an extra `jmp`, 95.6%). The source on entry tested f1 first to stop the threading (98.5%).

Fix: the f8 == 1 block moves into a `__forceinline` helper that takes the flags by value and returns `v`, and the tail
tests f8 then f1 as in the original:
```c
static __forceinline uint FlagsClear_00534b60(uint v, int f8, int f1, int f4, int f20)
{
  if (f8 == 1) { if (f1 == 1) v &= ~1; if (f4 == 1) v &= ~4; if (f20 == 1) v &= ~0x20; }
  return v;
}
...
  v = FlagsClear_00534b60(v, f8, f1, f4, f20);
  if (f8 == 0 && f1 == 0) { v = v | 1; }
```
After inlining, the block tests the parameter copy, and cl does not carry `f8 == 1` from it to the tail test of `f8`.

Variants tried this row (18 forms, 18 compiles):

| form | score |
|---|---|
| block in a by-value helper returning v, tail inline (kept) | 100 |
| block helper plus a by-value tail helper | 100 |
| block `f8 >= 1`, tail f8 first | 99.3 |
| f1-first tail (entry state) | 98.5 |
| tail `f8 < 1` or `(unsigned)f8 < 1` | 97.0 |
| tail `!f8 && f1 != 1` | 96.3 |
| f8-first tail, three separate `f8 == 1 && fX == 1` ifs, `else if (f1 == 0)`, tail `f8 != 1`, `f8 <= 0`, tail through a pointer helper, block helper with early return | 95.6 |
| block `f8 != 0` | 94.8 |
| tail helper by value alone | 87.4 |
| tail on `(v & 8)` or `(param_2 & 8)` | 48.1 / 39.7 |

The ddk toolchain scores the kept source 75.6% (frame size 0x1c), so the unit stays rw9044.

## Port

No mashedmod translation unit includes either source (`grep -rn "#include.*\(00534b60\|004d1e30\)" mashedmod/src`
is empty; the port reaches 0x00534b60 through `MashedRw_PTankAtomicCreate_00534b60` in `LibRw/RwApiBind.cpp`). So no
VS2022 compile was needed and the port axis is unchanged. The helper name carries the RVA suffix so it cannot collide
if a port TU includes the source later.

## Counts

- C4 by `c4=match` gained: 2 (0x004d1e30, 0x00534b60), both C2 in `hooks.csv` today. Replay-verified
  (`c4=frida-replay`): 0.

## Levers (new)

- **Widen through a 16-bit local to place a copy before an in-place op chain.** When the original copies a value into
  the destination register first and runs `not / shr / and` there, a `unsigned short` (or `short`) local widened
  with a cast before the `~` gives that copy. A cast after the `~` does not.
- **A by-value `__forceinline` helper stops jump threading.** The block that sets the known value goes into a helper
  that takes the flag by value. cl then does not thread the known-true arm past a later test of the same flag.

## Files

- `re/promotions/MATCH-RWC-NEAR-C.tsv`: the two promotion rows (dry run only).
- Edited sources: `re/match/s20/004d1e30.c`, `re/match/rwl03/00534b60.c` (header note in each).
- Variant harness and outputs stayed in the session scratchpad (not committed).
