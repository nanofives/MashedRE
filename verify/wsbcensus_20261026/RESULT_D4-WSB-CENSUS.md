# RESULT D4-WSB-CENSUS: RenderWare-Physics executed-set census

Verdict: **GREEN**. All 283 sub-C4 rows of `third-party-library[RenderWare-Physics-3.7]` are
classified, **181 EXEC / 102 NOT-EXEC**, unanimous across 3 runs (no row is 1/3 or 2/3).

Branch `orch/D4-WSB-CENSUS` off `orch/integration` @ `e11287674`. hooks.csv @ `271e57a53`
(5,913 rows, 288 in the subsystem, 5 C4, 283 below C4). No tracker, default or source change.

## What was measured

`re/frida/coverage_census.py --runs 3` (unchanged tool, method in `re/coverage/README.md`): one INT3
per hooks.csv entry VA (5,900 VAs), first hit recorded, phases `menu` / `race_load` / `race_run`.
Scenario: boot to main menu, 20 s settle, warp to track 0, mode 10, 4 slots (slot 0 parked, 1..3 AI),
45 s race hold. Run under `mashed_lock.py run --label D4-WSB-CENSUS` (it queued behind D4-WSJ-WIRE,
then held the machine lock), `MASHED_ROOT=D:\MashedWT\D4-WSB-CENSUS_root` (robocopy of `original\`,
`MASHED.exe.unpatched` SHA-256 = anchor `BDCAE093...3C0E`). PIDs spawned and killed by PID: 15388,
27932, 13020. Run agreement: menu 0.00%, race 0.00% (gate <= 1%: PASS). 0 clobbered sites,
0 write errors, no crash.

**Scope of the evidence.** The census instruments `MASHED.exe` (with `MASHED_RE_NO_AUTO_HOOK=1`), so
EXEC means "the original executes this RVA in the canonical scenario". That is the set the default
build has to execute faithfully under S-DoD criteria 2-3. The default build was built from this
worktree (`mashed_re.exe` SHA-256 `c18e87ea...d820`, `mashed_re_dev.asi` `888e7b4a...914d`,
`MASHED_NO_DEPLOY=1`, Build OK) but was not instrumented: there is no coverage shim for the
standalone, and the census tool cannot drive it. Whether each ported copy is the one the standalone
calls is a separate question (dual-copy / TU linking), not answered here.

NOT-EXEC means "not reached in this scenario", not dead. Unexercised by construction: the player
driving, tracks other than 0, modes other than 10, race end, pause, frontend screens past the main menu.

## Counts

| class | EXEC | NOT-EXEC | total |
|---|---|---|---|
| C2/mapped | 78 | 69 | 147 |
| C2/new | 13 | 1 | 14 |
| C2/ported | 20 | 29 | 49 |
| C3/impl | 69 | 3 | 72 |
| C3/ported | 1 | 0 | 1 |
| **total** | **181** | **102** | **283** |

| band | EXEC | NOT-EXEC | C3 EXEC / C3 total |
|---|---|---|---|
| BROAD `0x00558000-0x0055efff` | 48 | 29 | 9 / 11 |
| BVH `0x0055f000-0x00565fff` | 48 | 23 | 22 / 22 |
| SOLV `0x00566000-0x0056ffff` | 50 | 24 | 29 / 30 |
| NARROW `0x00570000-0x0057bfff` | 26 | 24 | 10 / 10 |
| TAIL `0x0057c000-0x0057cfff` | 9 | 2 | 0 / 0 |

Phase of first hit among the 181 EXEC rows: 96 race_run only, 57 race_load only, 25 race_load and
race_run, 2 menu only, 1 all three phases.

## What this unblocks

- **D4-WSB-C4-BROAD**: 9 executed C3 candidates (8 impl + 1 ported). `0x0055bae0`, `0x0055bb70`
  are NOT-EXEC: no canonical-scenario C4 path for them in this scenario.
- **D4-WSB-C4-BVH**: all 22 C3 rows EXEC.
- **D4-WSB-C4-SOLV**: 29 of 30 EXEC. `0x005685f0` is NOT-EXEC.
- **D4-WSB-C4-NARROW**: all 10 C3 rows EXEC.
- **D4-WSB-C3-PORTED**: 20 of 49 C2/ported rows EXEC (do these first), 29 NOT-EXEC.
- **D4-WSB-MAPPED-DISP**: 161 rows split into 91 EXEC (78 mapped + 13 new, need port steps) and
  70 NOT-EXEC (69 mapped + 1 new, candidates for explicit dispatch or a DEFERRED row via re-classify,
  after a wider scenario check: the census is one track, one mode, no player input).
- **D4-WSB-RAND-D11064**: `0x00564310` (C2/ported, the `_rand()` caller in D-11064) is EXEC in
  race_load and race_run, 3/3. The rebind is on the executed path.

No D4-WSB-* row is emptied. The three NOT-EXEC C3 rows are the only C4-lane rows this scenario cannot
serve.

## Files

Committed (summaries):
- `verify/wsbcensus_20261026/wsb_census.tsv`: 283 rows, `rva, band, confidence, status, census,
  phases, runs_menu, runs_race, name` (SHA-256 `38587d65...9585`). This is the EXEC / NOT-EXEC list.
- `verify/wsbcensus_20261026/wsb_join.py`: `py -3.12 -I wsb_join.py hooks.csv <reach.tsv> <out.tsv>`
  reproduces it byte for byte.

Raw (not committed), `D:\MashedWT\D4-WSB-CENSUS_data\`:

| file | SHA-256 |
|---|---|
| `reach.tsv` (all 5,913 rows) | `34bc0f22e6158940c35f6b7594b26d96e3eaa4ec2d67a21aeb3d8a9b44a8284b` |
| `census_20261026_122209_run1.json` | `12f1f3b77955439613e70ffc38324051472aee0f75581688cd0b25673fdec40e` |
| `census_20261026_122209_run2.json` | `1a45bee8194a5a49145affc6cd1868c22fe1fc9871697a7c56b413d47fcf1d13` |
| `census_20261026_122209_run3.json` | `fb0c9ede5464eba90ab2130957251e8dfc64381d7fa29cc9629266cf98711e78` |
| `census_transcript.txt` | `b1e6a239d3cddba17dbe52e7c712661580468db0b3db911c7e0f341766df3cfe` |

`re/coverage/reach.tsv` (the 2026-10-11 census) was not overwritten.

Gates: not run. No behaviour change on this branch (no source edit), so G-CTRL / B are unchanged
by construction.
