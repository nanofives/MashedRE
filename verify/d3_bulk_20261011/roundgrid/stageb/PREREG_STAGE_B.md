# PREREG round-restart grid Stage B (per-round slot order), knob ON (written 2026-10-11, before any knob-ON run)

## Knob and arms

**Knob:** `MASHED_ROUND_GRID_ORDER` (default OFF). It requires `MASHED_ROUND_GRID` (Stage A) and `MASHED_PATHPROG`.

**Arms** (`verify/d3_gatefire_20261008/run_h4.ps1`):

| arm | knobs | role |
|---|---|---|
| `RG_ON` | Stage A | the base, `c374f936 / 3caa0b26 / d92d7b56` |
| `RG_B` | Stage A + `MASHED_ROUND_GRID_ORDER` + trace | the knob |
| `RG_B2` | same as `RG_B` | repeat |

## Prediction

1. **Trace:** one `round-order` line per StartRound.
   - Round 1: `codes=-1000,-1000,-1000,-1000 slots=4,5,6,7`, so the identity formation, the same as RG_ON.
   - Later rounds: codes = the previous round's `score_delta_` (values in {2, 1, -1, -2}); slots =
     `RoundGrid_AssignMode4(codes)` (verified 1296/1296 against the original bytes).
2. **Hashes:**
   - RG_B == RG_B2.
   - RG_B equals RG_ON through round 1. The first difference is at the first round-2+ StartRound whose slots are
     not identity (0,1,2,3 / 4,5,6,7).
   - If every round's slots happen to be identity, RG_B == RG_ON over the whole run. That is allowed, and the
     trace shows it.
3. **The `round-end` codes** are each round's per-car awards from the exe's elimination order. They are not
   compared to the original: round outcomes do not reproduce.

## Gates

**G-RG-B-TRACE:** item 1.

**G-RG-B-DET:** RG_B == RG_B2.

**G-RG-B-LOCAL:** no difference from RG_ON before the first non-identity round-start. Check with the ptrace /
step first-difference frame against that round's start frame.

**Report only:** (b) per regime (unchanged expected: the 220-step window is in round 1). Collateral against
RG_ON, with the D2 watch list.

**No default change.** That is a user decision.

## Evidence already in hand (knob OFF)

- G-RG-ORDER: 1296/1296 exact against the original-bytes oracle (`order_oracle.txt`); negative control 3/3
  (`order_negctl.txt`).
- The oracle reproduces the live original's per-round order 7/7 (`o_grid1.msd.grid.tsv`, codes read before
  each placement).
