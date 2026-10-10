# RESULT: the B2 settings are on by default (USER DECISION 2026-10-09)

Pre-registration: `PREREG_B2FLIP.md` (`8f6d2140`, before the edit). Every gate passes.

## 0. Verdict first

> Default-ON now, each with an opt-out: `MASHED_A364_RESET` (`MASHED_NO_A364_RESET`),
> `MASHED_REFDIST` (`MASHED_NO_REFDIST`), `MASHED_RACEPCT_BRIDGE` (`MASHED_NO_RACEPCT_BRIDGE`),
> `MASHED_WIRE_B2` (`MASHED_NO_WIRE_B2`). The old names are no-ops. **`MASHED_NO_ELIM` is NOT
> flipped**: it is a harness override that turns off elimination, a game rule.
>
> - **G1 PASS:** the four opt-outs together reproduce the old default byte-for-byte
>   (`c741a4c5` / `642af0ac` / `05984e30`).
> - **G2 PASS:** two new-default runs are byte-identical.
> - **G3 PASS:** the new default's (b) window equals the fixed B2 arm's (`B2K00`). Elimination does not
>   act inside the first 220 calls.
> - **G4 PASS:** regimes A / B / C at K = 0 / 1 / 26, unchanged.
> - **R1 (e): PASS 3/3**, gated values (`ft_median_m0`, `launch`) unchanged from the old default.
> - **(b), new default:** regime A **2/3**, with car 2 failing **1 item** (abs steer median 0 vs
>   7..23; it was 5). Regime C 0/3. In C, flag 2 suppresses branch 2, so port C now equals the OLD
>   regime-A window while the new A differs: port A and C now differ, as the original's do. B has no
>   labelled reference.
> - **R2, collateral** (`collateral.py --mode paired`, old vs new, floors from same-arm repeats): 278 of
>   500 fields diverge, the AI targeting state from frame 0 by construction. **Player physics is
>   byte-identical until frame 1307**, which is the old default's round-1 end (21.8 s; the new default's
>   round 1 runs to 27.3 s). The player's trajectories part there because one arm re-grids. The only
>   D2 watch-list name hit, `c9ec`, first diverges at frame 7996, downstream of that. **No D2 REOPEN
>   CANDIDATE.**

## 1. New default controls (every future G-CTRL)

| | step | gates | ptrace |
|---|---|---|---|
| **new default** (`run_h4.ps1` recipe, e.g. `F_NEW`) | **`ae67b13a`** | **`c8317ce2`** | **`31db9384`** |
| old default (= new + the four `MASHED_NO_*`) | `c741a4c5` | `642af0ac` | `05984e30` |

## 2. Raw

`PREREG_B2FLIP.md`, `FLIP_RUN.txt`, `FK_RUN.txt`, `FLIP_SCORE.txt`, `COLLATERAL.txt`,
`COLLATERAL_stdout.txt`, `COLLATERAL_D2WATCH.txt`, `pivot_arm.py`, `run_fk.ps1`, `FK01/FK26.step.csv`,
`../d3_gatefire_20261008/F_{NEW,NEW2,OPTOUT}.*`. The pivoted CSVs (`piv_*.csv`) are regenerable and
not committed.
