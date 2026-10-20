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

## Amendment 2 (before its arms): the contact fixup's slide term

Measured on `CLST`: with `MASHED_A6A_SPIN_ORDER` the per-wheel terms equal the original's through call 112 and the record's first
difference stays at 113 (`+0x9b4`). In that frame A6a's entry, wheel terms and tail velocity are equal; a write watch on the
original (`o_ww9b4`, `--watch-write 9b4`) shows the jump after the tail in the contact fixup FUN_0046ef70 (from `0x00470b03`):
`0x0046f59e` -36.046154 (impulse), `0x0046f5e9` -32.367363 (damp), `0x0046f69c` -29.130291 (slide). The slide's dot product in
the original (`0x0046f631..0x0046f656`) is `(n1 * +0x9cc + n0 * +0x9c8) + n2 * +0x9d0`; the port's (`Collision/ContactFixup.cpp`)
is `(n2 * +0x9d0 + n0 * +0x9c8) + n1 * +0x9cc`.

Knob `MASHED_FIXUP_SLIDE_ORDER=1` (default OFF). H3: on `CLS` + the knob the record's +0x9b4 at 113 equals the original's. Arms
`CLSFT` (traced), then D2 x3 on `CLSF` (= `CL` + both knobs) and on `SF` (= default + both).

## Amendment 3 (before its arms): double-precision steps in the contact path

Measured on `CLSFTL` (`MASHED_FIXUP_LOG`, now with a slide line): `MASHED_FIXUP_SLIDE_ORDER` is inert here (H3 refuted, +0x9b4 at 113
unchanged). The fixup's impulse step already differs (`v1` after `v += imp`: -36.0461578 vs the original's -36.0461540; the entry
velocity -1.89379358 is equal). The record's only differing contact-slot word before the split is slot 5's normal y (`+0x604`,
0.00707320031 vs 0.00707320077, from call 94), later slot 9's (`+0x704`). A write watch (`o_ww604`) names its writers: `0x004690b1 /
0x004690be` (unnormalised, in the solver called from `0x00469bdc`) and FUN_004c39b0 (`0x004c3a72 / 0x004c3a7c`, from `0x004690ea`).
The original forms the spin term in float steps at PC_24 (`0x0046906b..0x004690a1`: `((|w| * 360) * [edi-0x18]) * _DAT_005cea44`,
then each component times it); the port (`Collision/CarWorldContacts.cpp`) in double, rounded once. The fixup's hull-slot block does
the same (`0x0046f1c2..0x0046f209`: the dot product stored as a float at `0x0046f1df`, then `|local_3c| * dot`, then `n * f`; the
sign test grouped `(p9*l1 + l0*p8) + p10*l2` at `0x0046f27b..0x0046f291`); the port keeps double.

Knobs (default OFF): `MASHED_TER_SPIN_F32=1` (the spin term) and `MASHED_FIXUP_F32=1` (the hull-slot block). H4: on `CLSF` + both,
slot 5's normal at 94 and the record at 113 equal the original's. Arms `CLSF3TL` (traced), D2 x3 on the stack that holds.

## Amendment 4 (before its arms): the fixup's per-contact sums

Measured on `CLSF3TL`: with amendment 3's knobs the call-113 split is gone (contact slots equal at every call through 138) and the
first difference is `+0x9b0` at 138 (3 ulp), made in the fixup's impulse step of that frame (`v0` after `v += imp`: original
130.570099 (`o_ww9b0`, `0x0046f58a`), port 130.570068; entry velocity and `v2` equal). The original sums each contact into its
accumulators as `(local_3c + local_54) + p*sc` and `(local_60 + local_3c) + p*sc` (`0x0046f0d8..0x0046f1a8`, the first sum held on
the x87 stack); the port as `acc + (p*sc + local_3c)`. D2 on `CLSF3`: slip 0.1974 / 0.2521, driving-median 1895.91 (in), 1 run.

Knob `MASHED_FIXUP_SUM_ORDER=1` (default OFF). H5: on `CLSF3` + it the record at 138 equals the original's. Arms `CLSF4TL`, then
D2 x3 on `CLSF4` (= `CL` + all five knobs) and on `F5` (= the default + the five, without `COLL_LUT`).
