# RESULT TL-MATCH-REST (2026-11-10): 5 rows byte-identical (3 promoted to C4), 36 parked with score and residual

Scope (RESULT_PORT-OT-TRACKLOAD.md, `tl_closure.py 0x00426e10`):
- 0x00409680 / 0x00409710 (C4, no matched body);
- the RW-math rows 0x004c39b0..0x004c52f0 and 0x004cc050 (8);
- the 22 closure rows in 0x005a6d60..0x005b3b80;
- the 9 row-less functions (0x004b3f20 already got its row and match in POWERUPINIT-C).

Binding the rwphysics family is out of scope (TL-PORT). No `mashedmod/` change. DDK compiler (XP DDK cl 13.00.9176 / c2
13.00.9178) through `re/tools/match/match.py --bytes`, and `relocheck.py`.

## Outcome

- **Byte-identical, relocheck OK (5):**
  - 0x00409680 (20 B) and 0x00409710 (119 B): new sources `re/match/tlr/`. Both rows were already C4 (c4_reclass
    skipped them), so the gain is a matched body the port can include.
  - 0x00474f90 (32 B) and 0x005aaaa0 (30 B): new rows (c1_add, then C1 -> C4).
  - 0x005addd0 (49 B): 88.2% -> 100%. The function returns the new node (`int *`, `if (n == 0) return n;`), which gives
    cl's `test / jne body / ret` layout. C3 -> C4.
  - Promotion: `c4_reclass.py batch --tsv verify/tlmatchrest_20261110/c4_batch.tsv --apply --strict`:
    `3 pass, 0 refused, 2 skipped (already C4)`, 3 CHANGELOG lines.
- **New C1 rows (c1_add, caller evidence from re/analysis/pointer_callers_2026-10-11.tsv):** 0x00474f90, 0x005aaaa0
  (both now C4), and 0x00440bc0, 0x00487180, 0x004b4eb0, 0x00562ac0, 0x005ac770, 0x005ac930, 0x0047f7d0.
  - 0x00562ac0 took its subsystem `third-party-library[RenderWare-Physics-3.7]` from its installer 0x00562600 (c1_add
    copies the installer's subsystem). Per L-116 that filing is context, not identity. Its code was not checked
    against RW Physics here.
- **Parked with best score and residual kind (36):** the table below.
  - Residual kind is from `verify/tlmatchrest_20261110/residual_kind.py`, run over every `!` row of the aligned
    `match.py --show` listing. It is calibrated on 0x005ae920 (SIB-only) and 0x0048e820 (register-only, the
    PARTICLE-3 byte table).
  - "bytes" is match.py's positional count.

## Admission list (field-only residuals; bundle at the end, do not ask per row)

| rva | best | residual | note |
|---|---|---|---|
| 0x005ae920 | 96.8%, regs 100, ops 100, 94/94 | SIB-only (3 byte accesses: [edx+esi+8] vs [esi+edx+8]) | Already-proven L-115 class. It can run as a near-match once TL-PORT binds it. 25 operand-order spellings and the earlier permuter run left the order unchanged. |
| 0x005aa060 | 85.2%, regs 100, ops 100, 27/27 | register-only | For the admission bundle. |

Every other parked row is structural (instruction count, order or mnemonics differ): it stays a stand-in until
matched.

## Levers tried this slice (beyond the earlier MATCH slices' notes in each source header)

- **0x005ae920:** 25 spellings of the two byte accesses (index/base order, char-pointer indexing, `+8` placement). No
  change.
- **0x005ac770 / 0x005ac930 (new):** container-of record type (link at +0x40), owner struct with the head at +0x20,
  do-while with `next` loaded before the head, a bare int-pointer walk. Best 79.3% / 71.4%. The residual is the head
  computation: the original does `lea edx, [eax+0x20]; mov eax, [eax+0x20]` where ours does `add edx, 0x20; mov eax,
  [edx]`.
- **0x004b4eb0 (new):** a typed rewrite (triangle / vertex / sector / out-record structs, struct copies) scored 24.4%,
  below the raw converter's 70.9%. The raw source is committed as the starting point.
- **0x00487180, 0x00440bc0, 0x00562ac0 (new):** raw converter output committed as starting points (11.7 / 12.4 /
  19.0%). Not worked further.

## Left (for a later slice)

- The structural near misses, closest first: 0x004c4dc0 91.2%, 0x004c4d20 90.9%, 0x005b3a00 84.7%, 0x005aba20
  81.9%, 0x005ac770 79.3%, 0x004cc050 78.5%, 0x005aeca0 78.4%, 0x005acd10 78.0%.
- Hand sources for 0x00487180, 0x00440bc0, 0x00562ac0 (raw starting points only).
- 0x004c51a0 / 0x004c52f0 (14.7 / 12.6%): instruction counts differ widely (129/107, 102/143). The sources need a
  rewrite, not levers.

## Per-row table

| group | rva | name | level now | source | best | bytes (positional) | ins ours/orig | relocheck | residual |
|---|---|---|---|---|---|---|---|---|---|
| C4, matched body added | 0x00409680 | LedArrayInit | C4 | `re/match/tlr/00409680.c` | **100%** | 0, BYTE-IDENTICAL | | OK | none |
| C4, matched body added | 0x00409710 | FUN_00409710 | C4 | `re/match/tlr/00409710.c` | **100%** | 0, BYTE-IDENTICAL | | OK | none |
| RW-math band | 0x004c39b0 | RwV3dNormalize | C2 | `re/match/s19/004c39b0.c` | 69.9% | 60B | 83/83 | BAD | structural |
| RW-math band | 0x004c4a50 | RwMatrixRotateInner | C3 | `re/match/s19/004c4a50.c` | 77.8% | 361B | 184/212 | BAD | structural |
| RW-math band | 0x004c4d20 | RwMatrixRotate | C2 | `re/match/s19/004c4d20.c` | 90.9% | 45B | 53/55 | BAD | structural |
| RW-math band | 0x004c4dc0 | RwMatrixInvertEntry | C3 | `re/match/s19/004c4dc0.c` | 91.2% | 19B | 80/80 | OK | structural |
| RW-math band | 0x004c4eb0 | RwMatrixInvert_CofactorPath | C3 | `re/match/s19/004c4eb0.c` | 47.7% | 256B | 128/125 | BAD | structural |
| RW-math band | 0x004c51a0 | RwMatrixTranslate | C3 | `re/match/s19/004c51a0.c` | 14.7% | 349B | 129/107 | BAD | structural |
| RW-math band | 0x004c52f0 | RwMatrixCombine | C3 | `re/match/s19/004c52f0.c` | 12.6% | 250B | 102/143 | BAD | structural |
| RW-math band | 0x004cc050 | RwStreamSkip | C3 | `re/match/s20/004cc050.c` | 78.5% | 164B | 92/93 | BAD | structural |
| audio closure rows | 0x005a6d60 | FUN_005a6d60 | C2 | `re/match/s25/005a6d60.c` | 50.0% | 45B | 20/22 | OK | structural |
| audio closure rows | 0x005a7b60 | FUN_005a7b60 | C2 | `re/match/s25/005a7b60.c` | 53.5% | 596B | 264/271 | BAD | structural |
| audio closure rows | 0x005aa060 | FUN_005aa060 | C2 | `re/match/s25/005aa060.c` | 85.2% | 4B | 27/27 | OK | register-only |
| audio closure rows | 0x005aa0c0 | AudioTreeWalkPredicateSearch | C3 | `re/match/s25/005aa0c0.c` | 56.2% | 154B | 80/80 | BAD | structural |
| audio closure rows | 0x005aaac0 | FUN_005aaac0 | C2 | `re/match/s25/005aaac0.c` | 77.3% | 17B | 22/22 | BAD | structural |
| audio closure rows | 0x005ab380 | AudioRwsChunkHeaderRead | C3 | `re/match/s25/005ab380.c` | 75.0% | 65B | 50/52 | OK | structural |
| audio closure rows | 0x005ab410 | AudioRwsChunkTypeSeek | C3 | `re/match/s25/005ab410.c` | 75.4% | 53B | 57/54 | BAD | structural |
| audio closure rows | 0x005aba20 | FUN_005aba20 | C2 | `re/match/s25/005aba20.c` | 81.9% | 331B | 236/237 | BAD | structural |
| audio closure rows | 0x005abd30 | FUN_005abd30 | C2 | `re/match/s25/005abd30.c` | 52.3% | 218B | 103/109 | BAD | structural |
| audio closure rows | 0x005abfa0 | FUN_005abfa0 | C3 | `re/match/s25/005abfa0.c` | 62.9% | 482B | 213/211 | BAD | structural |
| audio closure rows | 0x005ac210 | FUN_005ac210 | C2 | `re/match/s25/005ac210.c` | 57.1% | 527B | 294/293 | BAD | structural |
| audio closure rows | 0x005acaa0 | FUN_005acaa0 | C2 | `re/match/s26/005acaa0.c` | 60.4% | 491B | 223/225 | BAD | structural |
| audio closure rows | 0x005acd10 | AudioFmtTableSearch | C3 | `re/match/s26/005acd10.c` | 78.0% | 40B | 35/41 | BAD | structural |
| audio closure rows | 0x005addd0 | AudioListInsertHead | C4 | `re/match/s26/005addd0.c` | **100%** | 0, BYTE-IDENTICAL | | OK | none |
| audio closure rows | 0x005ae650 | AudioPoolConstruct | C3 | `re/match/s26/005ae650.c` | 74.1% | 240B | 106/108 | BAD | structural |
| audio closure rows | 0x005ae800 | AudioPoolBlockAlloc | C3 | `re/match/s26/005ae800.c` | 38.2% | 325B | 123/106 | BAD | structural |
| audio closure rows | 0x005ae920 | AudioPoolFree | C3 | `re/match/s26/005ae920.c` | 96.8% | 3B | 94/94 | OK | SIB-only |
| audio closure rows | 0x005aec30 | AudioByteSwapBuffer | C3 | `re/match/s26/005aec30.c` | 73.2% | 75B | 41/41 | OK | structural |
| audio closure rows | 0x005aeca0 | AudioFieldEndianPack | C3 | `re/match/s26/005aeca0.c` | 78.4% | 97B | 51/51 | OK | structural |
| audio closure rows | 0x005b3760 | FUN_005b3760 | C2 | `re/match/s27/005b3760.c` | 57.1% | 611B | 245/245 | BAD | structural |
| audio closure rows | 0x005b3a00 | FUN_005b3a00 | C2 | `re/match/s27/005b3a00.c` | 84.7% | 255B | 116/118 | BAD | structural |
| audio closure rows | 0x005b3b80 | FUN_005b3b80 | C2 | `re/match/s27/005b3b80.c` | 43.4% | 527B | 238/249 | BAD | structural |
| row-less before this slice | 0x00440bc0 | FUN_00440bc0 | C1 | `re/match/tlr/00440bc0.c` | 12.4% | 413B | 283/217 | BAD | structural |
| row-less before this slice | 0x00474f90 | FUN_00474f90 | C4 | `re/match/tlr/00474f90.c` | **100%** | 0, BYTE-IDENTICAL | | OK | none |
| row-less before this slice | 0x0047f7d0 | FUN_0047f7d0 | C1 | `re/match/tlr/0047f7d0.c` | 76.5% | 74B | 33/34 | BAD | structural |
| row-less before this slice | 0x00487180 | FUN_00487180 | C1 | `re/match/tlr/00487180.c` | 11.7% | 260B | 103/80 | BAD | structural |
| row-less before this slice | 0x004b4eb0 | FUN_004b4eb0 | C1 | `re/match/tlr/004b4eb0.c` | 70.9% | 223B | 116/127 | BAD | structural |
| row-less before this slice | 0x00562ac0 | FUN_00562ac0 | C1 | `re/match/tlr/00562ac0.c` | 19.0% | 647B | 216/327 | BAD | structural |
| row-less before this slice | 0x005aaaa0 | FUN_005aaaa0 | C4 | `re/match/tlr/005aaaa0.c` | **100%** | 0, BYTE-IDENTICAL | | OK | none |
| row-less before this slice | 0x005ac770 | FUN_005ac770 | C1 | `re/match/tlr/005ac770.c` | 79.3% | 51B | 29/29 | OK | structural |
| row-less before this slice | 0x005ac930 | FUN_005ac930 | C1 | `re/match/tlr/005ac930.c` | 71.4% | 61B | 35/35 | BAD | structural |

## Files

- Sources: `re/match/tlr/` (new: 00409680, 00409710, 00474f90, 005aaaa0 matched; 0047f7d0, 005ac770, 005ac930, 004b4eb0,
  00487180, 00440bc0, 00562ac0 near misses), and `re/match/s26/005addd0.c` (now matched).
- Scripts: `verify/tlmatchrest_20261110/` (`score_all.py` -> `scores.tsv`, `residual_kind.py` -> `residual_kinds.tsv`,
  `mksrc.py`, `mktable.py` -> `table.md`, `c4_batch.tsv`).
- Trackers: hooks.csv +9 rows (C1 via c1_add), 3 rows to C4 via c4_reclass, and the matching CHANGELOG lines.
- rva-lint: OK, 0 new. No build input changed.
