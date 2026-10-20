# PREREG u9774: the second D2 offsetting error, after call 138 (2026-10-20)

Branch `work/u9774` from `work/faithflip` `75d666613` (FAITH default ON; same content as the merge tip). Model claude-opus-5-5,
account2. Exe `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`. Written before any knob arm.

## 0. Measured before (d2off, RESULT_d2off.md)

On FAITH + `MASHED_COLL_LUT` (= the new default + `MASHED_COLL_LUT`, stack `CL`) the player's record equals orig_solo3 through
call 112; `+0x9b4` differs by 2 ulp at 113..116 and heals by 123; `+0x9b0` 3 ulp at 138, then `+0x9b0..+0x9c4`, `+0xb0c`,
`+0xb14`, `+0xb1c` from 139; |dv| > 1e-3 from 258. `CL` fails D2 (driving-median 1878.85..1882.20). New original capture
`o_a5a6` (D2 argv + `--a5-probe 1500 --a6t-probe`).

## 1. Method and hypothesis

`CLT` = `CL` + the player record dump + `MASHED_A5GDIAG` + the A6a tail / vz / force-split logs on slot 0. For calls 112..140,
compare A5's entry and exit velocity (`a5probe` vs `A5GDIAG`) and A6a's tail terms (`a6tprobe` vs `TAILLOG`) against the record
split. H1: the first stage whose output differs with equal inputs names the RVA of the second offsetting error. Any fix is a new
default-OFF knob, added by amendment with its measurement, then re-scored: D2 3/3 on `CL` + fix, with (b), 13 tracks, G-CTRL
reported. No default flip.

## Amendment 1 (before its arms): A6a's spin-arm grouping

Measured on `CLT` against `o_a5a6` and the per-wheel captures `o_w0..o_w3` (`--a6a-probe-wheel`, `fscmp.py`): A5's entry matches
through call 140 within print precision; A6a's tail velocity first differs at call 113 (`v1` -28.9506435 vs -28.9506474), between
A5's output and the tail. Per wheel, A6a's terms differ from call 63 (wheel 2: `le0` 728.663147 vs 728.663208, then `s0`, `lc8`,
`lac`, `dFx` -4563.40674 vs -4563.41211; wheel 3 at 63 / 65 / 78), absorbed until the record splits. `le0` is the spin arm's
`dst[0] * f - v0`. The original (`0x00468077..0x004680ae`) forms `f = ((+0x9e8 * _DAT_005cea18) * max(+0x9e4 * _DAT_005cea14,
_DAT_005cc9f4)) * _DAT_005ccac4`; the port had `((max(..) * +0x9e8) * _DAT_005cea18) * 360`.

Knob `MASHED_A6A_SPIN_ORDER=1` (default OFF). H2: on `CL` + the knob, wheel 2's and 3's terms at calls 63..89 equal the original's
and the record's split moves past 138. Arms `CLST` (traced, wheels 0..3), then D2 x3 on `CLS` (= `CL` + knob) and on `SO`
(= the default + knob); (b), 13 tracks and G-CTRL on the stack that passes D2.
