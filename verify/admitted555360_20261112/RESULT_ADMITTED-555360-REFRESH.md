# RESULT ADMITTED-555360-REFRESH (D4, run 40, 2026-11-12): GREEN

Row: top-down, refresh the `re/match/ADMITTED.tsv` row of 0x00555360 after MATCH-RT2D-RESID (4dd87899e) made
`re/match/s24/00555360.c` byte-identical. Branch `orch/ADMITTED-555360-REFRESH` off `orch/integration` c30545e37.

## Verdict

- `equiv.py re/match/s24/00555360.c --only 0x00555360` (tool commit eee2fc626): **EQUIV [BYTE-IDENTICAL]**. It was
  EQUIV-IF-CALLERS-IGNORE(EAX) for the old DDK text, so the caller scan (0x00552b79) no longer matters: plain EQUIV.
  Log `equiv.log`.
- Cross-check (`equiv_dyn.py`, 16 runs): default SAME same=16 inc=0 diff=0, seeded SAME same=2 inc=14 diff=0
  (`dyn_default.*`, `dyn_seeded.*`). Zero differences.
- Row refreshed: source_commit 4dd87899e, tool_commit eee2fc626, verdict EQUIV, classes BYTE-IDENTICAL, rules `-`,
  caller_scan `n/a (plain EQUIV)`, xcheck columns as above, port_binding names the wrapper and its Rt2d-open caller,
  evidence this file. CRLF kept.
- Builds (MASHED_NO_DEPLOY=1), both `=== Build OK ===`: default VS2022 (`build_default.log`) and `MASHED_DDKOBJ=1`
  (`build_obj.log`). `ddkobj.py check`: IDENTICAL 809/897, DIFFERENT 78, not-in-map 10, refusals 9
  (`ddkobj_check.log`). **0x00555360 is IDENTICAL** (368 B, `_Orig_00555360`, `BYTECHECK.tsv`).
- **Self-test:** knob boots (`run_boots.sh`: test-slot outside, mashed_lock inside, MASHED_ORIG_TOP=1,
  MASHED_DET_FRAMES=3000, RWMOD 0 and 1, default exe and object exe = 4 runs): all rc=0, `WinMain 0x00492370 returned 0
  after 3000 frames` (`boots.log`). With MASHED_ORIG_TOP_RWMOD=1 on both exes the log carries
  `rva 0x00552b60 Rt2d open ... ran: ... 0x00555360=1 ...`, so the byte-identical body ran and returned 1.
- No gates (top-down row, WORKER_RULES). No hash can move: only a comment in `Match/PortTop_00555360.cpp` and the TSV row changed
  after the builds (the wrapper `#include`s the edited source, so the binary was already built from it).

## Flag for the lead

Precedent: EQUIV-ADMIT-A6 did not admit 0x004cba80 once its source became a byte match ("superseded", C4 by match).
0x00555360 is now the same case: `hooks.csv` shows C4 `new` for it, and a byte match with relocheck OK (MATCH-RT2D-RESID)
is C4 on its own (L-93). The ADMITTED row is kept as asked and stays harmless, but it could be dropped as superseded.
Your call, not a decision row.

Files: `equiv.log`, `dyn_*`, `in.tsv`, `build_*.log`, `ddkobj_check.log`, `BYTECHECK.tsv`, `run_boots.sh`, `boots.log`.
Boot data: `D:/MashedWT/ADMITTED-555360_data`.
