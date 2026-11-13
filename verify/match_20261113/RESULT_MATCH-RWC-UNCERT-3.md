# RESULT MATCH-RWC-UNCERT-3 (2026-11-13, class P2, L-127)

Row kind: text/tracker. No build, no gates. Branch `orch/MATCH-RWC-UNCERT-3` from `orch/integration` @ e47e10795.
Follows `verify/rwc_20261113/RESULT_RWC-REFILE-514330.md`, which left 3 executed d3dx-labelled rows [UNCERTAIN].

Method: static only. Disassembly from `original/MASHED.exe.unpatched` via `re/tools/match/odis.py` and capstone over
`match.original_bytes`. Cross-references from a whole-image scan for E8/E9 rel32 targets and raw 4-byte absolute values in
every section (scratch script, not committed: it reads the PE and prints matches, no tracker access). Execution from
`re/coverage/reach.tsv`. Ghidra MCP not used (claude2 session, and the disassembly was enough).

## Verdicts

| RVA | old label | verdict |
|---|---|---|
| 0x005b8200 | third-party-library[d3dx9-fx] | resolved: `audio`, refiled |
| 0x005b8680 | third-party-library[d3dx9-fx] | resolved: `audio`, refiled |
| 0x0051085a | third-party-library[d3dx9-shader-compiler] | label kept; one shared 1-byte body referenced by D3DX9 code and by two CRT pointers; executed only through the CRT reference path recorded in reach.tsv. Source-level identity still [UNCERTAIN] |

## 0x005b8200 (877 B, reach: runs_race 3/3)

- Callers: one, `call 0x5b8200` at 0x005b1b32 inside Ghidra function 0x005b15e0 (size 2417, no hooks.csv row). 0x005b15e0
  is pushed as an immediate 11 times, first at 0x005b14d4 inside 0x005b1260 (`audio`, C2).
- Direct callees (all `audio` in hooks.csv): 0x005ae010 AudioSubStructLinkDevice (C4, calls at 0x005b8216, 0x005b829c,
  0x005b8361, 0x005b84f9), 0x005adfe0 AudioSubStructLinkBuffer (C4, 0x005b822a, 0x005b82cf, 0x005b83cf, 0x005b852a),
  0x005adf30 AudioFmtKeyCompare (C4, 0x005b8493, 0x005b84ae), 0x005aca80 AudioFmtSizeCalc (C4, 0x005b845f),
  0x005acaa0 (C2, 0x005b844e).
- Indirect: `call [eax+0xf4]` with eax = `[0x007d3ff8]` (RwGlobals pointer, memory `rwglobals-is-dat-007d3ff8`) at
  0x005b8235, 0x005b82db, 0x005b83da, 0x005b8535.
- Constants: `push 0x5e6454` at 0x005b8487 and `push 0x5e64b4` at 0x005b84a2, each followed by the AudioFmtKeyCompare
  call. Bytes at 0x005e6454: `2b a2 2f 63 dd 11 8f 45 aa 27 a5 c3 46 e9 79 0e`. Bytes at 0x005e64b4:
  `93 65 38 ef 11 b6 2d 43 95 7f a7 1a de 44 22 7a`. Neither is the "AVI " 4CC the plate claimed. The same two addresses
  are compared by audio 0x005ac650 (`re/analysis/bucket_005a6f30/0x005ac650.md`) and the neighbouring 0x005e6424 by audio
  0x005bc920 (`re/analysis/bucket_005bba60/0x005bc920.md`).
- No reference to the D3DX9 address band (0x004fcb51..0x005112ca) or to a D3DX9 import.
- hooks.csv neighbours 0x005b7010..0x005b81d0 and 0x005b8570..0x005b9460 are all `audio`.

## 0x005b8680 (604 B, reach: runs_menu 3/3)

- Callers: `call 0x5b8680` at 0x005b85d4 (function 0x005b85a0) and 0x005b861a (function 0x005b85e0). Neither start is in
  hooks.csv or in `re/console/pc_funcs.csv`. 0x005b85a0 is the dword at .data 0x00635174 and 0x005b85e0 the dword at
  0x00635198, inside a table 0x00635170..0x006351bc. Both callers push 0x007dde70 (the same global 0x005b8570 `audio` C4
  passes to 0x005aa560) and call 0x005aa060 (`audio` C4), 0x005a9ab0 (`audio`); 0x005b85e0 also calls 0x005aa200,
  0x005a9d00, 0x005bbbb0 (C4), 0x005a9de0 (C4), all `audio`.
- Direct callees: 0x005a9e10 AudioSubStructTwoCallInit (`audio` C4, with `push 0x5e6e40` at 0x005b8687) and
  0x005aee20 AudioBitScanForward (`audio` C4, 15 calls, argument `push 0x28`).
- Stores: [esi+0x28] = 0x005b8910 (0x005b88b0), [esi+0x34] = 0x005b8be0 (`audio` C4, 0x005b88bc), [esi+0x30] = 0x005b8c40
  (`audio` C2, 0x005b88c6), [esi+0x3c] = 0x005b92d0 (0x005b88cd).
- No reference to the D3DX9 band or a D3DX9 import.

## 0x0051085a (1 B `c3 ret`, reach: runs_menu 3/3, runs_race 0/3)

References, complete for E8/E9 rel32 and absolute dwords:
- Code: `jmp` from 0x004fccb7 (in 0x004fcc67), `call` from 0x004fe802, 0x004fe950, 0x004fe984 (in 0x004fe2fe),
  0x004feb74, 0x004febef (in 0x004fe9cf). All three containers are `d3dx9-shader-compiler`, reach 0/3 menu and race.
- .rdata: 0x005de534, 0x005de544, 0x005de554, 0x005de564, 0x005de574, 0x005de584 (one slot per 0x10 record, next to
  0x00510564 / 0x00510d43 / 0x0051061f and others in the D3DX9 band; the records end at 0x005de588 where the D3DX9 HLSL
  message "integer constant register '%s' must be defined as a variable of type int3 or int4 only" begins), and
  0x005de688, 0x005de698 (records with 0x0051054e / 0x0051167d / 0x005117d8; record base 0x005de67c is read by
  0x005115ce, 0x005115fe, 0x00511642).
- .data: 0x00616048 and 0x0061604c, both initialised to 0x0051085a. 0x0061604c is read and called at 0x005c2d9b/0x005c2da4
  (`mov eax,[0x61604c]; test; call eax`) in 0x005c2d9b (`msvc-crt-tail`, C4 waived-platform-crt, reach 0/3).
  0x00616048 is read and called at 0x005c2e30/0x005c2e39 in 0x005c2dd9 (`msvc-crt-tail`, C4, reach **menu 3/3**).
- libmatch (`verify/c4rdxlib_20261107/d3dx_sum2003.tsv`): identical-ambiguous, same bytes as many d3dx9.lib empty bodies
  (`??1CQuadric@@QAE@XZ`, `??1CArgument@D3DXShader@@QAE@XZ`, `?Print@CNode@D3DXShader@@UAEXXZ`, ...).

Reading: one address serves both D3DX9 (8 code references, 8 table slots) and the CRT (2 initialised pointers). Of all
referencing functions, the only one reach.tsv records as executed is CRT 0x005c2dd9, which calls through 0x00616048.
The 5 caller list in the old plate (`re/analysis/cluster_0051_first_pass/0051085a.md`) names 0x005c2d9b / 0x005c2dd9 as
callers: they reach it through those pointers, not by direct call.

Label: `d3dx9-shader-compiler` is kept. The D3DX9 references are real, and refiling to `msvc-crt-tail` would drop them.
No tracker edit for this row.
[UNCERTAIN] Which object file's definition the linker kept at 0x0051085a. Missing evidence: the link map, or a libmatch of
the VC7 CRT object that initialises 0x00616048 / 0x0061604c against `libcmt.lib` (the CRT libmatch used for
`waived-platform-crt` does not cover data initialisers). Also not shown: that the menu execution of 0x0051085a comes
from 0x005c2e39 and not from a D3DX9 vtable slot; a one-run caller trace (return address at entry) would show it. None of
this changes the label decision.

## Proposed changes (TSV)

```
rva	current	proposed	evidence
0x005b8200	third-party-library[d3dx9-fx]	audio	caller 0x005b15e0 (audio band, pushed by audio 0x005b1260); 5 audio callees incl. 4 C4; GUID-like keys 0x005e6454/0x005e64b4 compared by AudioFmtKeyCompare 0x005adf30
0x005b8680	third-party-library[d3dx9-fx]	audio	callers 0x005b85a0/0x005b85e0 in .data table 0x00635170 calling only audio fns; callees 0x005a9e10/0x005aee20 audio C4; stores 0x005b8be0 audio C4
0x0051085a	third-party-library[d3dx9-shader-compiler]	no change	shared 1-byte RET: 8 D3DX9 code refs + 8 D3DX9 table slots + CRT pointers 0x00616048/0x0061604c
```

## Edits (applied, class P2)

- `hooks.csv`: subsystem cell of 0x005b8200 and 0x005b8680 `third-party-library[d3dx9-fx]` -> `audio`, identity prepended
  to notes. Confidence (C2), status, file unchanged. Same form as 8a5aa66d8 (re-classify edit leg; the skill has no
  dedicated single-row refile helper for a game subsystem, `libid_reclass.py refile-lib` covers Lua / qhull only).
- `re/analysis/CHANGELOG.md`: 2 lines below `<!-- ENTRIES -->`.
- Plates `re/analysis/bucket_00565cd0/0x005b8200.md` and `0x005b8680.md`: their unfiled [UNCERTAIN] markers marked
  RESOLVED with a pointer here, old text kept as superseded.
- P2 under L-127: a refile on identity evidence needs no decision row.
- Wider note, not acted on: the d3dx9-fx "REGION C" in the old notes is 0x005b8200..0x005b8680. With these two refiled,
  no `d3dx9-fx` row remains in that span. Executed d3dx9-labelled rows: 3 -> 1 (0x0051085a, whose execution comes through
  a CRT reference).
- Related open item: U-0355 (meaning of 0x005e6424 at 0x005ba3a6) can cite `re/analysis/bucket_005bba60/0x005bc920.md`,
  which reads 0x005e6424 as the PCM subtype key. Not touched here.

## Checks

- `py -3.12 scripts/lint_rva_bodies.py`: OK, NEW=0 (237 known allowlisted).
- hooks.csv re-parsed with `csv.reader`: 6443 lines, widths {11: 6411, 9: 32}, identical to `orch/integration`. The two
  edited rows read back `audio`, 0x0051085a reads `third-party-library[d3dx9-shader-compiler]`. `git diff --stat`:
  hooks.csv 2 rows changed (CRLF kept). A first pass put commas in the notes and gave the two rows 12 fields. That was
  fixed before the commit and re-checked.
- Subsystem cells equal to or containing `d3dx9-fx` after the edit: 0 in the whole file. The two `grep d3dx9-fx` hits are
  the refile notes of 0x005b8200 / 0x005b8680 themselves.

## DECISIONS.md

Not written: `log/orch/DECISIONS.md` is the live file in the main checkout, and the row instruction says not to touch
the ledger. The lead may append `auto, class P2, MATCH-RWC-UNCERT-3, verify/match_20261113/RESULT_MATCH-RWC-UNCERT-3.md`.
