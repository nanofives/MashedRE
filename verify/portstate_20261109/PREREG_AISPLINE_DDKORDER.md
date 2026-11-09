# PREREG PORT-AISPLINE-DDKORDER (2026-11-09)

Step: PORT-AISPLINE-DDKORDER, parent cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-AISPLINE-DDKORDER` from
`orch/integration` 5873e911e. Committed alone, before any arm.

## 1. Finding being fixed

PORT-X87-PC24-AUDIT (branch `orch/PORT-X87-PC24-AUDIT` @ dc8b11eb8, `RESULT_PORT-X87-PC24-AUDIT.md` finding (a)):
the port body of 0x00443300 `AiSplineEval_x87` (`Match/MatchS08_00443300.cpp`, the byte-matched source
`re/match/s08/00443300.c`) differs from the original in every FPU mode on live-seeded calls: X53 RED 52/80,
X24 RED 44/80, S RED 44/80. The original runs it at CW 0x007f (PC_24) only.

Cause, read from the original's listing (MASHED.exe.unpatched, 0x00443373..0x00443427), x axis
(edi = p0, esi = p1, edx = p2, [ebx+ecx*8] = p3; the z axis at 0x004433d2..0x00443427 is the same with +4):

| addr | op | value |
|---|---|---|
| 0x00443373..0x00443381 | `fld st0; fmul st1; fld st0; fmul st2` | t2 = t*t, t3 = t2*t (kept in ST) |
| 0x00443383..0x0044339e | | A = ((((p0+p0) - p1*5.0) + p2*4.0) - p3) * t2 |
| 0x004433a0..0x004433b7 | | B = (((p1*3.0) - p0) - p2*3.0 + p3) * t3, as ((p1*3 - p0) - p2*3) + p3 |
| 0x004433b9 | `faddp` | S1 = A + B |
| 0x004433bb..0x004433c1 | `fld p2; fsub p0; fmul st4; faddp` | S2 = S1 + (p2 - p0)*t |
| 0x004433c3..0x004433c7 | `fld p1; fadd st0,st0; faddp` | S3 = S2 + (p1 + p1) |
| 0x004433c9..0x004433cf | `fmul [0x005cc32c]; fstp [ebp]` | out = S3 * 0.5f |

Constants: 5.0 `0x005cc358`, 4.0 `0x005cc35c`, 3.0 `0x005cc31c`, 0.5 `0x005cc32c`, clamp 1.0 `0x005cc320`, 0.0
`0x005d757c`. The C source writes `0.5f * ((((p1+p1) + (p2-p0)*t) + A) + B)`. The DDK (`/O2`, no `/Op`) reassociated
the outer sum to `((A + B) + (p2-p0)*t) + (p1+p1)`. VS2022 `/fp:precise` keeps the source order, so the port sums
in a different order and rounds differently. The source still byte-matches under the DDK, so it stays as it is.

## 2. Change (default OFF)

Knob **`MASHED_AISPLINE_DDK`** (environment, read once, `1` = on, anything else or unset = off).

- `Match/MatchS08_00443300.cpp`: the matched source compiles as `AiSplineEval_x87_match` (unchanged text). A new
  `AiSplineEval_x87_ddk` evaluates the same polynomial in the DDK's order above, one float operation per
  assignment (so every intermediate rounds to float, which equals PC_24 rounding on these values and does not
  depend on the CW). `AiSplineEval_x87` (the symbol every caller and the `.asi` export use) dispatches: knob on
  `_ddk`, else `_match`.
- No other file changes behaviour. Knob OFF runs the same matched body as today, plus one call and one cached
  flag test.

## 3. Arms

1. **Emulation (acceptance).** The arm-3 harness of PORT-X87-PC24-AUDIT (`x87_live_emu.py`, live-seeded Unicorn,
   copied under `verify/portstate_20261109/aispline_ddk/` with only a TU filter and a symbol override added), on
   the Quick-Battle race recipe, 0x00443300 attached late for 80 consecutive calls, two port images (X =
   `/arch:IA32`, S = SSE2) of this TU only.
   - Leg ON: port entry `AiSplineEval_x87_ddk`. **Pass: X24 GREEN 80/80 and S GREEN 80/80** (the original reaches
     it only at PC_24, and S applies because the body is float per operation). X53 is reported, not gated.
   - Leg OFF: port entry `AiSplineEval_x87_match`. Expected to reproduce the audit (RED in every mode). Control
     that the harness still separates the two bodies.
   - Controls: O (emulated original at the live CW) equals the live call on every scored sample, as in the audit.
2. **Default gates, knob OFF.** Worktree build (`MASHED_NO_DEPLOY=1`), merge-train gate recipe. Pass: G-CTRL
   `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (the `merge_train.json` refs win over this line).
3. **Knob-ON race gates.** Same recipe with `MASHED_AISPLINE_DDK=1`. Hashes are reported, with no pass bar. A change
   goes to a flip decision row, not into the merge.

## 4. Rules

Every game launch through `mashed_lock.py run` (test-slot outside). No build during a merge-train PRIORITY-HOLD or
SOLO. No default flip. Raw arm data stays in `D:/MashedWT/PORT-AISPLINE-DDKORDER_data` with SHA-256 in the RESULT.

## 5. Verdicts

- GREEN: arm 1 ON passes and arm 2 holds. The flip question (knob ON by default) goes to the lead with arm 3's hashes.
- RED: arm 1 ON fails (the body is wrong), or arm 2 moves (the wrapper changed default behaviour).
