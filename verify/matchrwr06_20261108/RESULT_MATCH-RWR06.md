# RESULT MATCH-RWR06 (MATCH-RWLIB-2 slice R06)

Status: **GREEN, honest partial**. Match-only: no build, no game, no gates, no tracker edits, no promotion.
Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 via `re/tools/match/match.py`, file flags `/O2 /Ob1 /G5` on every row.
All 7 rows below are plain C, no `__asm`, no naked function, byte-identical with relocations masked
(`bytes_bad=0`), and `re/tools/match/relocheck.py` reports OK for each (`relocheck.tsv`). `results.tsv` covers all
51 rows, with the round-1 best in `before%`.

## Counts

- Rows at 100% byte match with relocheck OK: **7 / 51** (before 0 / 51).
- 5 of the 7 are new sources for Ghidra-missed rows that had no file (0x0052df20, 0x00537b20, 0x00537d20, 0x00537d70,
  0x00537e60). 2 are reworked sources (0x004eb9e0, 0x005333a0).
- Raised but not at 100%: 0x00536e90 (no source -> 99.1), 0x00533d00 48.2 -> 97.6, 0x0052eb40 76.5 -> 88.0,
  0x00536780 (no source -> 21.8).

## Blocker for the C4R batch: 8 rows have no pc_funcs.csv entry

The 8 Ghidra-missed rows (0x0052df20, 0x00536780, 0x00536e90, 0x00536fe0, 0x00537b20, 0x00537d20, 0x00537d70,
0x00537e60) are not in `re/console/pc_funcs.csv`, so `match.py` refuses them ("no Ghidra size"). They were scored with
`tools/ns.py`, which sets the original extent read from the disassembly (`tools/odis2.py`: up to the `ret` followed
by `90` padding). Each new source states its extent in its header. The 5 identical ones also compare byte for byte
over the whole symbol (`bytes_n` = original size, `bytes_bad=[]`).

| rva | extent | end | next bytes |
|---|---|---|---|
| 0x0052df20 | 19 | 0x0052df33 | 90 x8 |
| 0x00537b20 | 84 | 0x00537b74 | 90 x8 |
| 0x00537d20 | 71 | 0x00537d67 | 90 x8 |
| 0x00537d70 | 106 | 0x00537dda | 90 x6, then 0x00537de0 |
| 0x00537e60 | 105 | 0x00537ec9 | 90 x7, then 0x00537ed0 |
| 0x00536780 | 443 | 0x0053693b | 90 x5, then 0x00536940 |
| 0x00536e90 | 330 | 0x00536fda | 90 x6, then 0x00536fe0 |
| 0x00536fe0 | 528 | 0x005371f0 | next function |

0x00536780 calls these as callbacks (stores at 0x005368df, 0x00536910, 0x00536878, 0x005368aa, 0x00536840), and
hooks.csv carries them as C1 `third-party-library[renderware]`. A C4R batch can promote the 5 identical ones only after
a pc_funcs.csv step adds the 8 rows (precedent: MATCH-S19-FU-PCFUNCS, C4R-DXLIB-GHIDRAFN). This slice did not edit
pc_funcs.csv (out of scope).

## Per-row before / after (rows touched)

| rva | size | before | after | what moved it |
|---|---|---|---|---|
| 0x0052df20 | 19 | no source | 100 | RtAnimInterpolatorSetAnimLoopCallBack: two stores, returns param_1 (eax = param_1 at the ret) |
| 0x00537d20 | 71 | no source | 100 | color packed as `((((a<<8)\|r)<<8)\|g)<<8\|b` from +0x7f/+0x7c/+0x7d/+0x7e, 4 strided stores per step |
| 0x00537b20 | 84 | no source | 100 | same packing from a strided per-particle byte array |
| 0x00537d70 | 106 | no source | 100 | per-particle uv through `a = src; b = src + 2` |
| 0x00537e60 | 105 | no source | 100 (first 97.1) | constant uv read through `uv = param_3 + 0x24`, which moves the param_3 load before the count test |
| 0x004eb9e0 | 85 | 64.7 | 100 | index read into a `ushort` local (gives `mov ax; and eax,0xffff` per element), range stored before min |
| 0x005333a0 | 116 | 64.9 | 100 | offset read into a plain local: the compiler homes it in param_2's dead stack slot on its own; typed entry array, returns param_2 |
| 0x00536e90 | 330 | no source | 99.1 | vertices as `p + (-r - u)` keep the original fchs/operand order (`-r - u + p` folds to `p - (u + r)`). One load (posStride into ebp) sits 3 slots later. Permuter 1195 compiles |
| 0x00533d00 | 247 | 48.2 | 97.6 (ops 100) | typed struct + indexed loop, aligned pointer stored before raw. The compiler-made p3-p2 delta lands in param_1's slot, the original's in param_5's. 6 perturbations, no change |
| 0x0052eb40 | 269 | 76.5 | 88.0 | copy loop as `while (n--)` with `n*0x40` offsets, header fields through param_1[3]/[5]. Residual: param_4 loaded into ebp then shl, original into ecx then `lea ebp,[ecx*4]`. Permuter 322 compiles |
| 0x00536780 | 443 | no source | 21.8 | written (compiles, relocations named). See the single-bit class below |
| 0x0052ea70 | 205 | 71.8 | 71.8 (unchanged) | a guarded do-while with an explicit row pointer reaches ops 97, but the row copy gets a new slot instead of param_3's. File left as it was |
| 0x005300d0 | 137 | 58.9 | 58.9 (unchanged) | early-fail returns class (4 shapes). File left as it was |
| 0x00535700 | 248 | 51.7 | 51.7 (unchanged) | single-bit class + ecx/edx swap. File left as it was |

## 100% rows (sources and sha256)

- `re/match/rwl03/0052df20.c` 8f3519f305c5a8ae77423e7b6d02a410269a7868358fcd9e7d0e8c1beebc0b82
- `re/match/rwl03/00537b20.c` dfc55b4439a4d24eee1937cad3be6213a7d615642bb10c35433bf7f7fb7aefe4
- `re/match/rwl03/00537d20.c` 067462c7f952a1e3d0190ec479d83aeb2d581002684428b64aa70bf62cc49678
- `re/match/rwl03/00537d70.c` fbf7e0e2515ff5643748bb7cc3ee2f10d88198946a346141f49fe79def8cfa51
- `re/match/rwl03/00537e60.c` 1bf3b03f36100edc6c10a6dd3506417eaab388c5823bd49a946142b4a563a276
- `re/match/rwl02/004eb9e0.c` 905a1985d14286134575833c2aaaa4a9faab496c4298d309d3b426dd97d2b8c6
- `re/match/rwl03/005333a0.c` 96f5022268358206301519558ec33a5a1bd9ed43f80316a44a45f51ae5e739e8

## Findings for the other slices

1. **The compiler homes address-taken locals in dead parameter slots itself.** 0x005333a0's original passes
   `&param_2` as a read buffer. The g2s source wrote exactly that and copied param_2 to a local (64.9%). A plain
   `uint off;` local plus `return param_2` puts `off` in param_2's home slot and gives 100%. Before writing into a
   param slot by hand (lever d), try a fresh local: the compiler often picks the same slot.
2. **Which dead param slot a temp gets is not steerable by declaration or type** (0x00533d00, 0x0052ea70): param types,
   declaration order, loop forms and a 120-variant permuter did not move it.
3. **x87 `-a - b + c` folds to `c - (b + a)`.** Writing `c + (-a - b)` keeps the original's `fld; fchs; fsub; fadd`
   (0x00536e90, 79.3 -> 91.9).
4. **New residual class: single-bit `(f & C) == C` kept as `mov edx,eax; and edx,C; cmp dl,C; jne`.** The DDK always
   folds it to `test`. Tried on 0x00536780 / 0x00535700: casts (uchar, int, ulong, __int64), ternary, `== 1` on the
   compare, inline-function wrapper, static const masks, a mask through a variable, xor/sub forms, boolean locals, C++
   front end, and flags `/O1 /Os /Ot /Og /Od /Op /Oy- /Ox /GX /Za /J /G6`. Only the `sub` form keeps the and/compare
   pair (`sub` instead of `cmp`). No 100% source in re/match has a single-bit `(x & C) == C`. Both rows are prtstd
   particle code. A later slice could check whether the RW toolkit objects were built by another compiler build.
5. **match_permute on rows missing from pc_funcs.csv**: `tools/nsperm.py VA SIZE <permute args>` sets the size
   in-process (the permuter scores in threads, so the override holds).

## Open rows worth a follow-up (do not repeat)

- 0x00536e90 (99.1): one load of posStride (`mov ebp,[esp+0x14]`) scheduled 3 slots later than the original. Tried:
  4 placements of `pos += pstride` (they break the p/pos aliasing, ~60%), no-alias forms, permuter 1195 compiles.
- 0x00533d00 (97.6): delta temp slot (param_1 vs param_5).
- 0x0052eb40 (88.0): param_4 temp register (ecx + lea vs ebp + shl). Variable, expression, shift and pointer forms of sz4 tried.
- 0x00536fe0: no source. 528 B x87 rotated billboard: `fsincos` (inline asm or intrinsic) writing sin/cos through
  `lea` of two param slots, then the same 4-vertex pattern as 0x00536e90. Not attempted (budget).
- 0x004f1fb0: compile fail, not reworked.
- 0x004e6650 (70.0): list-terminator CSE class from R02, not retried.
- Rows below 70% other than those above: not reworked (budget). Large raw-converter rows (>1 KB) untouched.

## Tools and raw (not committed)

- `tools/`: `ns.py` (score with an explicit size), `nsperm.py` (permuter with an explicit size), `odis2.py`
  (disassemble to the `ret` + padding), `mk.py` (FLAGS + note + preamble + body, CRLF), `vtry.py` (variant runner,
  `OLD = '@TAIL'` replaces the whole function), `collect6.py` (this folder's tables), `base.py` (baseline + void scan).
- Raw, D:/MashedWT/MATCH-RWR06_data (not committed): `specs/` (variant specs), `scratch/` (variant sources),
  `decomp1.txt` sha256 f05e012c8e20ad1989d6496c86d1062c418b9ff407d249a5a60f1f18c8edcd15 (decomp_pc.py, pool slot 0,
  read-only), `collect6.log` sha256 1efed68b70511c6abfda1e8d329f4166b1b56297bcfecf3cb84aac1c84b38dcc.
- No Ghidra slot was created or refreshed. `decomp_pc.py` took and released an existing slot.
