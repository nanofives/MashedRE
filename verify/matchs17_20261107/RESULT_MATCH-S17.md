# RESULT MATCH-S17, 2026-11-07

**Verdict: GREEN (match-only, L-93).** Slice 0x004a2bb8..0x004af32d, 54 functions (`slice_rvas.txt`, `tools/mkslice.py`).
**Identical: 35/41 attempted (35/54 of the slice).** Each of the 35 is byte-identical under `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178) and `relocheck.py` OK (`results.tsv`, `relocheck.tsv`). None is naked asm and
no source uses `__asm` (it appears only in three header comments). All 41 source files compile as C++ (VS2022 /TP, `cxxcheck.txt`).
No body swap into `mashedmod/src`, no tracker edits, no build. Gates: not run (match-only, the merge train runs them).

Branch `orch/MATCH-S17`, cut from orch/integration e498cb11a.

## Finding 1: the whole slice is the statically linked MSVC CRT

Every one of the 54 rows is a libcmt function that hooks.csv files under boot (44), util (7) or render (3).
`libid_slice.tsv` (`re/tools/match/libmatch.py` against the DDK `tools/vc7ddk/lib/wxp/i386/libcmt.lib`, restricted to
the slice by `tools/libmatch_slice.py`) names the CRT function for 47 of them: 12 identical to the DDK library plus 2 identical-ambiguous
33 differ, 7 not compared. Examples: 0x004a2bb8 report_failure, 0x004a31f3 `_cinit`, 0x004a3258
`doexit`, 0x004a4bb7 `WinMainCRTStartup`, 0x004a504f `_output`, 0x004aaff0 `memcpy`.
The Rich header of MASHED.exe lists comp.id 28/9178 (C, 8 objects) and 29/9178 (C++, 49), the same compiler as the
DDK, next to the game's 28/9466 (373). The `differs` rows are therefore source or flag differences between the
retail VS.NET 2002 libcmt and the DDK's own libcmt build, not a compiler difference.

**For C4R-MATCHBATCH-2:** these rows are CRT, which L-67 waives out of the P-DoD count
(`verify/matchlib_20261031/RESULT_MATCH-LIB.md`, "CRT functions at other-subsystem rows" were refiled there; these 54
were not, because libid did not find them identical). Whether a byte-matched CRT row becomes C4 under L-93 or is
refiled as `waived-platform-crt` is that step's call. The port does not take these bodies from the CRT it links.

## Finding 2: CRT C objects match under `/O1`, not the game flags

Raw-converter flag sweep (`flagsweep_raw.txt`, 11 flag sets over 23 compiling g2s outputs): 0x004a5f07 and
0x004a9744 go from 67-75% under `/O2 /GS /Ob1` to 100% under `/O1`, and every other row's best is an `/O1` variant
or a tie. Hand-matched files use `/O1` (22), `/O1 /GS` where the original has a cookie (7), `/O1 /Oy-` where the
original keeps an EBP frame (`mlock.c`: 0x004a77eb, 0x004a787f, 0x004a7800). The 9 raw-converter files at
`/O2 /GS /Ob1` are 4-6-instruction wrappers with the same bytes under either flag set. `__try` in the source gives
the original's `push N; push $T; call __SEH_prolog` (0x004a5984) entries.
cl 13 drops an empty macro argument, so `MGLOBALA(T, n, , addr)` misexpands: CRT section-bound tables are written
`typedef T TAB[]; MGLOBAL(TAB, DAT_x, 0x...)` (a sized array lets `/O1` drop the `pf < end` test; h1 note).

## Tool change: relocheck resolves pointers inside a const (SEH scope tables)

`relocheck.py` compared a referenced const's first 8 bytes unrelocated. An SEH scope table `$T` holds
`{prev, filter, handler}` with DIR32 pointers to labels inside the function, so every `__try` file was BAD
(`ours ffffffff00000000 vs orig ffffffffd52b4a00`). Now, when the const carries DIR32 relocations, the compare runs
to the const's next symbol (max 64 B) with each pointer resolved: a label in the function's section -> va + (label -
start); `FUN_<va8>` / MGLOBAL -> that address; anything else copied from the original and listed unchecked.
Checks: 0x004a2bb8, 0x004a4bb7, 0x004a78b0 BAD -> OK (filter and handler equal va + label offset). No change on the
305 S16 + S19 files (267 OK / 38 BAD with the old and the new tool). Control: 004a2bb8.c scored against 0x004a7800
reports BAD on the const and the callee.

## Results

| | count |
|---|---|
| slice functions | 54 |
| raw converter (g2s) at 100% | 9 (+2 under `/O1`) |
| hand-matched to 100% (helpers h1-h3, `helper_notes_h*.md`, brief `BRIEF.md`) | 24 |
| identical, relocheck OK | **35** |
| attempted, below 100% | 6 |
| not attempted: MASM source | 11 |
| not attempted: SEH `__finally` funclet of another function | 2 |

Identical rows scored by the whole compiled symbol (Ghidra size shorter than the function, `by=bytes` in
`results.tsv`): 0x004a2bb8 (48 B), 0x004a467e (187 B, includes the funclet at 0x004a4728), 0x004a4bb7 (466 B),
0x004a583a (329 B), 0x004a78b0 (68 B). 0x004af31a has no Ghidra size (`collect.py` refuses it, `results.tsv` shows
`-`): its 19-byte compiled symbol is identical to the original with relocation sites masked and ends at 0x004af32d,
the next function; relocheck OK.

Static callees held in-file, also 100% and relocheck OK (not slice rows): 0x004a504f.c holds 0x004a4fc1, 0x004a4ff4,
0x004a5018 (`write_char`, `write_multi_char`, `write_string`); 0x004aefd0.c holds 0x004aed77 (`CPtoLCID`).

Below 100%:

| rva | size | best % | class | residual |
|---|---|---|---|---|
| 0x004a2be9 | 14 | 50.0 | c | `__security_check_cookie` is naked asm in the CRT; C inverts the branch |
| 0x004a2c2f | 23 | 50.0 | c | `_fpmath` ends in inline `__asm fnclex`; no C form emits it |
| 0x004a5df5 | 64 | 58.3 | c | `_ms_p5_test_fdiv` computes in an inline `__asm` block |
| 0x004a57e4 | 86 | 92.6 | b | `__security_init_cookie`: epilogue `pop esi` scheduled before the `jne`; 12 rewrites + permuter (127 compiles) no gain |
| 0x004a7796 | 85 | 94.1 | b | `_mtdeletelocks`: second loop's pointer IV biased by +4; 10 rewrites + permuter no gain |
| 0x004ae635 | 191 | 82.8 | b | `_setlocale_get_all`: original keeps two pointer IVs (prev/cur); permuter no gain |

Not attempted (c): MASM CRT sources 0x004a34b0 strncpy, 0x004a9410 strlen, 0x004aaf90 memset, 0x004aaff0 memcpy,
0x004acd00 memmove, 0x004ac560 strcpy, 0x004ac570 strcat, 0x004a5984 `__SEH_prolog`, 0x004a3384 acos,
0x004a3700 / 0x004a37b0 `__CIsin` / `__CIcos`. Funclets: 0x004a2d18 (`__finally` of printf 0x004a2cbc, called at
0x004a2d05), 0x004a4158 (`__finally` of the SEH function at 0x004a4126, called at 0x004a414a).

## Files

- `re/match/s17/*.c`: 41 sources (line 1 FLAGS, header comment, shared preamble, MGLOBAL globals, `FUN_<va8>` names).
- `results.tsv` (`tools/collect.py`), `relocheck.tsv`, `cxxcheck.txt`, `libid_slice.tsv`, `flagsweep_raw.txt`,
  `slice_rvas.txt`, `helper_notes_h1..h3.md`, `BRIEF.md`, `tools/`.
- Raw data, not committed, `D:/MashedWT/MATCH-S17_data`:
  - `decomp.json` sha256 918bde5062bfd532388f3ff57a40bbc616fdf99f9edf4e69d748d8373f979693 (decomp_pc.py, slot Mashed_pool0)
  - `g2s/results.tsv` sha256 6708a921743ce238a83b2667bf06962a37579d060c5a58278d6ce1ef1f1e47b3
  - `libmatch_slice.tsv` sha256 148760eece0f83c7412cef068d720fdf56ee32d562b346f600d9ec90deee15ee (= `libid_slice.tsv`)

## Next

C4R-MATCHBATCH-2 takes the 35 identical rows (first deciding CRT C4 vs the L-67 waiver, Finding 1). The 4
held static callees can be counted once rows exist for them.
