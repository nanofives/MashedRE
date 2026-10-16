# RESULT rsky: sky slots (U-9714) and the Forest object BSP (U-9715), plus the sector mesh order they exposed

**Verdict: both rows close on the default build, and the C23 / C23b cite groups are empty.** On the same 13 tracks x 3 frames
(39 frames) with the same cites minus those two groups, the baseline has **32 RED rows (12 C23 + 20 C23b)** and the final build has
**0 RED**; divergence rows fall **1211 -> 1147**. Forest matches **91.9 / 91.9 / 87.7 %** of the original's triangles (was 70.1 / 70.1 / 68.5 %).
G-CTRL is unchanged (`2b4d47bb / 356821c8 / f7a155e1`, baseline, first and final build). One of r10's mapped causes was wrong
(C23b, section 3) and one change went further than the brief (the sector mesh order, section 1c), both reported below.

- Base `1fa71b277` (race/first-frame-parity), branch `work/rsky`, worktree `.worktrees/rsky`. Pre-registration `PREREG_rsky.md`
  (commit `0cf5522a5`; the file was written before the first measuring run, the commit followed one baseline smoke replay of Forest).
- Executables (clean builds in this worktree): baseline `log/rsky/exe_base.exe` SHA-256 `77B96188EE89D1D020A866DD8565B7C9CBDC2E56E29A3B25E1DF088AAEF3187A`;
  final `log/rsky/exe_after4.exe` `9EF5720624CE2B58697388964B63D0648BDBAC57EEE722DEDA79454F6BA84550`.
- **Isolation.** Every replay and G-CTRL ran with `MASHED_ROOT` and the CWD = a private copy of `original\` (482 MB, `original\` inside it, checked
  before the first run); the d1r1 / r10 drivers hard-code the main checkout. No foreign process was touched, no file was deployed to `original\`
  (the build's `.asi` deploy went to this worktree's own `original\`). The original-side capture is the one place that uses the main install, as every capture does.

## 1. What changed (all default ON, each with an opt-out)

a. **Sky slots.** `Sky_Filename` handler `0x0047a1e0` files the name under `names + (slot+1)*0x40`; `Course::CreateFromDescription` (`0x00479330`) loads slots 0..3
   into `course + 0x100f0`; `FUN_00426d20` / `FUN_00426d90` place each at the camera position and `RpClumpRender` them in ascending slot order (Arctic and Roundabout's
   slot 3, `0x00449010`, is the same pair). The port keeps one `Prop` per slot, makes the slot argument optional (the one-argument form lands in slot 0:
   `FUN_004b7090` returns `_DAT_005d1008` = 0.0 for a non-number, [UNCERTAIN U-9721]) and queues every loaded slot. `MASHED_NO_SKYSLOTS=1` = old parse.
b. **Forest object BSP.** `Object_Bsp_Filename` (`0x0047a610`) names `Trees.bsp`, loaded as a second RpWorld (`course + 0x10040`). Forest's slot 2 (`0x0044df00`)
   renders clump 0 (`Water.dff`) and then `FUN_0044dce0`: camera to the object world, cull NONE, `RpWorldRender`, back. The port builds the object world exactly as the
   main world (`RaceSubmit_SetObjectWorld`), queues a marker after the after-world props, draws it with cull NONE, and walks its sectors **back-to-front**
   (the hook swaps the near side when `world+0xc == 2`, `0x004e394d`; the original's 6 visible sectors come in the reverse of our front-to-back order on 3 of 3 frames;
   the writer of that 2 is not decoded, [UNCERTAIN U-9722]). `MASHED_NO_OBJBSP=1` = old Forest, `MASHED_OBJBSP_ORDER=f2b` = front-to-back (experiment).
c. **Sector mesh order (beyond the brief).** With the object world in, the sectors drew their five tree meshes as bark, Oak, Bush2, Tree'sBack, midtree; the original draws
   bark, Bush2, Tree'sBack, midtree, Oak. RenderWare draws a world sector's meshes in the order of its BinMesh PLG (chunk `0x50E`), and librw's `buildMeshes` gives list order.
   `Track::World` now reads each sector's BinMesh order and `BuildWorld` reorders the mesh structs (`bsp_binmesh.py` prints it; Trees.bsp sector 3 = `m5 m0 m2 m4 m1`).
   It applies to **every** world: all 13 tracks' sectors carry a BinMesh chunk naming exactly the materials their triangles use (`binmesh_census.txt`: 7 to 32 sectors per
   track, list order changed in 7..32 of them). `MASHED_NO_SECTOR_BINMESH=1` = list order.
d. **Probe.** `re/frida/camera_probe.py` names the two car shadow render-to-texture RwTextures `@shadowrt0/1` (section 4).

## 2. Per track (same cites for both arms; `evidence/tables_rsky.md`)

| track | rows base -> after | RED base -> after | matched triangles % base -> after |
|---|---|---|---|
| Egypt | 67 -> 67 | 0 -> 0 | 92.6, 95.9, 92.5 (unchanged) |
| Neustein | 67 -> 67 | 3 -> 0 | 78.9, 79.5, 67.7 -> 78.9, 79.5, 67.8 |
| Arctic | 103 -> 103 | 0 -> 0 | unchanged |
| Highway | 70 -> 70 | 0 -> 0 | unchanged |
| Sands | 88 -> 85 | 0 -> 0 | unchanged |
| SuperG | 210 -> 210 | 0 -> 0 | unchanged (ORDER counts fall) |
| Roundabout | 120 -> 120 | 6 -> 0 | 83.2, 90.0, 88.6 -> 83.5, 90.3, 89.0 |
| Storm | 69 -> 69 | 0 -> 0 | unchanged |
| Forest | 96 -> 57 | 20 -> 0 | 70.1, 70.1, 68.5 -> 91.9, 91.9, 87.7 |
| Dump | 63 -> 57 | 0 -> 0 | unchanged |
| Warzone | 129 -> 120 | 3 -> 0 | 87.3, 88.4, 74.7 -> 87.4, 88.5, 74.9 |
| City | 66 -> 59 | 0 -> 0 | unchanged |
| Training | 63 -> 63 | 0 -> 0 | unchanged |
| **all** | **1211 -> 1147** | **32 -> 0** | |

Row-by-row (`evidence/compare_base_after4.txt`): 15 frames identical, 76 rows removed, 55 counts reduced (ORDER and FOG), **12 rows added: the C23 sky draws**. They were
MISSING (uncited under these cites); now the draw matches in texture, triangles and AABB and shows as a STATE row under the existing sky cites (C19 / C20: librw's
ZENABLE 1 / ZFUNC ALWAYS against the original's ZENABLE 0, the row TRAINING's, City's and Arctic's sky already carry). The sky is no longer missing; its state encoding is C20's and untouched here.
**Opt-out control** (all three knobs on the final exe, 7 tracks, 21 frames): row identities and counts equal the baseline on 21 of 21 (`evidence/compare_base_ctl.txt`).

## 3. Findings

1. **C23b was misdiagnosed in r10.** The original does not draw "up to 6 placed copies" of Forest's excluded tree clumps. Slot 0's `FUN_0044dc00` makes six sets of `FUN_00479f30` clones
   (clumps 2, 3, 3, 4, 5, 5 by Mts 0, 1, 6, 4, 3, 5) of each clump's first atomic, but `FUN_00479e80` only gives each clone a frame (no world, clump or tie call) and nothing renders a set.
   Forest's original frames have no 157-triangle bark, 48-triangle OakTreeSprite3 or 8-triangle Bush2 draw; every tree draw is a Trees.bsp sector's material mesh. A first version that placed
   the sets added 43 EXTRA draws and was removed (`FOREST-TREE-CLUMP` log line: clumps 2..5 are loaded, drawn by nothing).
2. **Mesh order is BinMesh order, everywhere.** It also removes baseline ORDER rows: Forest `RockWall2` / `Rocks2`, Sands `distanttrees`, Dump `Wetroad` / `WetEntrance`,
   Warzone `ruin` / `girders` / `rope`, City `Path2` / `thickgirders` / `railings` (U-9709 had put these down to tie order), and shrinks SuperG's and Roundabout's.
3. **The `#WxH:21` textures are the car shadow render-to-texture** (U-9691): `FUN_0041f8f0` renders the car into the RwTexture `(&DAT_0063d854)[b]`, `FUN_004228f0` draws the shadow quads with it.

## 4. Neustein +105 I-NAME (the gate miss r10 left)

The 4 unnamed draws were that shadow texture: an RwTexture made from a camera raster, never passed to `RwTextureSetName`, so the probe's name map missed it. `camera_probe.py`
now reads `(&DAT_0063d854)[0..1]` into the map. A **new original capture of Neustein** (`capture_one.py --track 2`, shot rows 730 / 770 / 805, 55.6 s, `cap_neustein/`) shows the draws as
`@shadowrt0` / `@shadowrt1` and passes every gate: I-NAME 125/125, 122/122, 77/77 (I-COVER, I-FRAME also pass; standalone I-SYNC passes). Scored with the same cites (C10's pattern also accepts the new name):
baseline 3 RED (the C23 sky row), final **21 / 21 / 22 rows, 0 RED, CITED**. The gate was not relaxed. The 13-track table above still uses the capfix capture of Neustein, whose row 1169 keeps the
r10 miss (73/77); the new sample is additional and replaces it for gate purposes. Pixels (`evidence/neustein_named_imgdiff.txt`): frame 730 mean abs diff 14.28 -> 14.20, 770 and 805 unchanged
(the sky is behind the world there); the remaining diff is the leaf sprites and the shadow texture.

## 5. Predictions (PREREG_rsky.md)

- **Pr-1** (only Neustein, Roundabout, Warzone move): **refuted**. Sands, Dump, City, SuperG and Forest also move, all by fewer rows, from the BinMesh order the brief did not ask for.
- **Pr-2** (Forest >= 80 %): confirmed, 87.7 to 91.9 %. The residual ORDER rows I expected from the tie order did not appear: there are no ties in the object world.
- **Pr-3** (G-CTRL unchanged): confirmed on the first and the final build.
- **Pr-4** (name the texture from its creator without a new capture): **false as written**; the name needed the probe change and a new capture. It passed.
- Acceptance: A1 met (C23 MISSING 12 -> 0), A2 met (C23b 20 -> 0), A4 met, A5 met (0 RED on 39 frames), **A3 met with a caveat**: the only added row identities are the 12 STATE rows above, each under an existing cite.

## 6. What this does not show

- No pixel reference exists for Forest (the capfix BMPs were not kept), so the object world's lighting and the tree pixels are **unmeasured**; the port binds no lights to it (none in the original: `RpWorldAddLight` is called on the main world only, `0x00479330`).
- Sky pixels: measured on one Neustein frame only (above).
- The BinMesh order is validated by draw sequence on Forest and by unchanged rows elsewhere, not by pixels.
- The shadow render-to-texture itself is not ported (C10 stays open as MAPPED).

## 7. Decisions for the user

1. The BinMesh sector order is default ON for all worlds (it removes rows on Forest, Sands, Dump, Warzone and City, shrinks ORDER counts on SuperG and Roundabout, and leaves the other 6 tracks identical). Keep it ON, or limit it to the object world (`MASHED_NO_SECTOR_BINMESH` is the opt-out)?
2. Accept the new Neustein capture as the gate-clean sample for U-9702's footnote?
3. U-9721 / U-9722 are the next free ids at the base; siblings may have taken them (renumber at merge).

## 8. Reproduce

```
robocopy <main>\original <root>\original /E            # private game root, original\ inside
py -3.12 verify\rsky_20261016\run13.py run   --exe log\rsky\exe_base.exe   --root <root> --out log\rsky\base
py -3.12 verify\rsky_20261016\run13.py run   --exe log\rsky\exe_after4.exe --root <root> --out log\rsky\after4
py -3.12 verify\rsky_20261016\run13.py score --out log\rsky\after4 --cite verify\rsky_20261016\cites_rsky.json --prefix rs_
py -3.12 verify\rsky_20261016\compare_rows.py log\rsky\base log\rsky\after4 --prefix rs_
py -3.12 verify\rsky_20261016\run_gctrl_priv.py log\rsky\exe_after4.exe after4 --root <root> --out log\rsky\gctrl
```
