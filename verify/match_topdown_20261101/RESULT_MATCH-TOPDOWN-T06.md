# RESULT MATCH-TOPDOWN-T06 (2026-11-01)

Verdict: **GREEN, TEXT-ONLY.** 0 new swaps among the 11 unlock RVAs. 5 are already C4 with their matched or port
body in the exe (0x00404e50, 0x00404f50, 0x00404e80, 0x00404ee0, 0x004099e0) and 0x004099a0 is C4 by its port body.
The other 5 write autosave state the exe keeps in port statics. No build input changed.

L-72 (Mariano 2026-11-01). T06 = `RESULT_MATCH-TOPDOWN-MAP.md` unit 6: the save / autosave subtree under 0x00409a80,
a matched top with **no static caller** in the original (its installer is indirect and not in
`pointer_callers_2026-10-11.tsv`), no exe mention. 5 new bodies, 6 port bodies on the map, 1 x87.

## Why nothing new swaps

- **No exe call site for the top.** 0x00409a80 has no original static caller and no exe body, so there is no port
  site to call it from until its installer is found (map "Limits").
- **Autosave state is port-owned in the exe.** 0x004098d0 / 0x00409900 / 0x00409950 / 0x00409970 / 0x00409a80 read
  and write `0x008a9580..98`, `0x008a95ac`, `0x008a95b0`. Under `MASHED_STANDALONE` the exe keeps that state in
  `s_autosave[3]` and `GameFlow_SaveCounterPtr()` (`Save/GameSaveVFS.cpp:47-58`). The matched bodies would write
  the original addresses, which nothing in the exe reads.
- **The save path that runs is already matched.** `SaveLoad` / `SaveWrite` are the matched
  `re/match/s01/00404e50.c` bodies (MATCH-S01), called from `Race/GameFlow.cpp` (`SaveProgress`,
  `Campaign_LoadProgress`), with the file name, buffer and file helpers rebound per target (`Save/GameSave.cpp:236-256`).

## Per-RVA table

`match.py` = `re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`),
re-run 2026-11-01 (table `D:/MashedWT/MATCH-TOPDOWN-T01_data/t06.tsv`). `exe callers` = E8/E9 call sites in the
baseline `mashed_re.exe` of orch/integration `3a3186c4a`, scanned against its /MAP.

| rva | outcome | matched source | match.py | relocheck | exe body | orig callers | exe callers | reason |
|---|---|---|---|---|---|---|---|---|
| 0x00404e50 | left (already C4, S01) | `re/match/s01/00404e50.c` | 100.0% 9/9, 33 B identical | OK 4/0 | `SaveLoad`, `Save/GameSave.cpp` (matched source) | 0x00409a80 | 3 (`Campaign_LoadProgress`, `SaveProgress`, `GameSave_SelfTest`) | nothing to do |
| 0x00404f50 | left (already C4, S01) | `re/match/s01/00404e50.c` | 100.0% 9/9, 33 B identical | OK 4/0 | `SaveWrite`, `Save/GameSave.cpp` (matched source) | 0x00409a80 | 5 (`SaveProgress` x3, `GameFlowSaveSelfTest`, `GameSave_SelfTest`) | nothing to do |
| 0x00404e80 | left (C4) | `re/match/s01/00404e80.c` | 100.0% 26/26, 87 B identical | OK 9/0 | `DeserializeFromBuffer`, `Save/GameSaveBuffer.cpp` | 0x00409a80 | 0 | only original caller is the callerless top 0x00409a80; the exe save path uses `GameFlow`'s own image read |
| 0x00404ee0 | left (C4) | `re/match/s01/00404e80.c` | 99 B whole symbol identical | OK 10/0 | `SerializeToBuffer`, `Save/GameSaveBuffer.cpp` | 0x00409a80 (tail) | 0 | same as 0x00404e80 |
| 0x004098d0 | left | `re/match/s01/raw/004098d0.c` | 100.0% 6/6, 33 B identical | OK 0/4 | none | 0x00409a80 | 0 | writes `0x008a9584/88`, `0x008a95ac/b0`: port `s_autosave` in the exe |
| 0x00409900 | left | `re/match/s01/raw/00409900.c` | 100.0% 7/7, 44 B identical | OK 1/4 | none (asi `Util/UtilBatch.cpp` LoadingState2Enter) | 0x00409a80, 0x0043dfd0 | 0 | same state; the exe's 0x0043dfd0 port `RaceRuleFromEvent` (`Race/RaceModes.cpp`) makes no such call |
| 0x00409950 | left | `re/match/s01/raw/00409950.c` | 100.0% 5/5, 26 B identical | OK 0/3 | none | 0x00409a80 | 0 | same state |
| 0x00409970 | left | `re/match/s01/raw/00409970.c` | 100.0% 6/6, 41 B identical | OK 0/5 | none | 0x00409a80 (tail) | 0 | same state |
| 0x004099a0 | left (C4 by port body; match-route swap possible, not done) | `re/match/s02/004099a0.c` | 100.0% 10/10, 53 B identical | OK 5/1 | `AutosaveTriggerCore`, `Save/GameSaveVFS.cpp` (port, writes `s_autosave` in the exe) | 0x00409a80, 0x004117b0, 0x00430290 | static, inlined into `AutoCase` | the matched source writes the original addresses; it can replace the port body only with the `Save/GameSave.cpp` rebinding pattern (`#define DAT_008a9584 (*AutosaveState())` ...). The row is already C4, so the swap adds the match route only, at the cost of a build-input change and a gate run |
| 0x004099e0 | left (C4) | `re/match/s02/004099e0.c` | 100.0% 3/3, 10 B identical | OK 1/0 | `SaveStatusClear`, `Save/GameSave.cpp` | 0x00404e50, 0x00404f50, 0x00409a80 | 0 call sites: inlined into `SaveLoad` / `SaveWrite` in the same TU | runs in the exe through the matched `SaveLoad` / `SaveWrite` |
| 0x00409a80 | left | `re/match/s02/00409a80.c` | 100.0% 18/18, 72 B identical | OK 10/1 | none | none (indirect, installer unknown) | 0 | no port site to call it from; its state is port `s_autosave` |

## What T06 needs

1. The indirect installer of 0x00409a80 (a pointer-caller sweep after the 2026-10-11 table, map "Limits").
2. An exe autosave path at the original addresses, or the matched 0x00409a80 / 0x004098d0..70 / 0x004099a0 bodies
   compiled with the `GameSave.cpp`-style rebinding onto `s_autosave` / `GameFlow_SaveCounterPtr()`. That keeps
   behaviour only if the exe's autosave consumers are the same as the original's, which needs its own check.

## Gates

Not run: no build input changed.

## Re-classify

None.
