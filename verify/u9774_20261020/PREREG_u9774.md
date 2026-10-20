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
