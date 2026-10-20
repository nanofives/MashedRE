# PREREG wcsflip: MASHED_WCS_TRINORMAL default ON (2026-10-20)

Branch `work/wcsflip` from `race/first-frame-parity` `0d9f1e0c2` (u9773 merged). Mariano's decision (2026-10-20, relayed by the
orchestrator): flip `MASHED_WCS_TRINORMAL` default ON, accepting (b) A car 2's loss for C car 3; this overrides the "no passing car
may fail" rule for this flip only. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`,
13 tracks on `D:\MashedWT\u9749_root13`. Written before any arm.

Flip: `WCS_TRINORMAL` joins the FAITH seed list in `Core/FaithDefaults.cpp` (value 1); opt-out `MASHED_NO_WCS_TRINORMAL=1`;
`MASHED_NO_FAITH=1` still seeds nothing (the pre-FAITH default).

| gate | rule |
|---|---|
| G-CTRL x2 | `H_DEF` = `H_DEF-2`, new triple reported |
| G-OPT | `MASHED_NO_WCS_TRINORMAL=1`: `H_NT` = `df5ee50e / a90bbb16 / b3cd0e5f`; `B_NT_K0/K1/K26` = `c165d2e5 / c01e164a / 2e937af7` |
| G-B | `B_DEF_K0/K1/K26` = u9773 `B_TN` `28a76cf2 / 76a0b4cd / 040ce640`; (b) expected A 1/3 (car 3), B 2/3 (cars 1, 3), C 2/3 (cars 1, 3); every changed verdict reported |
| G-D2 | three runs alone, `score_d2_v2.py port`, 8-run band: PASS 3/3 (D2 or 13-track failure: no flip) |
| G-13 | 13 tracks: 0 RED, row changes against the f5flip default cited |
