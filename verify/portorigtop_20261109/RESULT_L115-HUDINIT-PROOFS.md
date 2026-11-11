# RESULT L115-HUDINIT-PROOFS (2026-11-10): every differing byte of 0x005c4ad0 and 0x00555360 accounted for

L2 worker `cmwb4f8nh18ymsc1bf03upqca` for lead `cmw7xu1960f37p41557tefpnd`. Text-only branch `orch/L115-HUDINIT-PROOFS` on
`orch/HUDINIT-A` `f70353697`. Nothing under mashedmod/ changes. It answers L0's standard for a new L-115 class: account for every
differing unmasked byte that `re/tools/match/match.py --bytes` reports, and label each one "explained by <class>" or "other".

Run 37 (L-129 (a), 2026-11-11): cherry-picked alone onto orch/integration `778b8b91d` as `orch/L115-HUDINIT-PROOFS`, with
no dependency on HUDINIT-A code. `l115_shiftcheck.py` / `l115_bytediff.py` read only re/match sources, re/tools/match
and the original binary. This text cites no librw binding; the 0x004c5890 binding HUDINIT-A carried earlier is dropped
by L-129 (a) and does not bear on either proof.

## Summary

| function | source | differing bytes | result |
|---|---|---|---|
| 0x005c4ad0 | re/match/s28/005c4ad0.c (96.7%, ops 100%) | 12 | **other found at 0x005c4b57..0x005c4b61** (11 bytes): the load of the first multiplicand and the independent store `[esp+4] = 1` are in the other order. 1 byte (0x005c4b65) is class 1 |
| 0x00555360 | re/match/s24/00555360.c (94.4%) | 55 | **all explained**: 50 bytes by class 2 (return blocks and the byte shift they cause), 5 bytes by class 3 (two-store order) |

Classes, as named in RESULT_HUDINIT-A.md:
1. Commutative operand order of one integer multiply.
2. Return-block layout with an equal eax.
3. Order of two stores to non-overlapping addresses, with no read between them.

**Consequence for 0x005c4ad0.** Class 1 alone does not cover it. The residual is class 1 plus a fourth difference: the
order of the 3-byte load `mov edx, [esi + 8]` and the 8-byte store `mov dword ptr [esp + 4], 1`.

The two are independent:
- The store writes the stack slot `[esp + 4]` (the ErrRec record's first word). The load reads the heap header `[esi + 8]`,
  which is a fresh RwGlobals +0x108 allocation that cannot be the caller's stack.
- Neither one writes a register the other reads.
- No flags are involved: `mov` sets none.

So the two orders leave the same state. That is a store/load reorder of independent accesses, a class of its own. It is
reported as "other" here, as L0's standard asks.

## How the bytes were produced

- `verify/portorigtop_20261109/l115_bytediff.py <source>` lists each differing unmasked byte with the instruction that holds
  it on each side. It uses `match.py`'s own `compile_obj` / `byte_compare` / `original_bytes` (recorded DDK
  13.00.9176 / 13.00.9178, the source's `// FLAGS`, relocation sites masked on both sides).
- `verify/portorigtop_20261109/l115_shiftcheck.py` labels every byte and checks each claim byte for byte. The tables below
  are its output, unedited. The checks it prints:

```
== 0x005c4ad0
check: window bytes as stated (ours): True     ours  +0x87 c744240401000000 | 8b560c | 0faf5608
check: window bytes as stated (orig): True     orig  +0x87 8b5608 | c744240401000000 | 0faf560c
check: differing bytes outside the window: 0
== 0x00555360
check: ours +0x2d = 74 4b (je +0x7a): True
check: orig +0x2d = 75 01 c3: True
check: ours +0x59 = 74 1f (je +0x7a): True
check: orig +0x5a = 75 01 c3: True
check: ours +0x78 = 75 03 33 c0 c3: True
check: orig +0x7a = 75 01 c3: True
check: ours [+0x2f,+0x59) == orig shifted +1 (unmasked): True
check: ours [+0x5b,+0x78) == orig shifted +2 (unmasked): True
check: ours +0x107 = 668908 668970fe: True
check: orig +0x107 = 668970fe 668908: True
```

### 0x005c4ad0: the 15-byte window +0x87..+0x95

- Both sides hold the same three instructions in the window:
  - the store `c7 44 24 04 01 00 00 00`;
  - a load `8b 56 xx`;
  - `0f af 56 yy` (imul).
- Ours: store, load [esi+0xc], imul [esi+8]. Original: load [esi+8], store, imul [esi+0xc].
- The window has the same length on both sides. Everything outside it is byte-identical (relocations masked), so no
  offset or jump displacement elsewhere moves.
- Byte +0x95 is the imul's disp8 (08 against 0c): the two multiplicands swapped, class 1.
- Bytes +0x87..+0x91 are the load and the store in the other order: other, as above. +0x92..+0x94 (`0f af 56`) are equal
  and not listed.

### 0x00555360: the shift the return blocks cause

The original ends each failed check with its own `jne +1` / `ret` (3 bytes). Ours:
- uses a 2-byte `je` to one shared `xor eax, eax / ret` for the first two checks;
- for the third, a `jne +3` over that shared block.

So our code between the checks sits 1 byte (after the first) and then 2 bytes (after the second) before the original's,
and both realign at +0x7d, where our shared block ends and the original's third `ret` ends. The checks above show:
- ours [+0x2f, +0x59) equals the original 1 byte on, and ours [+0x5b, +0x78) equals it 2 bytes on, every unmasked byte;
- the relocated operands at the shifted sites hold, in the original, the globals the source names there in that order:

| ours site | original site | original value | source operand |
|---|---|---|---|
| +0x31 | 0x00555392 | 0x00623ea0 | `DAT_00623ea0` |
| +0x36 | 0x00555397 | 0x00623e9c | `DAT_00623e9c` |
| +0x40 | 0x005553a1 | 0x007dc7c4 | `&DAT_007dc7c4` |
| +0x4b | 0x005553ac | 0xfff77630 | rel32 of `call FUN_004cc9e0` (0x005553b0 + 0xfff77630 = 0x004cc9e0) |
| +0x53 | 0x005553b4 | 0x00912a14 | `DAT_00912a14` |
| +0x5d | 0x005553bf | 0x007d3ff8 | `DAT_007d3ff8` |
| +0x72 | 0x005553d4 | 0x00912a0c | `DAT_00912a0c` |

From +0x7d to the end, nothing differs except the loop's store pair at +0x107..+0x10d. The original holds
`66 89 70 fe` then `66 89 08`, and ours the reverse: the same 7 bytes. The loop's backward `jl` displacement and every
later relative jump are equal (not listed), which follows from the realignment at +0x7d.

## Table: 0x005c4ad0 (12 bytes)

| off | addr | ours | orig | label | reason |
|---|---|---|---|---|---|
| +0x87 | 0x005c4b57 | c7 | 8b | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x88 | 0x005c4b58 | 44 | 56 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x89 | 0x005c4b59 | 24 | 08 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x8a | 0x005c4b5a | 04 | c7 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x8b | 0x005c4b5b | 01 | 44 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x8c | 0x005c4b5c | 00 | 24 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x8d | 0x005c4b5d | 00 | 04 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x8e | 0x005c4b5e | 00 | 01 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x8f | 0x005c4b5f | 8b | 00 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x90 | 0x005c4b60 | 56 | 00 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x91 | 0x005c4b61 | 0c | 00 | other | the 3-byte load of the first multiplicand (8b 56 disp) sits after the 8-byte independent store [esp+4] = 1 in ours and before it in the original: two independent instructions in the other order (not class 1). The byte also carries the swapped load displacement (0c vs 08) where it is the disp8 |
| +0x95 | 0x005c4b65 | 08 | 0c | explained by class 1 | imul m32 displacement: ours [esi+8], orig [esi+0xc] (the two multiplicands swapped) |

## Table: 0x00555360 (55 bytes)

| off | addr | ours | orig | label | reason |
|---|---|---|---|---|---|
| +0x2d | 0x0055538d | 74 | 75 | explained by class 2 | first failed check: ours je +0x7a (shared xor/ret), orig jne +1 over its own ret |
| +0x2e | 0x0055538e | 4b | 01 | explained by class 2 | first failed check: ours je +0x7a (shared xor/ret), orig jne +1 over its own ret |
| +0x2f | 0x0055538f | 8b | c3 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x30 | 0x00555390 | 15 | 8b | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x35 | 0x00555395 | a1 | 00 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x3a | 0x0055539a | 68 | 00 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x3b | 0x0055539b | 90 | 68 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x3c | 0x0055539c | 01 | 90 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x3d | 0x0055539d | 04 | 01 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x3e | 0x0055539e | 00 | 04 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x3f | 0x0055539f | 68 | 00 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x44 | 0x005553a4 | 52 | 00 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x45 | 0x005553a5 | 6a | 52 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x46 | 0x005553a6 | 04 | 6a | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x47 | 0x005553a7 | 50 | 04 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x48 | 0x005553a8 | 6a | 50 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x49 | 0x005553a9 | 0c | 6a | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x4a | 0x005553aa | e8 | 0c | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x4f | 0x005553af | 83 | ff | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x50 | 0x005553b0 | c4 | 83 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x51 | 0x005553b1 | 18 | c4 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x52 | 0x005553b2 | a3 | 18 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x57 | 0x005553b7 | 85 | 00 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x58 | 0x005553b8 | c0 | 85 | explained by class 2 | shift +1 after the first check: ours[off] == orig[off+1] |
| +0x59 | 0x005553b9 | 74 | c0 | explained by class 2 | second failed check: ours je +0x7a, orig jne +1 over its own ret |
| +0x5a | 0x005553ba | 1f | 75 | explained by class 2 | second failed check: ours je +0x7a, orig jne +1 over its own ret |
| +0x5b | 0x005553bb | 8b | 01 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x5c | 0x005553bc | 0d | c3 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x61 | 0x005553c1 | 68 | 7d | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x62 | 0x005553c2 | 90 | 00 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x63 | 0x005553c3 | 01 | 68 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x64 | 0x005553c4 | 04 | 90 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x66 | 0x005553c6 | 6a | 04 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x67 | 0x005553c7 | 51 | 01 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x68 | 0x005553c8 | ff | 6a | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x69 | 0x005553c9 | 91 | 51 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x6a | 0x005553ca | 08 | ff | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x6b | 0x005553cb | 01 | 91 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x6c | 0x005553cc | 00 | 08 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x6d | 0x005553cd | 00 | 01 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x6e | 0x005553ce | 83 | 00 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x6f | 0x005553cf | c4 | 00 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x70 | 0x005553d0 | 08 | 83 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x71 | 0x005553d1 | a3 | c4 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x76 | 0x005553d6 | 85 | 91 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x77 | 0x005553d7 | c0 | 00 | explained by class 2 | shift +2 after the second check: ours[off] == orig[off+2] |
| +0x78 | 0x005553d8 | 75 | 85 | explained by class 2 | third check: ours jne +3 over the shared xor eax,eax / ret, orig test/jne/ret shifted +2; both realign at +0x7d |
| +0x79 | 0x005553d9 | 03 | c0 | explained by class 2 | third check: ours jne +3 over the shared xor eax,eax / ret, orig test/jne/ret shifted +2; both realign at +0x7d |
| +0x7a | 0x005553da | 33 | 75 | explained by class 2 | third check: ours jne +3 over the shared xor eax,eax / ret, orig test/jne/ret shifted +2; both realign at +0x7d |
| +0x7b | 0x005553db | c0 | 01 | explained by class 2 | third check: ours jne +3 over the shared xor eax,eax / ret, orig test/jne/ret shifted +2; both realign at +0x7d |
| +0x109 | 0x00555469 | 08 | 70 | explained by class 3 | the two 16-bit stores [eax] / [eax-2] in the other order (same 7 bytes) |
| +0x10a | 0x0055546a | 66 | fe | explained by class 3 | the two 16-bit stores [eax] / [eax-2] in the other order (same 7 bytes) |
| +0x10b | 0x0055546b | 89 | 66 | explained by class 3 | the two 16-bit stores [eax] / [eax-2] in the other order (same 7 bytes) |
| +0x10c | 0x0055546c | 70 | 89 | explained by class 3 | the two 16-bit stores [eax] / [eax-2] in the other order (same 7 bytes) |
| +0x10d | 0x0055546d | fe | 08 | explained by class 3 | the two 16-bit stores [eax] / [eax-2] in the other order (same 7 bytes) |

## Checks

- rva-lint OK (no mashedmod/ change).
- The two scripts are read-only: they compile into match.py's temp directory and print.
