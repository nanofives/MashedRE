# RESULT u9764: car 1's wheel 1 force x `+0x2d8` at call 3 (2026-10-19), PARTIAL

Branch `work/u9764` from `67cdd752f` (default G-CTRL `3b3690f6 / 9a657924 / a386f43b`). PREREG `PREREG_u9764.md` with the knob
(`3c5f5b021`). Exe `dd94eaeb` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`. Original references: o_wobj2,
write watch `o_w2d8`, new A6a probe `o_aq1`. Model claude-opus-5-5, account2.

## 0. Verdict

1. **The call-3 gap is in A6a's addend, not A5.** `o_w2d8`: A5 projects `x -58.776126861572266` at call 3 and the addend
   brings it to `-28.201494216918945` (addend `30.5746327`). The port's A5 output is equal (`-58.7761269`), its addend is
   `30.5746307`.
2. **The first differing input is the steered wheel's forward axis y.** New original-side probe `--a6a-probe-wheel W`
   (`re/frida/scenario_launch.py`, entry hook at `0x00468320`, stack dwords only) against the port's split log (now with
   `lac / la8 / la4`): at calls 1-3 `le4`, `ld4`, `lbc` and `la4` are equal; `la8` is `2.66307367e-08` (call 2) /
   `2.71713265e-08` (call 3) in the original and `0` in the port, and at call 3 `lac` differs in the last digits
   (`-0.00486449152` / `-0.00486449199`), which carries the addend's ulp. `la8 = s1 - lc4`, `lc4 = lc0 * axis.y` (original
   `lc4 = -2.66307367e-08`), so the original's wheel-1 axis `p[0x20]` (`rec + 0x2e8`) holds about -2.66e-08 where the port's
   is exactly 0. At frame end the record's axis fields are equal, so the difference is A5 Phase 0's frame-start write.
3. **Where:** A5 Phase 0 builds a steered wheel's axis as `RwMatrixRotate(m, up, steer, 0)` then the vectors transform of the
   forward (`0x0046de5b` -> `FUN_004c3df0`, live slot `0x004dc410`, whose order the port already matches). The port uses
   `Rw_MatrixFromAxisAngle` (`Vehicle/ForceIntegrator.cpp` Phase 0). The rotation matrix's y terms are the remaining candidate;
   not measured here. New U-9764.
4. **`MASHED_A6A_LC0` (default OFF)** puts block #4's `lc0` in the original's order (`0x0046819f..0x004681c3`). It changes the
   B dumps and keeps (b) (A 2/3, B 3/3, C 0/3), but it is not the call-3 gap (the addend is unchanged). Not proposed for
   default.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `6efb49ed / 3e0724ac / 395fb8b7` | PASS |
| G-ADD | `XW1LC` addend at call 3 `30.5746307` (unchanged) | FAIL |
| G-POS | `recpos.py`: position / velocity equal through call 4, velocity differs from call 5, position from call 7 (unchanged) | reported |
| G-B | `B_LC_K0/K1/K26` `8f2b9c37 / 95317a93 / 2742fa2a`: A 2/3, B 3/3, C 0/3 | PASS |

## 2. Code / tools

`Vehicle/Integrate2.cpp`: `MASHED_A6A_LC0` (default OFF); the split log gains `lac / la8 / la4`. `re/frida/scenario_launch.py`:
`--a6a-probe-wheel W`. Evidence in `D:/MashedWT/u9764_data`.

## 3. Next (U-9764)

Probe the original's rotation matrix for the steered wheel (`RwMatrixRotate` output before `0x0046de5b`), compare with
`Rw_MatrixFromAxisAngle`'s, then port the difference behind a knob.
