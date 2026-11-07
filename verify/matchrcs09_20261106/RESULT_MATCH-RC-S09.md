# RESULT MATCH-RC-S09 (2026-11-06)

**Verdict: GREEN.** Text-only tracker legs, no code changed, gates: not run: tracker-only. Branch `orch/MATCH-RC-S09`
cut from `orch/integration` b166d58f4. Edits: `hooks.csv` (7 rows, nothing else touched, CRLF kept) and
`re/analysis/CHANGELOG.md` (7 lines under `<!-- ENTRIES -->`).

## (a) MATCH-RC-S09: five S09 C4 candidates, all admitted

Route `match`, criteria (1)-(3) of `re/CONFIDENCE.md`. Per-row source evidence is in `verify/matchs09_20261102/`
(`results.tsv`, `relocheck.tsv`, `swaps.tsv`, RESULT_MATCH-S09.md): `match.py` 100% under XP DDK cl 13.00.9176 /
c2 13.00.9178, `/O2 /GS /Ob1`, C, x87; relocheck OK with `unchecked=0` for all five; the wrapper
`Match/MatchS09_<va8>.cpp` is the only body (old bodies removed, `swaps.tsv`), and `exe_file == file`.

| rva | old level | match.py | relocheck | exe call sites (binary) | reach WinMain |
|---|---|---|---|---|---|
| 0x004576b0 | C3 | 100% 20/20 insn | OK checked=2 | 1 (`_PK_459000+0xf0`) | yes |
| 0x00458fa0 | C3 | 100% 38 B | OK checked=1 | 1 (`PickupField::InitReal+0x9a`) | yes |
| 0x00451990 | C2 | 100% 191 B | OK checked=3 | 1 (`Coupling+0x70`) | yes |
| 0x00451a50 | C1 | 100% 128 B | OK checked=4 | 4 (`Pontoon_Tick` x2, `RunReplay` x2) | yes |
| 0x00451cc0 | C2 | 100% 10/10 insn | OK checked=0 (no relocations) | 4 (`Pontoon_Init`, `Pontoon_Reset`, `StateMachine` x2) | yes |

Exe-caller witness (L-68): this worktree was built with `set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, exe sha256
`cd62a87e336b72d1e7eeb5d2dae0bd85048c4677d20f0b204e0a8a140a4ab434`. Then
`py -3.12 verify/matchs09_20261102/tools/exe_callsites.py mashedmod/build/mashed_re.exe mashedmod/build/mashed_re.map <symbols>`
and `exe_reach.py` with the same arguments. Every row has at least one direct call or jmp site outside its own body and a
direct chain from `WinMain`, so none is callerless. The chains are quoted in each `hooks.csv` note.

Gate (3): S09's own gates, cited in each note: exe c8d0c53c, G-CTRL 8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3, equal to the
`merge_train.json` refs of that date (RESULT_MATCH-S09-REMERGE.md). `merge_train.json` now lists G-CTRL gates.csv `67931834`
(RECORDS flip 2026-11-06, not an S09 effect). I did not re-run the gates here (tracker-only).

Caveats, stated in the notes and not refusals:
- The three pontoon rows (0x00451990, 0x00451a50, 0x00451cc0) run on Warzone only, which the default gates do not load.
  The rule asks for an exe caller, not a default-gate execution, so they are admitted. 0x00451990's swap changed the NaN
  case of the box test to the original's.
- The jump to C4 from C1/C2/C3: the match route has no prior-level requirement in CONFIDENCE.md, so it is allowed.
- The 14 asi-only S09 swaps (`exe-callers=0`) are not touched here. S09's text asks for the
  `match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)` note on them. That was not in this
  brief, so it is left to the conductor.

## (b) MATCH-GFFU-TRACKER-497450

- 0x00496900 SlotActiveThunk: C4 (earlier racediff route) kept. `file` and `exe_file` now `Match/MatchGFFU_00497450.cpp`. No
  `c4=match`: the body is a hand forwarder to the 0x00497450 body, not the byte-matched source `re/match/s16/00496900.c`
  (100%, relocheck OK checked=1, sha256 in the note). exe call sites: 1 (`FUN_0042c220+0x26` calls `0x0018f2b0`), reach WinMain.
- 0x00497450 Pred497450: C2 kept. `file` and `exe_file` now the same TU (was `PromoLoop_sessionB.cpp` and
  `Frontend/MenuNavSM.cpp`, the dual-copy defect of 2026-09-29). Note: `match evidence: identical, swapped, exe-callers=1`
  (through the thunk). The linker shares one address (`0x0018f2b0`) between both symbols, so `exe_callsites.py` reports 0
  sites for `Pred497450` and 1 for `SlotActiveThunk` for the same call. C4 was not requested.

## Raw
Nothing raw committed.
