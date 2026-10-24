# PREREG D3-CF-RFLAME, amendment 1

Committed alone, after arms `G3_OFF` .. `F_ON_NB` ran and before the arms below. Code: `a412188aa`.

## Why

On `g3f` the R_FLAME query count is 511 / 511 with the knob both unset and set, so P2 is
non-discriminating as written. `mixed_calls.py` shows 24 calls on `g3f` (23 on `g3`) where
`0x45afcc` returned a hit and a miss on the same call, so the scan order does decide which
spark takes the hit there. An equal count does not show the same spark landed. The new
STATE CHECK in `pu_replay` compares the port's pool after each call's tick with the
original's tick-exit dump (`L` rows) of the same call:

- slot order: alive, landed, age, pos, vel bit-equal in every slot;
- identity: each original record alive is found in the port's group by its age bits, then its
  landed latch and position bits are compared.

## Arms (rerun of the `g3f` arms with the new build; `G3_*` have no `.puflame.csv` and print no check)

`F_OFF`, `F_ON`, `F_ON_NB` as in PREREG section 4, then `SW_ON` and `GATES`.

## Added criterion

- **P5 (state).** `F_ON`: STATE CHECK slot order CLEAN and landing CLEAN. Reported beside it:
  `F_OFF` (no sort, real geometry) and `F_ON_NB` (sort on stand-in geometry). If `F_OFF` is
  also CLEAN on both, P5 is recorded as non-discriminating.

The verdict rule of PREREG section 5 gains P5 as a required condition when it is
discriminating. Nothing else changes.
