# MATCH-RWT-S7: 3 of 3 byte-identical (GREEN)

Row kind: match-only (no build, no gates). Branch `orch/MATCH-RWT-S7` from `orch/integration` `ba39eeb6b`. Opus 5.5.

| RVA | source | before (re-scored, ddk header) | after | bytes | relocheck |
|---|---|---|---|---|---|
| 0x004e4c30 RpLightStreamRead | `re/match/rwl02/004e4c30.c` | 71.3% | **100.0%** rw9044 | 338 B, 0 differing | OK, checked=10, unchecked=0 |
| 0x004e6e00 RpClumpDestroy | `re/match/rwl02/004e6e00.c` | 67.9% | **100.0%** rw9044 | 377 B, 0 differing | OK, checked=18, unchecked=0 |
| 0x004cee90 FUN_004cee90 | `re/match/rwl01/004cee90.c` | 66.7% | **100.0%** rw9044 | 320 B, 0 differing | OK, checked=12, unchecked=0 |

All three headers changed from the default ddk toolchain to `// TOOLCHAIN rw9044` (VC6 SP5 cl 12.00.8804 + Processor Pack
c2 13.00.9044, a recorded compiler since L-131, `c4_reclass.py` `RECORDED`). Flags unchanged: `/O2 /Ob1 /G5`. Under ddk the same
sources score 75.9 / 88.8 / 47.2: in all three the 9044 c2 places a local in the dead first-parameter slot, which the
9178 c2 does not. Plain C bodies, no `__asm`, no naked function.

Source SHA-256 (LF): 004e4c30.c `3e5c8e4f77f2818f37fbc114d79006322b3bcbd842351a2b47d65dd4098deaff`,
004e6e00.c `f0cfa40e2a003c41ddceb3f394645811a97b6d9f271bdd98b0116b550392d62e`,
004cee90.c `44df07904e1fde74e04e6371d83de2bef4d8a867cbe41924d3e41b9886b1990c`.

## Levers

### 0x004e4c30
- **Type byte as a shift of a dword field.** The original zero-extends one byte: `xor ecx,ecx` / `mov cl,[esp+0x2a]` at
  0x004e4c9e / 0x004e4ca0. A `uchar` field read compiles to a dword load + `and 0xff`, and c1 then pads the frame by 4 so
  the wide load stays inside it (frame 0x28 against the original 0x24 at 0x004e4c30). With the chunk's last dword as
  `uint tf` and the argument `(tf >> 16) & 0xff`, both go away (probe `v4c30_7.txt` `f_u32`: frame 0x18 and the byte load).
  The flag byte stored to light+2 is `(uchar)tf` (`mov dl,[esp+0x28]`, original 0x004e4d35).
- **size is a plain local.** The original passes the parameter slot as the chunk-size out argument
  (`lea ecx,[esp+0x28]` at 0x004e4c37) and keeps the stream in ebx (`mov ebx,[esp+0x2c]` at 0x004e4c3c). A separate `size`
  local reaches that shape only under rw9044: 9044 c2 puts it in the dead stream slot, ddk keeps a frame slot (75.9%).
  Writing through `&param_1` with a stream copy (the old source) loads the parameter before `sub esp` on both toolchains.
- Early returns in the RW order, the float temp as a block-scope local, the error record as a block-scope local in the
  else arm (they share frame+4, as in the original).

### 0x004e6e00
- Rewritten from the listing. The atomic loop reads every field off the atomic pointer `cur - 0x40`
  (`lea esi,[eax-0x40]` at 0x004e6e28), not off the link. The light and camera loops have one shape: unlink, stores
  +8 / +4 / +0 (0x004e6ed9..0x004e6edf and 0x004e6f27..0x004e6f2d), destroy call, then exit when the object pointer is 0
  (`cmp esi,ebp` / `je` at 0x004e6ee9 and 0x004e6f37). 97.0% under rw9044 at that point.
- `match_permute.py` (2 rounds, 324 compiles) moved `obj = cur - off - 4` above `next = *cur` in both loops: 100.0%.
- Semantic fix against the old source: its camera loop stored 0 to `cur[2]` (the link + 0xc) before re-basing the
  pointer; the original stores to the link + 8 (`mov [eax+8],ebp` at 0x004e6f27, eax = link).

### 0x004cee90
- The create is an `__inline` static helper with the three header words as arguments (inlined under `/Ob1`). Its NULL
  return gives the redundant `xor esi,esi` at 0x004ceefb, and under rw9044 header word 0 is held in the dead parameter slot
  across the allocator call (`mov [esp+0x38],eax` at 0x004ceed8, read back at 0x004ceeff), words 1 / 2 in ebp / edi
  (0x004ceed4 / 0x004ceed0). The old note's inline attempt (40.7%) was under ddk; under ddk this form is 47.2%.
- Row loop step written as the for-step `y++, off += stride`: `inc eax` at 0x004cef9b before `add ebp,edi` at 0x004cef9c
  (the body-end `off += stride` form is 99.2%, one swapped pair).

Probe runner and variant files: `verify/match_20261113/MATCH-RWT-S7_probes/` (`exp.py`, the S6 runner with the base source
as an argument; `v4c30_*.txt`, `v6e00_*.txt`, `vee90_*.txt`; the permute output is the committed 004e6e00.c).

## Checks (run on the branch)

```
py -3.12 re/tools/match/match.py --bytes re/match/rwl02/004e4c30.c
  0x004e4c30 _RpLightStreamRead: 100.0% (regs 100.0%, ops 100.0%)  108/108 ins  flags=/O2 /Ob1 /G5 tc=rw9044
  bytes: 338 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/match.py --bytes re/match/rwl02/004e6e00.c
  0x004e6e00 _RpClumpDestroy: 100.0% (regs 100.0%, ops 100.0%)  134/134 ins  flags=/O2 /Ob1 /G5 tc=rw9044
  bytes: 377 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/match.py --bytes re/match/rwl01/004cee90.c
  0x004cee90 _FUN_004cee90: 100.0% (regs 100.0%, ops 100.0%)  123/123 ins  flags=/O2 /Ob1 /G5 tc=rw9044
  bytes: 320 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py <each>
  0x004e4c30 OK checked=10 unchecked=0 / 0x004e6e00 OK checked=18 unchecked=0 / 0x004cee90 OK checked=12 unchecked=0
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S7.tsv      (dry run, no --apply)
  [1/3] 0x004e4c30 match PASS byte-identical 338 B, relocheck OK (10/0)
  [2/3] 0x004e6e00 match PASS byte-identical 377 B, relocheck OK (18/0)
  [3/3] 0x004cee90 match PASS byte-identical 320 B, relocheck OK (12/0)
  batch: 3 pass, 0 refused, 0 skipped (already C4), 3 rows  [by route: match=3]
```

VS2022 `cl /nologo /Zs /TP /W3 /DMASHED_STANDALONE /std:c++17` in `mashedmod/src/mashed_re/RwCore/` (after vcvars32), no
error for the six TUs that include or call the three sources: `RwCore_004cee90.cpp`, `RwCore_004e4c30.cpp`,
`RwCore_004e6e00.cpp`, `RwCore_004e7420.cpp`, `RwCore_0054fd60.cpp`, `RwCore_Lifted_10.cpp` (one C4101 warning in
`re/match/rwl04/0054fd60.c:54`, a file this row does not touch). `004e4c30.c` now declares `memset` with
`#pragma intrinsic(memset)` for both languages. No build run (match-only row).

## Not done here (for the lead)

- Port axis: the three bodies are listed provisional for the RW core knob path (`verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`).
  Moving them off that list, and `ddkobj.py check` on the object build with the rw9044 headers, are top-down steps, not done.
- Promotion TSV `re/promotions/MATCH-RWT-S7.tsv` (3 rows, route match), not applied on the branch.
