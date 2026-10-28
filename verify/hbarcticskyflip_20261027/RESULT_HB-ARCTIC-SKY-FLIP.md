# HB-ARCTIC-SKY-FLIP RESULT (2026-10-27): L-41 applied, the Arctic sky band fix is the default

**Verdict: GREEN.** `MASHED_ARCTIC_SKY_RTT` is gone. Sky copy A (`0x0044937d`) and the after-sky lens batch
(`0x00489240`) now stay out of the race frame by default, and `MASHED_NO_ARCTIC_SKY_RTT=1` puts them back
(the pre-flip default). Mariano decided L-41 (2026-10-27). Evidence for the cause is in
`verify/hbarcticsky_20261027/RESULT_HB-ARCTIC-SKY.md`.

- Branch `orch/HB-ARCTIC-SKY-FLIP` off `orch/integration` `ece372d27`, which contains HB-ARCTIC-SKY `e43711f96`
  (checked with `git merge-base --is-ancestor`). Flip commit `e508ccd3f`
  (`D3d9Render/TrackRenderer.cpp` and `LibRw/RwRaceSubmit.cpp`, both read `MASHED_NO_ARCTIC_SKY_RTT`). The partial
  attribution values `=a` / `=lens` are dropped. Their result is recorded in the HB-ARCTIC-SKY RESULT.
- Exe `MASHED_NO_DEPLOY=1`, SHA-256 `fe51013f099d22e4b9aa9e3f2b4c4149bada8264932353637d6996239a86ca99`.
  Private roots `D:/MashedWT/HB-ARCTIC-SKY-FLIP_root` and `..._root13`. Every boot ran under `mashed_lock.py` and
  `test-slot`. All game processes exited on their own and none was killed.

## 1. Sky band on the flipped default (no env var set)

`measure.py` (same metrics as HB-ARCTIC-SKY), `measure_table.txt`:

| pose | original | **DEF (flipped, no env)** | d | NOD (`MASHED_NO_ARCTIC_SKY_RTT=1`) | d | rest NOD vs DEF |
|---|---|---|---|---|---|---|
| e1 | 46.8 | **47.4** | **0.6** | 70.9 | 24.0 | 0.348 |
| w1 | 45.0 | **47.4** | **2.5** | 69.8 | 24.8 | 0.000 |
| w2 | 31.3 | **35.4** | **4.0** | 35.4 | 4.0 | 0.051 |
| n1 | 42.9 | **45.3** | **2.4** | 68.7 | 25.8 | 0.352 |

- The default reproduces 0.6 / 2.5 / 2.4 on e1 / w1 / n1. The rest of the frame moves at most 0.352.
- Byte checks: the flipped DEF frame equals the HB-ARCTIC-SKY K1 frame, and NOD equals the HB-ARCTIC-SKY DEF
  frame, on all 4 poses. Control e1 DEF vs DEF2 byte-identical.

## 2. Gates, old vs new

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the flipped exe:

| gate | old ref (`merge_train.json`) | flipped build | moved |
|---|---|---|---|
| G-CTRL (step / gates / ptrace) | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | no |
| B (K0 / K1 / K26) | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | no |

**Expected refs after the flip: unchanged** (`gctrl.ref` and `b.ref` stay as they are).

## 3. 13-track reference, old vs new

`verify/rsky_20261016/run13.py run` / `score --cite verify/rres_20261016/cites_rres.json`, output
`tracks_score.txt` (this folder). 0 RED on every row, `INSTRUMENT GATES: PASS`. **Only Arctic moved**. The other
12 tracks are identical, row for row, to `verify/colllut_20261026/tracks_score.txt`.

| Arctic row | old matched / m.tri / m.tri% / rows | new matched / m.tri / m.tri% / rows | red |
|---|---|---|---|
| 1051 | 155 / 34654 / 97.9% / 33 | 153 / 33000 / 93.2% / 34 | 0 -> 0 |
| 1091 | 150 / 34347 / 97.6% / 36 | 148 / 32693 / 92.9% / 37 | 0 -> 0 |
| 1126 | 107 / 28781 / 98.2% / 36 | 105 / 27127 / 92.6% / 37 | 0 -> 0 |

What changed on each row (`arctic_rows_diff.txt`, against `D:/MashedWT/D3-CF-COLLLUT_data/flip_tracks/03_Arctic`):
two new MISSING rows, both cited: `C15-SKY sky` (1 draw, 86 tri) and `C09-FX lensflare-fs8` (1 draw, 1568 tri). One
`C20-STATE-SKY` row is gone. 86 + 1568 = 1654 = the m.tri drop on each row. These are the two draws the original
makes through the 128x128 camera texture (`FUN_004840d0(0)`, `FUN_00483ed0`). The original's recorder logs every
draw without its render target, so they show as original-only now that the standalone leaves them off the race
frame. That is the intended effect, not a regression. A recorder that logs the render target would drop them from the
original side.

**Expected `full.run13_reference` after the flip: `verify/hbarcticskyflip_20261027/tracks_score.txt`** (Arctic rows
above, the other 12 tracks unchanged).

## 4. Raw data (not committed), SHA-256

Root `D:/MashedWT/HB-ARCTIC-SKY-FLIP_data/`.

```
fe51013f099d22e4b9aa9e3f2b4c4149bada8264932353637d6996239a86ca99 *mashed_re.exe
a64f6dee0babcb153f1f4381a92cdb175a9c6cd276ea82b67824e79b95d39be5 *render/e1_DEF/race1/01_grid.bmp
a64f6dee0babcb153f1f4381a92cdb175a9c6cd276ea82b67824e79b95d39be5 *render/e1_DEF2/race1/01_grid.bmp
8e1caba4cd87051040686fe56bf4d4da681852071865913c0e46301b709c0640 *render/e1_NOD/race1/01_grid.bmp
5e9b3c3b88c88c997a5f7ae5ae9a2b9d419bc761e2f7aba2b893c1d44ecb5315 *render/n1_DEF/race1/01_grid.bmp
9e7f39fa9ae39be0b9d69b88498a4bc7fbe52e50246ce858bd22319a9b4ce1bf *render/n1_NOD/race1/01_grid.bmp
486421b502f53bbb70c2a0da80081cec861b5a623eb3c51769c619dff1c45100 *render/w1_DEF/race1/01_grid.bmp
00b6405b71b96cb82e9bd5ac199d79c4ac5a2678ccdacea1fcf78f9e059208eb *render/w1_NOD/race1/01_grid.bmp
0d385d85244dc3bdfb8d116960a1ef227818387b30ba4c1933fd045f798b0947 *render/w2_DEF/race1/01_grid.bmp
b4ff1186b9f0d6a548646e27b25e9503db0569831bfdd73e5347fb3400a20005 *render/w2_NOD/race1/01_grid.bmp
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 *arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 *arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 *arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f *arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 *arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d *arms/B_DEF_K26.step.csv
3203bae374f6e339ec69d86d51d8ceb568da5af2ac5c5f042f2c73a795502213 *run13/03_Arctic/diff_01051.txt
1bf9d183cd92bab0444d4a8e1db135ff1a2ddd532d682f9430cfc0c677ae56b2 *run13/03_Arctic/diff_01091.txt
03ddfa5b4e98dc7f0e58195d8381e78f2d78c5d758e5aea92d4346ec5eb3dbbd *run13/03_Arctic/diff_01126.txt
626b0108bf1450c07daf8538d0ac2e88da289dc8fbb690fa2ecfe8254556404d *chain.log
ffa083240bc3d46438cc47970c443c2093dc1553cacb6930b1870ab02ba5ffa7 *render_run.log
8ea171d2efe01edcc874ec58cd2789d8367c3c94685e050e339fe0eed1c13277 *build.log
```
