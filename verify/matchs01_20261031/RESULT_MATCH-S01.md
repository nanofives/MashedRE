# MATCH-S01: C4 by byte match, slice 0x00401000..0x00409970 (2026-10-31)

Branch `orch/MATCH-S01` (worktree `.worktrees/MATCH-S01`, off `orch/MATCH-MERGE` @ 440e64e02). No tracker, ledger
(other than this row), `original/`, master-Ghidra or `.ps1`/`.bat` write. No re-classify: MATCH-RUBRIC is not merged.

## Verdict

**HANDOFF: static legs and body swap done and committed, default gates NOT RUN.** The gate run waited 2 h for
`mashed_machine` (other workers' solo windows and merge-train priority passes held it from 11:58 to 13:58) and hit
the tool's maximum background time without starting. Nothing about the swap has been measured at runtime yet.

- **103 of 128** slice functions are byte-identical to `MASHED.exe.unpatched` (whole obj symbol, relocations
  masked, at least Ghidra's body size): 32 straight from the converter (`re/match/s01/raw/`), 71 by hand
  (`re/match/s01/*.c`). 25 are partial (46.4-99.4%), classes below.
- **18 RVAs now use the matched source as their port body in both targets** (one body per RVA, lint clean). These
  are the re-classify candidates once MATCH-RUBRIC lands (table "Swapped").
- The other 85 byte-identical RVAs have no body in `mashed_re.exe` (the exe does not carry a port of them), so
  condition (b) of the rubric cannot hold yet. They are matched-only. Two exe-body RVAs were not swapped:
  0x00407600 (byte-identical, its port body is a ShadowAB wrapper, see "Not swapped") and 0x004046a0 (96.8%).

## Commands, compiler, flags

| leg | command |
|---|---|
| decomp (pool slot Mashed_pool0, released by the tool) | `py -3.12 re/tools/decomp_pc.py --file D:/MashedWT/MATCH-S01_data/rvas.txt --port --callees --json -o D:/MashedWT/MATCH-S01_data/decomp.json` (main checkout) |
| g2s | `py -3.12 re/tools/match/g2s.py --decomp D:/MashedWT/MATCH-S01_data/decomp.json --out D:/MashedWT/MATCH-S01_data/g2s --flags "/O2 /GS /Ob1"` (41 raw at 100%) |
| match | `py -3.12 re/tools/match/match.py <file.c> --bytes` (verdicts: `verify/matchs01_20261031/score.py`, table: `mkresult.py`) |
| permute (bounded) | `py -3.12 re/tools/match/match_permute.py <file.c> 0x<va> --rounds 6 --walk 40 --jobs 3-4 --time 300-420` |

Compiler: `tools/vc7ddk` cl/c1 13.00.9176, c2 13.00.9178 (XP DDK). Flags in every file: `// FLAGS: /O2 /GS /Ob1`,
language C, x87. In Git Bash, `MSYS_NO_PATHCONV=1` is required: without it `/O2` becomes `C:/Program Files/Git/O2`
and every score is wrong (first g2s pass here scored 0/128 that way).

Per-RVA table (all 128): `verify/matchs01_20261031/results.tsv` (rva, size, exact %, verdict, source, source sha256
prefix of the working copy, swapped_into, exe TU, asi TU). Hand-matching was split over 6 local agents by contiguous
range; their notes: `verify/matchs01_20261031/notes/a1..a6.md`, brief `AGENT_BRIEF.md`.

## Swapped (18 RVAs): matched source = port body, both targets

| rva | port symbol | matched source (git blob) | port TU (exe + asi) |
|---|---|---|---|
| 0x00401340 | CupSpinSpeedAndColor | re/match/s01/00401340.c (1801bf5a41d1) | Match/MatchS01.cpp |
| 0x00401570 | EntryTableScanByKey | re/match/s01/00401570.c (838e45f26d66) | Match/MatchS01.cpp |
| 0x00401ee0 | EntityTableSelectUpdate | re/match/s01/00401570.c (838e45f26d66) | Match/MatchS01.cpp |
| 0x004026d0 | BootQueueFlush | re/match/s01/004026d0.c (4ccf7f7b7bbd) | Match/MatchS01.cpp |
| 0x00402f50 | BootDefaultParamsInit | re/match/s01/00402f50.c (86154861ff22) | Match/MatchS01.cpp |
| 0x00402f80 | CupFloatInit | re/match/s01/00402f50.c (86154861ff22) | Match/MatchS01.cpp |
| 0x00405890 | Pred405890 | re/match/s01/00405890.c (7835c4fd9709) | Match/MatchS01.cpp |
| 0x00407a60 | UtilTableInit8a9620 | re/match/s01/00407a60.c (b3322a7374cc) | Match/MatchS01.cpp |
| 0x00408a50 | PerCarRaceProgressGet | re/match/s01/00407a60.c (b3322a7374cc) | Match/MatchS01.cpp |
| 0x00408ad0 | RaceScoreFloatGetBySlot | re/match/s01/00407a60.c (b3322a7374cc) | Match/MatchS01.cpp |
| 0x00409290 | SnapNodeToSafeStartLines | re/match/s01/00409290.c (58010819e9a3) | Match/MatchS01.cpp |
| 0x00404e00 | IdxPtr404e00 | re/match/s01/00404e00.c (713f4f48dfa1) | Vehicle/CarMaterialTables.cpp |
| 0x00404e20 | IdxPtr404e20 | re/match/s01/00404e00.c (713f4f48dfa1) | Vehicle/CarMaterialTables.cpp |
| 0x00404e50 | SaveLoad | re/match/s01/00404e50.c (5ba70f49b767) | Save/GameSave.cpp |
| 0x00404f50 | SaveWrite | re/match/s01/00404e50.c (5ba70f49b767) | Save/GameSave.cpp |
| 0x00404f80 | SaveFileExists | re/match/s01/00404e50.c (5ba70f49b767) | Save/GameSave.cpp |
| 0x00404e80 | DeserializeFromBuffer | re/match/s01/00404e80.c (bb04df1f4a9c) | Save/GameSaveBuffer.cpp |
| 0x00404ee0 | SerializeToBuffer | re/match/s01/00404e80.c (bb04df1f4a9c) | Save/GameSaveBuffer.cpp |

How the swap works (commits c73f918c3, adab10e4b):
- Each matched file keeps its own `extern` / prototype block inside `#ifndef MASHED_RE_PORT`. `match.py` compiles
  the file as is. The port TU defines `MASHED_RE_PORT`, maps the names, and `#include`s the same file inside
  `extern "C"`. Function bodies are identical text in both compiles.
  - Defined functions `FUN_<rva>` are mapped to the existing export names, so callers and `RH_ScopedInstall` are unchanged.
  - Globals are mapped to the storage the replaced bodies used: original addresses in `Match/MatchS01.cpp`, and the
    per-target bases already in `GameSaveBuffer.cpp` / `CarMaterialTables.cpp` / `GameSave.cpp` (original addresses
    in the .asi, the standalone storage in the exe).
  - Callees are mapped to exactly what the replaced bodies called: absolute original addresses, or a port function
    where the old body called one by name (`GetDat0066d6e0`, `GetTableEntry0066d658`, `SaveStatusClear`, `gFileRead`...).
- The old C++ bodies and installs were removed (9 TUs). Body-map diff before/after (`scripts/rva_body_scan.py`,
  both targets + installs): exactly the 11 `MatchS01.cpp` RVAs moved; nothing else changed.
- `Match/MatchS01.cpp` is in both rsp lists and in `x87_tus.txt` (the matched code is x87 code. None of the 9
  source TUs was x87; the only float arithmetic swapped is 0x00401340's one multiply).
- `scripts/rva_body_scan.py` now follows `#include "*.c"` (one level, appended after the TU's lines), so the
  one-body lint and the `exe_file` backfill see these bodies. Build lint: `CROSS-TARGET=16 allowlisted=16 NEW=0`.
- Source fixes made while matching that also fix the port:
  - 0x00404ee0: the original writes the counter at 0x00828254 after the 0x520-byte block copy to 0x00827d98, and
    0x00828254 is inside that block (+0x4bc). The source keeps that order (also byte-identical), so a compiler
    that sees the overlap keeps the original's result.
  - 0x00404e00 / 0x00404e20: the replaced exe bodies bounds-checked the table index. The original does not, so the
    matched bodies do not either.
  - 0x00402f50: zeroes all five dwords (inlined memset) before the two float stores. The final state is the same as before.

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`, Build OK, shared .asi not deployed.
Gated exe (frozen copy) sha256 `274d4930611722539a710bf79827373fa92781268f99dc339ef37098bca4923c`, asi
`f56fc6ec652fc2e3e8e8f0546d32515ffc8beb2a3b1823f3b93fb1663f265ce5` (built at adab10e4b; later commits are evidence only).

## Gates

Not run (see verdict). Refs (`re/orchestrator/merge_train.json`): G-CTRL `8284be46/ba407a16/31fe043e`,
B `1d5bf4f9` x3. A gate move from a body swap is a port divergence: file it as DECISION, do not fix it in this row.

## HANDOFF (next session, one step)

From the worktree root, sandbox disabled, `run_in_background` with a 7200000 ms timeout, and only when
`mashed_lock.py status` is free (no SOLO window):

```
export A4_EXE=D:/MashedWT/MATCH-S01_data/exe/mashed_re.exe A4_ROOT=D:/MashedWT/MATCH-S01_root A4_OUT=D:/MashedWT/MATCH-S01_data/arms
test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label MATCH-S01 --wait 7200 -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26'
```

The exe in `D:/MashedWT/MATCH-S01_data/exe/` is the frozen build of adab10e4b (sha256 above). The private root
`D:/MashedWT/MATCH-S01_root` is ready. Hash `H_DEF.step.csv`, `H_DEF.gates.csv`, `H_DEF.ptrace.log` and
`B_DEF_K*.step.csv` (SHA-256, first 8 hex) against the refs. Equal means GREEN: fill this section, `roadmap.ps1 set
MATCH-S01 review`. Different means DECISION. The bisect unit is the three port TUs: revert `Match/MatchS01.cpp`
(11 RVAs), the GameSave pair, or the CarMaterialTables pair. When the lead's queue is free, the merge train's own
gate pass (it rebuilds and runs the same arms) is an equivalent way to get these hashes.

On TaskStop of a queued run: the `py` waiter children survive the killed bash. Kill them by PID (label MATCH-S01),
or they stay queued on the lock.

## Not swapped

- **0x00407600** `PlayerPositionPtr` (byte-identical, `re/match/s01/00407600.c`): its port body is
  `ShadowAB::Run(ab, PlayerPositionPtr_impl, player)` (Render/SlotObjectAccessors.cpp). The RVA anchor sits on the
  wrapper, so swapping the impl would not make the matched source the RVA's body, and replacing the wrapper drops
  the ShadowAB harness. This needs a call by the MATCH-RUBRIC / lead: (1) keep ShadowAB and leave it matched-only,
  or (2) retire the wrapper for this RVA and swap.
- **0x004046a0** `Rule10Seed` 96.8% (6 bytes): one hoisted store and one register choice (EDX vs EDI), class a?/b.
  The header lists everything tried.
- **85 byte-identical RVAs with no exe body**: matched-only. They become C4 candidates only after a port body exists.

## Partial (25): best %, class

0x00401690 60.2 (a: loop unrolled x4 by the original, x8 or none by 9176, never x4) · 0x004019d0 76.0 · 0x00401da0 93.5 ·
0x00401fe0 46.4 · 0x004022a0 50.7 · 0x00402b70 99.4 · 0x004036a0 77.4 · 0x004039f0 97.1 · 0x004046a0 96.8 ·
0x00404fa0 94.6 · 0x00405540 88.1 · 0x00405730 96.2 · 0x00405920 99.2 · 0x00405ab0 96.9 · 0x00406160 88.7 ·
0x004064c0 99.4 · 0x004069d0 84.0 · 0x00406ae0 91.8 · 0x00406ce0 71.6 · 0x00407800 99.3 · 0x00407be0 67.6 ·
0x00407e20 76.6 · 0x00408590 94.9 (a?) · 0x00408610 95.8 · 0x00408b00 90.2. All others class (b). Each file's
header gives the residual and what was tried.

## Caveats for the re-classify step

- **Stand-in callers.** Like the pilot's 0x00419090 / 0x00499170, four files hold a small stand-in caller so the
  compiler picks the register convention: `00405920.c` (stand-in for 0x00406ae0's caller; affects byte-identical
  0x00405f60, 0x00406070, 0x00406410), `00407550.c` (0x00407550), and the partial `004064c0.c` and `00406ce0.c`.
  None of the 18 swapped RVAs uses a stand-in.
- **Compiled as C++ in the port.** `build_objs.ps1` refuses non-`.cpp` entries, so the matched `.c` files are
  compiled by MSVC 19 as C++ inside `extern "C"` (same text). Needs Mariano: allow `.c` entries in
  `mashedmod/build_objs.ps1` (and its `.cpp.json` dependency name) if the rubric wants them compiled as C.
- **Write-set leg.** CONFIDENCE.md's C4 write-set leg (L-38/L-40/L-54) is written for the Frida route. Whether a
  byte match covers it is MATCH-RUBRIC's call. Every swapped RVA except the pure readers (0x00405890, 0x00408a50,
  0x00408ad0, 0x00404e00, 0x00404e20, 0x00404f80) stores outside its frame.
- 0x00404830 matched (`00404830.c`). The exe's copy is generated data (`Vehicle/CarMaterialTables_dirt.inc`), not a body.

## For the re-classify step (after MATCH-RUBRIC merges)

For each of the 18 swapped RVAs: evidence = this file + `results.tsv` row (`BYTES-OK`, match.py `--bytes`, DDK
13.00.9176/9178, `/O2 /GS /Ob1`, C, x87) + the source blob above + port TU + the gate line. `hooks.csv` `file` /
`exe_file` change to the port TU in the table. The 85 matched-only RVAs: record the match (source path) without C4.

## Raw data (not committed), D:/MashedWT/MATCH-S01_data/

| file | sha256 (first 16) |
|---|---|
| decomp.json (Ghidra headless, 128 functions) | b79b68715367785c |
| g2s/results.tsv (raw converter scores) | 7614536e62cc3b20 |
| build3.log (gated build) | 26f7a3e150785dfd |
| exe/mashed_re.exe, exe/mashed_re_dev.asi (frozen gated build) | 274d4930611722539a710bf79827373fa92781268f99dc339ef37098bca4923c / f56fc6ec652fc2e3e8e8f0546d32515ffc8beb2a3b1823f3b93fb1663f265ce5 |

## Tool and process notes

- Static callees with register arguments (pilot finding 1) held across the slice: 0x00405920..0x00406cdb is one TU
  of EAX/ESI/ECX/EDI-arg statics, 0x00403fa0 / 0x00404320 and 0x004036a0 / 0x00403910 are file pairs, 0x00401690 is
  called with ESI.
- Constants in the 0x005cc3xx-0x005cc9xx and 0x005d757c pools are compiler literals: as externs, 9176 orders the
  x87 operands differently (a3, a4, a6). The exception is 57.29578f at 0x005cc98c, which matches only as an extern.
- Ghidra arity was wrong in several callees (0x004671a0 1 arg, 0x004c1bb0/0x004c1be0 3, 0x004e6680 1,
  0x00426060 0, 0x004e4320/0x004e4350 2), and Ghidra's body stopped short of the epilogue in 4 functions (`--bytes`
  verdict).
