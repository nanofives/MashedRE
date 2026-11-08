# RESULT PORT-TOP-MATCH-01, 2026-11-08

**Verdict: GREEN on the RE leg.** 7 of the 8 game targets are byte-identical under `re/tools/match/match.py --bytes`
and relocheck reports OK for each. So is the register-arg callee 0x004840f0, which shares 0x004841d0's file. All 8 rows
are promoted to C4 by route match (L-93) through `re/tools/c4_reclass.py batch`. 0x00407800 stalls at 99.3%. 0x004c5930
and 0x004caea0 are skipped as RenderWare core (L-98). `mashedmod/` is unchanged, so no gates were run.

Base: `orch/integration` @ `ea666edab`. Compiler XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), toolchain
ddk. Every source is plain C: no `__asm`, no `naked`. Names are `FUN_`/`DAT_`/`param_`/`local_` or mechanical
(`Rec_007f1a1c`, `Pair_00411d90`, ...).

## Per target

| # | rva | tracker name | file | match.py | --bytes | relocheck | flags | C before -> after |
|---|---|---|---|---|---|---|---|---|
| 1 | 0x004924f0 | sub_004924f0 | `re/match/topm01/004924f0.c` | 100.0% 100/100 | 464 B BYTE-IDENTICAL | OK 37/0 | /O2 /GS /Ob1 | C3 -> C4 |
| 1 | 0x00493900 | sub_00493900 | `re/match/topm01/00493900.c` | 100.0% 122/122 | 448 B BYTE-IDENTICAL | OK 36/2 (GS cookie pair) | /O2 /GS /Ob1 | C2 -> C4 |
| 1 | 0x004c5930 | sub_004c5930 | not attempted | (s19 source 89.1%) | | | | C2, skipped: RW core |
| 2 | 0x00407800 | FUN_00407800 | `re/match/s01/00407800.c` (unchanged) | **99.3%** 138/138 | not identical | | /O2 /GS /Ob1 | C2, stalled |
| 2 | 0x0040d110 | FUN_0040d110 | `re/match/topm01/0040d110.c` (new) | 100.0% 107/107 | 320 B BYTE-IDENTICAL | OK 16/0 | /O2 /GS /Ob1 | C2 -> C4 |
| 2 | 0x00411d90 | Replay::CreateOrLoad | `re/match/topm01/00411d90.c` (new) | 99.2% 118/117 (*) | 410 B BYTE-IDENTICAL | OK 34/2 (GS cookie pair) | /O2 /GS /Ob1 | C2 -> C4 |
| 3 | 0x004669b0 | FUN_004669b0 | `re/match/topm01/004669b0.c` | 100.0% 32/32 | 160 B BYTE-IDENTICAL | OK 24/0 | /O2 /GS /Ob1 | C2 -> C4 |
| 3 | 0x0047ba00 | FUN_0047ba00 | `re/match/topm01/0047ba00.c` | 100.0% 5/5 | 14 B BYTE-IDENTICAL | OK 1/0 | /O2 /GS /Ob1 | C2 -> C4 |
| 3 | 0x004841d0 | FUN_004841d0 | `re/match/topm01/004841d0.c` | 94.6% 56/53 (*) | 165 B BYTE-IDENTICAL | OK 10/0 | /O2 /GS /Ob1 | C2 -> C4 |
| + | 0x004840f0 | FUN_004840f0 | `re/match/topm01/004841d0.c` (static register-arg callee) | 100.0% 21/21 | 59 B BYTE-IDENTICAL | OK 2/0 | /O2 /GS /Ob1 | C3 -> C4 |
| 3 | 0x004caea0 | FUN_004caea0 | not attempted | (s20 source 77.5%) | | | | C2, skipped: RW core |

(*) The instruction score uses the Ghidra body size from `re/console/pc_funcs.csv`, and that size is short. For
0x00411d90 it is 407 B, which cuts the final 5-byte `jmp 0x004a2be9` (the cookie-check tail at 0x00411f25). For
0x004841d0 it is 159 B, which stops before the `mov eax,1 / pop ebx / ret` tail at 0x0048426e..0x00484273. The
whole-symbol byte compare is identical in both cases, and that is what the C4 gate reads.

## What closed each one

Each source's header comment has the full list of what was tried, with addresses.

- **0x0047ba00** (was 0% in `re/match/s13`): the only call site, 0x00402944 in 0x00402750, pushes two arguments
  (`push 0` at 0x00402937, `push eax` at 0x00402943, `add esp,8` at 0x00402949). With two parameters and
  `return FUN_00496940(param_1);` the DDK stops emitting the tail `jmp`. One parameter gives the `jmp` (0%). Two
  parameters with a void body give `pop ecx` in place of `add esp,4` (80%). 0x00496940 is declared without a
  prototype because its matched body (`re/match/s16/00496940.c`) takes no argument and the original still pushes
  one.
- **0x00493900** (was 66.9%): the token loop is a `do { tok = FUN_004a3ac9(s, seps); s = 0; if (tok) {...} } while (tok);`.
  The DDK does not rotate a do-while, so the call site stays at the loop head (0x00493942), and `s` lives in ebp
  (`xor ebp,ebp` at 0x00493952). The s15 for/while forms were rotated into two call sites.
- **0x004924f0** (was 71.8%): switch cases 1 and 3 written as if/else stores (`if (i < 1) x = 1; else x = 0;`) in
  place of value compares. The DDK then rewrites the compare of `i` as a row-pointer compare and removes `i`
  (pointer compares 0x7f0cc4 / 0x7f0d00 as in the original). The memset of DAT_007f0f60 goes above
  `DAT_007f0f2c = 0` (`rep stosd` at 0x00492502 before the store at 0x00492504).
- **0x0040d110** (new): an index loop `i < 4` over a 16-byte overlay gives the signed `jl` at 0x0040d20d. The two zero
  dwords of the frame record are zeroed before the colour byte stores (`xor ebx,ebx` at 0x0040d123, zero byte store at
  0x0040d14e). 0x00420420 gets two pushed arguments (0x0040d1f5/0x0040d1f6, `add esp,8`), while its matched body
  (`re/match/s05/00420420.c`) takes one, so it is declared without a prototype.
- **0x00411d90** (new): after the call to 0x00411350 the original loads the two int outputs once into esi/edi
  (0x00411eb7/0x00411ebf) and pushes them again after 0x004987b0 (0x00411ed6/0x00411ed7). The DDK does that only when
  0x00411350's body is in the same translation unit. So the file includes the matched text of
  `re/match/s02/00411350.c` (0x00411350 is already C4) under `#ifndef __cplusplus`, and C++ builds see only the
  prototype. With only the prototype it scores 82.5%. Also: `sub eax,2` at 0x00411dad is a one-case switch on the
  0x0042f6a0 result. The `first key != -1` guard before the {int, float} table loop at 0x005f29d0 is needed.
- **0x004669b0** (was 84.4%): `return 1` (eax = 1 at the ret, `mov eax,1` at 0x00466a0f; the only caller,
  0x004028db, does not read eax). 0x00690490..98 typed `float[3]` (read by fsub/fadd at 0x0045d85c/87b/896). 0x008aa300
  typed `int[4]`, cleared by one 16-byte memset (indexed by esi*4 at 0x00463883 and others). The zero stores to
  DAT_006900d0 and DAT_0068fccc go before the memset.
- **0x004841d0 + 0x004840f0** (was 75.0%): in the static callee, `DAT_006ce840[idx] = 0;` before
  `DAT_006cec40[idx] = p;` with `DAT_006ce840[idx]++`, and the parameter order `(int idx, char *p)`, give the original's
  EDX = idx / EAX = p and the `lea ecx,[edx*4+0x6ce840]` at entry. The letters loop is an unsigned 0..25 counter
  passing `DAT_00613b28[u]` directly.

## Stalled: 0x00407800 at 99.3%

The source stays `re/match/s01/00407800.c`, and no topm01 copy was written. 138/138 instructions, ops 100%. One
store/load pair is swapped. The original stores `mov dword ptr [esi+0xb0], 0` (rec+0xc8) before
`mov eax, [esi+0x9c]` (rec+0xb4, the table index), and the DDK schedules the load first. Tried in this row, all 99.3%
or worse:
- temp for the call result in both store orders (86.3% / 99.3%)
- all stores through `DAT_00639d80[i]` (78.6%)
- int store, comma expression, index in a local, explicit byte-offset table pointer, `(&r->fc8)[0]`
- the index call folded into the `FUN_004b47e0` argument
- `static __inline` helpers around the zero store, the table load, both, the call + zero store, and a returning
  table helper
- the +0xc8/+0xcc pair as a sub-struct, an `int[2]`, and an `int[2]` with a float store
- `/G5` (89.9%), `/G6`, `/G7`, `/GB`
- `match_permute.py` 4 rounds + a 400-step random walk, 1365 compiles, no gain (log: `log/permute/00407800.c`). The
  earlier slice ran 285 compiles.

Remaining diff: that one instruction pair. Class (b) scheduling.

## Skipped: RenderWare core (L-98)

`re/analysis/library_relabel_2026-10-11.md` puts the RenderWare core code range at `[0x004c0700, 0x00558c50)`. Both
targets are inside it, and the tracker labels them `boot` / `render`.
- **0x004c5930**: in the RW texture-dictionary block (0x004c5890 RwTexDictionaryCreate, 0x004c5a60 and 0x004c5da0
  are `third-party-library[renderware]`). The s19 source note records the RwTexDictionaryDestroy shape: it clears plugin
  slot +0x10 of DAT_007d4054, loops FUN_004c5a60, calls FUN_004d8060, unlinks the node and frees through slot +0x11c.
- **0x004caea0**: in the RW D3D backend block. The 2026-10-11 relabel left it labelled first-party only because it reads
  0x006181c8 / 0x006181cc / 0x006181d8, which game code 0x00499d90 / 0x0049a150 / 0x0049a2e0 writes. Its range is RW
  core. If the lead wants it treated as game code, the s20 source (77.5%) is the start.

## C4 claim

`verify/porttopm01_20261108/c4_batch.tsv`, 8 rows. Dry run: `batch: 8 pass, 0 refused, 0 skipped (already C4), 8 rows`.
`--apply --result verify/porttopm01_20261108/RESULT_PORT-TOP-MATCH-01.md` updates hooks.csv (8 rows to C4,
`c4=match`) and adds 8 CHANGELOG lines.

## Notes for the port rows (later PORT-TOP rows)

- C++ prototypes need to agree before these bodies are compiled as C++. 0x0040d110 is defined
  `void FUN_0040d110(int **param_1, int param_2)`, while `re/match/top07/0040d270.c` declares it `(int, int)`. Its
  `FUN_0041a8d0(int *, RGBA_0040d110 *)` differs from the matched `(int, RwRGBA_ *)`. The DDK C match does not care,
  but C++ name mangling does.
- Unprototyped callees (`FUN_00496940` in 0047ba00.c, `FUN_00420420` in 0040d110.c) are declared `(...)` under
  `__cplusplus`.
- 0x00411d90 carries 0x00411350's body only under C. C++ builds bind 0x00411350 by name.

## Gates

Not run. `mashedmod/` is unchanged on this branch: match sources, RESULT and trackers only.

## Files

- `re/match/topm01/0047ba00.c`, `00493900.c`, `004924f0.c`, `0040d110.c`, `00411d90.c`, `004669b0.c`, `004841d0.c` (new)
- `verify/porttopm01_20261108/RESULT_PORT-TOP-MATCH-01.md`, `c4_batch.tsv`
- `hooks.csv`, `re/analysis/CHANGELOG.md` (through c4_reclass.py `--apply`)
