# relocheck.py RE_G regression (MATCH-RWLIB lead, 2026-11-08)

relocheck.py uses RE_G only at line 33 (`gl = {name: int(a,16) for name, a in RE_G.findall(text)}`), so the
name->address map per source decides every relocheck verdict that RE_G can influence. Compared that map under three
RE_G variants for all 4002 sources: every re/match/**/*.c on orch/integration (incl. rwl01/02/05) plus the unmerged
re/match/rwl03, rwl04, rwl06 worktrees.

- orch/integration line (RWL01, `(?:[^,;]+,\s*)?`) vs RWL03 line (`(?:[^,;\n]+,\s*)?`): **0 files differ**. RWL03's
  change is a no-op on every existing source.
- pre-fix line (`(?:[^,]+,\s*)?`) vs RWL01 line: 11 files differ: rwl01/004c0b30, rwl01/004c57a0, rwl01/004c90a0,
  rwl05/005584c0, s05/00420d00, s23/0051c220, s24/00555f20, s25/005a8060, rwl03/004f0970, rwl04/005493e0, rwl04/00553ef0.
  Under the current tool: s05/00420d00 OK (unchanged, its recorded verdict), rwl01/004c0b30, 004c57a0, rwl03/004f0970,
  rwl04/00553ef0 OK (were false BAD, now counted in their slices). s23/0051c220 (25.1%), s24/00555f20 (96.1%),
  s25/005a8060 (40.0%), rwl01/004c90a0 (68.5%), rwl05/005584c0 (90.0%), rwl04/005493e0 (below 100%) are BAD, and all
  are below 100% match (misaligned bytes), none listed in any relocheck.tsv as identical.
