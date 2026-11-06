# RESULT MATCH-S02-RC (run 27): re-classify MATCH-S02's 19 swapped RVAs, route `match`

Branch `orch/MATCH-S02-RC` off `orch/integration` @ 5cf5cdd63 (MATCH-S02 and MATCH-RUBRIC merged). Tracker edits made
through the `re-classify` skill's own side effects (hooks.csv rows, CHANGELOG lines below `<!-- ENTRIES -->`, inline
`[UNCERTAIN]` tagging). No game run, no source or build change.

## Verdict

**GREEN. 11 rows C3 -> C4 by `match`. 3 rows already C4 get the `match` route added (C4 -> C4). 5 rows are refused
the `match` route and keep their level (3 x C3, 2 x C4 on the frida route), each for a stated check.**

## Gate evidence common to every row (re/CONFIDENCE.md "C4 routes by byte match")

| gate | evidence |
|---|---|
| (1) byte identity | `py -3.12 re/tools/match/match.py re/match/s02/<rva>.c` re-run on this tree: 19/19 at 100.0% (`match_rerun.txt`) |
| (1) relocheck | `py -3.12 re/tools/match/relocheck.py re/match/s02/<rva>.c`: 19/19 OK, 0 unchecked names on every row (`relocheck_rerun.tsv`) |
| (1) compiler / flags | XP DDK `cl` 13.00.9176 / `c2` 13.00.9178 (`tools/vc7ddk`, toolchain `ddk`), `/O2 /GS /Ob1` (file `// FLAGS:` line, match.py adds `/Gy`), C |
| (1) source + sha | `re/match/s02/<rva>.c`, sha256 of the committed (LF) blob in `source_sha256.txt` and in each row's notes |
| (2) port body | `mashedmod/src/mashed_re/Match/MatchS02_<rva>.cpp` includes that exact source inside `extern "C"`, in `exe_sources.rsp` and `asi_sources.rsp`; `file` = `exe_file` = that TU on every row now; `py -3.12 scripts/lint_rva_bodies.py`: `16 known, NEW=0`. No row uses float, so `x87_tus.txt` does not apply |
| (3) gates | merge train 20261031_131735 that merged MATCH-S02 (`-Full` pass): G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9` (= `merge_train.json` refs), (b) 3/3, (e) 3/3, D2 3/3, 13 tracks identical (`log/orch/events.jsonl`, MATCH-S02 merged event). Also the two worker gate runs in `verify/matchs02_20261031/RESULT_MATCH-S02.md` |

Additional checks made for this transaction:
- **Exe callers** (TUs in `exe_sources.rsp`, comments and `RH_ScopedInstall` lines excluded, `#ifndef MASHED_STANDALONE`
  blocks checked by hand): 0 for 18 of the 19 RVAs. `0x0040e370` (IsCarSlotActive) has 10 call sites in 4 exe TUs
  (TrackRenderer, CarRegistrant, HeadingAssist, PathProgStep). The `0x00411d60` hit is an `ExeOnce` log string, and
  the `0x0040e340` callers are `.asi` entries (Thresholds_ah4.cpp under `#ifndef MASHED_STANDALONE`).
- **Hidden second bodies** (all source file types, not only rsp TUs, because the lint scans TUs only): none for 18
  RVAs. `0x0040e370` has a naked verbatim copy `TG_40e370` in `Ai/AiTargetClosest_naked.inc`, included by the
  exe-only `Ai/AiTargetClosest.cpp`.
- **Stubs**: the 14 matched sources that pass call no callee, except `0x00411d60` (callee 0x0042f6a0 bound to the
  port's `GetRaceSubMode`, both targets). No open STUBS.md row has one of the 14 as its caller.
- **Raw-address passthroughs**: 4 wrappers bind callees to `extern "C"` forwarders that call the original address
  (0x0040bb70 / 0x0040bb90 -> 0x004c5c00, 0x0040e3a0 -> 0x004a332b, 0x0040dfc0 -> 13 callees). STUBS.md defines "a
  passthrough that calls the original at its RVA" as a stub, so these 4 fail "no stubs in the body".

## Rows

| rva | name | before | after | route | relocheck | exe-callers | source sha256 (first 16) |
|---|---|---|---|---|---|---|---|
| 0x0040ad20 | FrontendGlobalGet | C3 | **C4** | match | OK 1/0 | 0 | 4a142ece63dbd17a |
| 0x0040ad30 | FUN_0040ad30 | C3 | **C4** | match | OK 1/0 | 0 | 077f49f36ac83a8a |
| 0x0040b250 | FUN_0040b250 | C3 | **C4** | match | OK 8/0 | 0 | 79a6214cc7ce62a5 |
| 0x0040b430 | FUN_0040b430 | C3 | **C4** | match | OK 3/0 | 0 | 2f0c9b66d2088c98 |
| 0x0040b620 | SlotSortByModeScore | C3 | **C4** | match | OK 3/0 | 0 | f9eb0819586a5c94 |
| 0x0040b6b0 | ModeScoreGetBySlot | C3 | **C4** | match | OK 1/0 | 0 | 12f53f528727a76e |
| 0x0040b7a0 | HotkeyStringBaseGet | C3 | **C4** | match | OK 1/0 | 0 | 77c9627c5e3d9fef |
| 0x0040b7b0 | PlayerHotkeyTableGet | C3 | **C4** | match | OK 4/0 | 0 | fc183accfe2658ce |
| 0x0040b810 | TimerGlobalsReset | C3 | **C4** | match | OK 21/0 | 0 | d6f7387ebaa7493e |
| 0x0040d250 | DoubleDerefIndexedGetter | C3 | **C4** | match | OK 2/0 | 0 | 0c39ae4b2518cd26 |
| 0x00411d60 | ReplayCheckTimer | C3 | **C4** | match | OK 3/0 | 0 | b87b2c866854c7cb |
| 0x0040b6c0 | FrontendArrayGet | C4 | C4 | frida + match | OK 1/0 | 0 | 82d89858d6ac58cd |
| 0x0040b6d0 | FUN_0040b6d0 | C4 | C4 | frida + match | OK 1/0 | 0 | c42ab85a56bdffb4 |
| 0x0040e340 | GetLiveCarCount | C4 | C4 | frida + match | OK 1/0 | 0 | 46a8ff34c28c049a |
| 0x0040bb70 | SpriteLookupTableA | C4 | C4 | frida only (match refused: raw passthrough 0x004c5c00) | OK 2/0 | 0 | a20d439285b88225 |
| 0x0040bb90 | SpriteLookupTableB | C4 | C4 | frida only (match refused: raw passthrough 0x004c5c00) | OK 2/0 | 0 | 47fe23ca8861c238 |
| 0x0040e3a0 | PlayerColorTableGet | C3 | C3 | refused: raw passthrough 0x004a332b | OK 1/0 | 0 | 214c2c7ff015e1e8 |
| 0x0040dfc0 | HudIngameDispatch | C3 | C3 | refused: 13 raw passthroughs | OK 29/0 | 0 | 4efae3b0e833e07f |
| 0x0040e370 | FUN_0040e370 | C3 | C3 | refused: second exe body (naked TG_40e370) | OK 1/0 | 10 | dc117d70255108a5 |

Full sha256 per row: `source_sha256.txt` and the row's `notes`. Every row's `file` and `exe_file` now name its
`Match/MatchS02_<rva>.cpp` (the old TUs no longer hold the body). Status `verified` on the 14 C4 rows. The C4 -> C4
rows say in `notes` that their frida evidence measured the previous body, which the matched source replaced.

## What these C4s do and do not say

All 14 C4-by-match rows are **callerless in the exe** (`exe-callers=0` in notes, rubric "Callerless bodies"): their C4
rests on byte identity, relocheck OK and one body per RVA. The unchanged gates show only that the swap changed
nothing the default build runs. They are not run evidence and not evidence for callees or globals. `0x0040b7a0` and
`0x0040ad20` had been demoted C4 -> C3 on 2026-10-26 under strict C4 (no natural-run call count). That rule belongs
to the frida route and does not apply here. `0x0040b250`: the exe's `Race/RoundGrid.cpp:266` does its own `-1000`
fill into a port array `s_codes[]` instead of calling it (noted on the row).

Open uncertainty rows on promoted functions (naming and typing questions; byte identity does not depend on them, and
they stay open): U-0489 (0x0040b430), U-0491 (0x0040b6d0), U-1609 / U-1610 (0x0040b810), U-4203 (0x0040d250). The
uncertainty sweep found 2 unfiled `[UNCERTAIN]` markers in `re/analysis/title_screen_cont1/0x0040d250.md` and
`re/analysis/title_screen_d2/0x0040d250.md`. Both state U-4203's question, so they were tagged `[UNCERTAIN U-4203]`
and no duplicate row was filed. No new U-NNNN or S-NNNN IDs were created.

## Unblock paths for the 5 refused rows

- 0x0040bb70, 0x0040bb90, 0x0040e3a0, 0x0040dfc0: bind each callee to an exe port body instead of the raw original
  address (0x004c5c00 has only an asi-only naked port, 0x004a332b only an asi-only port, and 0x0041d870 / 0x0041e850 /
  0x0041ded0 have no port at all). Separately, the raw forwarders are stubs by definition but have no STUBS.md rows and
  no `// STUB S-NNNN` comments. Filing them needs a source-comment edit in the wrappers, so it was left out of this
  text-only transaction.
- 0x0040e370: route `Ai/AiTargetClosest.cpp` to `IsCarSlotActive` (or drop `TG_40e370`), so the exe has one body. The
  lint cannot see `.inc` bodies. A scanner extension for `#include "*.inc"` would have caught it (harness-backlog
  item, not done here).

## Files

- `match_rerun.txt`, `relocheck_rerun.tsv`, `source_sha256.txt` (this directory)
- tracker edits: `hooks.csv` (19 rows), `re/analysis/CHANGELOG.md` (19 lines),
  `re/analysis/title_screen_cont1/0x0040d250.md`, `re/analysis/title_screen_d2/0x0040d250.md` (marker tags)
