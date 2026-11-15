# RESULT TRK-0043C5B0-ROUTE (2026-11-14)

Row kind: tracker, no build, no gates. Verdict: DEMOTE 0x0043c5b0 MenuDrawLoop C4 -> C3.

## Question
hooks.csv carried 0x0043c5b0 MenuDrawLoop at C4 `verified` with no `c4=` route tag, while its generated source scores 40.5% and is PROVISIONAL.

## Evidence list
1. hooks.csv row (before): C4, status verified, file `Frontend/MenuDrawLoopTwin.cpp`, notes end at the C2->C3 entry. No `c4=` tag and no `C3->C4` text in the current notes.
2. History: `git log -G"^0043c5b0,MenuDrawLoop" -- hooks.csv` gives C3 at b37f8b312 (2026-06-11, nav-driven draw-sequence diff GREEN 20/20, `re/analysis/standalone_menu_sm/diff_menu_drawloop_navdriven.json`) and C4 at db06f5319 (2026-06-11, "C4 x3 subset-install canonical observe").
3. The C4 evidence of db06f5319 is `re/analysis/standalone_menu_sm/c4_manifest_frontend_twins_20260610.txt`: an install manifest (3/3 twins installed) plus 30 s menu survival with natural per-frame fire (~5400 invocations). It records no orig-vs-modded diff in that run. Pre-dates L-93 and L-128.
4. `re/match/PROVISIONAL.tsv` line 992: `0x0043c5b0 RwCore_0043c5b0 re/match/s08/0043c5b0.c /O2 /GS /Ob1 40.5 NOT-PROVEN` with reason "unsupported: orig conditional jump out of the function at 0x0043c616", relocheck `not run`, status provisional, evidence `verify/rwcframe_20261113/RESULT_RWC-FRAME-492E90.md` (that file is not in orch/integration at this head). The row says "never C4 by this status".
5. `re/match/ADMITTED.tsv` and `re/tools/match`: no entry for 0x0043c5b0 (grep over re/match, re/tools/match, re/analysis/CHANGELOG.md; the CHANGELOG hits are other rows that cite it as a caller).
6. No frida-replay RESULT naming 0x0043c5b0 exists (no `IDENTICAL-FOR-<N>` / `write-set:` file found), and there is no libid TSV row.

## Route check (CLAUDE.md "Confidence promotion", re/CONFIDENCE.md "C4 routes")
- match: not met. 40.5% is not a byte match, relocheck not run.
- libid: not met. Not a library function.
- frida / frida-replay: not met. The only run was install-and-survive; no committed canonical-scenario diff with the JMP live is cited on the row, and no replay RESULT exists. Provisional/admitted status is not C4 (L-128).

## Action (branch orch/TRK-0043C5B0-ROUTE)
- hooks.csv row 0x0043c5b0: C4 verified -> C3 impl, demotion note appended (commas removed so the 11-column shape holds).
- re/analysis/CHANGELOG.md: one line below `<!-- ENTRIES -->` with the `C4->C3` arrow.
- C3 is kept: the nav-driven draw-sequence diff GREEN 20/20 (2026-06-10) still stands as C3 evidence.

## Checks run
- hooks.csv parsed with csv module: widths {11: 6418, 9: 32}, identical to the widths before the edit. Row 0043c5b0 reads C3 / impl.
- `c4_reclass.py batch --help` loads. No promotion TSV written, since this is a demotion (the tool only promotes).
- No dedicated rva-lint or tracker lint script found under re/tools; none run. [UNCERTAIN] whether the merge train's `tracker_merge3.py` accepts a lone C4->C3 key-merge edit; it is not exercised here.
- No build, no gates, no game launch.

## Open point
If Mariano wants the June survival run to count as frida-route C4, that is a reading of "canonical-scenario run with the hook installed" that this row does not document; re-promote through a frida-replay RESULT or a match of the real source instead.
