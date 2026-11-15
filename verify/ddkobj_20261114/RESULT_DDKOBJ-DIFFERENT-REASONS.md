# RESULT DDKOBJ-DIFFERENT-REASONS (run 47, 2026-11-14)

Verdict: **GREEN** (text/static row, every DIFFERENT unit has a reason). Base: `orch/integration` @ 198109d50
(ledger snapshot over 30e60f93b, the last merge-train head). No build, no gates, no game runs.

## What was run

| Step | Command | Output |
|---|---|---|
| Merged-head DIFFERENT list | merge train drain 4, run 46 (`log/orch/run46_drain4.log` line 9) | IDENTICAL 1241/1275, DIFFERENT 19 |
| Unit map (which wrapper compiles which re/match source per RVA, static parse with ddkobj.py's own `exe_wrappers` / `parse_source`) | `py -3.12 verify/ddkobj_20261114/map_units_DIFFERENT_REASONS.py . <rvas>` on 198109d50 | `unit_map_DIFFERENT_REASONS.json` |
| Scores of every compiled source | `py -3.12 verify/ddkobj_20261114/score_DIFFERENT_REASONS.py . <srcs>` (match.py + relocheck.py, tc from each source) | `match_scores_DIFFERENT_REASONS.txt` |
| Per-symbol byte check | existing `BYTECHECK.tsv` of the RWC-FRAME-CLUMP object build (9f48df42f, merge base d1e5ac586) | `bytecheck_9f48df42f_DIFFERENT.tsv` |
| Audio 5 byte check | existing `BYTECHECK.tsv` of the AUDIO-STACK-STEP1 object build (2e89f3602) | `bytecheck_2e89f3602_audio5.tsv` |

Why the 9f48df42f BYTECHECK stands for the merged head: its DIFFERENT set is the same 19 RVAs as the train's summary on
30e60f93b, and `git diff 9f48df42f 198109d50` over every wrapper and source in the table below plus `exe_sources.rsp` and
`ddkobj.py` is two comment lines in `ddkobj.py` (L-156 note at line 829). Raw files (not committed):
`.worktrees/RWC-FRAME-CLUMP/mashedmod/build/ddkobj/BYTECHECK.tsv` sha256
`aa9651fd8fd6c17ac1825992b9efb541221d4048189bb358e31de7ab3d32f61b`,
`.worktrees/AUDIO-STACK-STEP1/mashedmod/build/ddkobj/BYTECHECK.tsv` sha256
`47c030a46aafd4184391178ee0cc7c52ec46f020a5061abef6d8490a91efb1b3`.
`vs_copy_in_map` is False on every DIFFERENT row: in each case the object build links the DDK compile under the exe name.

## Table (19 DIFFERENT on the merged head + the 5 audio units)

Status classes: **matched-dup** = bound as matched, the object build compiles a non-matching duplicate source;
**default-dup** = a second (default-path) unit of the same RVA; **admitted** = ADMITTED.tsv; **provisional-mixed** =
PROVISIONAL.tsv but compiled anyway (see cause B); **C2-unlisted** = C2 body in neither table.

| rva | unit (wrapper -> source) | status | one-line reason | evidence |
|---|---|---|---|---|
| 0x004929d0 | Race/GameModeMachine.cpp -> re/match/gffu/004929d0.c, symbol `_GameModeMachine_004929d0` | default-dup | Cause C: the DIFFERENT row is the default-path copy (gffu 95.7%, relocheck BAD, 47 bytes); the top-down `_Orig_004929d0` (PortTop_004929d0.cpp -> s15/004929d0.c, 100%, relocheck OK) is IDENTICAL. The top-down port already points at the matching source. | bytecheck_9f48df42f (both rows); match_scores |
| 0x00493710 | Match/PortTop_00493710.cpp -> re/match/s15/00493710.c | matched-dup | Cause A: compiles s15 (94.2%, relocheck BAD, 236 bytes, first +0x24); the 100% source is re/match/pilot/a2_00493710.c (relocheck OK, StubA/B/C + printf unchecked). Port should point at pilot/a2; needs bindings for the Stub names (no port preamble). | match_scores; bytecheck_9f48df42f |
| 0x00493900 | Match/PortTop_00493900.cpp -> re/match/s15/00493900.c | matched-dup | Cause A: compiles s15 (66.9%, relocheck BAD, 392 bytes, first +0x13); the 100% source is re/match/topm01/00493900.c (relocheck OK, same MATCH-S15 preamble). Port should point at topm01. | match_scores; bytecheck_9f48df42f |
| 0x004332a0 | Match/PortTop_004332a0.cpp -> re/match/s08/004332a0.c | admitted | ADMITTED.tsv; source 97.3% (relocheck BAD), 64 bytes differ. Expected DIFFERENT. | ADMITTED.tsv; match_scores |
| 0x00475a00 | Match/PortTop_00475a00.cpp -> re/match/s12/00475a00.c | admitted | ADMITTED.tsv; source 93.1% (regs/ops 100%, relocheck OK), 2 bytes differ. Expected DIFFERENT. | ADMITTED.tsv; match_scores |
| 0x0043c5b0 | RwCore/RwCore_0042aad0.cpp -> re/match/s08/0043c5b0.c | provisional-mixed | Cause B: PROVISIONAL (40.5%), but the unit also holds 100% helper 0x0042aad0, so skip:provisional does not fire and the DDK compile is linked (2559 bytes). | PROVISIONAL.tsv; bytecheck_9f48df42f |
| 0x00498510 | Match/PortTop_00498510.cpp -> re/match/s16/00498510.c | provisional-mixed | Cause B: PROVISIONAL (95.3%), unit also holds 100% 0x00497190 and 0x004971b0; DDK compile linked (11 bytes). | PROVISIONAL.tsv; bytecheck_9f48df42f |
| 0x004c0c20 | RwCore/RwCore_004c0c20.cpp -> re/match/s19/004c0c20.c | provisional-mixed | Cause B: PROVISIONAL (98.9%), unit also holds 100% 0x004c0ad0; DDK compile linked (1 byte, +0x45). | PROVISIONAL.tsv; bytecheck_9f48df42f |
| 0x004c0f00 | RwCore/RwCore_004c0f00.cpp -> re/match/s19/004c0f00.c | provisional-mixed | Cause B: PROVISIONAL (98.8%), unit also holds 100% 0x004c0ad0; DDK compile linked (1 byte, +0x35). | PROVISIONAL.tsv; bytecheck_9f48df42f |
| 0x00401690 | Match/PortTop_004025f0.cpp -> re/match/s01/00401690.c (`___ddkreg_00401690`) | C2-unlisted | C2 reconstruction, 60.2%, relocheck BAD; BINDINGS kind `port`; 713 bytes differ. In neither table. | match_scores; BINDINGS.tsv:420 |
| 0x0045a3a0 | Match/PortTop_0045a6b0.cpp -> re/match/s09/0045a3a0.c | C2-unlisted | C2, 81.0%, relocheck BAD; BINDINGS kind `near-match`; 36 bytes differ. In neither table. | match_scores; BINDINGS.tsv:490 |
| 0x00498810 | Match/PortTop_00498860.cpp -> re/match/s16/00498810.c (`___ddkreg_00498810`) | C2-unlisted | C2 (hooks status new), 86.7% (regs/ops 100%, relocheck OK); not in BINDINGS; 4 bytes differ. In neither table. | match_scores |
| 0x00552890 | RwCore/RwCore_00552890.cpp -> re/match/rwl04/00552890.c | C2-unlisted | RW core closure, 91.2%, relocheck BAD (two globals swapped, 0x00912be0/0x00912be4); 23 bytes. | match_scores |
| 0x005530f0 | RwCore/RwCore_005530f0.cpp -> re/match/rwl04/005530f0.c | C2-unlisted | RW core closure, 67.8%, relocheck BAD; 129 bytes. | match_scores |
| 0x005533d0 | RwCore/RwCore_005533d0.cpp -> re/match/rwl04/005533d0.c | C2-unlisted | RW core closure, 55.0%, relocheck BAD; 349 bytes. | match_scores |
| 0x005538a0 | RwCore/RwCore_005538a0.cpp -> re/match/rwl04/005538a0.c | C2-unlisted | RW core closure, 19.7%, relocheck BAD; 805 bytes. | match_scores |
| 0x00554200 | RwCore/RwCore_00553e80.cpp -> re/match/s24/00554200.c | C2-unlisted | RW core closure, 99.3% under tc=rw9044 (relocheck OK); 6 bytes (first +0x9d). Register-rotation residue class (memory c2-9044-scratch-register-rotation). | match_scores |
| 0x00554940 | RwCore/RwCore_00554940.cpp -> re/match/s24/00554940.c | C2-unlisted | RW core closure, 12.5%, relocheck BAD; 1262 bytes. | match_scores |
| 0x005572c0 | RwCore/RwCore_005572c0.cpp -> re/match/s24/005572c0.c | C2-unlisted | RW core closure, 17.9%, relocheck BAD; 1071 bytes. | match_scores |
| 0x0045cd30 | (merged head: no unit) / 2e89f3602: `_PortTop_Bound_0045cd30` -> re/match/s10/0045cd30.c | not on head | Not in the merged head's object build. On AUDIO-STACK-STEP1 bound as matched: 62.8% BAD, 319 bytes. The other source pilot/a6_0045cd30.c scores 51.4%, so no matching source exists. Owned by AUDIO-STACK-5-UNMATCHED-BOUND. | bytecheck_2e89f3602_audio5; match_scores |
| 0x004647f0 | 2e89f3602: `_PortTop_Bound_004647f0` -> re/match/s10/004647f0.c | not on head | As above: 83.3% BAD, 61 bytes; only source. | same |
| 0x005a6d60 | 2e89f3602: `_PortTop_Bound_005a6d60` -> re/match/s25/005a6d60.c | not on head | As above: 50.0% (relocheck OK), 47 bytes from +0x0; only source. | same |
| 0x005aab70 | 2e89f3602: `_PortTop_Bound_005aab70` -> re/match/s25/005aab70.c | not on head | As above: 75.6% (relocheck OK), 65 bytes; only source. | same |
| 0x005acda0 | 2e89f3602: `_PortTop_Bound_005acda0` -> re/match/s26/005acda0.c | not on head | As above: 44.3% BAD, 126 bytes; only source. | same |

Tally on the merged head (19): matched-dup 2, default-dup 1, admitted 2, provisional-mixed 4, C2-unlisted 10.
None of the 19 is a 100% source that DIFFERENTs in the object build (no linker or symbol-rewrite defect seen).

## Causes

- **A (0x00493710, 0x00493900).** The c4=match tags are true on other sources (pilot/a2_00493710.c, topm01/00493900.c,
  both 100% + relocheck OK, re-run here). The PortTop wrappers include the s15 drafts. BINDINGS.tsv lines 5 and 53 say
  "the only source", which is wrong. Open rows DDKOBJ-RETARGET-493900, DDKOBJ-RETARGET-493710. [UNCERTAIN] that the
  retarget gives IDENTICAL: needs the build + check those rows run.
- **B (4 provisional).** `ddkobj.py` main_build sets skip:provisional only when every MATCH rva of the unit is
  provisional (`all(...)`, line 751 on 198109d50). These four units also hold a 100% helper, so the unit is built and the
  provisional function's DDK compile is linked under the exe name (vs_copy_in_map False), not the VS2022 body that the
  L-126 (6a) comment at that line names. Which body the replay evidence in PROVISIONAL.tsv measured, and what the oracle
  should run, is [UNCERTAIN]: open row DDKOBJ-PROV-MIXED-UNIT.
- **C (0x004929d0).** Two units for one RVA. The top-down one is IDENTICAL; the DIFFERENT row is the default-path
  GameModeMachine.cpp copy of the 95.7% gffu source. Open row DDKOBJ-GFFU-DUP-4929D0 (that file is hand path: check the
  T4 freeze before editing).
- **Unlisted C2 (10).** Each runs an unproven DDK body in the object build with no L-126 record. [UNCERTAIN] for the
  replay oracle until equiv.py rules on each: open row DDKOBJ-UNLISTED-10.

## Side note (not acted on)
- hooks.csv has 0x0043c5b0 MenuDrawLoop at C4 `verified` with no `c4=` route tag while its source scores 40.5% and it is
  PROVISIONAL. Tracker hygiene for re-classify, outside this row.

## Open rows added (roadmap.ps1 add, D4, wave 7)
DDKOBJ-RETARGET-493900, DDKOBJ-RETARGET-493710, DDKOBJ-GFFU-DUP-4929D0, DDKOBJ-PROV-MIXED-UNIT, DDKOBJ-UNLISTED-10.
