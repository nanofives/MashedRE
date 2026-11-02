# RESULT MATCH-S05, 2026-11-01

**Verdict: GREEN.** Slice 0x0041f710..0x00426060 (129 game functions). 113/129 sources are byte-identical
under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87), and every one of
the 113 has `relocheck.py` OK. 59 of them are now the port's only body (54 wrapper TUs `Match/MatchS05_<va8>.cpp`):
30 in `mashed_re.exe`, 59 in the .asi. Gates on the merged build: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9` = merge_train.json refs (unchanged).
**C4 candidates after the merge: 1 row (0x0041f710).** The other 29 exe swaps have no exe caller (L-68).

Resumed after the 2026-11-01 14:23 reboot: the sources and swaps were committed at 6e575c787 and the build of
that tree had passed. The first gate run was cut by the reboot before any arm ran. This pass committed the two
uncommitted files (2ccfb0414), merged `orch/integration` ca02bde58 (clean, 7ab5f722b), rebuilt and ran the gates.

## Match (static leg)

| | count |
|---|---|
| slice functions | 129 (`slice_rvas.txt`) |
| byte-identical, relocheck OK | 113 (`results.tsv`, `relocheck.tsv`) |
| below 100% | 16: 0x0041f770 91.5, 0x0041faf0 90.8, 0x0041fcc0 98.9, 0x00420050 99.3, 0x00420290 86.7, 0x00420420 79.8, 0x00420870 72.1, 0x004212b0 69.5, 0x00421d20 94.5, 0x00422570 98.5, 0x004228f0 79.2, 0x00422ba0 92.2, 0x00424b80 88.7, 0x00424eb0 98.6, 0x004252c0 87.7, 0x00425ca0 86.5 |

Sources: `re/match/s05/<va8>.c`, blob ids in `swaps.tsv` / `results.tsv`.

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, source, match %, the removed old body, source-level exe callers.

| targets | rows |
|---|---|
| exe + asi | 29 |
| exe only | 1 (0x0041f710, old body was the verbatim naked `CR_41f710` from `Race/CarRegistrant_naked.inc`) |
| asi only | 29 (3 of them NEW: 0x004215e0, 0x00421690, 0x00422470 had no port body before) |

- x87 TUs added to `mashedmod/x87_tus.txt`: MatchS05_00420d80, _00420de0, _00422470, _004224d0, _00425ab0, _00425b10.
- `build_objs.ps1` unchanged. No allowlist key added or removed: `py -3.12 scripts/lint_rva_bodies.py` -> 47 known
  (CROSS-TARGET 33, DUP-IN-TARGET 14), NEW 0.
- 0x0041f710: the original passes the record in EAX (caller 0x00420870, `mov eax,esi` at 0x004209ed). The matched
  body is the cdecl `CR_41f710Body`. `CR_41f710` (the symbol `Race/CarRegistrant.cpp` calls with the record in EAX)
  is a 4-instruction naked adapter (push eax / call / add esp,4 / ret) with no RVA comment, so it is not a body.

**54 byte-identical rows not swapped.** 49 have no port body (nothing to swap, no exe or asi caller to serve).
5 have a port body and were not swapped in this pass (the pre-reboot session left no reason on record):
0x00422120 (`Util/TimerInit.cpp` TimerInitLoop), 0x00422440 (`Math/CosineLerp.cpp`), 0x00422b50
(`HUD/ScenarioWriters_sa2s2.cpp` VehicleDamageAccum), 0x00423040 (`Frontend/SlotZeroers_s1.cpp` FrontendDirInput),
0x004233e0 (`Ai/AiLeafTargets.cpp` AL_4233e0 + `Ai/HeadingAtan2.cpp`, allowlist key D4-DC-INC-004233e0). These are
follow-up swaps (a MATCH-S05-B row): 0x00422440, 0x004233e0 and 0x00422b50 sit on per-frame paths, so they need
their own gate run.

## Exe callers (L-68)

Binary count: `py -3.12 verify/matchs05_20261101/tools/exe_callsites.py mashed_re.exe mashed_re.map <symbols>`
(direct `call`/`jmp rel32` into the symbol from outside its body), on the merged build.

| rva | symbol | exe call sites | live? |
|---|---|---|---|
| 0x0041f710 | CR_41f710Body | 1 (`_CR_41f710+0x1`), and `CR_41f710` has 1 (`Race::CarsReg_FrameTick+0x67`) | yes, per frame |
| 0x00423270 | TabCycler | 1 (`_FrontendInputDispatch+0xf`) | no: FrontendInputDispatch (0x00423b00) has 0 exe call sites |
| 0x00423320 | CursorMover | 1 (`_FrontendInputDispatch+0x14`) | no, same |
| the other 27 exe swaps | | 0 | no |

## Gates (merged build)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on exe `c8c2220a`, private root
`D:/MashedWT/MATCH-S05_root`, test slot + `mashed_lock.py run --solo` (2026-11-01 15:50, one window with the
MATCH-0040E370-NAKED gates). Every arm exit 0, self-exited.

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here: `mashedmod/src` is in `full_set_paths`, so the
train runs it on enqueue.

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

- **0x0041f710** `FUN_0041f710`: C2 -> C4, route `match` (c4=match). `match.py re/match/s05/0041f710.c` ->
  `100.0% (regs 100.0%, ops 100.0%) 22/22 ins flags=/O2 /GS /Ob1 tc=ddk`; `relocheck.py` -> `OK checked=1
  unchecked=0`. Source sha256 `226667aa4d05a1de321387e0121574472e14a0deeb1a95ea7b3964dbac7e2532`. One body
  (`Match/MatchS05_0041f710.cpp`, exe only, as the old body was). exe-callers=1 (`CR_41f710` adapter <-
  `Race::CarsReg_FrameTick`). Gates unchanged on this build (G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9`). U-8047 (identity of FUN_00484cf0, Blocks=none) stays attached;
  it does not change the body. CHANGELOG line:
  `2026-11-0x  0x0041f710  FUN_0041f710  C2->C4  route match (c4=match), exe-callers=1 (Match/MatchS05_0041f710.cpp via CR_41f710 <- Race::CarsReg_FrameTick); evidence: verify/matchs05_20261101/RESULT_MATCH-S05.md`
- **0x00423270, 0x00423320**: stay at their level. notes `match evidence: identical, swapped, exe-callers=1 from
  FrontendInputDispatch (0x00423b00), which has exe-callers=0; C4 when a live caller lands (L-68 ruling)`.
- **the other 27 exe swaps and the 29 asi-only swaps**: stay at their level, notes
  `match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, .asi not deployed. exe sha256
`c8c2220a7fb3a148d0541a495005a30db47bf186904a5713e5107782e695f865`, asi
`48d84a12a686a5d2c4127a35b2e1e0f45d2a33f8bc94c731f96ad407950a49cc`.

## Raw files (not committed; `D:/MashedWT/MATCH-S05_data`)

| file | sha256 |
|---|---|
| arms2/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms2/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms2/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms2/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| gates_both.log, build2.log | run logs |

`exe_callsites.txt` (committed next to this file) is the binary call-site count for the 30 exe swaps.
