# RESULT u9774: the second D2 offsetting error, after call 138 (2026-10-20)

Branch `work/u9774` from `work/faithflip` `75d666613` (content = the merge tip `ddc8d09e8`; FAITH default ON). PREREG
`PREREG_u9774.md` (`25ae37e94`) and amendments 1..4 (`ed02c461a`, `3f74b1720`, `0487edb04`, `0d5c410a9`), each committed before its
arms. Exe `31d0be29` (last build), `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`, 13 tracks on `D:\MashedWT\u9749_root13`.
New original captures: `o_a5a6` (`--a5-probe 1500 --a6t-probe`), `o_w0..o_w3` (`--a6a-probe-wheel 0..3`), `o_ww9b4`, `o_ww604`,
`o_ww9b0` (write watches). All with the D2 argv. Arm outputs in `D:\MashedWT\u9774_data` (`MOVED_OFF_GIT.txt`). No default changed.

## 0. Verdict

1. **U-9774 RESOLVED.** `MASHED_COLL_LUT` failed D2 because four further float-order / precision sites on the player's contact
   path were not the original's. Each is now a default-OFF knob:

| knob | site | first effect |
|---|---|---|
| `MASHED_A6A_SPIN_ORDER` | A6a spin factor, `0x00468077..0x004680ae`: `((+0x9e8 * _DAT_005cea18) * max(+0x9e4 * _DAT_005cea14, 8)) * 360`; the port grouped `((max * +0x9e8) * k) * 360` | wheel 2/3 `le0` 1 ulp at call 63 (the +0x9c0 1-ulp at 64..68) |
| `MASHED_TER_SPIN_F32` | contact-normal spin term, `0x0046906b..0x004690a1`, float steps at PC_24; the port computed it in double (`CarWorldContacts.cpp`) | slot 5 normal y 1 ulp from call 94; record +0x9b4 at 113 |
| `MASHED_FIXUP_F32` | fixup hull-slot block, `0x0046f1c2..0x0046f291` (dot stored as float at `0x0046f1df`, `|local_3c| * dot`, `n * f`; sign test grouping); the port kept double | with the above, the call-113 split |
| `MASHED_FIXUP_SUM_ORDER` | fixup per-contact sums, `0x0046f0d8..0x0046f1a8`: `(local_3c + acc) + p*sc` / `(acc + local_3c) + p*sc`; the port `acc + (p*sc + local_3c)` | impulse x 2 ulp at call 138 |
| `MASHED_FIXUP_SLIDE_ORDER` | fixup slide dot, `0x0046f631..0x0046f656`: `(n1*+0x9cc + n0*+0x9c8) + n2*+0x9d0` | faithful; inert on this recipe (H3 refuted) |

2. **With `COLL_LUT` and the five, the player's record is bit-exact against the original through the original's own
   deterministic window.** The port equals each original capture until that capture departs from the others: solo3 / solo4 /
   o_w2 to call 315, r1 / r2 / o_w1 to 312, r4 to 311, r3 to 308, o_w3 to 309, o_ww9b4 to 306, o_a5a6 to 319 and o_w0 to 325
   (past their own split from solo3). Two original runs (solo3, r3) are equal in every record word at call 308 (apart from the
   wheel-history ring phase) and differ in 62 words at 309; A5's random impulse `+0xb00..b08` is 0 throughout, so the source of
   the original's run-to-run split is outside the record (new U-9775).
3. **D2:** `CLSF4` (= the new default + `COLL_LUT` + the five) fails on driving-median, 1890.05 x3 (floor 1893.63; slips 0.1945 /
   0.2537 in). **`F5` (= the new default + the five, without `COLL_LUT`) passes 3/3:** slip1500 0.1900 x3, slip2000 0.2550 / 0.2550
   / 0.2549, driving-median 1965.35 / 1963.95 / 1963.16, H1 398/400.
4. **`F5` is a default-ON candidate** (user decision): (b) keeps every default verdict car for car, 13 tracks 0 RED. `COLL_LUT`
   stays OFF until U-9775.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | every exe of the lane, knobs unset: `B_DEF_K0` `2c24bca3`; `H_DEF` `826277f2 / 250c1569 / 2c8164b2` (exe `31d0be29`) | PASS |
| G-D2 `F5` | 0.1900 / 0.2550..0.2549 / 1963.16..1965.35, H1 398/400, 3 runs alone | PASS 3/3 |
| G-D2 `CLSF4` | 0.1945..0.1947 / 0.2537 / 1890.01..1890.05, 3 runs alone | FAIL (driving-median) |
| G-B `F5` | `c165d2e5 / c01e164a / 2e937af7`: A 2/3 (cars 2, 3), B 2/3 (cars 1, 3), C 1/3 (car 1) = the default's verdicts | PASS |
| G-B `CLSF4` | `af00a5fd / 99fb37ad / b73bbb8f`: same verdicts | reported |
| G-13 `F5` | 39 frames, 870 rows, all cited, 0 RED; against the faithflip default (868): Arctic 1091 35 -> 36, Highway 853 17 -> 18, cited | PASS |
| G-CTRL `F5` | `H_F5` `df5ee50e / a90bbb16 / b3cd0e5f` | reported |

## 2. Method notes

- Alignment: record call k counts from each side's first moving frame; an A5 / A6a probe row k belongs to the frame that produces
  record k + 1 (its entry reads record k). The A6a tail probe is aligned on its first nonzero `+0x9e4` (one stray countdown row
  with velocity, n 769). `g490 / t494` are integers.
- Tools (all in this folder): `a5cmp.py`, `a6tcmp.py`, `fscmp.py` (u9771), `d2first.py`, `d2new.py`, `d2rec.py`, `recpos.py`.
- The fixup log (`MASHED_FIXUP_LOG`) gained a `slide` line (diagnostic, default OFF).
