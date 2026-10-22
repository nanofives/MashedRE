# RESULT rpct: race_pct readers load the slot 0x008a96ec (0x00408ad0) (2026-10-22)

Branch `work/rpct` from `492c4c8c1`. PREREG + survey + pre-gate `eb0be72c9`, knob `b645c64e2`. Exe `140d149e`. Private roots
`D:\MashedWT\rpct_root` / `rpct_root13`; evidence `D:\MashedWT\rpct_data` (`MOVED_OFF_GIT.txt`).

**Verdict: the knob is value-faithful and the default is unchanged with it OFF; ON changes one elimination victim, in round 4
of the G-CTRL race, where no original reference exists.** By the PREREG decision rule (section 5, second case): reported, no
flip recommendation until an original capture covers that round. Knob stays default OFF.

## Predictions

| id | result |
|---|---|
| P0 knob OFF | **held.** G-CTRL `c3bfcef3 / 4ed4849b / 4c3548a1` on `H_DEF-2`, `H_DEF-3`, `H_ET-1`; B `4985eb3b / b6e3d073 / 6717c15e`; (b) 9/9 (`vsum_b_DEF.txt`); (e) PASS 3/3, launch 1425.7 / 2052.5 / 2055.0 (`e_check_DEF.txt`); D2 PASS 3/3 (`score_d2_DEF.txt`); 13 tracks identical to dcmix (`tracks_def_score.txt`). `H_DEF-1` lost the dump rows of one frame (5759: 2 step rows, 4 gates rows missing, every other row identical, ptrace `4c3548a1`): an output dropout, not a sim change; the rerun `H_DEF-3` is the reference |
| P1 wiring | **held.** `H_RSET-1`: `pct<i> == o_pct<i>` on all 13498 trace rows x 4 cars |
| P2 step dump | reported: knob ON moves only `racepct` until frame 3651 (first other column at row 8055, frame 3651 = the round-4 split below). B arms (no elimination): only `racepct` moves, every row (K0 / K1 / K26: 2035 / 2988 / 2335 rows) |
| P3 victims | round 1 **held** against the original (car 0 at row 221 both ways; original `o_ep_1` car 0 at frame 1053, the same kill per u9779's alignment). Rounds 1-3: identical (car, row) OFF vs ON. **Round 4 differs** (below) |
| P4 | **held.** (b) 9/9 (`vsum_b_RS.txt`), (e) PASS 3/3 same launch (`e_check_RS.txt`), D2 PASS 3/3 (`score_d2_RS.txt`), 13 tracks identical (`tracks_rs_score.txt`) |

Knob-ON hashes: `H_RS-1` = `H_RSET-1` = `bacfc747 / 8dd1c004 / 6f9932b6`; B `f9deb9b9 / d3601fcc / 9da9a26a`.

## The round-4 split (`victims_H.txt`)

At row 3651 (round 4, zoom 10) the stand-in and the slot disagree by ~86 points on two cars:

| car | stand-in `pct` (gate count) | slot `o_pct` (FUN_00408610's value) |
|---|---|---|
| 1 | 3.333 | 89.065 |
| 3 | 2.898 | 99.509 |

The stand-in has cars 1 and 3 already past the line, the slot has them short of it. OFF kills car 3, ON kills car 1. From there
the match differs: OFF eliminates car0 / car2 at 3855 / 3856 and plays 6 rounds; ON eliminates car2 / car0 at 3654 / 3965 and plays
5 rounds. The slot is the original's quantity (u9779: bit-equal over round 1), so ON follows the original's law on the original's
input, but rounds 2+ of this recipe have no original counterpart (the original's round ends at k 220, u9779), so the round-4
victim is not scored.

## Correction

`RESULT_dcmix.md` section 6.1 says the default bridge writes the slot. On the default build `Core/FaithDefaults.cpp` seeds
`MASHED_PATHPROG` / `MASHED_PATHPROG_LATE`, so the bridge and arm R stand down and the verbatim `FUN_00408610` step writes it
(`survey.txt` section 2).

## Lint

11 findings, 0 NEW: `0x00408ad0` stays allowlisted while the stand-in is the default (`RacePct` keeps its `// 0x00408ad0` anchor).

## Decision (Mariano, 2026-10-22): flipped default ON on P1, merged. REFERENCES REBASED

Mariano accepted P1 (the readers now load the original's quantity into the original's law) and the unscored round-4 victim
change. `RacePctSlotEnabled()` is true unless `MASHED_NO_RACEPCT_SLOT` is set (`74f895883`; `MASHED_RACEPCT_SLOT` is no longer
read). An original multi-round reference was not captured: in this recipe the original freezes after its first round end
(memory `o-rg4-recipe-round-ends-early`) and round-level outcomes do not repeat between original runs
(`verify/d3_consumer_20261007/RESULT_E1.md` section 5), so it could not score round 4.

Gates on the flipped exe `fec9f5c4`:

| gate | flipped exe | vs the pre-flip default |
|---|---|---|
| **G-CTRL x2 (new default)** | **`bacfc747 / 8dd1c004 / 6f9932b6`** x2 | REBASED (was `c3bfcef3 / 4ed4849b / 4c3548a1`) |
| opt-out `H_NRS-1` | `c3bfcef3 / 4ed4849b / 4c3548a1` | = the pre-flip default |
| **B K0 / K1 / K26 (new default)** | **`f9deb9b9 / d3601fcc / 9da9a26a`** | REBASED (`racepct` column only) |
| (b) | 9/9 (`vsum_b_flip.txt`) | unchanged |
| (e) | PASS 3/3, launch 1425.7 / 2052.5 / 2055.0 (`e_check_flip.txt`) | unchanged |
| D2, 3 runs alone | PASS 3/3, slip 0.1900 / 0.2550 x3, drvmed 1964.25 x3, H1 398/400 (`score_d2_flip.txt`) | unchanged |
| 13 tracks | table identical to dcmix, gates PASS (`tracks_flip_score.txt`) | unchanged |

## Decisions for Mariano (before the flip, kept for the record)

1. Leave `MASHED_RACEPCT_SLOT` default OFF and merge the knob (and the oracle-free tooling `pregate.py` / `victims.py`)?
2. Or capture an original multi-round reference for this recipe (a longer `--alive-probe --elim-probe` hold, if the original's
   match continues past round 1 under the port's round knobs) and score round 4 before deciding a flip.
3. Or flip now on the strength of P1 (the value is the original's) and accept that rounds 4+ change unscored.
