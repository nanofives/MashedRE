"""r10: the tracker transaction of this lane (re-classify skill rules: UNCERTAINTIES rows need a path to resolution,
the CHANGELOG entry goes immediately below the <!-- ENTRIES --> marker, nothing is rewritten).

    py -3.12 verify\\r10_20261016\\apply_trackers.py [--dry-run]

Idempotent: it refuses to run twice (the U-9718 row is the marker). UNCERTAINTIES.md is CRLF, CHANGELOG.md is LF;
both are read and written as bytes so no line ending changes.
"""
import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
UNC = ROOT / "UNCERTAINTIES.md"
CHG = ROOT / "re" / "analysis" / "CHANGELOG.md"
R = "verify/r10_20261016/RESULT_r10.md"

ROWS = [
    ("U-9718", "semantic",
     "single rows of the r10 scoring with no mapped cause: Egypt (rows 740 of the capfix capture, 694 and 734 of the re-capture), Neustein (3 rows of each capture), car body parts on Dump 735 and SuperG 857 (re-capture)",
     "(a) Egypt: one standalone-only 6-triangle ground patch with an unnamed `#128x64:21` texture (blended, depth write on, y -0.25 to 0.006, AABB 10.79..11.99 / 1.68..3.00); the standalone draws it in all three rows of each capture and it has an original counterpart in two of the three capfix rows (780, 815) and one of the three re-capture rows. (b) Neustein, 3 rows per capture: one thin untextured mesh (AABB 0.07 x 2.5 x 0.09) drawn at the world origin by the standalone where the original has it with AABB minimum (7.70, 2.67, 9.23). (c) Car body parts whose AABB differs by 0.012 to 0.0123 (tolerance 0.01): Dump 735 two parts, SuperG 857 four parts of the re-capture; the car frame LTM is the transplanted one, the part poses are the standalone's own wheel / suspension state. Cites C29, C27 in `" + R + "`.",
     "the identity of the Egypt draw and of the Neustein mesh (no render-site chain captured for them); the pose source of the offset parts.",
     "`py -3.12 verify/r10_20261016/texcompare.py --prefix diff2_ --tracks 1,2` lists the draws; capture the original with `camera_probe.py --drawrec-bt '^-$'` on Neustein for the mesh; `camera_probe.py --carparts` for the parts.",
     "none (7 rows of 979)"),
    ("U-9717", "structural",
     "Dump `Train.dff` (COURSE.LUA `Clump_Filename( 0 , \"Train.dff\" )`, `General_Anim_Filename( 3 , \"Train.anm\" )`, `SetClumpAnim( LINEAR , 0 , 3 )`)",
     "`SetClumpAnim` has 0 hits in `mashedmod/src`, and the standalone draws no `train` texture on any of the three Dump rows of either capture; the original draws one 2112-triangle mesh in each of the three capfix rows (two on screen, one off-screen; render chain `0x004ec6bb <- 0x004ea957 <- 0x004d4129 <- 0x004e5fb4 <- 0x004722c7 <- 0x004e56cd <- 0x004e56d8 <- 0x004e47d6 <- 0x004e5678`, the default world sector path). `Train.dff` is not `Clump_Exclude_From_World`, so it is a world clump in the original. Cite C28 in `" + R + "`.",
     "the clock the linear animation runs on and the clump's position at a given race row.",
     "decode the `SetClumpAnim` handler (lua binding table `0x00440bc0..0x00440d40`) and the clump-animation tick, port, compare the `train` AABB per row.",
     "Dump D1 breadth (2 rows per capture)"),
    ("U-9716", "structural",
     "`RWP_Object` instances (Sign02, Trucktyre, Hut01 and Wall01..04, Post_1) against `Physics_Scenario_Add_Objects` / `Physics_Scenario_Trigger_Add_Objects` (Egypt, Warzone, dump, rouabout) in `D3d9Render/TrackRenderer.cpp`",
     "The standalone has no reference to the two `Physics_Scenario_*` commands (0 hits in `mashedmod/src`) and places every `RWP_Object` Mts record at its static pose (`TrackRenderer.cpp:1693`). The original draws the objects its scenario added, at their physics pose. Rows: Roundabout `Sign02` 5 standalone draws vs 2, `interactiveHut` 7 vs 2, `Trucktyre` AABB 0.0115 off; Warzone `interactiveHut` 6 vs 2; Neustein `post` 5 standalone-only draws. Cite C26 in `" + R + "`.",
     "which scenario the captured race ran, and the pose source of an object that has moved.",
     "read the `Physics_Scenario_*` handlers and the scenario object lists of rouabout / Warzone COURSE.LUA, compare with the standalone-only draws; U-9710 samples the same objects' matrices.",
     "none beyond the rows"),
    ("U-9715", "structural",
     "`Object_Bsp_Filename( 0 , \"Trees.bsp\" )` (Forest COURSE.LUA only; no source file of `mashedmod/src` names `Object_Bsp` or `trees.bsp`)",
     "Forest's trees are the object BSP `Trees.bsp` plus the excluded clumps FarTree / NearTree / MidTree / Bushes (Mts FarTree, NearTree, MidTree, Bushes, MidTree2, NearTree2). The standalone loads no object BSP and draws each excluded clump once at the origin (Bush2 8 triangles, midtree 2, Tree'sBack 14, OakTreeSprite3 48, bark 157); the original draws up to 6 placed copies per texture and row through the default sector path (`0x004ec6bb <- 0x004eb50f <- 0x004d4129 <- 0x004e51cd <- 0x004e568d <- 0x004e47d6 <- 0x004e5678`, no `0x004842ab` layer). Forest matches 68.5 to 70.1 % of the original's triangles, the lowest of the 13 tracks. Cite C23b in `" + R + "`.",
     "the handler of `Object_Bsp_Filename` in `Course::CreateFromDescription` (`0x00479330`) and which world the object BSP joins.",
     "decode the handler, load `Trees.bsp` as `graphics.bsp` is loaded, re-score Forest with `score_tracks.py`.",
     "Forest D1 breadth"),
    ("U-9714", "structural",
     "`D3d9Render/TrackRenderer.cpp:2121-2135` (`Sky_Filename` parse), `:1882` (`Prop::rw_model`)",
     "The parse calls `load_prop(sk, &sky_, ...)` once per `Sky_Filename( n , \"x\" )` line into one `Prop`, and `rw_model` is assigned on every call, so a track with two sky slots keeps the last. Neustein: the standalone draws `skyCyl` (SkyDome_01, slot 1), the original also `skyCylTop` (SkyDome_02, slot 0). Warzone: `horizon` (HorDome, slot 1) against the original's `horizon` and `sky` (SkyDome, slot 0). Roundabout's line is the one-argument form `Sky_Filename(\"skydome.dff\")`, which neither `sscanf` pattern matches: the standalone draws no sky texture, the original draws `Sky3` and `Sky4`. Original draw site `FUN_00426d90`, return `0x00426da2` (rprops item 6). Cite C23 in `" + R + "`.",
     "none for the cause. The sky pixels after a fix are not measured.",
     "make the index optional in the parse, keep one `Prop` per sky slot, call `RaceSubmit_AddSky` for each in slot order, re-score Neustein, Roundabout and Warzone: the C23 rows must reach 0.",
     "D1 sky on Neustein, Roundabout, Warzone"),
    ("U-9713", "structural",
     "per-track hook-table slot functions (table `0x005f33f8`, stride 0x48, 13 slots) for Warzone, Highway, Roundabout, Sands and the other excluded-clump tracks; standalone `D3d9Render/TrackRenderer.cpp:2278-2372` (`rw_kind = kInstAfterWorld`), `Render/ArcticTrackNodeSlot0.cpp`",
     "The original instantiates and draws the copies of its `Clump_Exclude_From_World` clumps from the slot functions of the track's record: Warzone slot 10 (`0x00452da0`) draws the `road` meshes through `0x00479f96` (returns `0x00452dc5` / `0x00452dd0`, 282 of 309 captured draws), Highway slot 2 (`0x0044e470`, return `0x0044e4ee`) the lorries (49 of 61), Roundabout slot 2 (`0x0044d6f0`) the barrel and crane shadows, Neustein / Sands / Warzone slot 10 the leaf sprites through `0x0047cd47`. The standalone ports slots only for Arctic and the `Clump_Mts_Pair` copies (rprops); every other excluded clump is one instance at the origin or at its DFF frame. Warzone: trunk 30 original draws vs 1 at the origin, fence 16 vs 2, road 103 vs 10. Highway `traffic3`: 18 to 29 vs 96, 92 at the origin. Roundabout comp2 / girders / gravel2 / rustycable / Camstand differ in both directions. Sands camstand / concrete3 / roadnew: 11 / 12 / 10 standalone draws vs 7 / 9 / 7. Texture ownership by an excluded clump: `verify/r10_20261016/tex_owner.py`. Cite C22 in `" + R + "`.",
     "the per-copy placement code of each slot (counts, matrices, source Mts rows); for Sands whether Bridgehafbroke / BridgeBroke / BridgeWhole are drawn alone in the original.",
     "decode Warzone slots 0 and 10 (`0x004527f0`, `0x00452da0`) and Highway slots 0 and 2 (`0x0044f970`, `0x0044e470`) first (the largest row counts), port on the Arctic port's model, re-run `run_tracks.py` + `score_tracks.py`: the C22 rows must fall to the C01c residue.",
     "D1 breadth on Highway, Roundabout, Sands, Warzone"),
]

U9702_NEW = ("| U-9702 | ~~coverage~~ **RESOLVED 2026-10-16** (r10: ten tracks replayed and scored) | camera-men and `Clump_Mts_Pair` copies on the ten tracks with no matched-pose original capture (Egypt, SuperG, Forest, Storm, sands, Warzone, dump, Highway, Neustein, rouabout) | "
             "capfix captured the ten tracks (3 rows each); r10 replayed them on the default build and scored them with `re/tools/drawlist_diff.py` 3D mode (`" + R + "`): 30 of 30 rows CITED, 979 rows, 0 RED (first scoring with the unmodified union of the sibling cites: 159 RED rows on 30 of 30 frames); an independent second original capture of the ten tracks scores 30 of 30 CITED, 979 rows, 0 RED (4 RED under v2 of the cites, widened to v3). Instrument gates pass on 58 of 60 frames: Neustein +105 fails I-NAME at 73/77 = 94.8 % (threshold 95 %) in both captures, the 4 unnamed draws being one `#64x64:21` runtime texture. What the row asked about: the camera-man MOVED rows are 0.014 to 0.223 on all ten tracks (C13, yaw, U-9699); the textures of the `Clump_Mts_Pair` clumps of Sands (OakTrunk: bark), Storm (Bush: Alpha) and Roundabout (Shrub01 / FatTree: shrub, Rock2) appear only in matched-draw rows (C21 order, C19 state: bark 153 draws, Alpha 16, shrub 40, Rock2 9 matched draws), Highway's Lampost raises no row, and Roundabout's Pylon_H / Frame_H (comp2, Camstand) and Warzone's Pylon_H (office-block_plain) sit inside C22 rows. The causes found are U-9713 .. U-9718. "
             "| none | none | none |")

CHG_ENTRY = ("2026-10-16  r10 (work/r10): U-9702 RESOLVED. Ten capfix tracks x 3 rows replayed on the default build 0006dbf2d "
             "(mashed_re.exe SHA-256 18c470b2..., clean build) and scored with drawlist_diff 3D: 30/30 CITED, 979 rows, 0 RED after the v3 cites "
             "(first run, unmodified union of the sibling cites: 159 RED rows on 30/30 frames); a second original capture scores 30/30 CITED. "
             "New U-9713 (per-track hook slots), U-9714 (Sky_Filename multi-slot / one-argument form), U-9715 (Object_Bsp_Filename, Forest), "
             "U-9716 (RWP scenario objects), U-9717 (SetClumpAnim, Dump train), U-9718 (unmapped single rows); U-9691 / U-9701 / U-9706 / U-9710 "
             "extended. Gate: Neustein +105 I-NAME 94.8 % in both captures.  " + R + "\n")

APPEND = {
    "U-9691": " [r10] The same chain `0x004e11d9 <- 0x004d4129 <- 0x004cd3d7 <- 0x00422a5e` draws `#64x64:21` strips on nine more tracks (Dump, Egypt, Forest, Highway, Neustein, Roundabout, Sands, SuperG, Warzone: 35 rows of sample A, `" + R + "` cite C10).",
    "U-9701": " [r10] The leaf-sprite renderer is reached from slot 10 of the track's record on Neustein (`0x0044af75`, `TreeSprite`), Sands (`0x00450a4b`) and Warzone (`0x00452dee`) (`OakTreeSprite`): chain `0x00536129 <- 0x005356d4 <- 0x0047cd47 <- slot 10`; wfall `FUN_0048e7b0`, flames3 `FUN_00475e50`, Roundabout barrel / crane shadows `FUN_0044bd50` / `FUN_0044be70` from slot 2 are the same RwIm3D family (`" + R + "` cite C24).",
    "U-9706": " [r10] The car-part MISSING rows of the ten tracks (AdvantageSmall on Neustein, Glass on Warzone and, in the re-capture, Sands) fit (a) / (c); cite C25 in `" + R + "`.",
    "U-9710": " [r10] RWObjShad rows on Roundabout (AABB 0.0281 off, 2 rows of the re-capture) and Warzone (10 triangles vs 6) fit (b), (d), (e); cite C04b in `" + R + "`.",
}


def main() -> int:
    ap = argparse.ArgumentParser(); ap.add_argument("--dry-run", action="store_true"); a = ap.parse_args()
    u = UNC.read_bytes().decode("utf-8")
    if "| U-9718 |" in u:
        print("U-9718 already present: the transaction was applied, nothing to do"); return 1
    eol = "\r\n"
    # 1. new rows, newest id first, right under the table separator
    sep = "|----|------|-------|-----------|------------------|--------------------|--------|" + eol
    i = u.index(sep) + len(sep)
    block = "".join("| " + " | ".join(r) + " |" + eol for r in ROWS)
    u = u[:i] + block + u[i:]
    # 2. close U-9702 (replace the whole row)
    j = u.index("| U-9702 |")
    k = u.index(eol, j)
    u = u[:j] + U9702_NEW + u[k:]
    # 3. extend four rows (text appended to the Statement column, i.e. before the 'Evidence missing' separator)
    for uid, txt in APPEND.items():
        j = u.index(f"| {uid} |")
        k = u.index(eol, j)
        row = u[j:k]
        cols = row.split(" | ")
        # columns: ID | Type | Where | Statement | Evidence missing | Path | Blocks
        if len(cols) != 7:
            print(f"{uid}: row has {len(cols)} columns, not appended"); continue
        cols[3] = cols[3] + txt
        u = u[:j] + " | ".join(cols) + u[k:]
    c = CHG.read_bytes().decode("utf-8")
    m = "<!-- ENTRIES -->\n"
    p = c.index(m) + len(m)
    c = c[:p] + CHG_ENTRY + c[p:]
    if a.dry_run:
        print("dry run: would write", len(u), len(c)); return 0
    UNC.write_bytes(u.encode("utf-8"))
    CHG.write_bytes(c.encode("utf-8"))
    print("applied: 6 rows, U-9702 closed, 4 rows extended, 1 CHANGELOG entry")
    return 0


if __name__ == "__main__":
    sys.exit(main())
