# PREREG round-restart grid Stage A, knob ON (written 2026-10-11, before any knob-ON race run)

## Arms (`verify/d3_gatefire_20261008/run_h4.ps1`)

| arm | knobs |
|---|---|
| `RG_CTRL` | none (done: `ae67b13a / c8317ce2 / 31db9384`) |
| `RG_PP` | `MASHED_PATHPROG` (base; expected = AL_PP `4e250237 / 22cceec1 / dc72ab09`, same build lineage) |
| `RG_ON` | PATHPROG + `MASHED_ROUND_GRID` + trace |
| `RG_ON2` | same as `RG_ON` (repeat, collateral floor) |

## Prediction

1. **Trace:**
   - `race-init node=0`;
   - one `round-end` line per continuing round, node = trunc(progress of the last alive car);
   - one `round-start` line per round with the snapped node and its anchor.
   - Round 1 starts at node 0. Node 0's formation differs from the gate-based one by 0.0035 / 0.0079 /
     0.0157 / 0.0236 (slots 0..3, `grid_check_synth.txt`). Node 0 matches the original's round-1 grid at 0.0001.
2. **Hashes:** RG_ON ≠ RG_PP, and RG_ON == RG_ON2 (deterministic).
3. **Where the first divergence lands:** at the first `round-start` after round 1, or at round 1's own start,
   because node 0's formation differs from the gate formation by up to 0.0236 in x/z.
   - Collateral (`--mode paired`, A = RG_PP, B = RG_ON, floor-B = RG_ON2): the first divergent frame is reported.
   - Whether it is round 1's start or round 2's is the measurement; both are allowed by this prediction.
   - Any divergence BEFORE the first `StartRound` is a FAIL.
4. **(b) per regime:** reported for RG_PP and RG_ON. Unchanged if the first divergence is after the 220-step
   window. Otherwise report the difference.
5. **Round-2 grid:** the trace's round-2 anchor is not the start-line gate (unless the snap returns 0).
6. **D2 watch list:** `rec_4a4`, the contact collector, substep 0x00470c70. Any collateral hit is a D2 REOPEN
   CANDIDATE row.

## Gates

**G-RG-ON-TRACE:** items 1 and 5 hold.

**G-RG-ON-DET:** RG_ON == RG_ON2.

**G-RG-ON-LOCAL:** there is no divergence before the first StartRound.

**Report only:** (b), collateral, and the effect on G-AILEAF when stacked (its 19 mode-1 commits came from the
round-2 start-line restart).

**No default flip.** That is a user decision.
