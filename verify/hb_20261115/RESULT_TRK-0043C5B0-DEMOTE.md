# RESULT TRK-0043C5B0-DEMOTE (2026-11-15)

Row kind: tracker, no build, no gates. Verdict: GREEN. 0x0043c5b0 MenuDrawLoop demoted C4 -> C3 (L-163 a).

## Demotion reason (from verify/trk_20261114/RESULT_TRK-0043C5B0-ROUTE.md, cherry-picked from 17d6f823a)
- The C4 of db06f5319 (2026-06-11) has no c4= route tag. Its evidence, re/analysis/standalone_menu_sm/c4_manifest_frontend_twins_20260610.txt, is an install manifest plus 30 s menu survival, with no orig-vs-modded diff (pre-L-93, pre-L-128).
- match not met: re/match/s08/0043c5b0.c scores 40.5% NOT-PROVEN (orig conditional jump out of the function at 0x0043c616), relocheck not run, row is PROVISIONAL (never C4 by status).
- libid not applicable. No frida-replay RESULT exists for the RVA.
- C3 kept on the nav-driven draw-sequence diff GREEN 20/20.

## Transaction (re-classify demotion flow: same flow in reverse)
- hooks.csv row 0043c5b0: C4 verified -> C3 impl, demotion note appended to notes (no commas, so 11 fields hold).
- re/analysis/CHANGELOG.md: one line below the `<!-- ENTRIES -->` marker (line 28), `C4->C3`.
- STUBS.md, UNCERTAINTIES.md, DEFERRED.md: untouched (no stub or [UNCERTAIN] change in a demotion).
- c4_reclass.py has no demote operation, so both edits were made by hand with targeted sed, exactly the two files the skill names.

## Checks
- hooks.csv parsed with csv: widths {11: 6452, 9: 32} (9-wide rows are pre-existing). Row reads C3 / impl.
- `py -3.12 scripts/lint_rva_bodies.py`: `[rva-lint] 313 finding(s): CROSS-TARGET=188, DUP-IN-TARGET=125 | allowlisted=313 NEW=0` / `[rva-lint] OK -- no NEW duplicate bodies`.
- No other tracker lint script exists under re/tools.
- [UNCERTAIN] whether tracker_merge3.py key-merges a lone C4->C3 edit cleanly; not exercised here.

Commit: c709d691d (hooks.csv, CHANGELOG, both RESULT files)
