# PRE-REGISTRATION: turn the B2 settings on by default (USER DECISION 2026-10-09), committed before the edit

## 1. What flips

Default-ON, each with a `MASHED_NO_*` opt-out (the H4 `MASHED_NO_SLOT_PLAYER` pattern; the old
`MASHED_*` names become no-ops):

| knob | read at | opt-out |
|---|---|---|
| `MASHED_A364_RESET` | `Race/AiDifficultyReset.cpp:89` | `MASHED_NO_A364_RESET` |
| `MASHED_REFDIST` (FUN_00442a60 bridge, fixed `59c07d7e`) | `Race/SpectatorDistances.cpp:211` | `MASHED_NO_REFDIST` |
| `MASHED_RACEPCT_BRIDGE` | `D3d9Render/TrackRenderer.cpp:5731` | `MASHED_NO_RACEPCT_BRIDGE` |
| `MASHED_WIRE_B2` | `Ai/AiStandalone.cpp:938` | `MASHED_NO_WIRE_B2` |

**NOT flipped: `MASHED_NO_ELIM`.** It is a harness override that disables elimination, a game rule.
Every B2 measurement so far ran with it set, so the new default (elimination ON) is measured fresh here.

## 2. Arms (`run_h4.ps1` recipe, 14,400 frames, plus the `run_bscore.ps1` K arms)

| arm | config |
|---|---|
| `F_OLD` | = `L2_CTRL` (old default, hashes known) |
| `F_NEW`, `F_NEW2` | new default, two runs |
| `F_OPTOUT` | new build + all four `MASHED_NO_*` |
| `FK01`, `FK26` | new default, `MASHED_RING_SKIP` 1 / 26 (regimes B / C), 1200 frames |

## 3. Predictions and gates

| # | prediction | refuted if |
|---|---|---|
| G1 | `F_OPTOUT` is byte-identical to the old default (`c741a4c5` / `642af0ac` / `05984e30`) | differs |
| G2 | `F_NEW` == `F_NEW2` byte-identical (determinism holds) | differs |
| G3 | (b) regime A on `F_NEW`: car 1 PASS, car 2 FAIL 1 item (abs~ 0), car 3 PASS, i.e. the same window as `B2K00` (fixed). Elimination does not act inside the first 220 calls | the window differs from `B2K00`'s |
| G4 | the regime labels hold: `F_NEW` A, `FK01` B, `FK26` C | any differs |
| R1 | (report) criterion (e) `ai_speed_env.py --check` on `F_NEW` (old default: PASS 3/3) | n/a |
| R2 | (report) `collateral.py --mode paired` `F_OLD` vs `F_NEW` (csv step, csv gates, kv ptrace), floor from `F_NEW` vs `F_NEW2`: the top 10 diverging fields and their first frame. Any hit on the D2 watch list (+0x4a4, contact collector, clamp #6, substep loop) is flagged "D2 REOPEN CANDIDATE" | n/a |
| R3 | (report) (b) regimes B and C on `FK01` / `FK26` | n/a |

**If G1 or G2 fails, the flip is reverted.** A G3 / G4 miss is reported and does not block a flip
the user decided. R1 / R2 findings are reported, not tuned.
