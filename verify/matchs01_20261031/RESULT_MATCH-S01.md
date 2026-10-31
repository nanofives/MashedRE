# MATCH-S01: C4 by byte match, slice 0x00401000..0x00409970 (2026-10-31)

Branch `orch/MATCH-S01` (worktree `.worktrees/MATCH-S01`, off `orch/MATCH-MERGE` @ 440e64e02, `orch/integration` @
5cf5cdd63 merged in, MATCH-RUBRIC + MATCH-S02 included). No tracker, `original/`, master-Ghidra or `.ps1`/`.bat` write. No re-classify (a separate row does it).

## Verdict

**GREEN for the worker legs: build OK, one-body lint clean (NEW=0), relocheck OK on all 18 swapped RVAs (0
unchecked relocations). Default gates: train-run only.** The worker's own gate run waited 2 h for `mashed_machine`
(11:58-13:58, other workers' solo windows and merge-train priority passes) and never started. After the merge the
machine was again in a merge-train solo window (14:46), so per the lead the G-CTRL/B gates are left to the merge
train's `-Full` pass on the merged tree. Merged-tree exe sha8 `04ea8761` (below).

- **103 of 128** slice functions are byte-identical to `MASHED.exe.unpatched` (whole obj symbol, relocations
  masked, at least Ghidra's body size): 32 straight from the converter (`re/match/s01/raw/`), 71 by hand
  (`re/match/s01/*.c`). 25 are partial (46.4-99.4%), classes below.
- **18 RVAs now use the matched source as their port body in both targets** (one body per RVA, lint clean). These
  are the re-classify candidates (rubric route `match`, conditions 1 and 2 met, 3 = the train gates; table "Swapped").
- The other 85 byte-identical RVAs have no body in `mashed_re.exe` (the exe does not carry a port of them), so
  rubric condition 2 cannot hold yet. They are matched-only. Two exe-body RVAs were not swapped:
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

Machine form: `swaps.tsv` (MATCH-S02 format) and `relocheck.tsv`. exe callers = calls by name in exe TUs other than the
port TU (text scan; calls that reach the RVA through `Compat/StandaloneRvaThunks` are not counted). `0` = no by-name caller.

| rva | port symbol | best % (by) | relocheck (checked/unchecked) | matched source (blob) | port TU (exe + asi) | exe callers |
|---|---|---|---|---|---|---|
| 0x00401340 | CupSpinSpeedAndColor | 100.0 (insn) | OK (7/0) | re/match/s01/00401340.c (c90b761df14b) | Match/MatchS01.cpp | 0 |
| 0x00401570 | EntryTableScanByKey | 100.0 (insn) | OK (4/0) | re/match/s01/00401570.c (fe3c96dcac42) | Match/MatchS01.cpp | 0 |
| 0x00401ee0 | EntityTableSelectUpdate | 100.0 (insn) | OK (4/0) | re/match/s01/00401570.c (fe3c96dcac42) | Match/MatchS01.cpp | 0 |
| 0x004026d0 | BootQueueFlush | 100.0 (insn) | OK (8/0) | re/match/s01/004026d0.c (4ccf7f7b7bbd) | Match/MatchS01.cpp | 0 |
| 0x00402f50 | BootDefaultParamsInit | 100.0 (insn) | OK (7/0) | re/match/s01/00402f50.c (1095d4c11682) | Match/MatchS01.cpp | exe_main.cpp |
| 0x00402f80 | CupFloatInit | 100.0 (insn) | OK (4/0) | re/match/s01/00402f50.c (1095d4c11682) | Match/MatchS01.cpp | 0 |
| 0x00404e00 | IdxPtr404e00 | 100.0 (insn) | OK (2/0) | re/match/s01/00404e00.c (c8074b0b893d) | Vehicle/CarMaterialTables.cpp | LibRw/RwCarFx.cpp |
| 0x00404e20 | IdxPtr404e20 | 100.0 (insn) | OK (2/0) | re/match/s01/00404e00.c (c8074b0b893d) | Vehicle/CarMaterialTables.cpp | LibRw/RwCarFx.cpp |
| 0x00404e50 | SaveLoad | 100.0 (insn) | OK (4/0) | re/match/s01/00404e50.c (e3c9a06649ab) | Save/GameSave.cpp | Race/GameFlow.cpp |
| 0x00404e80 | DeserializeFromBuffer | 100.0 (insn) | OK (9/0) | re/match/s01/00404e80.c (cc39d420a214) | Save/GameSaveBuffer.cpp | 0 |
| 0x00404ee0 | SerializeToBuffer | 89.3 (bytes) | OK (10/0) | re/match/s01/00404e80.c (cc39d420a214) | Save/GameSaveBuffer.cpp | 0 |
| 0x00404f50 | SaveWrite | 100.0 (insn) | OK (4/0) | re/match/s01/00404e50.c (e3c9a06649ab) | Save/GameSave.cpp | Race/GameFlow.cpp |
| 0x00404f80 | SaveFileExists | 100.0 (insn) | OK (2/0) | re/match/s01/00404e50.c (e3c9a06649ab) | Save/GameSave.cpp | Race/GameFlow.cpp |
| 0x00405890 | Pred405890 | 100.0 (insn) | OK (2/0) | re/match/s01/00405890.c (a149fc2f1d70) | Match/MatchS01.cpp | 0 |
| 0x00407a60 | UtilTableInit8a9620 | 100.0 (insn) | OK (2/0) | re/match/s01/00407a60.c (d7efc90aca08) | Match/MatchS01.cpp | 0 |
| 0x00408a50 | PerCarRaceProgressGet | 100.0 (insn) | OK (1/0) | re/match/s01/00407a60.c (d7efc90aca08) | Match/MatchS01.cpp | 0 |
| 0x00408ad0 | RaceScoreFloatGetBySlot | 100.0 (insn) | OK (1/0) | re/match/s01/00407a60.c (d7efc90aca08) | Match/MatchS01.cpp | D3d9Render/TrackRenderer.cpp |
| 0x00409290 | SnapNodeToSafeStartLines | 100.0 (insn) | OK (2/0) | re/match/s01/00409290.c (58010819e9a3) | Match/MatchS01.cpp | Race/RoundGrid.cpp |

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
- One-body lint: the merged `scripts/rva_body_scan.py` binds each `// MATCH:` line of an included `re/match/` source
  to the including TU (MATCH-S02's third pass). This branch's earlier include-appending pass was dropped in the merge
  so only one mechanism remains. Every swapped RVA binds to one TU in each target (table above). Lint on the merged
  tree: `CROSS-TARGET=16 allowlisted=16 NEW=0`.
- relocheck (rubric condition 1): the 10 swapped sources declare their globals with `MGLOBAL(T, name, 0x<addr>)` /
  `MGLOBALA(...)` inside the `#ifndef MASHED_RE_PORT` block (expands to the same `extern` as before, so the match
  compile is unchanged: all 18 still byte-identical). `re/tools/match/relocheck.py` reports OK for all 18 with 0
  unchecked relocations (`relocheck.tsv`). The port compile skips that block and keeps its own name map.
- Source fixes made while matching that also fix the port:
  - 0x00404ee0: the original writes the counter at 0x00828254 after the 0x520-byte block copy to 0x00827d98, and
    0x00828254 is inside that block (+0x4bc). The source keeps that order (also byte-identical), so a compiler
    that sees the overlap keeps the original's result.
  - 0x00404e00 / 0x00404e20: the replaced exe bodies bounds-checked the table index. The original does not, so the
    matched bodies do not either.
  - 0x00402f50: zeroes all five dwords (inlined memset) before the two float stores. The final state is the same as before.

Merge: `git -c merge.autoStash=false merge orch/integration` (5cf5cdd63). Conflicts were only in the two rsp lists,
both sides appending lines (MatchS01.cpp and the 19 MatchS02 TUs). Both were kept. `SmallLeaves_t1.cpp`,
`UtilLeaves_ac.cpp` and `rva_body_scan.py` merged automatically.

Build (merged tree): `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmoduild.bat"`, Build OK, rva-lint NEW=0, asm-lint OK,
shared .asi not deployed. Frozen copy `D:/MashedWT/MATCH-S01_data/exe2/`: exe sha256
`04ea876117feb9c54eeecd7e375ed024082e1d3c35efb37a9f9d4e6ad9963fcd` (sha8 `04ea8761`), asi
`99baa2b759cf04008fc661974e52a1c3be6b8505c4d0d175c5458c5b8aa8c682`. The pre-merge build (`exe/`, sha8 `274d4930`)
is superseded.

## Gates

**Train-run only.** No worker gate run happened: the first attempt waited 2 h for the lock without starting, and
after the merge the machine was in a merge-train solo window (since 14:46). The merge train's `-Full` pass runs
H_DEF + B_DEF_K0/K1/K26 on the merged tree. Refs (`re/orchestrator/merge_train.json`): G-CTRL
`8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3. A gate move from a body swap is a port divergence (L-n decision).
The bisect unit is the port TU: `Match/MatchS01.cpp` (11 RVAs), `Save/GameSave.cpp` (3), `Save/GameSaveBuffer.cpp`
(2), `Vehicle/CarMaterialTables.cpp` (2). The private root `D:/MashedWT/MATCH-S01_root` is ready if a worker run
is wanted later (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` under
`mashed_lock.py run`, `A4_EXE` = the frozen exe).

Process note: a TaskStop of a queued run leaves its `py` waiter children on the lock queue. Kill them by PID
(label MATCH-S01).

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

## For the re-classify step (separate row)

For each of the 18 swapped RVAs: evidence = this file + its `swaps.tsv` row (match.py `--bytes` BYTES-OK, DDK
13.00.9176/9178, `/O2 /GS /Ob1`, C, x87; relocheck OK, 0 unchecked; source and port-TU blobs) + the merge train's
gate line. Rows with `exe callers = 0` in the table get `exe-callers=0` in `notes` (rubric "Callerless bodies"). `hooks.csv` `file` /
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
