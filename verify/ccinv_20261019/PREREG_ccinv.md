# PREREG ccinv: MASHED_CC_MATRIX_INVERT default ON (2026-10-19)

User decision 2026-10-19 (relayed by the orchestrator): flip `MASHED_CC_MATRIX_INVERT` default ON, accepting (b) regime A
2/3 -> 1/3 (car 2's A miss, u9769), because the current default's car<->car spin reads an uninitialised stack local
(`Rw_MatrixDerive` stub, `Collision/ContactStubs.cpp`; u9769 section 0 item 5). This overrides the "no passing car may fail"
rule for this flip only. Branch `work/ccinv` from `race/first-frame-parity` `3ca5a4756`. Model claude-opus-5-5, account2. Exe
`MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\d3bc_root`. Written with the flip, before any arm.

Code: `Rw_MatrixDerive` calls `RwMatrixInvertEntry` (0x004c4dc0) unless `MASHED_NO_CC_MATRIX_INVERT=1` (the opt-out = the
stub, the pre-flip default). `MASHED_CC_MATRIX_INVERT` becomes a no-op. Nothing else changes.

| gate | rule |
|---|---|
| G-CTRL | `H_DEF` twice equal (new triple reported); `H_NOCI` = `3b3690f6 / 9a657924 / a386f43b` |
| G-BOPT | `B_NOCI_K0/K1/K26` = `6efb49ed / 3e0724ac / 395fb8b7` |
| G-B | `B_DEF_K0/K1/K26`: A 1/3, B 3/3, C 0/3 expected; every car whose verdict changes against the old default reported. Any B drop = no flip |
| G-D2 | `verify/d2rebase_20261019/score_d2_v2.py port` (whole log), the 8-run band, three `D2_DEF` runs each alone: 3/3 PASS, else no flip |
| G-13 | `verify/rsky_20261016/run13.py` run + score (cites `verify/rres_20261016/cites_rres.json`): 0 RED, every row change against a6flip's `tracks_def` cited, else no flip |
