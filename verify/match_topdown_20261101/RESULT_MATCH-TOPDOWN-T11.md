# RESULT MATCH-TOPDOWN-T11 (2026-11-01)

Verdict: **GREEN, TEXT-ONLY.** 0 of the 8 unlock RVAs can be swapped soundly into `mashed_re.exe`; 6 of them are
already swapped (MATCH-S02 `Match/` bodies) with 0 exe callers. No build input changed. The blocking gaps 0x00434720
and 0x0043bf30 (S08, unmatched) hold the whole subtree, and the state below them is dead in the exe.

L-72 (Mariano 2026-11-01). T11 = `RESULT_MATCH-TOPDOWN-MAP.md` unit 11: connector
`0x0043c5b0* (MenuDrawLoop) > 0x0043bf30* > 0x00434720*`, 8 matched functions, 2 new / 6 already swapped, 1 x87.

## Why nothing swaps

- **No exe call site.** Every original caller is unmatched with no exe body: 0x00434720 (menu, S08), 0x0042c960
  (`CameraTransitionStateMachine`, S07 gap), 0x0040e590 (S02 not attempted), 0x00429e10, 0x004427c0. The one reached
  exe caller is the port of 0x00410510 (`Race/EvalResultCore.cpp`), which reads its metrics through the
  `EvalResultInputs` interface fed the port snapshot (MATCH-S02 "Not swapped", map RESULT).
- **The state is dead in the exe** (T10 RESULT table): the score arrays `0x008a94f0`, `0x008a9530/40/50/60` and
  `0x0063b8ec` have no exe writer. The only live input is `0x0089a870` (read by 0x00417740), which
  `AiPreTickRubberBand` writes; the exe's standings sort (`exe_main.cpp:4238-4247`) states the modes 4/7 override
  through 0x00417740 is unmodelled (U-9080).

## Per-RVA table

`match.py` = `re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`),
re-run 2026-11-01 (table `D:/MashedWT/MATCH-TOPDOWN-T01_data/t11.tsv`). `exe callers` = call edges in the baseline
`mashed_re.exe` of orch/integration `3a3186c4a` (/MAP + `verify/match_topdown_20261101/cg_build.py`), excluding the
`s_AutoReg_*` registry entry and the function's own jump table.

| rva | outcome | matched source | match.py | relocheck | exe body | orig callers | exe callers | reason |
|---|---|---|---|---|---|---|---|---|
| 0x0040b460 | left | `re/match/s02/0040b460.c` | 100.0% 78/78, 221 B identical | OK 7/0 | none (asi `Frontend/MenuMenusB.cpp` MenuMenusBD) | 0x0042c960, 0x00434720 | 0 | both callers unmatched, no exe body; reads `0x008a94f0` (no exe writer) |
| 0x0040b620 | left (swapped, S02) | `re/match/s02/0040b620.c` | 100.0% 51/51, 136 B identical | OK 3/0 | `SlotSortByModeScore`, `Match/MatchS02_0040b620.cpp` | 0x00434720 | 0 | only caller is the S08 gap; sorts `0x008a9530` (no exe writer) |
| 0x0040b6b0 | left (swapped, S02) | `re/match/s02/0040b6b0.c` | 100.0% 3/3, 12 B identical | OK 1/0 | `ModeScoreGetBySlot`, `Match/MatchS02_0040b6b0.cpp` | 0x0042c960, 0x00434720 | 0 | callers unmatched, no exe body; `0x008a9530` dead in the exe |
| 0x0040b6c0 | left (swapped, S02) | `re/match/s02/0040b6c0.c` | 100.0% 3/3, 12 B identical | OK 1/0 | `FrontendArrayGet`, `Match/MatchS02_0040b6c0.cpp` | 0x0042c960, 0x00434720 | 0 | same; `0x008a94f0` dead in the exe |
| 0x0040b7a0 | left (swapped, S02) | `re/match/s02/0040b7a0.c` | 100.0% 2/2, 6 B identical | OK 1/0 | `HotkeyStringBaseGet`, `Match/MatchS02_0040b7a0.cpp` | 0x00434720 | 0 | only caller is the S08 gap; `0x0063b8ec` dead in the exe |
| 0x0040b7b0 | left (swapped, S02) | `re/match/s02/0040b7b0.c` | 100.0% 18/18, 84 B identical | OK 4/0 | `PlayerHotkeyTableGet`, `Match/MatchS02_0040b7b0.cpp` | 0x00434720 | 0 | only caller is the S08 gap; `0x008a9530..60` dead (the 12 imm32 byte hits of its address in the exe are coincidental values in `.rdata` / `.data` / `.text`, checked by symbol) |
| 0x0040e3a0 | left (swapped, S02) | `re/match/s02/0040e3a0.c` | 100.0% 44/44, 168 B identical | OK 1/0 | `PlayerColorTableGet`, `Match/MatchS02_0040e3a0.cpp` | 0x0040e590, 0x00434720 | 0 | both callers unmatched gaps with no exe body (0x0040e590 is T09's gap) |
| 0x00417740 | left | `re/match/s03/h1_00417740.c` | 100.0% 3/3, 16 B identical | OK 0/2 | none (raw pointer in asi `Frontend/MenuMenusB.cpp`) | 0x0040b460, 0x0040b540, 0x00410510, 0x00429e10, 0x004427c0 | 0 | `0x0089a870` is live, but the only reached caller port (0x00410510, `EvalResultCore`) uses the port snapshot, and wiring the modes 4/7 standings override (U-9080) is a behaviour change |

The 6 already-swapped rows keep their level and their L-68 note (`exe-callers=0`).

## What T11 needs from S08 (and after)

1. 0x00434720 and 0x0043bf30 matched (S08), with MenuDrawLoop 0x0043c5b0 above them.
2. An exe menu that calls them: the exe frontend draws through RVA-less port code (`Frontend/Cluster_v1.cpp`,
   `exe_main.cpp`; map `entry_port`), not through 0x0043c5b0.
3. Live score state: the score arrays at `0x008a94f0` / `0x008a9530..60` / `0x0063b8ec` written in the exe (they
   belong to the round end, T10), or these getters rebound to the port's `scores_` (a port change, not a swap).

## Gates

Not run: no build input changed.

## Re-classify

None.
