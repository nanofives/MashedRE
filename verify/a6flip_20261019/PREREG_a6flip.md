# PREREG a6flip: MASHED_A6A_LUT + MASHED_A6A_F32 + MASHED_A5_SUSP_LUT default ON (2026-10-19)

User decision 2026-10-19 (relayed by the orchestrator): all three default ON, one combined gate run. Branch `work/a6flip` from
`race/first-frame-parity` `67666a33b` (current default G-CTRL `726b8fab / b048d6f0 / ecc20282`, B dumps `85561724 / 76ef3eab
/ a000895f`). Model claude-opus-5-5, account2, host LIVING-PC. Exe `997e76a9` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root`. Opt-outs `MASHED_NO_A6A_LUT=1`, `MASHED_NO_A6A_F32=1`, `MASHED_NO_A5_SUSP_LUT=1`. Written before any arm.

Code: `Vehicle/Integrate2.cpp` `A6aLut()` / `A6aF32()` and `Vehicle/ForceIntegrator.cpp` `A5SuspLut()` read the opt-out
variables; nothing else changes. Earlier separate measurements: u9762 `TRI` ((b) A 2/3 B 3/3 C 0/3, 13 tracks 870 rows 0 RED,
G-CTRL `3b3690f6 / 9a657924 / a386f43b`), d2rebase `D2_TRI` (new gate PASS 3/3).

## Arms

`D2_DEF_1..3` (each alone, first); `H_DEF`, `H_DEF-2`, `H_NOALL`, `H_NOLUT`, `H_NOF32`, `H_NOSUSP`; `B_DEF_K0/K1/K26`,
`B_NOALL_K0/K1/K26`; 13 tracks on the new default (`verify/rsky_20261016/run13.py`, cites `verify/rres_20261016/cites_rres.json`).

## Gates (any failure: no flip)

| gate | rule |
|---|---|
| G-CTRL | `H_DEF` = `H_DEF-2` (the new triple, reported); `H_NOALL` = `726b8fab / b048d6f0 / ecc20282`; single opt-outs reported |
| G-BOPT | `B_NOALL_K0/K1/K26` = `85561724 / 76ef3eab / a000895f` |
| G-B | `B_DEF_*` (b): no car that passes on the current default (A cars 2, 3; B cars 1, 2) fails; expected A 2/3, B 3/3, C 0/3 |
| G-D2 | the new gate: `verify/d2rebase_20261019/score_d2_v2.py port` (whole log), band `band.json` (slip1500 0.18396..0.20586, slip2000 0.24555..0.25557, driving-median 1893.63..1997.46), H1 >= 200/400 post-trough with median >= 900; 3 runs agreeing within `h` |
| G-13 | 0 RED; any row change against the last default run (fwdsse `tracks_def`, 867 rows) cited |
