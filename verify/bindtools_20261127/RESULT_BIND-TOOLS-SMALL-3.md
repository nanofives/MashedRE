Binder selftest: NOT MEASURED (not run: L0 DISK HOLD, C: at 14 GB free, under the 25 GB line; baseline log/orch/run56_selftest_head.txt is PASS 0 on every profile). Not met: 2 of 3 commits done, BIND-MGLOBAL-C-FORM not done, stop moved from "none started" to "needs a ddkobj.py decision".

# RESULT BIND-TOOLS-SMALL-3 (run 56)

Branch orch/BIND-TOOLS-SMALL-3, base 1379467a6.

## Commits
1. f4ef38f85 BIND-MOVED-TSV-AUTO: new `porttop_bind.py moved --profile P [--rva R ...] [--cause TEXT] [--dry]`. Appends to
   state/P/moved.tsv one line per listing line that moved from the frozen listing and is not named yet (only the --rva ones
   when given), cause = TEXT + kind change + "now bound as PortTop_Bound_<rva> (RVA 0x<rva>)", then a totals line. Existing
   lines are never rewritten (RULE 3).
2. e1c7de25d BIND-BULK-BINDINGS-TAG: `bindings --profile bulk` died with KeyError 'bindings_tag'. Fixes: PROFILE_DEFAULTS
   bindings_tag=None, fallback to own_tag, clear exit when neither exists, and a None caller (bulk has no bindings_root_caller)
   written as an empty field (it crashed `'\t'.join`). Data: state/bulk/profile.json bindings_tag RACE-BIND-C4-BULK.
3. BIND-MGLOBAL-C-FORM: NOT done, see below.

## Checks run
- `py -3.12 re/tools/bind/test_porttop_bind_lib.py`: 10/10 PASS (new case `moved` included, link case ran).
- `porttop_bind.py moved --profile bulk --dry` (and with --rva 00401570): 0 lines, bulk moved.tsv already names every moved line.
- `porttop_bind.py bindings --profile bulk --dry`: before the fix KeyError, after: "updated 4 added 425 (dry run)". BINDINGS.tsv not written.
- Binder selftest, VS2022 build, object build, ddkobj check: NOT RUN (disk hold). Neither commit changes plan or wrapper generation
  (commit 1 adds a command, commit 2 only touches `bindings`), but that is a reading, not a measurement: [UNCERTAIN], test = `py -3.12 re/tools/bind/porttop_bind.py selftest` must end PASS 0 on the merged tree.

## BIND-MGLOBAL-C-FORM: why not done
0x0047b860 / 0x0047b8d0 are C units (Match/PortTop_<rva>.c). Their wrapper (state/raceload/rules.tsv extra_pre, lines for 0047b860 and
0047b8d0) defines `MGLOBAL(T, n, a) enum { mglobal_##n }` plus `#define DAT_x (*(T *)0x...u)`, because the source's C form
`extern T n` is unresolved in the exe. re/tools/ddkobj/ddkobj.py (lines 154-160, MGLOBAL_ADDR_RE at 216) accepts a wrapper MGLOBAL
only in the exact C++ form `static T &n = *(T *)(a)`; any other form sets defs["@mglobal"] and the unit is refused
wrapper-redefines-globals. A C unit cannot use the C++ form, and ddkobj reads a .c wrapper without __cplusplus, so no binder-only
text is accepted. The needed change is in ddkobj.py: accept the exact enum form, plus a fixture beside `mgaddr` in _ST_SRC.
ddkobj.py is not in this row's allowed edit list, and its fixture needs a ddkobj selftest run, which the disk hold forbids. Not applied.
Proposed follow-up row: DDKOBJ-MGLOBAL-ENUM-FORM (ddkobj.py + fixture, then ddkobj check of the two units against run56_headcheck).

## Housekeeping
- frida-<hash> folders in %LOCALAPPDATA%\Temp: 698 at the disk-hold check, none created by this row.
- No build outputs, no game runs.
