# RESULT u9761: the A6a torque chain at float precision, and the A6L / D2 conflict (2026-10-19)

Branch `work/u9761` from `race/first-frame-parity` `5ca4815fd` (default G-CTRL `726b8fab / b048d6f0 / ecc20282`). Model
claude-opus-5-5, account2. PREREG `PREREG_u9761.md` with the knob (`4c9a5fac5`), amendment 1 (`883654da0`). Exe `effef35d`
(`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`. Reference o_wobj2.

## 0. Verdict

1. **`MASHED_A6A_F32` (default OFF)** computes A6a's torque / grip chain (`l_60`, `l_d0`, `m15`, `m16`, `r`, the blend factor
   `(l_d0 - m78) / l_d0` and its products, `lb0f`, `grip`) in float, as the original's x87 at PC_24 rounds it; the port held
   it in `double` ([U-A6A-FLOAT10]). With `MASHED_A6A_LUT` too, car 1's omega `+0x9c0` at call 1 equals o_wobj2. At calls 1-2
   the record now differs from o_wobj2 only in the A4 torque ring's index (`+0x1d4 / +0x1e0`, the phase offset), pad dwords
   and the forward's one-call dump lag (`+0x9d4`, U-9758); the first remaining physics field is `+0x460` at call 2
   (-0.04996714 / -0.0499671251). The call-21 drift is unchanged (1.164e-4).
2. **The D2 conflict resolves with the pair.** `MASHED_A6A_LUT` alone: slip1500 0.1877 OUT (U-9761). `MASHED_A6A_F32` alone:
   slip1500 0.1863 OUT. Both: **slip1500 0.1887, in the band** (0.18855-0.19635; original 0.1943), D2 3/3.
3. **Default candidate, user decision: the pair** `MASHED_A6A_LUT` + `MASHED_A6A_F32`: (b) A 2/3, **B 3/3**, C 0/3 (regime B
   car 3 passes, no car that passes on the default fails), D2 3/3, 13 tracks 0 RED. slip1500 sits 0.0002 above the band's
   lower edge.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `85561724 / 76ef3eab / a000895f`; `H_DEF` `726b8fab / b048d6f0 / ecc20282` | PASS |
| G-OMEGA | `B_XA6F32FS_K1` `+0x9c0` at call 1 = o_wobj2 | PASS |
| G-DRIFT | call 21 1.164e-4 (unchanged) | reported |
| G-B | `F32` `3a6b0f48 / af1fbaca / fb6c6358`: A 2/3 B 2/3 C 0/3; `A6LF32` `30fbd10c / 60b2c8be / d5c173ed`: A 2/3 **B 3/3** C 0/3 | PASS |
| G-D2 | `F32`: slip1500 0.1863 OUT x3 (slip2000 0.2525, driving-median 1919.38-1924.60); `A6LF32`: slip1500 0.1887 / slip2000 0.2537-0.2539 / driving-median 1959.04-1965.90, all in, H1 398/400, 1628-1635 lines | `A6LF32` 3/3 PASS |
| G-13 | `A6LF32`: 39 frames, 870 rows, 870 cited, 0 RED, instrument gates PASS; against the default (fwdsse, 867): Egypt 740 21 -> 20, Storm 928 21 -> 22, Dump 735 19 -> 20, Training 884 20 -> 21, Training 959 20 -> 21, all cited | PASS |
| G-CTRL | `H_A6LF32` `bbe1bfd5 / a054388e / 9f599ae9` | reported |

## 2. Code

`Vehicle/Integrate2.cpp`: `A6aF32()`; float branches at `l_60`, the `m15 / m16 / r` block, the blend, `lb0f` and `grip`.

## 3. Trackers (re-classify, no C-level change; `apply_trackers.py`)

U-9761 RESOLVED (omega exact with both knobs; the pair is in the D2 band); new U-9762 (`+0x460` at call 2, the next field, and
the A4 torque-ring phase); one CHANGELOG line.

## 4. User decision needed

The pair `MASHED_A6A_LUT` + `MASHED_A6A_F32` default ON (opt-outs at the flip)? (b) regime B 3/3, D2 3/3 (slip1500 0.1887),
13 tracks 0 RED; G-CTRL would move to `bbe1bfd5 / a054388e / 9f599ae9`.
