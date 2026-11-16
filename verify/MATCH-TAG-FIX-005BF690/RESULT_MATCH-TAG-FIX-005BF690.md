# RESULT MATCH-TAG-FIX-005BF690 + MATCH-551A50-INCLUDE

Run 50, 2026-11-15. Branch `orch/MATCH-TAG-FIX-005BF690` on `orch/integration` 9a44eee36. One branch carries both rows.
Row kinds: TAG-FIX is match-only (no build needed, a build was run anyway for the second row). 551A50-INCLUDE is a
top-down row: both builds, `ddkobj.py check`, no G-CTRL / B (WORKER_RULES).

## 1. MATCH-TAG-FIX-005BF690

Change: `re/match/s28/005bf690.c` line 29, the `// MATCH: 0x005bf690 FUN_005bf690` tag moved onto its own line
(comment-only, 3 lines changed).

| Check | Before (HEAD) | After |
|---|---|---|
| `match.py --bytes` | 100.0%, 60 B, 0 differing unmasked bytes, BYTE-IDENTICAL, tc=ddk | same |
| `relocheck.py` | OK checked=2 unchecked=1 (`__imp__InterlockedDecrement@4`) | OK checked=2 unchecked=1 (run on the edited file) |
| ddkobj `MATCH_RE` (`ddkobj.py:62`, `^\s*//\s*MATCH:`) on the file text | `[]` (tag not seen) | `[('005bf690','FUN_005bf690')]` |

The [UNCERTAIN] "was the unit skipped because of the tag": settled in part, by test. The tag was invisible to
`MATCH_RE` before the fix and is visible after. But `grep -rln 005bf690 mashedmod/src` returns nothing: **no exe
wrapper includes `re/match/s28/005bf690.c`**, so ddkobj never had this unit as a candidate in either state. The tag
fix does not make it substitute. A substitution needs an exe wrapper (a PortTop_005bf690.cpp) and a caller, which is a
separate row. ddkobj was not run per unit before/after because there is no wrapper to build the unit from; the
`MATCH_RE` test on the file text is the before/after evidence.

## 2. MATCH-551A50-INCLUDE: include-and-drop, done (not a port decision)

Findings:
- `re/match/rwl04/00551a50.c` `--bytes`: 100.0%, 56/56 ins, 121 B whole symbol, 0 differing unmasked bytes,
  BYTE-IDENTICAL with its own `// TOOLCHAIN rw9044` and `// FLAGS /O2 /Ob1 /G5`. `relocheck.py`: OK checked=2 unchecked=0.
  The PROVISIONAL row's 96.4% was stale (the source was later matched, hooks.csv C4 by match 2026-11-11).
- Before the change, `prov_drop.py` (dry run) read `KEEP not-included 0x00551a50`.

Change:
- New `mashedmod/src/mashed_re/Match/PortTop_00551a50.cpp`, same shape as PortTop_004593b0 / PortTop_00459290:
  `FUN_00551a50 -> Orig_00551a50`, `FUN_004c5770 -> Orig_004c5770` (matched, PortTop_004c5770.cpp),
  `FUN_004c57a0 -> MashedRw_MatrixCreate_004c57a0` (existing binding in LibRw/RwBridge.cpp, the one PortTop_00459290 uses;
  no new librw binding, L-124). Under `MASHED_STANDALONE`, `RVA-LINT: ORIGTOP-ONLY`.
- `mashedmod/exe_sources.rsp`: one line added after PortTop_00551550.cpp.
- `prov_drop.py` dry run then `--apply`: 1 row removed (0x00551a50) from `re/match/PROVISIONAL.tsv`, nothing else.

Builds (worktree, `MASHED_NO_DEPLOY=1`, nothing deployed to `original\`):
- VS2022 default build: Build OK, `mashed_re.exe` and `mashed_re_dev.asi` produced.
- Object build (`MASHED_DDKOBJ=1`): the ddkobj build and check were run through `ddkobj.py build --link` and
  `ddkobj.py check` directly (the build.bat step exceeded a 600 s tool timeout once and was rerun by hand).
  Link rc=0 both times.

ddkobj check, two states of the same tree and base:

| State | ddk objects | IDENTICAL | DIFFERENT | not-in-map |
|---|---|---|---|---|
| wrapper included, PROVISIONAL row still present (ddkobj `skip:provisional` for 0x00551a50, equal to the base for every other unit) | 1539 | 1553/1589 | 21 | 15 |
| wrapper included, row dropped | 1540 | 1553/1590 | 21 | 16 |

The DIFFERENT set is the same 21 RVAs in both states (0x004332a0 0x0045c480 0x00475a00 0x004929d0 0x00554200 0x005a6b30
0x005a6e50 0x005a7810 0x005a78b0 0x005a8890 0x005a9970 0x005aa480 0x005aab00 0x005ad030 0x005ad320 0x005adb70 0x005ae210
0x005ae5b0 0x005ae610 0x005ae920 0x005aea60). They are not caused by this row. I did not build a second tree of the
base commit. The "wrapper in, row kept" state stands in for it: ddkobj skips a provisional unit, so every other unit
is compiled exactly as on the base.

**Caveat (must be read):** ddkobj now substitutes 0x00551a50 (ddk objects 1539 to 1540), but its byte check row is
`0x00551a50 _Orig_00551a50 ... not-in-map`: nothing in the exe calls `Orig_00551a50`, so `/OPT:REF` drops the body and
`ddkobj.py check` cannot compare it against `MASHED.exe.unpatched`. WORKER_RULES asks IDENTICAL for every substituted
function whose source is 100%. For this unit the check is `not-in-map` (15 other units are already `not-in-map`).
The byte identity of this source is certified by `match.py --bytes` and `relocheck.py` above, not by the exe check.
It becomes checkable once a caller binds `Orig_00551a50` (the sibling source `re/match/rwtb/005518f0.c` calls
`FUN_00551a50`, and is itself not included by any exe wrapper). That is a follow-up row, not done here.

Not run: G-CTRL, B, knob boot, menu walk (top-down row kinds that touch only a wrapper and a tracker file; the default
path does not execute `Orig_00551a50`). The unit is not C4 by this change (C4 by match was already set 2026-11-11).

## Files

`re/match/s28/005bf690.c`, `mashedmod/src/mashed_re/Match/PortTop_00551a50.cpp` (new), `mashedmod/exe_sources.rsp`,
`re/match/PROVISIONAL.tsv`, this file. No `hooks.csv` edit, no promotion TSV (no C4 change).
Raw logs are in the session scratch (`build_vs.log`, `ddk_build*.log`, `ddk_check*.log`), not committed.
