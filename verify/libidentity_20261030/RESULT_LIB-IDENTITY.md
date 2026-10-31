# LIB-IDENTITY: library-object identity measurement (2026-10-30)

Measurement only. No tracker, hooks.csv, ledger or `original\` write, no MASHED.exe run, no Frida, no promotion.
Reference: `original\MASHED.exe.unpatched` (SHA-256 BDCAE093..., checked by `coffpe.Image` on load), read as a file.

## Verdict

Partial identity with the period compiler available (XP DDK cl 13.00.9176 / c2 13.00.9178).
406 of 943 compared library rows are instruction-identical to their library object: Lua 87 / 294, qhull 116 / 307,
CRT 203 / 342 (DDK libcmt.lib). Another 213 library functions are identical at addresses outside these rows (197 of them
CRT functions tagged boot / d3dx9-shader-compiler / render / util). d3dx9 (419 rows), DirectShow BaseClasses (159) and
libpng (4) were not compared: no matching library object was obtainable within this run's fetch rule (section 4).

The residual differences are prologue / argument-load order and register allocation, at the same flags that give the
identical functions. The game's Rich header lists its C objects at cl 13.00.9466 (prodid 28 build 9466 x373), not 9178.
[UNCERTAIN] that the residual is the 9178 -> 9466 compiler difference: it needs a 13.00.9466 C2 to confirm.

## Counts

| library | rows (hooks.csv subsystem) | identical | of which ambiguous | differs | not compared | identical at other addresses |
|---|---|---|---|---|---|---|
| lua-4.0 | 294 (`[lua-4.0]`) | 87 | 0 | 176 | 31 | 10 (9 no row, 1 `render`) |
| qhull-2002.1 | 307 (`[qhull-2002.1]` 227 + `-continuation` 80) | 116 | 0 | 152 | 39 | 6 (no row) |
| msvc-crt vs DDK libcmt.lib | 342 (`[msvc-crt-main]` 302, `-tail` 38, `[msvc-crt]` 2) | 203 | 17 | 88 | 51 | 197 |
| d3dx9 | 419 (`[d3dx9-*]`) | - | - | - | 419 | - |
| directshow-baseclasses | 159 | - | - | - | 159 | - |
| libpng | 4 | - | - | - | 4 | - |

- identical: every byte of the object function (to the next function symbol, int3/nop fill trimmed) equals the image,
  except 4-byte relocation sites; a relocation into the same function (switch tables) must also hold the right address.
  Relocations to other symbols are masked (MASHED.exe has no base relocations) but the address each implies is voted on
  across all identical functions, and a candidate that disagrees with the vote loses.
- ambiguous: the same bytes as another library function too (CRT `_fread` / `_fwrite`, `_ceil` / `_floor` / `_modf`,
  import-jump thunks such as `_atoi` = 10 other names). The address is identical to a library member, the name is not
  determined.
- differs: paired with one library function by instruction-mnemonic similarity (difflib ratio >= 0.6, objects with an
  identical neighbour within 0x800 bytes first). The first differing offset and instruction pair are in the TSV.
  Pairs under 32 bytes that differ at +0x0 were dropped to not-compared (the pairing says nothing there).
- not compared: no library function paired (inlined in our build, not in this source, or a function the game added).
- CRT against the DDK's single-threaded libc.lib: 122 identical, 157 differs, 63 not compared. libcmt.lib (multithreaded)
  is the closer archive.

## Flags found

| library | flags | evidence |
|---|---|---|
| Lua 4.0 | `/O2 /Ob1 /G5` (`/Og /Oi /Ot /Oy /Ob1 /G5`; `/Gs` `/GF` `/Gy` change nothing; `/Ob0`, `/G3`, `/G4` give the same 87) | `/G6` or blend: 33; `/Ob2` (plain `/O2 /G5`, `/Ox /G5`): 73-74; `/Oy-`: 0; `/O1`: 1; `/Os`: 3; `/Op`: 16 |
| qhull 2002.1 | `/O2 /G5` (`/Ob2` with `/GF`; `/G4` the same 116) | `/O2` blend: 14; `/G6`: 13; `/O2 /Ob1 /G5`: 112; `/Ox /G5`: 112; `/Op`: 91; `/O1`: 0 |
| CRT | DDK libcmt.lib members (C 13.00.9178 x540, MASM 7.00.9210 x84, C++ 13.00.9178 x27) | identical: C 255, C++ 63, MASM 82 members (rows and other addresses together) |

`/G5` (or `/G3` / `/G4`, indistinguishable here) is the switch that matters: it stops 9178 from splitting
read-modify-write memory ops (`add [esi], 0x10` vs `mov eax, [esi]; add eax, 0x10; mov [esi], eax`, lua_pushnil 0x004b7110).
Lua and qhull differ in inlining (`/Ob1` vs `/Ob2`), so they were built as two projects with different settings.

Rich header of the anchor (raw prodid / build / count): 28/9466 x373, 29/9466 x18, 64/9466 x57, 69/9466 x1, 61/9466 x1,
28/9178 x8, 29/9178 x49, 25/9210 x19, 48/9044 x349, 39/9162 x3, 37/8755 x2, 42/8803 x3, 18/8444 x6, 1/0 x188.
Our Lua / qhull objects carry @comp.id 28/9178. The DDK CRT members carry 28/9178, 29/9178 and 64/9210; the anchor has no
64/9210 entry and has 64/9466 x57, so the anchor's prodid-64 objects are not DDK CRT members. [UNCERTAIN] which objects
the 28/9178 x8 and 29/9178 x49 entries are: the Rich header holds counts only, no per-object address.

## Residual-diff categories (first differing instruction of each pair)

| category | Lua | qhull | CRT |
|---|---|---|---|
| prologue / stack frame / argument-load order (`push esi` vs `mov eax, [esp+4]` first, `sub esp, 0x28` vs `0x2c`) | 82 | 52 | 26 |
| register allocation (same instruction, other registers) | 59 | 54 | 14 |
| operand / constant (same instruction, other immediate or displacement) | 25 | 18 | 26 |
| branch displacement (a size difference earlier in the body) | 7 | 22 | 17 |
| instruction selection / scheduling (`imul eax, eax, 0xf` vs two `lea`, luaT_validevent 0x004b9540) | 2 | 6 | 4 |
| x87 (double copied with two `mov` vs `fld` / `fstp`, luaV_tostring 0x004b7f70) | 1 | 0 | 0 |
| read-modify-write split or fused | 0 | 0 | 1 |

Checked by hand: luaT_validevent's table at 0x005d8880 is Lua 4.0's 6 x 15 `luaT_validevents` byte for byte; the
original indexes it as `3t + (e + 12t)` = `15t + e`, our build as `imul 15`. Same source, different instruction selection.
In the CRT, frame-size differences (`__cftoe` 0x004a5b7a `sub esp, 0x28` vs `0x2c`, `__except1` 0x004a85d2 `0x70` vs
`0x80`) are consistent with a different CRT source revision as well as a different compiler; they are not separated here.

## Findings

1. Lua is 4.0, not 4.0.1 (string at 0x005d8790 "Lua 4.0"). The 4.0.1 source scores one more: luaC_checkGC 0x004ba570 is
   identical only when `luaC_collectgarbage` is extern (4.0.1). In 4.0 it is static and 9178 tail-jumps without
   re-storing the argument. [UNCERTAIN] whether the game patched lgc.c or 9466 does not do that tail call: no other
   4.0 / 4.0.1 change separates the two.
2. 197 functions identical to DDK libcmt members sit at hooks.csv rows of other subsystems: boot 102,
   third-party-library[d3dx9-shader-compiler] 40, render 7, util 7, save 2, audio 2, frontend 1 (36 more have no row).
   Lua: `_lua_typename` is identical at 0x004b7020, a `render` row. Candidates for re-classify (not done here).
3. Functions identical at addresses with no hooks.csv row: Lua 7 addresses, 9 entries (`_errormessage` 0x004b7450, `_f_call` 0x004b79a0,
   `_f_parser` 0x004b7a20, `_math_atan2` 0x004c0000, `_math_randomseed` 0x004c04f0, `_math_mod`/`_math_pow` at
   0x004c00b0 and 0x004c0560), qhull 6 (`qh_comparevisit` 0x00585360, `qh_compareangle` 0x00585d20,
   `qh_comparemerge` 0x00585d50, `qh_compare_facetvisit` 0x0059dd50, `qh_compare_facetmerge` 0x0059e720,
   `qh_printvridge` 0x005a5790). Full list: TSV rows with status `identical-unlisted`.
4. D3DX9 is linked statically (no d3dx9_*.dll import; strings "D3DX9 Shader Compiler" 0x005e2d30, "D3DX9 Shader
   Assembler" 0x005db9d4, "D3DXPSGP" 0x005daf87). The renderer calls `Direct3DCreate9(0x1f)` at 0x004c7e02-0x004c7e0b:
   D3D_SDK_VERSION 31 = DirectX 9.0 / 9.0a / 9.0b headers (9.0c is 32). No D3DX version string beyond those names.
5. libpng 1.0.12 (string 0x005e3200) with zlib 1.1.3 ("deflate 1.1.3" 0x005e33b1, "inflate 1.1.3" 0x005e42f1, "1.1.3"
   0x006214b0).

## 4. Availability of the SDK libraries (fetch rule: microsoft.com / official DirectX SDK archives only)

- d3dx9 (static, DX 9.0-9.0b era) and DirectShow BaseClasses: not obtainable from Microsoft today. The Download Center
  keeps only the legacy June 2010 DirectX SDK (details.aspx?id=6812, DXSDK_Jun10.exe, 571.7 MB), which ships D3DX as
  d3dx9_43.dll import libraries and no BaseClasses. The 9.0b-era pages found (id=31246 "DirectX 9.0b SDK Extras",
  id=10084) return 404. Stopped for both, as instructed.
- libpng 1.0.12 and zlib 1.1.3: the sources are public from the libpng / zlib projects, not Microsoft. Not fetched this
  run (outside the fetch rule); 4 rows.

## Files

- `libidentity_functions.tsv` (beside this file): one row per hooks.csv library row (library, rva, hooks_name,
  subsystem, confidence, status, library_object, lib_function, lib_size, first_diff, category, note), then the
  `identical-unlisted` rows.
- `raw/`: per-library TSVs and run logs (Lua 4.0 and 4.0.1, qhull two equivalent flag sets, CRT libcmt and libc).
- Tools (new, `re/tools/match/`): `coffpe.py` (COFF / archive / PE reader, anchor check), `libmatch.py` (identity,
  vote-based disambiguation, similarity pairing, residual classifier), `period_cc.py` (DDK compiler driver),
  `sweep_flags.py` (flag sweep per library), `summarize.py` (this table). Masking rule and the library-member approach
  follow SimGolf's `re/tools/match.py` and `log/roadmap/S10/7_tools/libscore.py`; SimGolf's files were not copied
  verbatim because they are bound to SimGolf's module and Ghidra-export layout. SimGolf was not edited.

Reproduce (from the worktree; compiler, headers and Lua sources live in the main checkout's gitignored `tools/`):

```
MSYS_NO_PATHCONV=1 py -3.12 re/tools/match/sweep_flags.py lua   --search --flags "/Og /Oi /Ot /Oy /Ob1 /GF /G5" --tsv-dir <dir>
MSYS_NO_PATHCONV=1 py -3.12 re/tools/match/sweep_flags.py qhull --search --flags "/Og /Oi /Ot /Oy /Ob2 /GF /G5" --tsv-dir <dir>
MSYS_NO_PATHCONV=1 py -3.12 re/tools/match/libmatch.py --objs <main>/tools/vc7ddk/lib/wxp/i386/libcmt.lib --subsystem "third-party-library[msvc-crt" --tsv <file>
```

DDK extraction (into `tools/vc7ddk/` only): X86DLIB1-3.CAB -> `lib/wxp/i386/` (libc, libcmt, libcp* ...),
SDKINCS1-3.CAB -> `inc/crt/` and `inc/wxp/`, names mapped through each cab's INF (`X86dLIBn_FILE_k`). Lua sources:
https://www.lua.org/ftp/lua-4.0.tar.gz (sha256 b476abc7...1337) and lua-4.0.1.tar.gz (df746e14...4d2a) in
`tools/_download/lua/`.

## Next

1. A cl 13.00.9466 back end (VS.NET 2002 RTM c2.dll) to test whether the 416 Lua / qhull / CRT residuals close at the
   same flags. [UNCERTAIN] where Microsoft still offers it; not searched this run.
2. Rubric wiring (Mariano's call): C4 by library-object identity for the 406 identical rows (389 unambiguous), ambiguous ones excluded.
3. re-classify the 197 CRT-identical rows tagged boot / d3dx9-shader-compiler / render / util / save / audio / frontend,
   and add rows for the 15 Lua / qhull functions found with no row.
4. libpng 1.0.12 + zlib 1.1.3 from their official projects, if the fetch rule allows it.
