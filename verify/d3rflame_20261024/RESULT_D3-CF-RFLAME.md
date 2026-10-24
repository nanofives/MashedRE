# RESULT D3-CF-RFLAME: the R_FLAME sort FUN_0045ac40, ported and measured

**Verdict: GREEN.** The ported sort reproduces the original's post-sort slot order on 295 of 295
tested groups, and with it on, the port's whole spark pool equals the original's bit for bit
after every call (315 of 315 groups). The g3 2-query residue itself cannot be re-scored on g3:
that capture holds no spark geometry. The new capture g3f gives 511 / 511 with the knob off as
well, so the count alone does not separate the two. The STATE CHECK does: with the knob off,
4 sparks land that do not land in the original and 74 positions differ, at the same 511 count.
Knob default OFF, default gates unchanged. No flip proposed here (that is a decision row).

PREREG: `PREREG_D3-CF-RFLAME.md` (`c4200c5d6`) and amendment 1 (`b75fd47e8`), each committed
alone before the arms it governs. Branch `orch/D3-CF-RFLAME`.

## 1. FUN_0045ac40, read off the instructions

Read-only pool clone (`Mashed_pool10`) of the anchor. Body `0x0045ac40..0x0045ad69`, 297 B.

- Args: EAX = owner (`MOV EAX,[ESP+0x20]` `0x0045af3b`, the tick's `local_c8`), ECX = group
  (`MOV ECX,EDI` `0x0045af3f`, `local_c0`), one stack arg = the reference pointer
  (`[ESP+0x54]` `0x0045ac92`). The reference is `FUN_004671d0(0)` (CALL `0x0045af35`,
  PUSH EAX `0x0045af3a`), which returns `*(DAT_006905b0 + 4) + 0x40` when `FUN_0042b930() != 3`.
- Group base `0x0068bd00 + (owner*5 + group)*0x104` (`0x0045ac43..0x0045ac48`).
- For i = 0..4: skip unless the dword at +0x30 is nonzero (integer TEST `0x0045ac73`) and
  ageN +0x18 < `_DAT_005cc320` = 1.0 (`0x0045ac7e..89`). Key = (dy*dy + dx*dx) + dz*dz with
  d = ref - pos (`0x0045ac96..0x0045acb6`), stored to a float temp once (`FSTP [ESP+0x18]`
  `0x0045acb8`).
- For j = i+1..4, same live / young test, then `FCOMP [ESP+0x18]` / `TEST AH,0x41` / `JNZ`
  (`0x0045acff..0x0045ad0e`): swap the two whole 0xd-dword records (three `MOVSD.REP`,
  `0x0045ad10..0x0045ad33`) only when d_j > key strictly.
- The key is **not** recomputed after a swap. Every later j compares against the record that
  used to sit at i. The fresh-key control below misses 6 groups, so this matters on real data.
- Direction: farther records move earlier. The plate `re/analysis/bucket_gameplay_00458a40_0045ac40/0x0045ac40.md`
  says "bubble nearer records earlier". The instructions say the opposite, and the
  nearest-first control misses 295 of 295.

## 2. What changed (all default OFF or replay-only)

| commit | change |
|---|---|
| `55138534c` | `RFlame_SortFive` in `Powerup/PowerupEffects.cpp`. `RFlame_Tick` runs it once per group before the 5-spark scan only under `MASHED_RFLAME_SORT=1` with a reference. `IPowerupBackend::FlameSortRef` / `FlameSparkBirth` (non-pure, default false). `TrackRenderer` gives the last rendered eye as the reference. `scenario_launch.py --puhook-flame`. `pu_replay` births + reference injection and the SORT CHECK. `pu_replay/build.bat` links `PowerupTypeSearch.cpp` (the replay had stopped linking since `0x0045baa0` moved there). |
| `a412188aa` | `pu_replay` STATE CHECK, read-only accessor `RFlame_Pool()` |
| `71634502c` | STATE CHECK identity fix (section 4.3) |

`FlameSpark` moved to `PowerupEffects.h` and gained the emitter's angle (dword 10), which the
port never reads.

## 3. Capture g3f

The g3 argv (`verify/d3_contact_20260928/g3.msd.provenance.json`) plus `--puhook-flame`,
private root `D:\MashedWT\D3-CF-RFLAME_root`, under `mashed_lock.py`. Log
`log/d3rflame_g3f_capture.txt`. 6622 frames. Agent: stepper calls 57, ticks 6647, births 25,
`refMoved` 0 (the reference read on tick entry equals the one on tick exit on every tick it was
recorded), 760 rows. Committed: `g3f.msd.puhook.csv`, `.pucontact.csv`, `.puflame.csv`,
`.provenance.json`. Not committed: the 22 MB `g3f.msd` (no replay reads it, as with d3close).

## 4. Arms

`run_replay_arms.ps1`, outputs in `arms/`.

### 4.1 Query counts (P2, P3)

| arm | input | knob | RFLAME query `0x45afcc` orig / port | lerp | basis |
|---|---|---|---|---|---|
| `G3_OFF` | g3 | unset | 546 / 548 DIVERGES | 23 / 23 | 23 / 23 |
| `G3_ON` | g3 | 1 | 546 / 548 DIVERGES | 23 / 23 | 23 / 23 |
| `F_OFF` | g3f | unset | 511 / 511 clean | clean | clean |
| `F_ON` | g3f | 1 | 511 / 511 clean | clean | clean |
| `F_ON_NB` | g3f | 1, no births | 511 / 511 clean | clean | clean |

`G3_ON` = `G3_OFF` as PREREG section 2 predicts: g3 has no `.puflame.csv`, so no reference and
no sort. P2 is non-discriminating on g3f (F_OFF is clean too).

`mixed_calls.py`: calls where `0x45afcc` returned a hit and a miss on one call: g3 23, g3f 24,
c3 23, g4 23. On those calls the scan order picks which spark lands.

### 4.2 SORT CHECK (P1), identical in all three g3f arms

Ported sort on the original's tick-entry records with that tick's births overlaid, against the
original's tick-exit slot order, identity = dword 10.

| groups | trivial (<2 young) | tested | moved by the original | port | no-sort | fresh-key | nearest-first |
|---|---|---|---|---|---|---|---|
| 315 | 20 | 295 | 49 | **0** | 49 | 6 | 295 |

**P1 holds.**

### 4.3 STATE CHECK (P5, amendment 1)

Port pool after each call's tick vs the original's tick-exit dump of the same call.

| arm | groups | slot-exact mismatches | records by unique age | skipped (shared age) | landed mismatches | position mismatches |
|---|---|---|---|---|---|---|
| `F_OFF` | 315 | 128 | 1360 | 115 | **4** | 74 |
| `F_ON` | 315 | **0** | 1360 | 115 | **0** | **0** |
| `F_ON_NB` | 315 | 315 | 1360 | 115 | 4 | 1360 |

**P5 holds and is discriminating.** With the original's births and reference, the sort on
gives a pool that is bit-equal to the original's in every slot after every call. Without the
sort, 4 sparks land that do not land in the original, at the same 511 query count. Without the
births (stand-in geometry), the sort orders the wrong positions and lands 4 wrong sparks too.

Tool correction, recorded: run 1 of this check matched records by the first equal age. Aged-out
sparks all freeze at the same age under a constant dt, so that paired wrong records. Run 1
printed, for `F_OFF` / `F_ON` / `F_ON_NB`: landed 5 / 4 / 8, position 154 / 80 / 1475, while
`F_ON`'s slot-exact count was already 0 of 315 (a contradiction). `71634502c` skips records
whose age is shared in the group (115). The slot-exact column did not change between runs.

### 4.4 SW_ON (P3)

`sweep.ps1` with `MASHED_RFLAME_SORT=1` vs unset, the 12 captures: replay text identical
(0 differing lines once the knob echo line is removed) and `port.csv` hashes equal on all 12.
Decision 12 / 12 CLEAN, contact CLEAN except g3 (546 / 548), the d3close state. **P3 holds.**

### 4.5 GATES (P4)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe `9918b987`
(this branch, knob unset), root `D:\MashedWT\D3-CF-RFLAME_root`, log `log/d3rflame_gates.txt`.

| gate | arm | sha8 | expected | |
|---|---|---|---|---|
| G-CTRL | `H_DEF` step / gates / ptrace | `bacfc747 / 8dd1c004 / 6f9932b6` | `bacfc747 / 8dd1c004 / 6f9932b6` | equal |
| B | `B_DEF_K0 / K1 / K26` step | `f9deb9b9 / d3601fcc / 9da9a26a` | `f9deb9b9 / d3601fcc / 9da9a26a` | equal |

All four runs exit 0, self-exited. **P4 holds.**

## 5. What this does and does not close

- Closes: the mechanism named in `D3_CONTACT_PORT_2026-09-28.md` §4.2d is ported and verified
  on the original's own records (P1), and given the original's geometry it makes the port's
  R_FLAME pool exact (P5).
- Does not close: the g3 count itself. g3 lacks the geometry, and g3f did not reproduce the
  2-query tail with the knob off. The residue is explained and its fix verified, but no
  capture shows 546 / 548 turning into 546 / 546.
- Live standalone: the sort needs real spark positions. The port's emitter still uses the
  stand-in (no jitter, normalised forward for the `at` row), so in the live exe the knob sorts
  stand-in positions against the last rendered eye. [UNCERTAIN] whether the original's
  reference at tick time is this frame's camera or the previous render's: no capture compares
  them.
- Flip: not proposed here. `MASHED_RFLAME_SORT` default ON would be a decision row. It changes
  no gate (P3, P4) and only matters with the original's spark geometry, which the standalone
  does not have yet.
- Tracker: `FUN_0045ac40` has no hooks.csv promotion in this step. The SORT CHECK is a replay
  measurement on captured state, not a Frida A/B of the hook. Any C-level move goes through
  `re-classify`.
- Plate correction owed: the 0x0045ac40 plate's "nearer records earlier" is wrong (section 1).
