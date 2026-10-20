# PREREG trinflip: MASHED_WCS_TRINORMAL default ON, after wheelcite (2026-10-20)

Branch `work/trinflip` from `race/first-frame-parity` `3537e66f7` (wheelcite merged). Mariano's decision (2026-10-20, this
session): accept cite `C27b-CAR-WHEEL-ROT` and flip `MASHED_WCS_TRINORMAL` default ON, accepting (b) A car 2's loss for C car 3.
Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`. Written before any arm.

Change: the wcsflip seed (`WCS_TRINORMAL` in `Core/FaithDefaults.cpp`, value 1) re-applied; comments only otherwise. The physics
source equals wcsflip's exe `24a608c8` (wcsflip branch `4daee232f` = `0d9f1e0c2` + the seed; `3537e66f7` changes no source).

| gate | rule |
|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2` = wcsflip `91cc1c0b / 2d77a5d7 / ab6c9329` |
| G-OPT | `MASHED_NO_WCS_TRINORMAL=1`: `H_NT` = `df5ee50e / a90bbb16 / b3cd0e5f` |
| G-B | `B_DEF_K0/K1/K26` = `28a76cf2 / 76a0b4cd / 040ce640` (wcsflip `B_DEF`), (b) A 1/3 B 2/3 C 2/3 |
| G-D2, G-13 | carried from wcsflip when G-CTRL and G-B reproduce its hashes: D2 3/3 (logs byte-identical to the knob-off default), 13 tracks 869 rows 0 RED with C27b (wheelcite rescore of the wcsflip run) |

Any hash mismatch: no flip, re-run D2 and 13 tracks on this exe before deciding.
