# RESULT MATCH-RWT-S2: compiler shaping for 0x004c6750 and 0x004cdd60

Verdict: GREEN. Both functions are byte-identical with relocheck OK. Promotion file `re/promotions/MATCH-RWT-S2.tsv`
(route `match`, 2 rows). The `c4_reclass.py batch --tsv` dry run passed both rows. `--apply` was not run on the
branch (HB-TRACKER-AT-MERGE).

Row kind: match-only. No build, no gates, no game run.

## Results

| RVA | Source | Toolchain / flags | Before | After | Bytes | relocheck |
|---|---|---|---|---|---|---|
| 0x004cdd60 | re/match/rwl01/004cdd60.c | rw9044, /O2 /Ob1 /G5 | 79.2% (ddk, MATCH-RWL01) | 100.0% (77/77 ins, regs and ops 100%) | 188 B, 0 differing unmasked bytes | OK, checked=3 unchecked=0 |
| 0x004c6750 | re/match/s19/004c6750.c | rw9044, /O2 /Ob1 /G5 | 93.5% on rw9044 today (83.3% was recorded before L-131) | 100.0% (349/349 ins) | 1092 B, 0 differing unmasked bytes | OK, checked=46 unchecked=1 (`__chkstk`) |

Both sources contain no inline `__asm`. Neither body is a naked transcription.

## Levers

**0x004cdd60.** The source header had no `// TOOLCHAIN` line, so it compiled under ddk. It now names `rw9044` (L-131,
RW core). The new body also scores 100% under ddk. The remaining residual was block layout. The original places the
`depth == 4 || depth == 8` then-block (`mov ebp,1 / mov edx,ebp / shl edx,cl / shl edx,2 / jmp 0x004cdd7c`) after
the first return. Source forms tried, scored on rw9044:

| Form | Score |
|---|---|
| zero-init, then `if (d==4 \|\| d==8) {flag=1; pal=...}` | 85.7% (zeros hoisted to the prologue) |
| if/else, inverted condition, switch, goto, `flag << d` | 90.9% (then-block inline) |
| `flag = (d==4) \|\| (d==8); pal = flag ? (1<<d)<<2 : 0;` with the +0x18 store `flag ? pixels+size : 0` | **100%** |

A flag sweep on the 85.7% form (`/O2 /G5`, `/Ob2`, `/Ox`, `/G6`, `/O1`, `/Oy-`, `/Ogityb1`) found nothing better than
the recorded `/O2 /Ob1 /G5`.

**0x004c6750.** On rw9044 the only residual was the size-match early return. The original joins the common exit
(`je 0x004c6b87`, where `mov eax, esi` comes before the pops), and `image` stays in esi through the resample
(`mov esi, ebp` before `jne 0x004c6ad7`). The rebuild had a separate epilogue there and kept the result in ebp. The
fix: the whole resample tail sits in `if (image->width != *width || image->height != *height) { ... }`, followed by one
`return image;`. Semantics are unchanged: the early `return image` became the fall-through to that return.

## Checks run

- `py -3.12 re/tools/match/match.py <src> --bytes` for both: BYTE-IDENTICAL (relocations masked).
- `py -3.12 re/tools/match/relocheck.py <src>` for both: OK.
- `py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S2.tsv` (dry run): `2 pass, 0 refused,
  0 skipped, by route: match=2`.
- Both sources are `#include`d as C++ by `mashedmod/src/mashed_re/RwCore/RwCore_004cdd60.cpp` and
  `RwCore_004c6750.cpp`, which run only under `MASHED_ORIG_TOP=1` + `MASHED_RWC_CORE=1`. A VS2022
  `cl /Zs /TP /DMASHED_STANDALONE` syntax check of both TUs exited 0. No full build was run because this is a
  match-only row. The train treats these files as build inputs, because both are `#include`d from mashedmod.

## For the lead

- Both RVAs are listed as provisional RW core bodies (L-126, `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`). Once
  the train applies the C4 promotion, they no longer need to be provisional. That provisional list was not edited here.
- The object build (`MASHED_DDKOBJ=1`, `ddkobj.py check`) was not run. Whether it substitutes rw9044 units is for a
  build row.
