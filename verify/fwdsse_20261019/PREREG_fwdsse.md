# PREREG fwdsse: MASHED_FWD_AT + MASHED_RW_SSE_POINTS default ON (2026-10-19)

User decision 2026-10-19 (relayed by the orchestrator): both knobs default ON, one combined gate run. Branch `work/fwdsse`
from `race/first-frame-parity` `b0ad0796e` (current default G-CTRL `b40b47bc / fb690fb1 / d3a20f4c`, B dumps `c7b40cf2 /
5b9905d8 / f680a529`). Model claude-opus-5-5, account2. Opt-outs `MASHED_NO_FWD_AT=1`, `MASHED_NO_RW_SSE_POINTS=1`. Private
root `D:\MashedWT\f60flip_root`. Written before any arm.

Code: `Vehicle/VehiclePhysicsRun.cpp` `FwdAt()` and `Math/RwV3dTransformPointsCPU.cpp` `RwSsePoints()` read the opt-out
variables; nothing else changes. Separate measurements so far: u9758 (`FA`) and u9759 (`SSE`), each on its own.

## Arms

`H_DEF`, `H_DEF-2` (new default), `H_NOBOTH`, `H_NOFWD`, `H_NOSSE`; `B_DEF_K0/K1/K26`, `B_NOBOTH_K0/K1/K26`; `D2_DEF_1..3`
(each alone); 13 tracks on the new default (`verify/rsky_20261016/run13.py`, cites `verify/rres_20261016/cites_rres.json`).

## Gates (any failure: no flip)

| gate | rule |
|---|---|
| G-CTRL | `H_DEF` = `H_DEF-2` (the new triple, reported); `H_NOBOTH` = `b40b47bc / fb690fb1 / d3a20f4c`; `H_NOFWD`, `H_NOSSE` reported |
| G-BOPT | `B_NOBOTH_K0/K1/K26` = `c7b40cf2 / 5b9905d8 / f680a529` |
| G-B | `B_DEF_*` (b): no car that passes on the current default (A cars 2, 3; B cars 1, 2) fails |
| G-D2 | `D2_DEF_1..3`: slip ~0.1942 / ~0.2545, driving-median in [1904.70, 1982.44], H1 >= 398/400, >= 1600 motion_diag lines each |
| G-13 | 0 RED; any row change against the last default run cited |
