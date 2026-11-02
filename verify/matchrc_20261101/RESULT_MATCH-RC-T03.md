# RESULT MATCH-RC-T03 (2026-11-01)

Verdict: **GREEN.** `0x0040dfc0` HudIngameDispatch C3 -> C4 by route `match`, per L-77 (a) (Mariano, 2026-11-01).
Text-only leg: no build, no game run. Tracker edits: `hooks.csv` row 0040dfc0 and one `CHANGELOG.md` line.

## The three legs (CONFIDENCE.md, "C4 routes by byte match", `match`)

| leg | result | how checked |
|---|---|---|
| 1 byte-identical + relocheck | holds | `py -3.12 re/tools/match/match.py re/match/s02/0040dfc0.c --toolchain ddk --bytes`: 100.0% 84/84 ins, 424 B, 0 differing unmasked bytes, flags `/O2 /GS /Ob1`, tc=ddk (cl 13.00.9176 / c2 13.00.9178). `re/tools/match/relocheck.py re/match/s02/0040dfc0.c`: OK checked=29 unchecked=0. Source sha256 `c6274d547d7821c77e5e71f8dba794c3a2d269e4e55d539350820064425563ac`. Both re-run on this branch. |
| 2 exe body and caller | holds | `Match/MatchS02_0040dfc0.cpp` is the only body (`exe_sources.rsp:508`, `asi_sources.rsp:828`). `mashed_re.exe` calls it: `LibRw/RwRaceFx.cpp:1874` and `:1877` (`RaceFx_RenderHud`, default arm and `MASHED_HUD_REPLAY` arm). RwRaceFx.cpp is an exe-only TU compiled by `mashedmod/build.bat:125`, called from `RwRaceSubmit.cpp:2037`. T03 counted 2 E8 sites in the built exe's `.text` against `/MAP` (`_HudIngameDispatch` 0x001503e0). T03 commit 71fd65a70 is an ancestor of this branch base. Not a physics TU, so no `x87_tus.txt` entry is required (not listed, none needed). |
| 3 gates unchanged | holds | T03 RESULT, run g1 on the swap build (exe `ab1a841eaaa09655`): G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3, equal to `re/orchestrator/merge_train.json` refs (read on this branch). Not re-run here (text-only leg). |

## What C4 does not cover (names the stand-ins, L-77)

The body decides over 13 callees. In `mashed_re.exe` (`MASHED_STANDALONE` arm of `MatchS02_0040dfc0.cpp`):

- 0x00426ba0: port stand-in. It returns the HUD pass's `world_up`. DAT_0066d704 (the course object the original reads) is
  never written by any exe TU, so the exe's own `HudDrawEnabled` would return 0 and switch the HUD off.
- 0x0041c0c0, 0x0041b630, 0x0041db80: librw HUD pieces (`RaceFx_HudBindPanels`, `RaceFx_HudBindNonTeam`, `RaceFx_HudBindLights`).
- 0x0042f6a0, 0x0042f500: the exe's one body each (`GetRaceSubMode`, `GetDat0067ea64`).
- No-ops in the exe: 0x00403160, 0x0041a3e0, 0x0041c300, 0x0041ded0, 0x0041e850 (per-mode HUDs, U-9704) and
  0x0041ccc0, 0x0041d870 (state 7, not reached because the pass calls the dispatcher for states 5 and 6 only).

Coverage limit from T03: the draw-choice witness showed 0 mismatches against the old switch over every HUD pass of H_DEF and
B_DEF_K0, both round type 10 with Team Play off. The team-panel arm and mode-gated arms were not run, and the d1r1 draw-list
parity harness was not run. The row is C4 for the dispatcher's own bytes and its caller, not for those callees or arms.

## Changes

- `hooks.csv` 0040dfc0: confidence C3 -> C4, notes appended (`c4=match`, evidence above, stand-in and no-op callees, supersedes the earlier exe-callers=0 note).
- `re/analysis/CHANGELOG.md`: one line below `<!-- ENTRIES -->`.
- STUBS.md / UNCERTAINTIES.md: no change (U-9704 stays open).
