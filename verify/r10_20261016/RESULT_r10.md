# RESULT r10: D1 breadth, matched-pose draw-list adjudication on the ten capfix tracks (closes U-9702)

**Verdict: U-9702 closes. All ten tracks x 3 rows are replayed on the default build and scored; every divergence row is
cited with a cause group.** Measurement only: no renderer, physics or AI code changed, so the default build and its G-CTRL
hashes are unchanged by construction (G-CTRL was not re-run).

- **Scoring runs.** First run, unmodified union of the sibling cites (the pre-registered file): **159 RED rows on 30 of 30
  frames**. After the cite groups of this lane (v3): **979 rows, 979 cited, 0 RED on 30 of 30 frames**. Those groups were written
  after seeing the first run, so the 0 is an assignment, not a prediction (section 4 lists every change).
- **Second sample.** An independent second original capture of the same ten tracks (the backtrace run, new race, different
  race-start rows) was replayed and scored with the same cites: v2 left **4 RED rows in 4 of 30 frames**, all instances of
  groups already defined for the first sample; v3 widens three groups and gives **979 rows, 0 RED on 30 of 30**. v3's three
  widenings were made after seeing those four rows, so sample B is out of sample for v2 only.
- **Gates.** 58 of 60 frames pass every instrument gate (29 of 30 in each sample). Neustein row +105 fails I-NAME in **both** captures
  (73 of 77 textured draws named = 94.8 %, threshold 95 %); the four unnamed draws are one `#64x64:21` runtime texture. Not
  relaxed, see section 2 and the decision at the end.
- **Cause status.** 120 of 979 rows (12 %) of sample A, 115 of 979 of sample B, sit in groups marked OPEN (the row is explained
  only as far as its note says, a U-row carries the rest). Six new U-rows: **U-9713..U-9718**.

Base `0006dbf2d` (`race/first-frame-parity`), branch `work/r10`, worktree `.worktrees/r10`. Pre-registration
`PREREG_r10.md` (committed `da41e8a41` before the first replay). Standalone: clean build of the base in this worktree,
`mashedmod\build\mashed_re.exe`, SHA-256 `18c470b215ddddf4eef2153fe5ea907de12b2a4e09eab141c38c81cc1aace4cd`.

## 1. What ran

| step | tool | output |
|---|---|---|
| replay x 10 tracks (capfix traces) | `run_tracks.py` -> d1r1 `run_replay3d.py` (one deterministic round, `MASHED_CAM_REPLAY_BASIS=1`, `MASHED_CAR_REPLAY=1`, `MASHED_DBG_D3DREC=1`, no opt-in knobs) | `<NN_Track>/stand/d1r2/stand_row_<K>.draws.json` |
| score | `score_tracks.py` -> `re/tools/drawlist_diff.py` 3D mode (`--trace`, `--cite`) | `<NN_Track>/diff{0,1,2}_<K>.{json,txt}`, `scores_*.txt`, `gates_*.txt` |
| render sites on the original | `bt_capture.py` -> `camera_probe.py --drawrec-bt` (new original run per track, 34 textures; `bt2/` adds Sands and Warzone for `OakTreeSprite`) | `bt/<NN_Track>/orig_row_*.draws.json` with `bt` chains, `bt_summary.py`, `bt_slots.py` |
| asset ownership | `tex_owner.py` (texture name inside the track piz's DFFs / BSPs), `course_lua/` (COURSE.LUA of 13 tracks) | `tex_owner_diff0.json` |
| second sample | `run_tracks.py --cap bt --out bt_rerun`, `score_tracks.py --cap bt --work bt_rerun` | `bt_rerun/` |
| tables | `report_tables.py`, `audit_cites.py` | `tables_r10.md` |

Every replay passed first time (22 of 22 including the two determinism-control replays, no piz-collision re-run). The instrument gates are d1r1's (`check_gates.py`):
I-COVER 30/30 and I-FRAME 30/30 on the original side of each sample, I-SYNC 60/60 (camera deviation from the trace's
`M_*` at most 3.7e-7, `cam_src` 4), standalone I-NAME 60/60.

## 2. Instrument gates

- **I-N0 (determinism).** Egypt and Warzone replayed a second time (`n0/`): 6 of 6 draw records identical to the first replay.
- **I-NAME, Neustein +105.** capfix row 1169 and the re-capture's row 825 both read 73 of 77 named. The four unnamed
  draws are `#64x64:21` strips (render chain `0x004e11d9 <- 0x004d4129 <- 0x004cd3d7 <- 0x00422a5e`, the C10 site). The differ keys
  an unnamed texture by `#WxH:format`, so the rows of that frame are not affected by the missing name. The frame is scored and
  flagged; PREREG said a frame failing a gate is re-run and left UNSCORED if it never passes. The re-run (the second capture)
  fails the same way, so strictly that frame stays unscored and U-9702 closes on **10 of 10 tracks, 29 of 30 gate-clean rows
  in sample A and 29 of 30 in sample B**.

## 3. Per track (sample A = capfix captures; sample B in `tables_r10.md`)

"matched" = draws of the original's frame that find a standalone draw of equal texture, triangle count and world AABB
(0.01). Rows: first run (unmodified union) -> final (v3). GREEN rows: none, every frame has divergence rows.

| track | matched draws (3 rows) | matched triangles | rows | RED first run -> final |
|---|---|---|---:|---|
| Egypt | 150/186, 143/175, 81/116 | 92.6, 95.9, 92.5 % | 67 | 2 -> 0 |
| Neustein | 103/258, 99/255, 54/210 | 78.9, 79.5, 67.7 % | 67 | 12 -> 0 |
| Highway | 127/172, 118/164, 72/131 | 89.7, 89.0, 83.3 % | 70 | 7 -> 0 |
| Sands | 278/334, 278/337, 226/305 | 87.9, 87.5, 79.5 % | 88 | 11 -> 0 |
| SuperG | 333/369, 333/370, 174/224 | 91.8, 91.7, 89.1 % | 210 | 11 -> 0 |
| Roundabout | 295/359, 272/324, 197/250 | 83.2, 90.0, 88.6 % | 120 | 43 -> 0 |
| Storm | 227/252, 223/249, 154/179 | 95.1, 95.3, 94.2 % | 69 | 3 -> 0 |
| Forest | 123/180, 119/176, 71/116 | 70.1, 70.1, 68.5 % | 96 | 25 -> 0 |
| Dump | 330/368, 313/345, 202/234 | 90.3, 92.5, 89.1 % | 63 | 5 -> 0 |
| Warzone | 175/364, 175/353, 133/336 | 87.3, 88.4, 74.7 % | 129 | 40 -> 0 |
| **all** | 5578 of 7691 (72.5 %) | 575,881 of 664,726 (86.6 %) | 979 | 159 -> 0 |

Sample B: 5500 of 7620 draws (72.2 %), 569,418 of 658,010 triangles (86.5 %), per-frame 68.6 to 95.6 %. The lowest tracks are
Forest (68.5 to 70.1 %, U-9715) and Neustein (67.7 to 79.5 %). For reference d1r1's three tracks matched 77.7 to 91.5 %.

## 4. Cause groups

IDs continue d1r1's C01..C21. "Rows" = sample A / sample B. MAPPED = the cause is traced to code or data (here or in a sibling
lane). OPEN = explained as far as the note says, a U-row carries the rest. Full notes with every address:
`cites_r10_v3.json`. Per-track counts: `tables_r10.md`.

| id | status | rows | tracks | cause | U-row |
|---|---|---:|---|---|---|
| C10 (widened) | OPEN | 35 / 34 | 9 | `#64x64:21` runtime strips: **same chain as TRAINING's `#256x256:21`**, `0x004e11d9 <- 0x004d4129 <- 0x004cd3d7 <- 0x00422a5e` (27 draws) | U-9691 |
| C16 (widened) | MAPPED | 12 / 12 | SuperG, Storm | two-stage world: SuperG `Ice`, `IceRoad1/2` chain `0x004cb321 <- 0x0049aa7a` (the sector loop of C16's note); Storm `water` MatFX DUAL `0x00540d4a <- 0x005436ca` | U-9692 |
| C07 (widened) | MAPPED | 41 / 40 | 7 | `machinegun`, `shotgun`, `depthcharge`: drawn by `FUN_00458b10` (0x00458b10), the function that also draws C06's `PUGlow` (`0x00458b3f`); `shotgun` was not in the backtrace run, matched by name | none |
| C11b (new) | MAPPED | 6 / 6 | Sands, Forest | static `wet_watercol00` where the original rebinds `waterNN` (C11): draws, triangles and AABB equal in 6 of 6 frames (`evidence_water_pairs.txt`) | none |
| C01c (new) | MAPPED | 26 / 28 | Roundabout, Forest, Dump, Warzone | original draws whose AABB lies wholly outside the frustum (A-side twin of C01); rasterise nothing | none |
| C23 (new) | MAPPED | 12 / 12 | Neustein, Roundabout, Warzone | `Sky_Filename`: only the last of two sky slots is kept (`TrackRenderer.cpp:2121-2135`, `:1882`); the one-argument form is not parsed. Draw lists: Neustein original `skyCyl`+`skyCylTop`, standalone `skyCyl`; Warzone `horizon`+`sky` vs `horizon`; Roundabout `Sky3`,`Sky4` vs no sky | U-9714 |
| C24 (new) | MAPPED | 10 / 11 | Neustein, Sands, Roundabout, Warzone | game-code RwIm3D / DrawPrimitive effects with no standalone draw site: leaf sprites `TreeSprite` / `OakTreeSprite` (`0x0047cd47` from slot 10 of the track's record: `0x0044af75`, `0x00450a4b`, `0x00452dee`), `wfall` (`FUN_0048e7b0`, also C04's `flatshad`), `flames3` (`FUN_00475e50`), `barrel_shad` / `crane_shad` (`FUN_0044bd50` / `FUN_0044be70` from Roundabout slot 2) | U-9701 |
| C04b (new) | MAPPED | 1 / 3 | Warzone, Roundabout | object-shadow stand-in: AABB 0.0281 off (Roundabout), 10 triangles vs 6 (Warzone) | U-9710 |
| C22 (new) | OPEN | 50 / 46 | Warzone, Roundabout, Sands, Highway | copies of `Clump_Exclude_From_World` clumps placed and drawn by the original's per-track hook-table slots; the standalone has one instance per clump and ports slots only for Arctic. Warzone trunk 30 vs 1 (at the origin), fence 16 vs 2, road 103 vs 10; Highway `traffic3` 18-29 vs 96 (92 at the origin) | U-9713 |
| C23b (new) | OPEN | 20 / 20 | Forest | `Object_Bsp_Filename(0,"Trees.bsp")` not loaded (0 hits in `mashedmod/src`); excluded tree clumps drawn once at the origin vs up to 6 placed copies | U-9715 |
| C26 (new) | OPEN | 6 / 7 | Roundabout, Warzone, Neustein | `RWP_Object` instances (Sign02, Trucktyre, Hut01 + walls, Post_1): standalone has no `Physics_Scenario_*` handling | U-9716 |
| C28 (new) | OPEN | 2 / 1 | Dump | `Train.dff` driven by `SetClumpAnim(LINEAR,0,3)` + `Train.anm`, 0 hits in `mashedmod/src`, standalone draws no `train` | U-9717 |
| C25 (new) | OPEN | 2 / 1 | Neustein, Warzone, Sands | car part drawn by the original, not by the standalone (AdvantageSmall, Glass) | U-9706 |
| C27, C29 (new) | OPEN | 5 / 6 | Dump, SuperG, Egypt, Neustein | car-part AABB offsets 0.012; Egypt B-only 6-triangle patch; Neustein thin mesh at the origin | U-9718 |
| C12, C15 (restricted) | MAPPED | 0 | | now City / Arctic only: their notes name those tracks' mechanisms, and matching by texture name alone had cited Forest `bark`, Warzone `sky` and Sands / Warzone `OakTreeSprite` wrongly | |

Sibling cites kept as they were, with their signatures checked on the new tracks (`audit_cites.py`): C13 camera-man MOVED rows
are 0.014 to 0.223 (21 rows, none above 0.25), C14 copter MOVED 0.04 to 0.20, C20 sky STATE rows all differ in `Z,ZF`.
The `Clump_Mts_Pair` copies that rprops placed match the original: Sands `bark` (OakTrunk), Storm `Alpha` (Bush) and
Roundabout `shrub` / `Rock2` appear only in matched-draw rows (C21 order, C19 state: bark 153 draws, Alpha 16, shrub 40,
Rock2 9); Highway's Lampost raises no row.

**How the cites changed (nothing hidden).** `cites_r10.json` = union of the five sibling files, committed before the first
replay; first scoring `diff0_*`: 159 RED. `cites_r10_v2.json` = the groups above after reading those rows (`make_cites_v2.py`),
scoring `diff1_*`: 0 RED on sample A, 4 RED on sample B. `cites_r10_v3.json` = three widenings after reading sample B's four
rows (`make_cites_v3.py`): C25 for any track, C27 and C04b new (they took Dump / SuperG car parts and the RWObjShad rows out of
C29): scoring `diff2_*`, 0 RED on both samples.

**C03-UNTEXTURED is looser than its note.** The sibling cite covers every untextured MISSING row. In these rows 574 (A) /
576 (B) draws are cited; 8 / 10 of them are outside the four documented signatures (Neustein 366 draws of 14 triangles,
Dump 3, Warzone 4 to 6): all are off-screen except one Dump draw (untextured, 32 triangles, depth test on, opaque) in each
sample. Reported here, not given a group.

## 5. Found on the way

1. **`Sky_Filename` defect (U-9714).** A real standalone bug with a one-screen fix: multi-slot skies collapse to the last slot
   and Roundabout (one-argument form) has no sky at all. Three tracks, 12 rows.
2. **Forest has an object BSP (U-9715).** `Object_Bsp_Filename` appears in one of the 13 COURSE.LUAs; the standalone does not
   know the command. It is the lowest-matching track (68.5 to 70.1 % of the original's triangles).
3. **Per-track slot 10 is the leaf-sprite pass** on Neustein, Sands and Warzone (`0x0047cd47` reached from `0x0044af75`,
   `0x00450a4b`, `0x00452dee`); Highway slot 2 and Roundabout slot 2 draw the lorries and prop shadows. The excluded-clump
   rows are the same family: `tex_owner.py` shows every C22 texture is owned by an excluded clump.
4. **Pickup function.** `FUN_00458b10` draws both the glow (C06) and the weapon model (C07): porting one covers the other's site.
5. **`verify/capfix_20261016/run_capture.py` deletes every file in its output directory on each attempt**
   (`f.unlink()` loop at the top of the attempt). Re-running it would erase the capfix evidence; `bt_capture.py` writes to its own tree.
6. The capfix traces carry race-start rows that differ between captures of the same track (Egypt 710 vs 664, Neustein 1064 vs 720):
   a replay needs its own trace's shot rows, which `run_tracks.py` reads from `summary.json`.

## 6. Predictions (PREREG_r10.md)

- **Pr-1** (most rows in existing groups, RED on at least 3 of 10 tracks with the unmodified union): **confirmed, understated.** RED on 10 of 10 tracks.
- **Pr-2** (matched triangle fractions 77 to 91 %): **partly refuted.** 67.7 to 95.9 % per frame (Forest, Neustein 1169 below; Egypt, Storm above).
- **Pr-3** (no gate failure beyond piz collisions): **refuted** by Neustein +105 (I-NAME 94.8 %); no piz collision occurred.

## 7. Harness audit (task 2): no edit needed

`camera_probe.py` was the only script that built the request path relative; capfix fixed it. The five named scripts already
resolve every path they hand to the shim (the game's cwd is `original\`):

| script | request path | content written to the request |
|---|---|---|
| `re/frida/race_draw_burst.py` | `out_bmp = Path(args.out).resolve()` (:242), `req = out_bmp.parent / ...` (:245), env `str(req)` (:268) | `str(out_bmp)` (:563) |
| `re/frida/pickup_pos_probe2.py` | `Path(args.out).resolve()` (:257), :260, env :295 | `str(out_bmp)` (:720) |
| `re/frida/race_hud_burst.py` | `Path(args.bbdump).resolve()` (:592), :594, env :605 | `str(bb_out)` (:760) |
| `re/frida/capture_relight_parity.py` | `REQF = OUTD / ...`, `OUTD = ROOT / ...`, `ROOT = Path(__file__).resolve()...` (:21, :32, :37), env `REQF.resolve()` (:164) | `Path(path).resolve()` (:41-43) |
| `re/parity/sweep.py` | `req_file.resolve()` (:294) | `bb_target = (bundle / "orig.bmp").resolve()` (:287, :328) |

The other `re/frida` scripts that arm `MASHED_ORIG_BBDUMP_REQ` (capture_orig_screens, force_screen, frontend_parity,
probe_action_semantics: `ROOT / ...` with `ROOT` from `__file__`; nav_champ_probe: `SHOT_DIR` from `ROOT`; carpos_source_probe:
`os.path.abspath` on every path) are absolute too. No file was changed for this item.

## 8. Trackers

`apply_trackers.py` (re-classify rules: rows with a path to resolution, CHANGELOG entry below `<!-- ENTRIES -->`, nothing
rewritten, CRLF kept): **U-9702 RESOLVED**; new **U-9713..U-9718**; U-9691, U-9701, U-9706, U-9710 extended with one sentence
each; one CHANGELOG entry. `hooks.csv`, `STUBS.md`, `DEFERRED.md`: unchanged (no function moved on the ladder).
U-ids were taken as the next free ones at the base (`U-9712` is the highest there); the `work/d1r3` branch already carries
a `U-9713`, so the merge needs a renumber of one of the two.

## 9. Reproduce

```
py -3.12 verify\r10_20261016\run_tracks.py                                  # 10 replays of the capfix traces
py -3.12 verify\r10_20261016\score_tracks.py --prefix diff2_ --cite verify\r10_20261016\cites_r10_v3.json
py -3.12 verify\r10_20261016\bt_capture.py                                  # new original run per track, render-site chains
py -3.12 verify\r10_20261016\bt_summary.py --depth 9 ; py -3.12 verify\r10_20261016\bt_slots.py
py -3.12 verify\r10_20261016\run_tracks.py --cap verify\r10_20261016\bt --out verify\r10_20261016\bt_rerun
py -3.12 verify\r10_20261016\score_tracks.py --cap verify\r10_20261016\bt --work verify\r10_20261016\bt_rerun --prefix diff2_ --cite verify\r10_20261016\cites_r10_v3.json
py -3.12 verify\r10_20261016\report_tables.py > verify\r10_20261016\tables_r10.md
```

## 10. What this does not show

- No pixel comparison (draw lists only); the 16-bit-versus-8888 colour precision of d1r1 still applies.
- C22, C23b, C26, C28: the cause is ownership plus the standalone's missing handling, not a decode of the original's placement code.
- `shotgun` (Sands) is cited under C07 by name; its render site was not in the backtrace run.
- The Sands bridge-variant explanation (three variants drawn together by the standalone) is an [UNCERTAIN] lead in U-9713, not a finding.
- G-CTRL not re-run (no source changed).

## 11. Decisions for the user

1. **I-NAME threshold.** Accept Neustein +105 (94.8 %) as a documented gate miss that closes U-9702 (done here), or relax I-NAME to 94 % for frames whose unnamed draws are one runtime texture?
2. **Next lane.** U-9714 (sky slots) and U-9715 (object BSP) are small, mapped fixes on three and one track; U-9713 is the large item (Warzone slots 0 and 10, Highway slots 0 and 2 first).
