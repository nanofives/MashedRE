# PREREG D3-CF-BOX amendment 1

Written after O1, V1 and the first arms (`033891cd6`, exe `31381b32`), before any arm below runs. Committed alone.

## 1. What the first runs showed

- P1 held: `H_DEF` `bacfc747 / 8dd1c004 / 6f9932b6`, `B_DEF_K0 / K1 / K26` `f9deb9b9 / d3601fcc / 9da9a26a`.
- O1 (`o1.msd.elim.csv`, 27 rows): race init = 4 writes of 1 from `0x0040ffa1` (`DAT_0063ba8c` = 1), then **two** blocks of
  4 writes of 1 from `0x0040eeb3` (FUN_0040e590), one at `DAT_0063ba8c` = 2 and one at 3. At frame 1084 (sub-state 6):
  `kill(0)`, the 2 write `0x0042300a` slot 0, `resolve3`, `kill(1)` + 2, `kill(2)` + 2, then sub-state 7. No `box3` row:
  the dispatcher pass needs sub-state 6 (`0x0045bc2b`). The capture ends in sub-state 7. FUN_00410d10 ran 220 times, so
  no second round, and no grid write after the kills, exists in O1.
- V1 (the d3close `b3r` argv trimmed to `--statediff-puhook`, plus `--elim-probe --box-probe`): `box3` fires (slot 0,
  frame 1074, the forced state), so the `0x0045bc85` hook is live. At frame 1082 `kill(1)` alone, the 2 write, and `box3`
  slot 1 in the same frame with `[EDI+0x18]` = `0x5f9b58`.
- P3 as written DIFFERS at index 8: the original's second FUN_0040e590 block against the port's first kill.

## 2. Changes

- Port, knob ON only: the race init writes the FUN_0040e590 block twice, as O1 shows (`0x0040eeb3` at `DAT_0063ba8c` 2
  and 3). The values are the same (1 over 1). Rebuild, knob OFF path untouched.
- P3 is re-stated as **P3a**: the span is the original capture's own rows (O1, and the repeat O2), from the first row to
  the last. The port arm `B_BOXR_K0`'s trace, cut to the same number of rows, must equal it as (class, slot, value)
  rows. The P3 rule that the span runs to the first grid after the kills cannot be scored: the original never reaches it
  in this recipe.
- P1 is re-run on the rebuilt exe (the knob OFF path must still give the six hashes).

## 3. Arms

- `O2`: O1's argv, a repeat (reproducibility of the O1 sequence).
- `H_DEF`, `B_DEF_K0`, `B_DEF_K1`, `B_DEF_K26`, `B_BOXR_K0`, `B_BOX_K0`, `H_BOX` on the rebuilt exe.
- `B_ER_K0`: `MASHED_ELIM_RESOLVE` + `MASHED_ROUND_END7` without the box knob, the control for `B_BOXR_K0`'s step hash.

## 4. Pass rules

- P1: as before.
- P3a: O1 = O2 as (class, slot, value) rows, and the port's cut sequence equals them. Holds -> DECISION, knob stays OFF.
- Report only: `B_BOXR_K0` against `B_ER_K0` step hashes (does the box knob move the u9779 chain), V1's same-frame latch
  against the port's next-dispatcher-call latch.
