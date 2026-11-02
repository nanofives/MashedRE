# RESULT MATCH-TOPDOWN-T07 (2026-11-01)

Verdict: **GREEN, TEXT-ONLY.** 0 of the 12 unlock RVAs can be swapped soundly into `mashed_re.exe`; each has a
one-line reason below. No build input changed. The blocking gap 0x0043d7c0 (S08, unmatched) blocks the whole
subtree, and below it the exe has no pause-menu overlay to call into: it wires only ESC = resume and ENTER = quit.

L-72 (Mariano 2026-11-01): swap matched callers in, top-down. T07 = the pause-menu subtree from
`RESULT_MATCH-TOPDOWN-MAP.md` unit 7: connector `0x00492d30* > 0x0043d7c0*`, 12 matched functions in 4 roots
(0x0040de10, 0x0040ab40, 0x0040ac80, TimerGetBasePtr 0x00413f90), all new exe bodies on the map, 2 x87
(0x00414220, 0x00417750).

## Why nothing swaps

**No exe call site.** In the original, 0x0043d7c0 is the pause-menu overlay (event 0xff210000; records Resume /
Restart / Quit). Its actions call the four roots: Restart `-0xe00000` -> 0x0040de10 (`Race/GameFlow.h:42-43`),
the autosave-state steps 0x0040ab40 / 0x0040ac80, and TimerGetBasePtr 0x00413f90. The exe's pause is
`GameFlow_SetPaused` / `GameFlow_RequestExit` in `exe_main.cpp:2229-2262`: ESC toggles the pause, ENTER while
paused quits, and the comment there states "Restart (-0xe00000 -> FUN_0040de10) is left for the full port". No exe
code corresponds to any of the 0x0043d7c0 -> root edges.

**The state the subtree uses is dead in the exe** (zero-filled original range, T13 RESULT):

| global | used by | exe status |
|---|---|---|
| `0x008a9584/88/8c/90/94/98`, `0x008a95b0` | 0x0040ab40, 0x0040ac80, 0x00409930 | the exe keeps the autosave state in port statics: `Save/GameSaveVFS.cpp:47-58` (`s_autosave[3]` under `MASHED_STANDALONE`); the `0x008a9584` mention in `Race/GameFlow.cpp:250-251` is a comment |
| `0x005f2770` (course-handler table), `0x0063ba78` | 0x0040ddb0 (`**(0x005f2770 + 0x0063ba78*4)`), 0x0040e4b0 (`0x005f2770[0xd+i]`) | not filled in the exe (T13 row 0x0040ce80, U-9783; the exe's slot state is `CarSlotStateGet`): both reads dereference near 0 |
| `0x00639d74` | 0x00405420 | referenced only by asi-only TUs |
| `0x0063bb10/14/1c` | 0x00411750 | its only arm runs when 0x0042f6a0() == 2; 0x0042f6a0 reads `0x0067e9fc`, 0 in the exe (`Race/SubStateMachine.cpp:25-34`) |
| `0x005f2b10` | 0x00413f90 (returns its address) | original initialised `.data`, zero in the exe |
| `0x0089a364/68/78/7c/80`, `0x0089a420` | 0x00414220, 0x00417750 | `0x0089a37c` and `0x0089a364` are **live**: the exe seeds the AI difficulty from `0x0089a37c` (`TrackRenderer.cpp:6613`) and writes the `0x0089a364 = -1` sentinel (`Race/AiDifficultyReset.cpp`). Calling 0x00417750 would move AI difficulty, a gate-visible behaviour change |

## Per-RVA table

`match.py` = `re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`),
re-run (log `D:/MashedWT/MATCH-TOPDOWN-T05_data/match_rerun.log`, shared with T05 / T10). `exe callers` = E8/E9
rel32 sites in the baseline `mashed_re.exe` of orch/integration `3a3186c4a` (this branch's base), scanned against
its `/MAP`. Original callers from the map's call graph (`D:/MashedWT/MATCH-TOPDOWN-MAP_data/cg.json`).

| rva | outcome | matched source | match.py | relocheck | exe body | orig callers | exe callers | reason |
|---|---|---|---|---|---|---|---|---|
| 0x0040de10 | left | `re/match/s02/0040de10.c` | 100.0% 4/4, 25 B identical | OK checked=4 unchecked=0 | none | 0x0043d7c0 | 0 | Restart: the exe has no Restart action (`exe_main.cpp:2259-2260`); body sets `DAT_0063ba8c = 1` and runs 0x0040ddb0 / 0x00411750 / 0x00405420 on original state. Wiring Restart is a new feature: knob + PREREG + decision |
| 0x0040ddb0 | left | `re/match/s02/0040ddb0.c` | 100.0% 12/12, 50 B identical | OK checked=8 unchecked=0 | none | 0x0040de10 | 0 | reads `**(0x005f2770 + 0x0063ba78*4)` (zero table in the exe) for 0x00426c50, and calls 0x00475a60 / 0x0045bf30 / 0x00475ef0 / 0x004046a0 with no exe body |
| 0x00411750 | left | `re/match/s02/00411750.c` | 100.0% 21/21, 88 B identical | OK checked=8 unchecked=0 | none (asi `Vehicle/Replay.cpp` ReplayStartLap) | 0x0040de10, 0x00411d90 | 0 | switch on 0x0042f6a0() == 2, which is 0 in the exe, so the body is a no-op there; 0x00411d90 has no exe body |
| 0x00405420 | left | `re/match/s01/004053d0.c` | 100.0% 2/2, 11 B identical | OK checked=0 unchecked=1 | none (asi `Render/PromoLoop_round11.cpp` ReplayCursorReset) | 0x0040de10 (tail) | 0 | writes `0x00639d74 = 0.0f`, no exe reference; only reached from Restart |
| 0x00417750 | left (x87) | `re/match/s03/h3_00417750.c` | 100.0% 22/22, 91 B identical | OK checked=3 unchecked=8 | none | 0x0040ddb0 | 0 | writes live `0x0089a37c` (AI difficulty seed): a call is a behaviour change. Only reached from Restart |
| 0x00414220 | left (x87) | `re/match/s03/h2_00414220.c` | 100.0% 67/67, 213 B identical | OK checked=8 unchecked=14 | none | 0x00417750, 0x0040dd60 | 0 | writes live `0x0089a364` and `0x0089a380`; 0x0040dd60 is asi-only (`Save/Race_Guard.cpp`); `TrackRenderer.cpp:7517` documents the original's case-8 call but the exe's next-round path does not make it |
| 0x00413fa0 | left | `re/match/s03/h2_00413fa0.c` | 100.0% 20/20, 49 B identical | OK checked=2 unchecked=0 | none (asi `Util/PromoLoop_sessionB.cpp` Calc413fa0) | 0x00417750, 0x00414060 | 0 | 0x00414060's exe port `AiDifficultyResetTail` (`Race/AiDifficultyReset.cpp`, exe only) ports step 6 only; steps 1-5 (the 0x00413fa0 call) are left out because their inputs are 0 in the exe and a faithful port would write `0x0089a360 = 0.0` (header of that file) |
| 0x0040e4b0 | left | `re/match/s02/0040e4b0.c` | 100.0% 36/36, 73 B identical | OK checked=2 unchecked=0 | none | 0x00414220, 0x00448220, 0x0045d250 | 0 | reads `0x005f2770[0xd+i]` (zero table, dereferences near 0); the exe's port of 0x00448220 (`TrackRenderer.cpp:4502-4512`) models only the record-state-0 slice and makes no such call; 0x0045d250 has no exe body |
| 0x0040ab40 | left | `re/match/s02/0040ab40.c` | 100.0% 16/16, 116 B identical | OK checked=11 unchecked=0 | none | 0x0043d7c0 | 0 | autosave state machine on `0x008a9584..98` (with hidden statics 0x0040a9a0 / 0x0040aa30); the exe's autosave state is port `s_autosave`; callees 0x00409970 / 0x0042c1a0 / 0x0045b350 have no exe body |
| 0x0040ac80 | left | `re/match/s02/0040ac80.c` | 54.5% insn 12/22, 80 B whole symbol identical (`by=bytes`, as S02) | OK checked=7 unchecked=0 | none | 0x0043d7c0 | 0 | same autosave globals as 0x0040ab40 |
| 0x00409930 | left | `re/match/s01/raw/00409930.c` | 100.0% 4/4, 31 B identical | OK checked=0 unchecked=3 | none (asi `Util/UtilBatch.cpp` LoadingState3Enter) | 0x0040ab40 (tail), 0x0043dfd0 | 0 | writes `0x008a9584 = 3`, `0x008a9590 = 1`, `0x008a95b0 = 0`, dead in the exe; the exe's frontend machine `RaceRuleFromEvent` (`Race/RaceModes.cpp`, 0x0043dfd0) does not make this call |
| 0x00413f90 | left (sound body swap available, not done) | `re/match/s03/raw_00413f90.c` | 100.0% 2/2, 6 B identical | OK checked=0 unchecked=1 | `TimerGetBasePtr`, `Util/TimerState.cpp` (both targets; `rva_body_scan` does not bind it: its RVA comment is in the file header, not above the definition) | 0x0043d7c0, 0x0043dfd0, 0x00430670 | 0 | the port body and the matched source both return `&0x005f2b10`, so a swap is sound, but the row is already C4 (frida route, hooks.csv), the exe has 0 callers (0x00430670 `MenuOrdinalToPlayerSlot` is asi-only, `RaceRuleFromEvent` does not call it), so the match route adds no level (L-68) and would cost a build-input change plus a gate run. Carry it with the S08 0x0043d7c0 swap |

## What T07 needs from S08 (and after)

1. **0x0043d7c0 matched** (S08). Deep gaps under the subtree (map, `topdown_ranked.tsv` unit 7, 10 total), top:
   0x00426c50, 0x00476430, 0x004241b0, 0x0042c1a0, 0x0042f6a0, 0x00430790.
2. **An exe pause overlay that calls it**: today `exe_main.cpp:2229-2262` has no menu records, no Restart, and the
   frozen-scene overlay is port-drawn. Porting 0x0043d7c0's records (Resume / Restart / Quit) is a feature, so it
   lands as a default-OFF knob + PREREG + decision.
3. **State the swapped bodies can read**: the autosave state at `0x008a9584..` (or rebind 0x0040ab40 / 0x0040ac80
   / 0x00409930 to `s_autosave`), the course table `0x005f2770` (or rebind 0x0040ddb0 / 0x0040e4b0 to
   `CarSlotStateGet` / the port's course record), and a decision on whether Restart may move the AI difficulty
   (`0x0089a37c`, via 0x00417750), which changes the gates.

Until then the 12 stay matched with no exe caller. If S08 matches 0x0043d7c0 but the overlay port is not
approved, 0x00413f90 is the one body that can still be swapped, below C4 for lack of a caller.

## Gates

Not run: no build input changed. The baseline build used for the `/MAP` counts is the T05 branch's
(orch/integration `3a3186c4a`, the same base as this branch): `=== Build OK ===`, `[rva-lint] ... NEW=0`,
`[asm-lint] OK`.

## Re-classify

None. No level changes for the 12 rows.

## Raw data (D:/MashedWT/MATCH-TOPDOWN-T05_data, shared with T05 / T10, not committed)

```
81e39d800314d4ad10c36b0c9421ac850093169fbf1fc3550cda7a59ebcd8609  mashed_re.exe (baseline build, orch/integration 3a3186c4a)
a7780b0705f0b0b17c44254522bdb2c5ad1ce939ddc61f538941e0c1e75bd1e7  mashed_re.map
2ac3848f7d1f2e88a72fa8df1e193550654a2588dc7a7e598e3c200e2b65cfd8  match_rerun.log
50521efb7b9168068fb115fb6b623ab1e72d86eeabe9cee6e0c6d1fd0c7faeab  ../MATCH-TOPDOWN-MAP_data/cg.json (original call graph)
```
