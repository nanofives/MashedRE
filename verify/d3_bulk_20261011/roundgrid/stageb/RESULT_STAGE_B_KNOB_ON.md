# Round-restart grid Stage B, knob ON: RESULT (2026-10-11), per PREREG_STAGE_B.md

Verdict: **all three registered gates pass.**
- With `MASHED_ROUND_GRID_ORDER` the exe now reorders the cars on each restart grid from the previous round's score
  deltas, exactly as the original's FUN_00408b00 assignment does (oracle-checked 1296/1296).
- No default change (user decision).

## Arms

| arm | step | gates | ptrace |
|---|---|---|---|
| RG_ON (Stage A, base) | c374f936 | 3caa0b26 | d92d7b56 |
| RG_B | 87571cc3 | 28c60543 | 8e8c24a6 |
| RG_B2 | 87571cc3 | 28c60543 | 8e8c24a6 (identical; trace identical) |

## G-RG-B-TRACE (`verify/d3_gatefire_20261008/RG_B.roundgrid.txt`)

Each row is a StartRound; the codes are the previous round's score deltas.

| round | codes | slots |
|---|---|---|
| 1 | -1000 ×4 | 4,5,6,7 (identity, = Stage A) |
| 2 | -2,1,-1,2 | 0,2,1,3 |
| 3 | -2,-1,1,2 | 0,1,2,3 |
| 4 | -1,-2,2,1 | 1,0,3,2 |

Every row equals `RoundGrid_AssignMode4` and the original-bytes oracle (`slotorder_m4.tsv`).

Round 1's end is the same as RG_ON's (survivor car 3, progress 21.3067, node 21). The rounds then diverge, as
predicted (RG_ON round 2 ends at 10.2311 against RG_B's 10.3615).

## G-RG-B-DET

RG_B == RG_B2.

## G-RG-B-LOCAL

- The player ptrace is identical to RG_ON through round 1 and through the round-2 restart at sim step 1555. The
  player is at (8.0, 25.52) in both, because it keeps slot 0 (code -2 → 0).
- The first player difference is at sim step 1603, 48 steps into round 2. That is after the first non-identity
  round-start (slots 0,2,1,3: AI cars 1 and 2 swapped), not before it.

## Report only

**(b) per regime:** RG_ON and RG_B are identical, 1/3 (v3 PASS; v1 and v2 abs_steer only). The window is in
round 1, as predicted.

**Collateral:** not run separately. The only difference starts at the round-2 restart, which is outside every
D3 gate window. The D2 watch list is unaffected in round 1 (identical ptrace and step until step 1555).

## State of the round-restart grid

The restart node (Stage A) and the slot order (Stage B) are both ported and gated:
- `MASHED_ROUND_GRID` gives the node.
- `MASHED_ROUND_GRID_ORDER` gives the order.
- Both are default OFF and need `MASHED_PATHPROG`.

**Open:**
- The tracker row (re-classify).
- The user's call on default-ON. A flip also fixes the exe's round-1 grid offset, up to 0.0236.
- Modes 1..3 and the rule overrides are not ported (Training runs mode 4, rule 0).
