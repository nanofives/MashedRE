# HB-ARCTIC-OPENSEA RESULT (2026-10-27): the Arctic open sea matches the original within 2.8 luma

**Verdict: GREEN.** On four original Arctic frames with open sea in view (24% to 52% of the
frame is water), the default librw build (no fold knob) lands within **2.0 to 2.8 luma** of
the original over the water. The bar from `verify/hbarctic_20261027/RESULT_HB-ARCTIC.md`
section 3 is 4.5. This closes that file's section 3 gap ("no Arctic water pixel was
compared"). Measurement only: no game source changed.

One thing outside the water is worth a row of its own: the sky band at the top of the frame
is about 24 luma brighter on the default build than on the original (section 4). It is not
in the water mask and is not scored here.

Exe `mashedmod/build/mashed_re.exe` built from this branch's base, `orch/integration`
`2d75d00d1`, `MASHED_NO_DEPLOY=1`, SHA-256
`0e60101677b933fab7db7a4dddfc37158805fe98f53a10074fa720e22510726d`. Private root
`D:/MashedWT/HB-ARCTIC-OPENSEA_root` (robocopy of `original/`, `original/` inside). Every game
boot ran under `mashed_lock.py` and `test-slot`. No PREREG: no code was changed (the lead's
brief asked for one only in that case). The bar and the method are the ones fixed by
HB-ARCTIC before this step ran.

## 1. Why the brief's recipe could not frame the sea, and what was done instead

The brief: `race_draw_burst.py --challenge N` with a sea-dominant `--settle`. Arctic is
challenge index **3**, not 0 (`verify/arctic_ref/FINDINGS.md` table: 0 = TRAINING); every
capture here printed `ORIGINAL TRACK = ARCTIC`.

The settle does not move the camera. Seven captures (settles 20, 30, 40, 50 s, and three with
a held control) all frame the start grid. In all seven, car 0 is at exactly
`(-25.60, 0.00, 13.70)` (the camera target) while cars 1 to 3 lap the track (for example s30:
`(16.69, 0, -1.07)`, s40: `(-34.68, 0, -44.34)`). The player car never leaves the grid.

Input does not move it either. `--hold` (added here) overrides the resolver `FUN_00497310`
return for every player index. Census over a 6 s hold of control 4: only player 0 is polled,
controls 0 to 12, 356 calls each (`orig/h6/capture.log`). Forcing each of the 13 controls for
2 s (`sweepA`, `sweepB`) left car 0 at the same position. The cook `FUN_00496530` takes the
throttle and steer floats from those bytes only in config mode 2 (keyboard); in mode 1 they
come from the joypad axis (`re/analysis/bucket_input_dinput_0047b860_0049b300/00496530.md`
step 7). [UNCERTAIN] which mode player 0 is in on this machine: `0x007e95c0 + 0x13c` was not
read.

So `--pose` (added here) pins the original's own camera instead. The racing frame is not
built by `Camera::Apply` (`0x00441760`): `Camera::InitWithMatrix` (`0x00442a20`) and
`Spectator::SelectCamera` (`0x004427c0`) copy the record matrix over the RW camera frame
after it (`re/analysis/race_camera/U9058_frame_ltm_writer_20260830.md`). On leaving each of
the three, the script writes the 12 row floats of the frame's modelling (`+0x10`) and LTM
(`+0x50`) matrices with a fixed basis (frame = `*(*(0x00897fe0 + 0x84) + 4)`). The original's
renderer, world, sea pass, fog and lens are untouched. Only the vantage is chosen.

Check that the pin holds: on every pinned capture the frame LTM the capture reads back equals
the pinned basis to 5 decimals (`orig/<pose>/orig_cambasis.txt`). Between 294 and 298 frame
writes ran before the capture. The basis formula was checked first against an unpinned
original frame: from eye `(-26.001, 4.4414, 7.36908)`, azimuth 3.2165, elevation 33.3128 it
gives `orig/s20/orig_cambasis.txt` digit for digit.

## 2. Poses

Basin floor from `GRAPH.BSP` (`re/tools/track_dump.py`, min Y per 10 m cell): Y = -5.0 on a
ring around the track (X -50..-40 west, Z 40..50 north, X 20..30 east) and in an inner basin
(X -10..20, Z -30..-10). The sea sheet is at Y = -4.1 over X, Z in [-180, +120]
(`re/analysis/SEA_LEVEL_2026-09-29.md` section 4). Poses: eye (x, y, z), azimuth (0 = +Z,
90 = +X), elevation downward.

| pose | eye | azimuth | elevation | looks at |
|---|---|---|---|---|
| e1 | (10, 6, 20) | 90 | 25 | east basin, the bridge, the helicopter |
| w1 | (-28, 6, 14) | 270 | 25 | west basin past the start-grid buildings |
| w2 | (-30, 8, 0) | 270 | 35 | west basin, the pier sheds |
| n1 | (-5, 6, 30) | 0 | 25 | north basin, the bridge railing |

The standalone renders each pose from the original's read-back basis
(`MASHED_CAM_POSE`), `MASHED_TRACK_SEL=0`, `MASHED_RACE_DEMO=1 MASHED_GOTO=6`, deterministic
clock, same recipe as HB-ARCTIC's `run_render.py`. The default build logs
`SEA-TILE course_id=0 clump=2 dff=sea.dff instances=25 grid=5x5 origin=-150 step=60 y=-4.1`.

## 3. Water (same method as HB-ARCTIC sections 2 and 3)

Luma `0.299R + 0.587G + 0.114B`. Water mask M = `|L_SEA - L_OFF| > 6` on the standalone pair,
where SEA = `MASHED_LIBRW_AMBFOLD_SEA=1`. That knob changes water atomics only, so it is used
here only to find the water pixels. Score = |mean L over M, default (OFF) - original|.
`measure.py`, table `measure_table.txt`.

| pose | water mask % | original | **default** | SEA fold | **d default** | bar 4.5 | frame mean abs dL |
|---|---|---|---|---|---|---|---|
| e1 | 46.06 | 17.2 | **19.6** | 32.7 | **2.4** | PASS | 8.3 |
| w1 | 24.48 | 21.1 | **23.9** | 35.5 | **2.8** | PASS | 6.4 |
| w2 | 48.02 | 19.7 | **21.7** | 34.3 | **2.0** | PASS | 4.5 |
| n1 | 52.41 | 15.8 | **17.8** | 32.8 | **2.0** | PASS | 7.3 |

Control: e1 OFF vs OFF2, frame mean abs dL 0.000, 0.000% of pixels differ.

The default build is 2.0 to 2.8 luma brighter than the original on the water, the same sign
on all four poses. Per-pixel mean abs dL over the water is 3.5 to 4.7 (`topband_table.txt`).
The fold would put it 14.4 to 17.0 away. This matches the section 1 mechanism of HB-ARCTIC
(sea drawn as texture x prelit, no course light) and the SuperG result (5.6). It is the open-
sea counterpart of HB-ARCTIC's 4.1 / 4.5 road-region rows.

Whole frame, `re/tools/imgdiff.py` RGB (`imgdiff_summary.txt`, grid 4x3): mean abs diff 8.73
(e1), 6.87 (w1), 5.08 (w2), 7.79 (n1). The middle and bottom rows of cells, where the water
and the near geometry are, read 2.6 to 6.3. The top row reads 10.0 to 20.7 on e1, w1 and n1.
That is section 4.

## 4. Not scored: the sky band (follow-up)

Rows 0 to 79 (the countdown light box at x 200..440, rows 0..59 cut out; the standalone draws
it at the grid, the pinned original is mid-race and shows no HUD):

| pose | original | default | d |
|---|---|---|---|
| e1 | 46.8 | 70.9 | 24.0 |
| w1 | 45.0 | 69.8 | 24.8 |
| w2 | 31.3 | 35.4 | 4.0 (35 deg down, little sky in view) |
| n1 | 42.9 | 68.7 | 25.8 |

Cropped side by side (`e1_topband.png`): the original's sky is a dark cloud layer with rain
streaks and a dark ridge on the horizon. The default build's is a lighter cloud texture with
no rain. The rest of the non-water frame differs by 3.0 to 5.1 in mean luma.
`verify/rsky_20261016/RESULT_rsky.md` and U-9696 record the Arctic sky as drawn and ordered
but say "the sky pixels are not measured". This step does not decode the cause. Candidates
to check, none tested: fog on the sky pass (U-9696 says the port draws it with fog off), the
second Arctic sky draw (`0x0044937d`), and the missing rain layer. Proposed row
**HB-ARCTIC-SKY**: measure and attribute the +24 luma sky band on these four poses (the
`--pose` captures make it repeatable).

## 5. Harness changes (committed, tool only)

`re/frida/race_draw_burst.py`, both default off, so existing invocations are unchanged:
- `--hold C:S[,C:S...]`: hold control C for S seconds before the settle, every player index,
  and print the `(player:control)` census the race polls.
- `--pose <12 floats>` (pass as `--pose=...` when the value starts with `-`): pin the race
  camera frame to a basis in `MASHED_CAM_POSE` order.

`verify/hbarcticopensea_20261027/`: `capture_orig.py` (original-side runs from the private
root copy, so `run_with_unlocked_save.py` swaps only `<root>/original/gamesave.bin`),
`run_render.py`, `measure.py`, and the tables.

## 6. Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe (`0e601016`),
`A4_ROOT=D:/MashedWT/HB-ARCTIC-OPENSEA_root`, through test-slot and `mashed_lock.py`:

| gate | this exe | `merge_train.json` ref | |
|---|---|---|---|
| G-CTRL (H_DEF step / gates / ptrace) | `5943c5f1 / 364350ae / 52052890` | `5943c5f1 / 364350ae / 52052890` | **identical** |
| B (K0 / K1 / K26) | `3a03670c / e85ea6fb / 53778664` | `3a03670c / e85ea6fb / 53778664` | **identical** |

Expected: no game source changed. All four arms self-exited (rc 0). A first attempt stopped
after H_DEF because the output folder did not exist (copy error in `run_arms.py`, nothing
killed). All game processes in this step exited on their own, and none was killed.

## 7. Raw data (not committed), SHA-256

Root `D:/MashedWT/HB-ARCTIC-OPENSEA_data/`.

```
019a06149f0aab9946f87e1c572883bbb0e33e291e91a38a65eedd060970458c *orig/e1/a.bmp
24920e926cebad92760605ef59ee368e143b1c9b774ea8078f1280a57ad122b7 *orig/w1/a.bmp
98c52dac0007e2a31a3ffebec4c4a27f15745c66cf9b07521768d8f04fd6b407 *orig/w2/a.bmp
cf923781e08d1d3c2cbb8b88e35bbd9e30e05033bb7f0291e2994a9c6075527e *orig/n1/a.bmp
36ea4ce3bc661f77ce713eb98afcd8e1763ab66f9cdae68b28d01b57ceb43208 *orig/e1/orig_cambasis.txt
37ea9c4a54ce7eda04ca8e28a8948dd1c18f8147eebab0c956828304332ea0be *orig/w1/orig_cambasis.txt
b6e6dc86b44d678a9be587f5eed3b1ad8cb91770f308a70a52804186aefa69c3 *orig/w2/orig_cambasis.txt
894f337bfc1374e527d7e1b83ab4dee48a42814f273e269bda6667d7fe2c97bd *orig/n1/orig_cambasis.txt
8e1caba4cd87051040686fe56bf4d4da681852071865913c0e46301b709c0640 *render/e1_OFF/race1/01_grid.bmp
8e1caba4cd87051040686fe56bf4d4da681852071865913c0e46301b709c0640 *render/e1_OFF2/race1/01_grid.bmp
072ddfc9b3f0058dde448ec421d12ad938416aa5ec74bb68b15af4c522241e38 *render/e1_SEA/race1/01_grid.bmp
00b6405b71b96cb82e9bd5ac199d79c4ac5a2678ccdacea1fcf78f9e059208eb *render/w1_OFF/race1/01_grid.bmp
111bbcb658be5013c5a0045f0ed89bc1a6eecaa2742c956c61770933e5a2b62e *render/w1_SEA/race1/01_grid.bmp
b4ff1186b9f0d6a548646e27b25e9503db0569831bfdd73e5347fb3400a20005 *render/w2_OFF/race1/01_grid.bmp
58f4e904c6ac5d879b2ff656f3da091091a663b145a9b5312c57365ccfdcea66 *render/w2_SEA/race1/01_grid.bmp
9e7f39fa9ae39be0b9d69b88498a4bc7fbe52e50246ce858bd22319a9b4ce1bf *render/n1_OFF/race1/01_grid.bmp
ab91b7875f27d6fa434bae460298c2ae426d1f5bdd7c5a658d4f1ad32777a1b4 *render/n1_SEA/race1/01_grid.bmp
d46580068064d7793f97e5f96b83ff716d9282f1c9bd4e2c5b04c73446a4508d *sheet_poses.png
aa2ee8e6de868c34d94cbaa6fe01b221d9931bd8661f26582a2abf15e38c5b53 *e1_topband.png
85891738e1cc2add5ea1dd85b3e07f9760d8b281cfa8c68c14f0bafc8fdd3ec6 *imgdiff_e1.png
185d2095aad3e4fa393052f2481a8229c828b5322cef23984a3dbfe957af7441 *imgdiff_w1.png
f6ff80c1ff38bc5ad46852ca256290d3ed119b6ef8fea02addbcc76c64918d12 *imgdiff_w2.png
d6fcb81ffe3d587d8aa73770a158fc8f2c58d319f467d0bfa061c72e029d42aa *imgdiff_n1.png
33520dcfddfdc0355299c509e7b1713564fd81cf68580c5f8be53df1fa08dcfc *capture6_render.log
576a59bcc34ab0d2df9e8d6602ffe07245d3cd027f5c8a93f632bbd72fd90733 *save_unlock_0123.bin
c5e4c4b17c00a65765bd63996bf77a985fa71e8acc1aeba3c8f95f2feba4d4c1 *orig/s20/a.bmp
a23c85cb1a3e7ca9fcf04b4ced2f46b3565f45bb4eda2651b608eca1242c84da *orig/a14/a.bmp
290b07e76ea25fb8132a382f5b7c2e90901be5f96d4ffd6dac2bbbb8a0fff981 *orig/h6/capture.log
565b94bb08ab03e4345c607b56c35e0a7b015bf128b88fa008197a47eedce55a *orig/sweepA/capture.log
4b62ee1fa4ddf9654521266158278fdc99320b4ad457340cbfb324e3c996e309 *orig/sweepB/capture.log
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 *arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 *arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 *arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f *arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 *arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d *arms/B_DEF_K26.step.csv
52db6f0df0692dca705909233706cd842d6fa1497a2f84de342405eeb1625c10 *gates.log
7545cc3589648208dd97788a7b1fbf81bb1dbe3b35673ddbad44365b5e2f36c7 *topband.txt
```
