# RESULT HB-ARCTIC-NOLIGHT-CAP: the original's Arctic sea is the default build's sea, and NOLIGHT=4 changes no pixel

Verdict: **GREEN.** Fresh original captures on four sea-dominant Arctic poses (24% to 52% of the frame is water) put
the original's water at 15.8 to 21.1 luma. The default build reads 2.0 to 2.8 luma above it on every pose (bar 4.5,
`verify/hbarctic_20261027/RESULT_HB-ARCTIC.md` section 3). `MASHED_TERRAIN_NOLIGHT=4` renders **bit-identical** to the
default on all four poses (same SHA-256, 0.000% of pixels differ), so it matches the original exactly as well as the
default does: the knob does not reach the default renderer. The open question at `TrackRenderer.cpp:797-800` ("the
Arctic sea ... does darken and is not yet confirmed against an original") is answered for the default build: the
original draws the sea without an ambient fill, and so does the default build. L-66 (b) holds: NOLIGHT stays a dev
knob, with nothing to flip.

Branch `orch/HB-ARCTIC-NOLIGHT-CAP` off `orch/integration` @ `0e3d71a75`, LIVING-PC, 2026-11-01. Measurement only, no
game source changed, so no PREREG (same rule as HB-ARCTIC-OPENSEA). Decision source: L-66 (b), Mariano 2026-10-30
(`re/orchestrator/DECISIONS.md` line 126).

## 1. Setup

- Exe `mashedmod/build/mashed_re.exe`, built from this branch's base with `MASHED_NO_DEPLOY=1` (build OK), SHA-256
  `19e494fe89cea5116850c6420fd2eadee808661f02313c8f258c5fe7cdfe5915`.
- Private root `D:/MashedWT/HB-ARCTIC-NOLIGHT-CAP_root`: robocopy of `original\` (265 files, 482.68 MB) plus this
  worktree's `re/frida`, `re/tools`, `scripts/canonical`, so `race_draw_burst.py` and the save swap of
  `run_with_unlocked_save.py` resolve inside the root. `<root>/original/MASHED.exe` SHA-256 equals the main install's
  (`b9977dab...`).
- Original side: `run_orig.py` runs HB-ARCTIC-OPENSEA's `capture_orig.py` (Arctic = challenge index 3, Bronze Cup 1
  unlock save, settle 3 s, the race camera frame pinned with `race_draw_burst.py --pose`) once per pose. The audio check
  (`Get-PnpDevice -Class AudioEndpoint -PresentOnly`, VB-Audio required) ran right before each of the four boots, all
  OK at 12:20:12, 12:20:44, 12:21:13, 12:21:43 (`audio_checks.txt`, raw; its two earlier lines, 11:15 and 11:45,
  precede lock waits that timed out with no boot). Every capture printed `track=ARCTIC`, `PASS`, rc 0.
- Standalone side: `run_render.py` (HB-ARCTIC-OPENSEA recipe: `MASHED_RACE_DEMO=1 MASHED_GOTO=6`, deterministic clock,
  `MASHED_TRACK_SEL=0`, `MASHED_CAM_POSE` = the original's read-back basis). Arms DEF (no knob), DEF2 (repeat, control),
  NL4 (`MASHED_TERRAIN_NOLIGHT=4`). 12/12 PASS, rc 0, PIDs spawned and exited by themselves. Every DEF log carries
  `SEA-TILE course_id=0 clump=2 dff=sea.dff instances=25 grid=5x5 origin=-150 step=60 y=-4.1`.
- All 16 boots ran inside one `mashed_lock.py run` hold.

## 2. Poses

HB-ARCTIC-OPENSEA's four (`verify/hbarcticopensea_20261027/RESULT_HB-ARCTIC-OPENSEA.md` section 2): e1 (10, 6, 20)
az 90 el 25, w1 (-28, 6, 14) az 270 el 25, w2 (-30, 8, 0) az 270 el 35, n1 (-5, 6, 30) az 0 el 25. Each capture's
read-back basis `orig_cambasis.txt` is byte-identical to OPENSEA's (same SHA-256 for all four), so OPENSEA's water mask
applies pixel for pixel.

## 3. Water mask

M = `|L_SEA - L_OFF| > 6` on OPENSEA's standalone pair at the same pose (`D:/MashedWT/HB-ARCTIC-OPENSEA_data/render/<pose>_{SEA,OFF}`).
The knob that made that pair (`MASHED_LIBRW_AMBFOLD_SEA`) was deleted by HB-ARCTIC-FOLDRM, so the mask is reused, not
remade. It still fits today's frames: today's DEF water luma equals OPENSEA's OFF to 0.1 on all four poses (19.6, 23.9,
21.7, 17.8 in both).

## 4. Result (`measure_table.txt`, `measure.py`)

Luma `0.299R + 0.587G + 0.114B`, mean over M.

| pose | water % | original (today) | original (OPENSEA, 2026-10-27) | DEF | NL4 | d DEF | d NL4 | bar 4.5 | frame mean abs dL DEF / NL4 | DEF vs NL4 pixels differ | DEF vs DEF2 pixels differ |
|---|---|---|---|---|---|---|---|---|---|---|---|
| e1 | 46.06 | 17.0 | 17.2 | 19.6 | 19.6 | 2.6 | 2.6 | PASS / PASS | 4.1 / 4.1 | 0.000% | 0.000% |
| w1 | 24.48 | 21.1 | 21.1 | 23.9 | 23.9 | 2.8 | 2.8 | PASS / PASS | 2.5 / 2.5 | 0.000% | 0.000% |
| w2 | 48.02 | 19.6 | 19.7 | 21.7 | 21.7 | 2.0 | 2.0 | PASS / PASS | 3.6 / 3.6 | 0.000% | 0.000% |
| n1 | 52.41 | 15.8 | 15.8 | 17.8 | 17.8 | 2.0 | 2.0 | PASS / PASS | 2.9 / 2.9 | 0.000% | 0.000% |

- The original's sea is dark: 15.8 to 21.1 luma, repeatable to 0.2 against the 2026-10-27 captures (n1 is
  byte-identical, e1 / w1 / w2 differ elsewhere in the frame, mid-race cars).
- The default build is 2.0 to 2.8 above it, same sign on all four, within the 4.5 bar. Same numbers as OPENSEA.
- NL4 = DEF to the byte. The whole-frame dL fell from OPENSEA's 4.5-8.3 to 2.5-4.1, which is the sky (HB-ARCTIC-SKY-FLIP
  landed since), not the water.

## 5. Why NL4 cannot move the default frame (static, port source)

The value 4 is read in one place, `LightAtomicVertex` (`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:782`), whose
only caller is `BuildDffBatches` (`:927`, the call at `:989`). That builds the legacy D3D9 vertex buffers. The default
renderer is librw, where the sea is drawn as texture x prelit through `RwSceneBuild::BuildClump` with no fill (the
HB-ARCTIC mechanism: in the original, `0x004ea4e0` turns lighting off for geometry without `rpGEOMETRYLIGHT`, and
`SEA.DFF` lacks it). The comment block at `:755-800` already records mode 4 as "NOT THE SITE" for TRAINING. This step
measures the same for the Arctic sea. What it does NOT settle: how the legacy D3D9 path (fill on vs mode 4) compares
with the original. That path is not the default, and a search of `mashedmod/src/mashed_re` for a renderer switch (`MASHED_LIBRW*`, `MASHED_RENDERER`, `MASHED_LEGACY`) found none, so it was not rendered.

## 6. Proposed follow-up (not done here, no source touched)

Update the stale sentence at `TrackRenderer.cpp:797-800` in the next row that edits that file, to cite this RESULT:
"the Arctic sea ... does darken and is not yet confirmed against an original" -> the original draws the sea dark
(no fill) and the default librw build matches it within 2.8 luma (`verify/hbarcticnolight_20261101`); mode 4 acts only
on the legacy batches. A comment-only edit still makes the merge train run a source pass, so it is left out of this
evidence-only branch.

## 7. Gates

Not run: no game source changed (scripts and evidence only). The exe is the `orch/integration` exe.

## 8. Raw data (not committed), `D:/MashedWT/HB-ARCTIC-NOLIGHT-CAP_data`

```
067095042cd7bd866b8839287fe0e8f0320d04d16b3e4a5858695eed32c24687 *orig/e1/a.bmp
ee9554a68c58e8a8cd4249fea31707cd17639a5de21b26418e515b79ddebab7f *orig/w1/a.bmp
fd167d19bd66a072e32cb6160b871e77ade040e91898ac8019fee57a6ccc594d *orig/w2/a.bmp
cf923781e08d1d3c2cbb8b88e35bbd9e30e05033bb7f0291e2994a9c6075527e *orig/n1/a.bmp
36ea4ce3bc661f77ce713eb98afcd8e1763ab66f9cdae68b28d01b57ceb43208 *orig/e1/orig_cambasis.txt
37ea9c4a54ce7eda04ca8e28a8948dd1c18f8147eebab0c956828304332ea0be *orig/w1/orig_cambasis.txt
b6e6dc86b44d678a9be587f5eed3b1ad8cb91770f308a70a52804186aefa69c3 *orig/w2/orig_cambasis.txt
894f337bfc1374e527d7e1b83ab4dee48a42814f273e269bda6667d7fe2c97bd *orig/n1/orig_cambasis.txt
8fb2c8254be797b6b30a7c889f4d41097f98d1103d0454a8c96b805977d6c1e9 *render/e1_{DEF,DEF2,NL4}/race1/01_grid.bmp
c89cb38e0d852ce9f206898651ce6471f51a8a285dcf0bec582e92d4db3af24c *render/w1_{DEF,DEF2,NL4}/race1/01_grid.bmp
cf1fc1d95ef0fe933d10e1cbd1bafc6917a4cfa2bb8fca66d16e144d8e8a7320 *render/w2_{DEF,DEF2,NL4}/race1/01_grid.bmp
851a645681be19991f6fe21c3c8cfece4867da6068f0a9ff29b2918350f0c0ca *render/n1_{DEF,DEF2,NL4}/race1/01_grid.bmp
57b3301dd8c463eea8f081130b669a9bd88079021293c9ddc1bb7da75d7115b3 *audio_checks.txt
656ee687352887bfb4c8f3c9096e9a09b02cd5643312786c92917dc1b188320f *run_all.log
079a6492bfa990e6ac7faa3dfd34a88da1fc05725b672fa64b4fa30dca8a9b46 *build.log
576a59bcc34ab0d2df9e8d6602ffe07245d3cd027f5c8a93f632bbd72fd90733 *save_unlock_0123.bin
```

Scripts here: `run_orig.py`, `run_render.py`, `measure.py`. Mask source: `D:/MashedWT/HB-ARCTIC-OPENSEA_data/render/`
(hashes in `verify/hbarcticopensea_20261027/RESULT_HB-ARCTIC-OPENSEA.md` section 7).
