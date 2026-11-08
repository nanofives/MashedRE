# C4R-MATCHPY-REL32 RESULT (2026-11-07)

Verdict: GREEN. Patch applied to `re/tools/match/match.py` (`byte_compare`). Full regression over every `re/match` source:
3046 MATCH rows in 2693 files, 3022 scored (24 raise "no Ghidra size" in `score_function`, same before and after, not related to this fix).
**0 rows go down, 19 rows change, all 19 explained by a self-recursive REL32 site** (`explain.tsv`: reduction in bad bytes == number of self REL32 sites, 19/19).

## 1. Why the fix is correct (COFF semantics)
- COFF i386 relocation types: DIR32 = 0x06 (field = absolute address, S + A), REL32 = 0x14 (field = S + A - (P + 4), displacement from the end of the field).
- `function_from_obj` builds `local` from every relocation whose target symbol sits in the function's own COMDAT section, with no type. `byte_compare` then checks each as DIR32: `theirs = dword - addr`.
  Right for jump-table entries (DIR32 to a label). For a REL32 `call` to the function's own symbol the original holds a displacement
  (`e8 dfffffff` = -0x21 = target +0x0 from site+4), so `theirs` is a meaningless number and the site is always reported bad (first byte of the rel32).
- With /Gy each function has its own COMDAT, so a REL32 landing in `local` is only ever a call to the function itself.
- Fix: for sites whose relocation type is 0x14 resolve both sides as a target offset from the function start:
  `ours = sym_offset + signed(stored addend)` (MSVC stores 0), `theirs = site + 4 + signed(rel32 in original)`. DIR32 sites keep the old rule.
  A self call must give 0 == 0 on both sides, so a call to any other offset would still be reported bad.

## 2. Regression (`regress.tsv`, tools `regress.py`, `match_old.py` = HEAD copy)
Each source compiled once with its own `// FLAGS:` / lang / toolchain, old and new `byte_compare` run on the same obj.
Changed rows (`explain.tsv`):
- 100% to 100% bytes, 1 bad byte to 0 (14 rows incl. the 11 named by NEARMISS): 0x004c0ad0 (listed in 3 sources: 004c0ad0.c, 004c0c20.c, 004c0f00.c), 0x004d4bf0, 0x004d8280, 0x004d83d0, 0x004da430, 0x004da480, 0x004dab70, 0x00554010, 0x00554150, 0x0055c4f0, 0x00553e80 (in 00554200.c).
- Non-matching rows that each lose exactly their one false byte and stay non-matching: 0x004d8300 (18 to 17), 0x004d9ba0 (46 to 45), 0x004d9d00 (58 to 57), 0x004da270 (92 to 91), 0x004dfbc0 (1246 to 1245). 0x00516de0 has 3 self calls: 23 to 20 (97.57% to 97.89%).
No PASS row changed that was not a self-REL32 case.
## 3. relocheck on every flipped row (`relocheck.tsv`, branch copy of relocheck.py)
All 11 target RVAs: OK (also 0x004c0c20, 0x004c0f00 in the same files). The only BAD line is 0x00554200 (cbd30/554150 operands), a pre-existing BAD row in 00554200.c, untouched by this step: 0x00553e80 in that file is OK.

## 4. 0x0040ac80 extent (pc_funcs.csv not edited)
- pc_funcs (current and orch/MATCH-PCFUNCS-REGEN, identical): `0x0040ac80,0x0040acb5,228`. Ghidra's body is 0x0040ac80..0x0040acb5 (0x36 = 54 B of code) plus the four handlers it inlined.
- Dispatcher tail jumps (original bytes): 0x0040ac92 jmp 0x0040ac40, 0x0040ac97 jmp 0x0040ac00, 0x0040ac9c jmp 0x0040abe0, 0x0040aca1 jmp 0x0040abc0; 0x0040aca6 calls 0x0042c1a0 inline.
- Handler sizes (HIDDEN markers in re/match/s02/0040ac80.c): 0x0040abc0 = 31, 0x0040abe0 = 31, 0x0040ac00 = 62, 0x0040ac40 = 50. 31+31+62+50+54 = **228**, so the 228 B is exactly body + the four handlers.
- None of the four has a pc_funcs.csv row (current or regenerated) or a hooks.csv row. Each is byte-identical as a static in the source (per its header comment).
- Evidence now: `match.py --bytes`: 80 B whole symbol (54 B code + 2 pad + 6-entry jump table), 0 differing bytes (jump table checked as DIR32 labels); relocheck OK (checked 7, unchecked 0).
- **Recommendation:** score 0x0040ac80 at its own symbol extent, 80 B (code to 0x36 plus its jump table), which is 100% / 0 bad bytes. The 228 B pc_funcs size is not the extent of this function. Treat 0x0040abc0/abe0/ac00/ac40 as four separate unregistered functions, matched byte-identical as statics, and add them as their own rows (needs a Ghidra size, so a pc_funcs/function-creation step) if each is to carry C4. Do not claim the 228 B as one function. In the `collect.py` rule `bytes_n >= ghidra_size` this row fails (80 < 228), so the lead should accept it by this extent explicitly.

Files: `regress.py`, `match_old.py`, `regress.tsv` (3046 rows), `explain.py`, `explain.tsv`, `relocheck.tsv`, this RESULT. Tool change: `re/tools/match/match.py` (+9/-2).
