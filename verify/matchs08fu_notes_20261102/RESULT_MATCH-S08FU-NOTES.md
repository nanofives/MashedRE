# RESULT MATCH-S08FU-NOTES, 2026-11-02

**Verdict: GREEN (text-only).** Branch `orch/MATCH-S08FU-NOTES2` (redo of the rejected `orch/MATCH-S08FU-NOTES` @ `086d38f42`, which hand-edited trackers), from `orch/integration` @ `bf8abf3d0`. No source
change, no level change, no build, no game run, so no gates were run (the three rows' gates are those recorded in
`verify/matchs08fu_20261102/RESULT_MATCH-S08-FU.md` section 3: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3 on
exe `22cc3200`).

Evidence base: `verify/matchs08fu_20261102/RESULT_MATCH-S08-FU.md` (ledger row MATCH-S08-FU, merged @ `456bc2924`),
section 6 "Re-classify text", and `re/CONFIDENCE.md` "C4 routes by byte match".

## What changed

`hooks.csv`, three rows. Each got the new `file` cell, a leading `MATCH-S08FU-NOTES 2026-11-02:` notes block, and
(for the two rubber-band rows) the `exe_file` cell set to the same wrapper (`exe_file == file`, one body). Level and
status are untouched: all three stay C4 / verified on the frida route.

| rva | level | `file` (and `exe_file`) | match evidence in notes |
|---|---|---|---|
| 0x00442c80 | C4 (kept) | `mashedmod/src/mashed_re/Match/MatchS08_00442c80.cpp` | `re/match/s08/00442c80.c` 100.0% 20/20, 63 B BYTE-IDENTICAL, relocheck OK checked=4 unchecked=0, sha256 `999e15583068`, literal-constant form, exe-callers via A5 `0x0046ddb0` <- WinMain |
| 0x00442ce0 | C4 (kept) | `mashedmod/src/mashed_re/Match/MatchS08_00442ce0.cpp` | `re/match/s08/00442ce0.c` 100.0% 81/81, 249 B BYTE-IDENTICAL, relocheck OK checked=19 unchecked=0, sha256 `6a8081246561`, literal-constant form, same A5 chain |
| 0x00413f90 | C4 (kept) | `mashedmod/src/mashed_re/Match/MatchS08FU_00413f90.cpp` (`exe_file` stays empty) | `re/match/s08fu/00413f90.c` 100.0% 2/2, 6 B BYTE-IDENTICAL, relocheck OK checked=1 unchecked=0, sha256 `232cb7399a57`, exe-callers=0 |

Common fields (re-classify `match` table): tool `match.py --toolchain ddk --bytes <src>`, compiler `cl` 13.00.9176 /
`c2` 13.00.9178 (XP DDK), flags `/O2 /GS /Ob1` as C (x87 for the two rubber-band bodies), one body per RVA in both
targets, gates unchanged (cited above).

## Exe-callers wording (L-68)

- 0x00442c80 and 0x00442ce0: the exe calls the swapped-in bodies (A5 `VehicleWheelForceIntegrate` runs every racing
  frame on the default physics path, chain to `WinMain` in the S08-FU RESULT section 2). The basis is the direct-call
  graph plus A5 default ON, not a per-function trace, and the notes say so. The match route's caller clause holds, but
  the rows were already C4 by the frida route, so this is a second static witness and no promotion.
- 0x00413f90: exe-callers=0 (address taken by the AutoReg install table only). Under L-68 the match route gives no
  level, and the notes carry no `c4=match` token. The row's C4 is the pre-existing frida route (re-confirmed R1-B
  2026-06-09 in the same notes), which this row does not touch. The skill's demotion clause applies to a *match* C4
  with exe-callers=0, and this row is not one.

## L-89 (log/orch/DECISIONS.md, answered 2026-11-02: no)

A call made only by an env-gated replay harness is not an exe caller under L-68. 0x00442cc0 (`AiVehicleFloat4Get`)
and 0x00443300 (`FUN_00443300`) therefore stay **C3**. Their `hooks.csv` rows were not edited (nothing about their
evidence changed); each got one no-change line in `re/analysis/CHANGELOG.md` so the ruling is findable from the RVA.

## Tracker mechanics (re-classify transaction)

Transaction `RECLASS-NOTES-S08FU` (re-classify skill, no level change, nothing refused because nothing is promoted):

| rva | transaction | old -> new |
|---|---|---|
| 0x00442c80 | notes + file + exe_file update, match evidence as second witness | C4 -> C4 |
| 0x00442ce0 | notes + file + exe_file update, match evidence as second witness | C4 -> C4 |
| 0x00413f90 | notes + file update, `exe-callers=0`, no `c4=match` token (L-68) | C4 -> C4 |
| 0x00442cc0 | no change (L-89), CHANGELOG line only | C3 -> C3 |
| 0x00443300 | no change (L-89), CHANGELOG line only | C3 -> C3 |

The re-classify skill approved it (C4 gate for the three rows already met by the frida route, `match` fields all present
in the table above, L-68 callerless clause applied to 0x00413f90). It was applied mechanically by the `tracker-editor`
agent: `hooks.csv` cells and five CHANGELOG lines inserted immediately below `<!-- ENTRIES -->`. The cell text is the
content of the rejected commit's tracker diff (facts unchanged, only the route differs). STUBS.md, UNCERTAINTIES.md,
DEFERRED.md untouched (no new stub or uncertainty). No ad-hoc script ran by hand.

New tracker IDs: none. Open uncertainties still attached: U-9784 (0x00442c80 game-mode word), U-9787 (0x00442ce0 tier coverage), U-3468 (0x00413f90).

## Raw files

None cited beyond the S08-FU RESULT's section 8 table (kept in `D:/MashedWT/MATCH-S08-FU_data`).
