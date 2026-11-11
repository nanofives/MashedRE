# RESULT RWC-CENSUS: the 220 static-only RW-band functions, measured

Branch `orch/RWC-CENSUS` from orch/integration `778b8b91d`. Row kind: tracker and census (no `mashedmod/` edit, no
build, no gate). Input: `verify/rwsizing_20261111/RESULT_RW-SIZING.md`, "static-only part": 220 RW-band functions in
the static callee closure of executed RW code with no census row, 218 of them with no hooks.csv row
(`RW_FUNCS.tsv`, `phase=uncensused`, `static_below=y`).

## Verdict

**Measured.** Of the 220, **100 execute** in the canonical census scenario (18,404 B) and **120 do not** (18,094 B).
Every one of the 220 now has a hooks.csv row and a `re/coverage/reach.tsv` row.

| phase (majority of 3 runs) | functions |
|---|---|
| menu only | 27 |
| menu + race_load | 34 |
| menu + race_load + race_run | 28 |
| race_load only | 6 |
| race_load + race_run | 2 |
| race_run only | 3 |
| not run | 120 |

Executed by component (RW-SIZING's heuristic): rwcore 88, d3d9 5, rt2d 4, rwphysics 3. By band: upper 61, core 36,
physics 3. Not run: rwcore 105, rt2d 7, rwphysics 6, d3d9 2. Per row: `STATIC_SCORED.tsv`.

For L-124 (C): RW-SIZING's "220 functions (36,498 B), executed or not not measured" becomes **100 executed (18,404 B)**
on the matched-RW-core route, plus 120 static-only (18,094 B) that the canonical scenario does not reach.

## What was done

1. **C1 rows, 218** (`rwc_rows.py` -> `NEWROWS.tsv`, filed with the re-classify skill's `re/tools/c1_add.py`, tag
   RWC-CENSUS, one `(none)->C1` CHANGELOG line each). Commit `85a9084f8`.
   - C1 gate (`re/CONFIDENCE.md` C0->C1 (c), a caller chain to a known entry point): each row cites a BFS chain from an
     executed RW-band function (2026-10-11 census) down to it, over the same graph RW-SIZING used (direct E8 edges +
     address-taken reloc edges, `origtop_size.py`).
   - **All 218 are reached only through an address-taken edge** (`r`): none has a direct `call` from executed code.
     Depth 1: 198, depth 2: 20. That is why they had no rows: Ghidra created them after the census, as pointer
     targets (callbacks / plugin tables).
   - Subsystem by band: 210 `third-party-library[renderware]`, 8 `third-party-library[RenderWare-Physics-3.7]`
     (physics band `[0x00558c50, 0x0057c5b0)`). Names `FUN_<rva>`, status `mapped`, file empty.
   - The other 2 of the 220 already had rows: `0x004d1000` zcalloc (zlib, C4) and `0x00562ac0` (RW-Physics, C1).
     Both measure `not-run`.
2. **INT3 census refresh** (`re/frida/coverage_census.py --runs 3`, unchanged tool and scenario: boot -> menu + 20 s,
   warp to track 0 mode 10 with 4 slots, 45 s race hold). It now arms 6,378 entry VAs (6,427 hooks.csv rows) against
   5,888 on 2026-10-11. Private root `D:\MashedWT\RWC-CENSUS_root` (robocopy of `original\`), `MASHED_ROOT` set,
   `test-slot` outside and `mashed_lock.py run --label RWC-CENSUS` inside, PIDs spawned and killed by the tool only.
   `scripts/scoreboard.py` re-run on the new reach.tsv.

## Census run quality (read this before using reach.tsv)

Two attempts, both under machine load (the test-slot memory budget was exceeded for 10 min before each start, with
SimGolf runs and PROV-INSITU in parallel). The tool's own gate "run agreement <= 1%" is FAIL on both:

| attempt | menu | race (load or run) | race_load | race_run | gate |
|---|---|---|---|---|---|
| 1 (13:16) | 0.00% | 10.33% | 10.03% | 17.94% | FAIL |
| 2 (13:34), **committed** | 0.00% | 2.52% | 3.18% | 5.23% | FAIL |

The race hold is 45 s of wall clock, so a slowed game simulates less race: in attempt 1, runs 1 and 2 missed 254 and
138 of the October race set, run 3 missed 18. Attempt 2 is the committed reach.tsv. Its cross-check against the
2026-10-11 census on the 5,888 VAs both censuses arm:

- any phase: **3 lost, 1 gained**. The 3 lost are input functions `0x004957a0`, `0x00495830` JoypadStrcpy,
  `0x00495870` (the `fix_joypad` site), all 3/3 in October and 0/3 now: no controller is attached to the machine
  today. Gained: `0x00415860` InteractionCooldownSet (race 2/3). `0x00499820` moved from race to menu only.
- `reached_race` 2,301 -> 2,299. `reached_menu` 1,640 -> 1,637 (the 3 joypad rows).
- **The race_load / race_run split moved**: 689 functions that October put in race_load now show only in race_run
  (race_load 1,917 -> 1,228, race_run 1,273 -> 1,587). The README already calls the split a boundary effect of the
  100 ms phase-3 poll. Treat `reached_race` as the stable column and `reached_race_load` / `reached_race_run` as
  load-sensitive.

The 220 target rows are stable across both attempts: per-run executed counts 99 / 99 / 100 (attempt 1) and
99 / 100 / 100 (attempt 2), 1 verdict differs between the attempts, and 1 row (`0x004e4650`, race 2/3) is split
within attempt 2.

## Not covered

- The 4 RW-band functions RW-SIZING lists `uncensused` with `static_below=n` and no hooks.csv row have no C1 caller
  chain from executed code, so they got no row and no census site.
- R=0 means "not executed in this scenario" (menu idle + a parked-player 45 s race), not dead code. The 120 not-run
  rows cover frontend screens other than the main menu, the player driving, race end and every other track and mode
  only as far as this scenario does.

## Checks

- `py -3.12 -m pytest re/tools/tests/test_c1_add.py`: 6 passed.
- hooks.csv: 6,427 data rows, no duplicate RVA, 11 fields on every non-comment row (the 31 short rows are the
  pre-existing `#` comment lines, as on orch/integration).
- `rwc_rows.py --score`: 220 of 220 rows have a reach.tsv row.

## Files

| file | what |
|---|---|
| `rwc_rows.py` | NEWROWS.tsv (caller chains, C1 evidence) and `--score` (STATIC_SCORED.tsv) |
| `NEWROWS.tsv` | the 218 new rows: subsystem, band, component, size, root, root phase, depth, chain |
| `STATIC_SCORED.tsv` | the 220 static-only rows with their measured phase and run counts |
| `re/coverage/reach.tsv`, `re/coverage/scoreboard.tsv` | refreshed (attempt 2) |

Raw data (not committed), `D:\MashedWT\RWC-CENSUS_data\`, SHA-256:

```
c917d4194abea726f69e81c7dbf2ee1debcf64179b447b3afb13d8dc86709b2b  census_20261111_131604_run1.json
59a731855ebb357745d052c22a6fae1b4f056b7d87aaee44d1108fe0e3f8338d  census_20261111_131604_run2.json
42a3ad548c3eac01725ea7f9606f44af3b04e275e3f5e7ef0f13b1bfd49bf2fb  census_20261111_131604_run3.json
d9b9dd07828ab75708726a5a4b04f01189690d1366aeb37b3f61e89d8b8e3be4  census_20261111_133407_run1.json
7f51c4d74dff32d7baca25a77f296a86cc20f6c7da4e701e9eccd7ee17e8897f  census_20261111_133407_run2.json
58d6ce34878ac7ccafda126ef9d0bcc01092a1fa7f6bde6c5cd167bc7d71f7aa  census_20261111_133407_run3.json
ebf79b254d5d20f5146b06847f714569fe1c84b70b2c560a8f59a818c4aaf6d4  census_run.log    (attempt 1)
dfac983abfa500814c5b1047c571f7a2fac71bbaf53a27dc644b7facf22de28c  census_run2.log   (attempt 2)
0c3585fe1f0312af95295d8c01737d8f17730051477ebf9a9ae6db6b7b3c6013  reach_attempt1.tsv
```

## Next

- Feed `STATIC_SCORED.tsv` into the RW core work list: the 100 executed rows join the 668 on the (C) route. All 218
  new rows are C1 with no match attempt.
- A census re-run on a quiet machine (no concurrent test slots) should bring the race gate back under 1%. A
  frame-count race hold in `coverage_census.py` would remove the load dependence. That needs a cited frame counter
  address, which this row did not establish.
