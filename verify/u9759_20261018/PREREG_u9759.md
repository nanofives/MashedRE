# PREREG u9759: the up vector +0x9cc (2026-10-18)

Branch `work/u9759` from `race/first-frame-parity` `f22c7d7a1` (default G-CTRL `b40b47bc / fb690fb1 / d3a20f4c`, default B
dumps `c7b40cf2 / 5b9905d8 / f680a529`). Model claude-opus-5-5, account2. Exe `92b5e645` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root`. Written before any knob arm.

## 0. Measured before this PREREG

- Write watch `o_up1` (car 1, `+0x9c8 / +0x9cc`): the only writer is `FUN_0046c5f0` (`pc 0x0046c6a8 / 0x0046c6b1`, called at
  `0x0046f7b0` inside `FUN_0046f6c0`), ~3060 hits. At call 0 the original holds (0, 0.9999889731407166, 0) (`3f7fff47`).
- Batch probe `o_bq1` (car 1, classifier entry `0x0046cc40`): the four wheel points and the up vector per solver call.
  `trinorm.py` (the port's `MASHED_UP_TRINORMAL` arithmetic, float32, generated RW sqrt LUT) on the original's points gives
  exactly the original's up vector on every row checked (`3f7fff45` rows 0-2, `3f7fff47` rows 3-6). So the formula is right.
- Port diagnostic (`MASHED_BATCH_DUMP_SLOT=1`, new, diagnostic only; `B_UTBQ1_K1` step dump `97ffed3f` = u9152's
  `SPLSBKNF60UTR1`): the port's points differ by one ulp from the spawn pass on (wheel 3 x `bf590939` vs `bf59093a`,
  wheel 2 / 3 z), so its up vector is `3f7fff48`.
- Same matrix: the extended batch probe `o_bq2` logs the dst rows at the classifier entry; they equal the record's
  (`m0 -1, m2 33bbbd2e, m8 b3bbbd2e, m10 -1`). Recomputing the original's points from its own scratch (`rec[0x16c] /
  rec[0x174]` x `_DAT_005cea60`, `0x0046f6c0` reset loop) and rows: `((x*m0 + y*m4) + z*m8) + m12` (the x87 default
  `0x004c3670`) and extended precision give `bf590939`; the orders that add the translation before the z term give
  `bf59093a`.
- Live slots (probe `o_bq3`): `*(*(0x007d3ff8) + *(0x007d3ffc) + 8 / 0xc / 0x10 / 0x14)` = `0x004dc4a0 / 0x004dc370 /
  0x004dc530 / 0x004dc410`, not the installer's `0x004c3670` for `+0xc`. `0x004dc370` is SSE: `(z*at + y*up) + (x*right +
  pos)` (`0x004dc3d2..0x004dc3e7`). `0x004dc410` (vectors) sums `(x*right + y*up) + z*at`, as the port already does.

## 1. Hypothesis

H1: with the SSE order in `RwV3dTransformPointsCPU`, car 1's wheel points and up vector equal the original's at every row
checked. H2 (prediction): the call-1 velocity gap and the call-21 drift close or shrink, since the points feed the
classifier depths and the solver's y correction.

## 2. Knob and arms

- `MASHED_RW_SSE_POINTS=1` (default OFF, `Math/RwV3dTransformPointsCPU.cpp`): the per-component order of `0x004dc370`.
  It applies to every slot-`+0xc` user (wheel points, car-vs-world body points, contact query, `Race/PathNodes.cpp`).
Arms: `B_DEF_K0/K1/K26`, `H_DEF` (G-NOOP); `B_UTBQ1SSE_K1` / `B_BQ1SSE_K1` (u9756 drift stack + the knob, with and without
`MASHED_UP_TRINORMAL`, car-1 batch dump and record); `B_SSE_K0/K1/K26`; `H_SSE`.

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knob off: B dumps and `H_DEF` as above |
| G-PTS | `UTBQ1SSE` car-1 wheel points and up vector = `o_bq1` on the first 8 rows |
| G-DRIFT | car 1 x/z gap at call 21 < 1e-5 (now 1.202e-4); else the next first-differing field |
| G-B | (b) on `SSE`: no car that passes on the default turns to FAIL |
| G-D2 / G-13 | for a default proposal: D2 3/3, 13 tracks 0 RED, G-CTRL reported |

## Amendment 1 (2026-10-18, after the B arms, before these)

Measured: G-NOOP PASS. G-PTS PASS: with `MASHED_RW_SSE_POINTS` the port's car-1 spawn-pass and first-call wheel points and
up vector equal `o_bq1` bit for bit (later rows are not comparable row by row: the original is still in countdown hold
ticks). With `MASHED_UP_TRINORMAL` + the knob, `+0x9cc` is exact at calls 0 and 1 and the rotation rows at call 1 too.
G-DRIFT FAIL: call 21 1.202e-4 unchanged; call-1 velocity `+0x9b0` and omega `+0x9c0` unchanged; force words `+0x214 /
+0x21c` differ at call 1. Also found: the 16-entry ring A4 writes at `rec + 0x1ac + (DAT_007f101c & 0xf)*4` (watch `o_w1d4`,
`pc 0x00470848 / 0x0047090d`) holds the call-1 value at index 13 in the original and 10 in the port (`g_torqueRingPhase`).
G-B PASS (`SSE`: A 2/3, B 2/3, C 0/3). Default candidate `SSE`: D2 `D2_SSE_1..3`, 13 tracks with `MASHED_RW_SSE_POINTS=1`
(cites `verify/rres_20261016/cites_rres.json`), `H_SSE`.
