# RESULT MATCH-RWC-NEAR part A: compiler shaping for four RWC near misses

Row kind: match-only (no build, no gates). Branch `orch/MATCH-RWC-NEAR` from `orch/RWC-MATCH` f83829aad (RWC-MATCH
had not merged into `orch/integration` at start). Part A RVAs: 0x004c1040, 0x004cc820, 0x004d1e30, 0x00534b60. Part B
(another worker) has 0x004f0ae0 and the low three. 0x005cb404 skipped (libid only).
Scored with `re/tools/match/match.py` under each source's `// TOOLCHAIN` and `// FLAGS` (`/O2 /Ob1 /G5`).

## Verdict

GREEN, partial. 2 of 4 to 100% BYTE-IDENTICAL with relocheck OK, both plain C (no inline asm):
**0x004c1040** (ddk) and **0x004cc820** (rw9044). 0x004d1e30 and 0x00534b60 stay at 99.1% and 98.5%.
Promotion file `re/promotions/MATCH-RWC-NEAR.tsv` (route match), not applied on the branch.

`c4_reclass.py batch --tsv re/promotions/MATCH-RWC-NEAR.tsv` (dry run):
```
[1/2] 0x004c1040 match PASS byte-identical 461 B, relocheck OK (10/0)
[2/2] 0x004cc820 match PASS byte-identical 433 B, relocheck OK (13/0)
batch: 2 pass, 0 refused, 0 skipped (already C4), 2 rows  [by route: match=2]
```

## Per RVA

| RVA | toolchain | before | after | source |
|---|---|---|---|---|
| 0x004c1040 | ddk | 99.2 | **100**, BYTE-IDENTICAL 461 B, relocheck OK 10/0 | `re/match/rwl01/004c1040.c` |
| 0x004cc820 | rw9044 | 99.3 | **100**, BYTE-IDENTICAL 433 B, relocheck OK 13/0 | `re/match/rwl01/004cc820.c` |
| 0x004d1e30 | ddk | 99.1 | 99.1 | `re/match/s20/004d1e30.c` (unchanged) |
| 0x00534b60 | rw9044 | 98.5 | 98.5 | `re/match/rwl03/00534b60.c` (unchanged) |

### 0x004c1040 (100%)

Residual was one load base in the +0x9c list search: after `mov edx,eax` the original reads `[edx + 0x9c]`
(0x004c1076), every node-pointer spelling made cl read `[eax + 0x9c]`. The search now walks a pointer to the link field:

```c
int *link = (int *)(iVar3 + 0x9c);
while (*link != param_2) { iVar3 = *link; link = (int *)(iVar3 + 0x9c); }
```

Same loads and the same final `iVar3` as the old loop. Tried before it (all 99.2% or lower): do-while with entry test,
two goto forms, `for (;;)` with break, assignment in the condition, `while (cur->next != child)` (87.5%). The rw9044
toolchain scores this source 10.4% (it keeps a zero register), so the unit stays ddk.
Port: no mashedmod file includes `re/match/rwl01/004c1040.c` (only comments cite the RVA), so no VS2022 compile needed.

### 0x004cc820 (100%)

Residual was the position of `test edx,edx` (the `param_4 > 0` test): the original schedules it right after
`shr ebp,3` (0x004cc8c8), before the header stores, while every `uint *` spelling left it last before the `jbe`.
Writing the header stores through a struct typedef of the 0x24-byte block (`FL_004cc820`, fields named by offset:
f0, f4, f8, fc, f10 = two-pointer link, f18, f1c) moves the test into place (ops 100%). One store pair was then out of
order: an exhaustive search over 630 orders of the seven header statements (compute before its store, +0x0 before +0x4, +0x10 before +0x14) found 4 at 100%. Kept order:
+0x0, +0x4, compute `(param_2 + 7) >> 3`, +0xc, +0x8, +0x10, +0x14.

Also tried, no gain: exhaustive order of the seven `uint *` stores (2520 orders, best 99.3%), loop forms (`for` with a
counter 89.1%, `while (param_4 > 0)`, `for (; param_4 > 0; param_4--)`, 99.3%), condition forms (`0 < param_4`,
`!= 0` 98.7%, `>= 1` 94.1%, precomputed flag 99.3%), a local copy of `param_4` (93.5%), recomputed field reads (81.3% / 99.3%).

**Build input.** `mashedmod/src/mashed_re/RwCore/RwCore_004cc820.cpp` includes this source as C++ (MATCH_PORT, extern "C").
Compiled that TU alone with the exe's VS2022 flags (`cl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE /c`, vcvars32): exit 0.
One warning, C4554 on `param_2 + 7 >> 3`, a line this row did not change. The type names carry the RVA suffix so they
cannot collide in the port TU. No full build run (match-only row). `PortTop_004cc820.cpp` does not include the source
(it is the HUDINIT-A binding).
`verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv` still lists 0x004cc820 as a 74.5% provisional body. Not edited here.

### 0x004d1e30 (99.1%, unchanged)

Residual: case 5 computes `levels = (~format >> 15) & 1`. The original copies `format << 8` from edx into edi first and
runs not/shr/and on edi, interleaved with the pushes. cl runs them on edx, then copies. Tried (34 spellings): bool forms
(`? 0 : 1`, `!`, `== 0`, `^ 1`, unsigned short cast) all 88.4-88.7%, `1 - bit` 98.6%, and at 99.1%: unsigned cast,
int or uint format local, `/ 0x8000`, mask-then-shift, in-place three-step. Levels as call arguments from a format local
or a case-scope format: 74.0-74.4%. Permuter (`match_permute.py`, 30 rounds + 300-step walk, 1751 compiles): no gain.
Prior row tried function scope, int, two-step and before-the-texture-test placements (RWC-MATCH RESULT).

### 0x00534b60 (98.5%, unchanged)

Residual (from RWC-MATCH): the original tests f8 then f1 at the tail with no jump threading. With f8 first cl threads the
f8 == 1 arm past the tail test (95.6%). Tried (25 spellings): `!f8 && !f1`, `!(f8 || f1)`, `(f8 | f1) == 0`, `f8 != 1`,
nested ifs, ternary, goto, switch, an f8 copy variable, the f8 expression inline at the tail or the block (39.7-42.0%),
`do { } while (0)` around the tail, the block or both: all 95.6% or lower. f1-first inside `do { } while (0)` stays 98.5%.
Permuter (`match_permute.py`, seed 11, 30 rounds + 400-step walk, 2105 compiles): no gain. RWC-MATCH ran 425 compiles before.

## Counts

- C4 by `c4=match`: 2 (0x004c1040, 0x004cc820). Replay-verified (`c4=frida-replay`): 0.

## Files

- `re/promotions/MATCH-RWC-NEAR.tsv`: the two promotion rows (dry run only).
- Edited sources: `re/match/rwl01/004c1040.c`, `re/match/rwl01/004cc820.c` (header note in each).
- Variant search scripts and outputs stayed in the session scratchpad (not committed).
