# RESULT u9760: car 1's call-1 force words (2026-10-19), PARTIAL

Branch `work/u9760` from `race/first-frame-parity` `0bb215a3b`. Model claude-opus-5-5, account2. PREREG `PREREG_u9760.md`
with the knob (`bf3574f2b`). Exe `acbea31f` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`. Original
references: o_wobj2, new write watch `o_f214`.

## 0. Verdict

1. **The call-1 force words are A5 plus an A6a addend.** Write watch `o_f214` (the watch traps after each store), car 1
   wheel 0 at call 1: A5's all-grounded block (called from `0x004684d7`) writes the raw `z = -11923.7763671875`, then the
   projected `x = -13.935943603515625`, `z = -11923.7607421875` (`pc 0x0046e91a / 0x0046e94f`). A6a then adds a term
   (`0x00468320..0x00468337`, `fadd [edi+0x70 / 0x74 / 0x78]`, `edi = rec + 0x1a4`), giving `x = -2.88651180267334`, `z =
   -2385.2158203125` (`pc 0x0046832a / 0x0046833a`). The A6a addend scales with the speed `+0x9e4` (`0x004682d7`), compares
   against `_DAT_005cd120` and multiplies by `_DAT_005ce18c` (`0x004682dd / 0x004682ea`), and the result is normalised by
   `FUN_004c3ac0` (`0x0046833e`).
2. **The port's A5 block is not the original's arithmetic.** `Vehicle/ForceIntegrator.cpp` Phase 6 takes the length with
   `std::sqrt` (`ForceIntegrator.h:81-82`), keeps `inv`, the unit vector, the dot product and `t` in `double`, and sums the
   dot `(a.z*az + a.x*ax) + a.y*ay`. The original (`0x0046e885..0x0046e925`, x87 at PC_24) uses `FUN_004c3ac0` =
   `LutSqrt(float((x*x + y*y) + z*z))`, rounds every step to float, and sums `(a.y*ay + a.x*ax) + a.z*az`.
   **`MASHED_A5_SUSP_LUT` (default OFF)** ports that. It changes the B dumps but not car 1's record at calls 0..2 on the
   drift stack, and leaves the (b) pass set as it is.
3. **Parity at the force words is not shown.** The dumped `+0x214 / +0x21c` at call 1 are unchanged with the knob
   (-2.88652229 / -2385.22363 against -2.8865118 / -2385.21582). The dumped call-0 record does not reproduce the original's
   A5 output offline (the load and normals A5 reads at that point are not the frame-end values), so A5 and A6a's shares of
   the gap cannot be split from the dumps. The next step is a port-side log of A5's projected force and A6a's addend for car
   1, compared with `o_f214`'s per-store values.
4. **Another call-0 difference on the full knob stack** (`XR1`: drift stack + `MASHED_UP_TRINORMAL`, `MASHED_RW_SSE_POINTS`,
   `MASHED_RING_DBUF`, `MASHED_FWD_AT`): `+0xb3c` -1.00002646 original / +1 port, `+0xaf8` -1.00000167 / -1, `+0xb40` 0 / -0.
   Not traced here.
5. Drift unchanged (1.202e-4 at call 21) on both `XR1` and `XA5LR1`.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529` | PASS |
| G-F214 | `XA5LR1` `+0x214 / +0x21c` at call 1 unchanged (-2.88652229 / -2385.22363) | FAIL |
| G-DRIFT | call 21 1.202e-4 | FAIL |
| G-B | `B_A5L_K0/K1/K26` `f2f4e541 / 647d36c3 / 2059f3c7`: A 2/3, B 2/3, C 0/3 | PASS |
| G-D2 / G-13 / `H_DEF` / `H_A5L` | not run: no default proposal | - |

## 2. Code

`Vehicle/ForceIntegrator.cpp`: `A5SuspLut()` and the original-arithmetic branch of Phase 6's all-grounded block; includes
`Math/RwLutTables.h`.

## 3. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9760 note (the decomposition, the knob, the next step, the `+0xb3c` lead); hooks.csv note on `0046ddb0`; one CHANGELOG
line. U-9760 stays open.

## 4. Next

A diagnostic in `ForceIntegrator.cpp` / `Integrate2.cpp` that logs car 1's A5 projected force and A6a's addend per frame
(default OFF), then compare with `o_f214`. Then the A6a addend's inputs (`+0x9e4`, the locals at `[esp+0x1c / 0x34 / 0x44 /
0x48 / 0x4c]`).

## 5. Amendments 1-3 (branch `work/u9760b`, rebased onto `c43ebeba1`, exe `b0f02b3c`): the A6a addend

1. **A5 is exact; the gap was A6a's addend.** New diagnostic `MASHED_FORCE_SPLIT` (inert: `B_XA6FS_K1` step dump identical to
   the stack without it) logs, at A6a's addend for wheel 0 of one car, the force before (A5's output), the addend and its
   inputs. At call 1 the port's force before the addend equals the original's A5 output (`-13.9359436 / -11923.7607`); the
   addend differed (`11.0494213 / 9538.53711` against the original's `11.0494318 / 9538.54492`).
2. **The cause: A6a's lengths.** The original takes all nine of A6a's lengths with the LUT `FUN_004c3ac0` (`0x0046766e`
   `+0x9e4`, `0x00467680` `+0x9e8`, `0x004680f6` le4, `0x0046820a` ld4, `0x0046833e` |F|, `0x004684bb` m15, `0x004684d7` m16,
   `0x004685b7` m78, `0x004686a4` speed); the port used exact sqrt at the same nine sites in the same order.
   **`MASHED_A6A_LUT` (default OFF)** makes them the LUT form: the addend, the call-1 force words (`-2.8865118 / -2385.21582`)
   and the call-1 velocity `+0x9b0` equal the original's. Drift: call 4 3e-9 (was 2.4e-7), call 12 4.05e-6 (was 6.2e-6),
   call 21 1.164e-4 (was 1.202e-4). Next first-differing field: omega `+0x9c0` at call 1 (-8.73517183e-06 / -8.73517274e-06);
   the port's A6a torque chain runs in `double` (U-A6A-FLOAT10) where the original runs x87 at PC_24.
3. **Gates on the new default (`c43ebeba1`):**

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `85561724 / 76ef3eab / a000895f`; `H_DEF` `726b8fab / b048d6f0 / ecc20282` | PASS |
| G-F214 | `B_XA6FS_K1` addend and force words = `o_f214` | PASS |
| G-B | `B_A6L_K0/K1/K26` `f43742fb / 089b0877 / 0ffa5a81`: A 2/3, **B 3/3** (car 3 passes for the first time), C 0/3 | PASS |
| G-D2 | `D2_A6L_1..3`: slip1500 **0.1877 OUT** (band 0.18855-0.19635; original 0.1943), slip2000 0.2547 in, driving-median 1956.70 in, H1 398/400, 1627-1628 lines | **FAIL** |
| G-13 | 39 frames, 868 rows, 868 cited, 0 RED, instrument gates PASS | PASS |
| G-CTRL | `H_A6L` `6f2fb5ea / 6ba84713 / 8953e594` | reported |

   On the pre-flip default (`b0ad0796e`) the same knob was in band (slip1500 0.1904). **Not a default candidate on the new
   default**: the faithful A6a lengths move the player's slip1500 out of band, the same pattern as U-9757. New U-9761.
