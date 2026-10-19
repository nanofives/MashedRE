# RESULT u9762: car 1's `+0x460` at call 2 (2026-10-19)

Branch `work/u9762` from `race/first-frame-parity` `57afde846` (default G-CTRL `726b8fab / b048d6f0 / ecc20282`). Model
claude-opus-5-5, account2. PREREG `PREREG_u9762.md` (`34e80bea9`). Exe `6ca0f4e6` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root`. Original references: o_wobj2, new write watch `o_w460`.

## 0. Verdict

1. **`+0x460` is wheel 3's force x, and its call-2 gap is A5's arithmetic.** Watch `o_w460`: written by A5's all-grounded
   block (`pc 0x0046e91a`) and A6a's addend (`pc 0x0046832a`). The extended split log (`MASHED_FORCE_SPLIT_WHEEL=3`, new,
   diagnostic) on the u9761 pair stack: at call 2 the port's A5 output is `-0.248552054` against the original's
   `-0.24855206906795502`, while A6a's addend is equal (`0.198584929`). With `MASHED_A5_SUSP_LUT` (u9760, A5's original
   arithmetic) the A5 output is `-0.248552069`, equal.
2. **Car 1 is bit-exact for the first calls.** With `MASHED_A6A_LUT` + `MASHED_A6A_F32` + `MASHED_A5_SUSP_LUT`, car 1's
   record equals o_wobj2 through call 2 on every physics field, and (new `recpos.py`, full float precision) position and
   velocity are exact through call 4. Velocity differs from call 5 (dvz 1.2e-4), position from call 7. First differing field:
   wheel 1's force x `+0x2d8` at call 3 (-28.2014942 / -28.2014961).
3. **`posdrift.py` is not a physics drift measure at the early calls.** It reads the AI's `own_x / own_z`, which is the
   previous frame's record position (the AI runs before physics), from the port's 9-digit CSV. Its call-0 gap (9.2e-5) is the
   pre-window pose, and the ~1e-9 values at calls 1..6 are print quantisation. `recpos.py` reads the record.
4. **The triple is not a default candidate.** `TRI` (the three knobs): (b) A 2/3, B 3/3, C 0/3, 13 tracks 0 RED, but D2
   slip1500 0.1859 OUT and driving-median 1989.05-1990.80 OUT (band top 1982.44). Each faithful A5 / A6a arithmetic fix so far
   moves the player's D2 metrics (U-9757, U-9761, here). The pair `MASHED_A6A_LUT` + `MASHED_A6A_F32` (u9761) stays the
   candidate.

## 1. Gates (triple)

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `85561724 / 76ef3eab / a000895f`; `H_DEF` `726b8fab / b048d6f0 / ecc20282` | PASS |
| G-B | `B_TRI_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7`: A 2/3, B 3/3, C 0/3 | PASS |
| G-D2 | `D2_TRI_1..3`: slip1500 0.1859 OUT, slip2000 0.2534 in, driving-median 1990.80 / 1989.05 / 1989.96 OUT, H1 398/400, 1628-1631 lines | FAIL |
| G-13 | 39 frames, 870 rows, 870 cited, 0 RED, instrument gates PASS | PASS |
| G-CTRL | `H_TRI` `3b3690f6 / 9a657924 / a386f43b` | reported |

## 2. Code

`Vehicle/Integrate2.cpp`: `MASHED_FORCE_SPLIT_WHEEL` (diagnostic). `recpos.py` (record position / velocity per call).

## 3. Trackers (re-classify, no C-level change; `apply_trackers.py`)

U-9762 RESOLVED; new U-9763 (A5's faithful arithmetic against the D2 band; next fields `+0x2d8` at call 3 and the call-5
velocity); one CHANGELOG line.
