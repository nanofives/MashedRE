# HB-ARCTIC-SKY RESULT (2026-10-27): the +24 luma Arctic sky band is sky copy A, which the original renders off-screen

**Verdict: GREEN (knob ships default OFF, the flip is a decision row).** The default build draws the
second Arctic sky copy (copy A, `diag(-1,-1,1)`, `0x0044937d`) into the race frame. The original
draws it, and the big lens batch, through a separate 128x128 camera-texture camera, so on the race
frame that band shows the clear colour (Arctic fog colour, 40/44/48). With
`MASHED_ARCTIC_SKY_RTT=1` the band lands **0.6 / 2.5 / 2.4 luma** from the original on e1 / w1 / n1
(default 24.0 / 24.8 / 25.8). Copy A carries all of it: the copy-A-only arm is byte-identical to K1
on 3 of 4 poses, and the lens-only arm moves one pixel on one pose.

- PREREG `verify/hbarcticsky_20261027/PREREG.md`, commit `eb0268577`, alone, before any build or arm.
- Code + driver + scorer: commit `6ea75ab5e` on `orch/HB-ARCTIC-SKY` (base `orch/integration` `37383a4bb`).
- Exe `D:/MashedWT/HB-ARCTIC-SKY_data/mashed_re.exe` (copy of `mashedmod/build/mashed_re.exe`,
  `MASHED_NO_DEPLOY=1`), SHA-256 `978284560bddc219b55f54dd61a923f5e2b9ca29d820514f88ad20ce1d8adc7d`.
  Private root `D:/MashedWT/HB-ARCTIC-SKY_root` (robocopy of `original/`, `original/` inside). Every
  boot ran under `mashed_lock.py` and `test-slot`. No new original-side run: the original frames are
  HB-ARCTIC-OPENSEA's pinned captures.

## 1. What the original does (decomp_pc.py, pool 10, read-only)

`FUN_004492b0` = Arctic record `"arctic"` slot 4 (`0x005f34ac`), the sky pass:

| addr | what |
|---|---|
| `0x004492b4`..`0x004492c3` | local dword `0xff605850` (bytes 0x50, 0x58, 0x60, 0xff) |
| `0x004492df` | `FUN_004c1c80(FUN_004840d0(0), FUN_004671a0(0) + 0x68)`: writes camera `+0x68`/`+0x6c` from the race camera's `+0x68` (`FUN_004671a0` returns `DAT_006905b0` outside mode 3) |
| `0x004492e6` | `FUN_00451060(0)`: `FUN_004c1480(*(FUN_004840d0(0) + 4), FUN_00467210(0,0))` (`FUN_00467210` returns `*(DAT_006905b0 + 4) + 0x10`) |
| `0x004492f0`..`0x00449300` | `FUN_004c1bb0(FUN_004840d0(0), &0xff605850, 1)`, which calls `[DAT_007d3ff8 + 0x9c]` with the same three arguments |
| `0x0044931f` | `FUN_004491e0(course)`: sky slot frames, `Rx 180` then `Ry 180`, translate to camera frame `+0x40` |
| `0x00449325`..`0x00449335` | `FUN_004c1a00(FUN_004840d0(0))` = `(*(cam + 0x18))()`, the rest runs only if nonzero |
| `0x0044933d`..`0x0044937a` | RW render states (0xc,0) (0xa,5) (0xb,6) (0x6,0) (0x8,0) |
| `0x0044937d` | `FUN_004e6680(*(course + 0x100f0))` = **sky copy A** |
| `0x00449393` | `SkySecondaryDispatch` `0x00489240` = **the big lens batch** |
| `0x004493a4` | `FUN_004e6680(*(course + 0x10130))` (`windows.dff`, cull state 0x14 = 2 around it) |
| `0x004493fc`..`0x00449407` | `FUN_004c19f0(FUN_004840d0(0))` = `(*(cam + 0x1c))()` |

The camera: `FUN_004840d0(n)` returns `DAT_006ce81c[n]` (`0x004840dc`), whose only writer is
`FUN_00484000` (`0x0048404b`), called once, from Arctic slot 0 at `0x00448979` as
`FUN_00484000(0, 0x80, 0x80, 0x600)`. `FUN_00483ed0` builds it:

- `FUN_004c77c0(w, h, 0, flags | 5)` stored at camera `+0x60`, `FUN_004c77c0(w, h, 0, 1)` at `+0x64`,
- `FUN_004c1a40(cam, 0x3dcccccd)` writes `+0x80` (0.1), `FUN_004c1b10(cam, 0x43960000)` writes `+0x84` (300.0),
- `FUN_004c5a00(<the +0x60 raster>)`, its byte `+0x50` set to 2, stored to `DAT_006ce82c[0]`.

So copy A, the big lens batch and `windows.dff` are drawn through a second camera whose `+0x60`
buffer is a 128x128 raster created with type bits `| 5`, and copy B (generic path,
`FUN_00426d20` / `FUN_00426d90` from `FUN_004270f0`) is the only sky copy drawn through the race
camera. [UNCERTAIN] what reads `DAT_006ce82c[0]`: `0x00448986` passes `FUN_004840b0(0)` to
`FUN_0049a0a0`, a 3-way dispatch on `DAT_00773968` to `FUN_0045b350` / `FUN_0049a740` /
`FUN_0049a2e0`, not decoded here. Also not read: the RW function behind `[DAT_007d3ff8 + 0x9c]`.

Side note for the trackers (not edited here): the notes `re/analysis/breadth_unmapped_0048x/0x00483ed0.md`,
`0x00484000.md`, `0x004840b0.md` (D-10584, "0 callers", "physics joint pool") and `0x004840d0.md`
describe this pool as a physics joint / body pool. The bytes above show a camera and texture pool,
reached from `0x00448979` (Arctic slot 0, a table call Ghidra lists as `(data)`). Correct through
`re-classify`.

## 2. Why that is the band

- Original pixels (`D:/MashedWT/HB-ARCTIC-OPENSEA_data/orig/<pose>/a.bmp`), 10-row means, countdown
  box cut: rows 20..69 on e1, w1 and n1 are RGB (40,44,48) to (42,47,50). (40,44,48) is
  `Setup_Fog(0.1,70,40,44,48)` (Arctic `COURSE.LUA` line 14). The port clears the race frame to the
  fog colour too (`exe_main.cpp:3498`, `g_track.fog_color()`).
- `SKY.DFF` (Arctic.piz): flags `0x1000f` (prelit, unlit), white prelit, alpha 0 on the lowest ring
  (y -0.094, r 1.79..1.86), 127 on y 0.031, 255 on y 0.229..0.36. Texture `sky` 256x256, mean luma 90.0.
- The poses look 25 (e1, w1, n1) or 35 (w2) degrees down with a 48.46 degree vertical FOV
  (`orig_lens.json`), so rows 0..79 are below the eye. Copy B's alpha-0 ring sits about 3 degrees
  below the eye, so copy B barely reaches row 0. Copy A (y flipped) puts its alpha-255 rings below
  the eye and covers the band with the sky texture. Without copy A the race frame shows the clear colour.
- The original's own d1r1 draw list (`log/d1r1/bt_arctic/orig_row_01044.draws.json`) records copy A
  (`bt ... 0x44937d`), the 1568-triangle lens batch and the 68-triangle `World` mesh (`0x4493b0`)
  with Z test off, fog off, lighting off, SRCALPHA / INVSRCALPHA. The recorder does not log the
  render target, so that list alone did not separate the two cameras. The decomp above does.

## 3. Arms (`measure.py`, `measure_table.txt`)

Band = rows 0..79 minus the countdown box (x 200..440, rows 0..59). Sub-band = rows 20..69, same cut.
Rest = rows 80..479, mean abs luma difference against DEF.

| pose | original band | DEF | K1 | KA | KL | d DEF | **d K1** | sub-band orig / DEF / K1 | rest K1 vs DEF |
|---|---|---|---|---|---|---|---|---|---|
| e1 | 46.8 | 70.9 | 47.4 | 47.4 | 70.9 | 24.0 | **0.6** | 45.4 / 72.4 / 45.4 | 0.348 |
| w1 | 45.0 | 69.8 | 47.4 | 47.4 | 69.8 | 24.8 | **2.5** | 43.5 / 71.7 / 45.4 | 0.000 |
| w2 | 31.3 | 35.4 | 35.4 | 35.4 | 35.4 | 4.0 | **4.0** | 32.1 / 35.1 / 35.1 | 0.051 |
| n1 | 42.9 | 68.7 | 45.3 | 45.3 | 68.7 | 25.8 | **2.4** | 42.4 / 74.8 / 44.4 | 0.352 |

K1 = `MASHED_ARCTIC_SKY_RTT=1` (copy A and the after-sky lens batch out of the race frame),
KA = `=a` (copy A only), KL = `=lens` (lens batch only).

- Control: e1 DEF vs DEF2 byte-identical.
- KA vs K1 byte-identical on e1, w1, w2. On n1 they differ in one pixel (118, 441) by 5 summed RGB,
  the same pixel KL moves against DEF. KL is byte-identical to DEF on e1, w1, w2. So the lens batch
  has no visible share on these poses.
- w2 (35 degrees down) has little sky in view, and K1 does not move its band. Its 4.0 is outside this cause.
- The rest of the frame moves at most 0.352 (the band below row 80 where nothing else is drawn).
- Crop strip original / DEF / K1, e1 rows 0..119: `D:/MashedWT/HB-ARCTIC-SKY_data/e1_band_orig_def_k1.png`.
  K1's band is the flat fog colour with faint cloud in the top rows, as on the original. The original
  also shows rain streaks, which no arm draws (separate gap, `RainEnable()` in Arctic `COURSE.LUA` line 162).

## 4. Predictions (PREREG.md)

- P1 (K1 band d <= 6 on e1, w1, n1): **met**, 0.6 / 2.5 / 2.4.
- P2 (KA >= 80% of K1's reduction): **met**, 100% (KA = K1 to the pixel on e1 / w1, one pixel on n1).
- P3 (K1 rest <= 1.0): **met**, max 0.352.
- P4 (gates unchanged on the knob-unset exe): **met** (section 5).
- P5 (DEF vs DEF2 byte-identical): **met**.

## 5. Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe (`97828456`, knob unset),
`A4_ROOT=D:/MashedWT/HB-ARCTIC-SKY_root`, through test-slot and `mashed_lock.py`:

| gate | this exe | `merge_train.json` ref | |
|---|---|---|---|
| G-CTRL (H_DEF step / gates / ptrace) | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | **identical** |
| B (K0 / K1 / K26) | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | **identical** |

P4 **met**. All four arms self-exited (rc 0). All 21 game processes in this step (17 render arms, 4
gate arms) exited on their own, none was killed.

## 6. What this does not do

- It does not build the 128x128 camera, its render, or its consumer. With the knob on, copy A, the
  big lens batch and `windows.dff` are simply not in the race frame, which is where the original
  does not put them either. Whatever surface samples `DAT_006ce82c[0]` still lacks it ([UNCERTAIN],
  section 1).
- `windows.dff` (`course + 0x10130`): Arctic is not a hook-slot covered track in the port
  (`HOOKSLOT course_id=0 covered=0`), and this step did not check whether the port draws clump 8 in
  the race frame. Not touched.
- Rain streaks: not ported, not scored.
- No original-side capture was made in this step.

## 7. Decision for the user

Flip `MASHED_ARCTIC_SKY_RTT=1` to the default (sky band 24 to 26 luma -> 0.6 to 2.5 off the
original on three poses, the rest of the frame moves at most 0.35, gates unchanged), or keep it opt-in
until the 128x128 camera texture is ported? Recommendation: flip it. The original never draws those
two batches into the race frame, so the default build is drawing something the original does not.

## 8. Raw data (not committed), SHA-256

Root `D:/MashedWT/HB-ARCTIC-SKY_data/`.

```
978284560bddc219b55f54dd61a923f5e2b9ca29d820514f88ad20ce1d8adc7d *mashed_re.exe
8e1caba4cd87051040686fe56bf4d4da681852071865913c0e46301b709c0640 *render/e1_DEF/race1/01_grid.bmp
8e1caba4cd87051040686fe56bf4d4da681852071865913c0e46301b709c0640 *render/e1_DEF2/race1/01_grid.bmp
a64f6dee0babcb153f1f4381a92cdb175a9c6cd276ea82b67824e79b95d39be5 *render/e1_K1/race1/01_grid.bmp
a64f6dee0babcb153f1f4381a92cdb175a9c6cd276ea82b67824e79b95d39be5 *render/e1_KA/race1/01_grid.bmp
8e1caba4cd87051040686fe56bf4d4da681852071865913c0e46301b709c0640 *render/e1_KL/race1/01_grid.bmp
9e7f39fa9ae39be0b9d69b88498a4bc7fbe52e50246ce858bd22319a9b4ce1bf *render/n1_DEF/race1/01_grid.bmp
5e9b3c3b88c88c997a5f7ae5ae9a2b9d419bc761e2f7aba2b893c1d44ecb5315 *render/n1_K1/race1/01_grid.bmp
64d27df2f607e649d3c8909573f5685b9242fcb998a1b8849043678274444f80 *render/n1_KA/race1/01_grid.bmp
f2c83df73707026895dd3425acfa0304cc7a1799672d56508d71288791278bc7 *render/n1_KL/race1/01_grid.bmp
00b6405b71b96cb82e9bd5ac199d79c4ac5a2678ccdacea1fcf78f9e059208eb *render/w1_DEF/race1/01_grid.bmp
486421b502f53bbb70c2a0da80081cec861b5a623eb3c51769c619dff1c45100 *render/w1_K1/race1/01_grid.bmp
486421b502f53bbb70c2a0da80081cec861b5a623eb3c51769c619dff1c45100 *render/w1_KA/race1/01_grid.bmp
00b6405b71b96cb82e9bd5ac199d79c4ac5a2678ccdacea1fcf78f9e059208eb *render/w1_KL/race1/01_grid.bmp
b4ff1186b9f0d6a548646e27b25e9503db0569831bfdd73e5347fb3400a20005 *render/w2_DEF/race1/01_grid.bmp
0d385d85244dc3bdfb8d116960a1ef227818387b30ba4c1933fd045f798b0947 *render/w2_K1/race1/01_grid.bmp
0d385d85244dc3bdfb8d116960a1ef227818387b30ba4c1933fd045f798b0947 *render/w2_KA/race1/01_grid.bmp
b4ff1186b9f0d6a548646e27b25e9503db0569831bfdd73e5347fb3400a20005 *render/w2_KL/race1/01_grid.bmp
4a31d0ed73c4d775a6f9cec038a202bff777a341b151b3b9c4bfca4837d32c53 *e1_band_orig_def_k1.png
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 *arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 *arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 *arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f *arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 *arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d *arms/B_DEF_K26.step.csv
2042a8f6200cbba731d4818be4a9f26acee6621a5279a13fbb5491532d0070f8 *gates.log
bb8ed92b31b121d91f3018728e41f344dd14520ac00d65cefef7ca03e3294804 *render_run.log
479c0ce96ef32325f2241f33a562d909ce629ab4d38dabc7fb8f660bdfbb2805 *build.log
```

## 9. Reproduce

```
cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"
robocopy <main>\original D:\MashedWT\HB-ARCTIC-SKY_root\original /E
py -3.12 verify/hbarcticsky_20261027/run_render.py <exe> D:/MashedWT/HB-ARCTIC-SKY_root D:/MashedWT/HB-ARCTIC-SKY_data e1_DEF e1_DEF2 e1_K1 e1_KA e1_KL w1_DEF ... n1_KL
py -3.12 verify/hbarcticsky_20261027/measure.py D:/MashedWT/HB-ARCTIC-SKY_data
A4_ROOT=D:/MashedWT/HB-ARCTIC-SKY_root A4_OUT=D:/MashedWT/HB-ARCTIC-SKY_data/arms A4_EXE=<exe> py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
```
