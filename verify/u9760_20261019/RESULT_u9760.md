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
