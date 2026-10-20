# RESULT trinflip: MASHED_WCS_TRINORMAL default ON - FLIPPED (2026-10-20)

Branch `work/trinflip` from `3537e66f7` (wheelcite merged). PREREG `PREREG_trinflip.md` (`5da6744ee`, with the seed), committed
before any arm. Mariano's decision (this session): accept cite `C27b-CAR-WHEEL-ROT` and flip, trading (b) A car 2 for C car 3.
Exe `e5e982bb` (`MASHED_NO_DEPLOY=1`). Evidence in `D:\MashedWT\trinflip_data` (`MOVED_OFF_GIT.txt`).

| gate | measured | verdict |
|---|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2` = `91cc1c0b / 2d77a5d7 / ab6c9329` (= wcsflip) | PASS |
| G-OPT | `MASHED_NO_WCS_TRINORMAL=1`: `H_NT` `df5ee50e / a90bbb16 / b3cd0e5f` (the f5flip default) | PASS |
| G-B | `B_DEF` `28a76cf2 / 76a0b4cd / 040ce640` (= wcsflip `B_DEF`); (b) A 1/3 (car 3), B 2/3 (cars 1, 3), C 2/3 (cars 1, 3) | as expected |
| G-D2 | carried from wcsflip (hashes reproduce): 0.1900 x3 / 0.2550 x3 / 1963.95..1965.35, H1 398/400, motion_diag byte-identical to the knob-off default | PASS 3/3 |
| G-13 | carried from wcsflip with the wheelcite rescore: 869 rows, 0 RED, instrument gates PASS | PASS |

## New default

FAITH stack (38 knobs) + u9774's five + `WCS_TRINORMAL`. G-CTRL `91cc1c0b / 2d77a5d7 / ab6c9329`; opt-out
`MASHED_NO_WCS_TRINORMAL=1` gives the f5flip default `df5ee50e / a90bbb16 / b3cd0e5f`; `MASHED_NO_FAITH=1` seeds nothing.
B dumps K0/K1/K26 `28a76cf2 / 76a0b4cd / 040ce640`.

Failing cars on the new default: A car 1 (`c1_distinct 16 [21,70]`), A car 2 (`c0_distinct 36 [13,35]`), B car 2, C car 2.
`kHiProj` is now the binary's 0.999 (`_DAT_005cea88`) on the default, since the TRINORMAL path reads it.
