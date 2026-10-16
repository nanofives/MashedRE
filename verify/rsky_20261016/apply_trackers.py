"""rsky: the tracker transaction of this lane (re-classify skill rules: UNCERTAINTIES rows need a path to resolution,
the CHANGELOG entry goes immediately below the <!-- ENTRIES --> marker, nothing is rewritten). No function changes C-level
here (the evidence is draw-list adjudication, not a Frida diff), so hooks.csv / STUBS.md / DEFERRED.md are untouched.

    py -3.12 verify\\rsky_20261016\\apply_trackers.py [--dry-run]

Idempotent: refuses to run twice (the U-9720 row is the marker). UNCERTAINTIES.md is CRLF, CHANGELOG.md is LF; both are
read and written as bytes so no line ending changes. Same shape as verify/r10_20261016/apply_trackers.py.
"""
import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
UNC = ROOT / "UNCERTAINTIES.md"
CHG = ROOT / "re" / "analysis" / "CHANGELOG.md"
R = "verify/rsky_20261016/RESULT_rsky.md"

ROWS = [
    ("U-9720", "structural",
     "`LibRw/RwRaceSubmit.cpp` `WorldWalk::b2f` for the object world (Forest `Trees.bsp`); the original's `cmp [world+0xc], 2` at `0x004e394d` (`sete dl` `0x004e3953`) in the camera sync hook `0x004e38a0`",
     "The sector sequence of Forest's object world is the REVERSE of the front-to-back walk in all 3 captured frames (original 509, 586, 373, 501, 314, 543 triangle bark meshes; the port walked 543 .. 509). Taking the far side first (the branch the hook takes when `world+0xc == 2`, `0x004e3a83`..`0x004e3a99`) gives the original's sequence and its whole draw set on 3 of 3 frames. The port sets that for the object world by measurement. What writes 2 into the object world's `+0xc` (the world stream, `RpWorldSetRenderOrder`, or Forest code) is not decoded. Cite: `" + R + "`.",
     "the writer of the object world's render order; whether `Trees.bsp`'s stream carries it.",
     "read `[world+0xc]` of the object world live (`course + 0x10040`) at a Forest shot, or decode the world reader `FUN_004b3c60` (tail of `FUN_0042a640`) for the field; `MASHED_OBJBSP_ORDER=f2b` reproduces the front-to-back walk.",
     "none beyond Forest"),
    ("U-9719", "semantic",
     "`TrackRenderer.cpp` `parse_sky_filename`: the one-argument form `Sky_Filename(\"skydome.dff\")` (Roundabout) lands in slot 0",
     "Handler `0x0047a1e0`: for `n` arguments the slot is `ftol(FUN_004b7090(L, n-1))` and the name `FUN_004b70d0(L, n)` (`0x004b6fc0` is `(top - base) >> 4`). For `n = 1` the slot argument is index 0, one cell past the arguments; `FUN_004b7090` returns `_DAT_005d1008` (the file bytes at `0x005d1008` are zero) for a cell whose tag is not a number, so the port files the name under slot 0. The witness is the original drawing `Sky3` / `Sky4` on Roundabout; a stale numeric tag in that cell would file it under another slot (the draw order of a single sky is the same). Cite: `" + R + "`.",
     "the tag of the stack cell at index 0 when a one-argument call runs.",
     "read `course + 0x100f0 + 4*i` on Roundabout live (the sky clump handles) or decode `FUN_004b6ff0` (index -> cell) for index 0.",
     "none (a single-sky track)"),
]

U9714_NEW = ("| U-9714 | ~~structural~~ **RESOLVED 2026-10-16** (rsky: four sky slots, the one-argument form, slot-ordered draw) | `D3d9Render/TrackRenderer.cpp` `Sky_Filename` parse, `Prop skies_[4]` | "
             "Handler `0x0047a1e0` files the name under `names + (slot+1)*0x40`; `Course::CreateFromDescription` (`0x00479330`, the `Loading SkyDome [%s] into sky[%d]` loop) loads slots 0..3 into `course + 0x100f0`; `FUN_00426d20` / `FUN_00426d90` (`0x00426d20`, `0x00426d90`) place and `RpClumpRender` each non-null slot in ascending order at the camera position. The port keeps one `Prop` per slot, parses the slot argument as optional and queues `RaceSubmit_AddSky` for each loaded slot in order. Neustein `skyCylTop`, Roundabout `Sky3` / `Sky4` and Warzone `sky` are matched draws on 9 of 9 frames (C23 MISSING rows 12 -> 0); each now appears as a STATE row under the existing sky cites (C19 / C20: librw's ZENABLE 1 / ZFUNC ALWAYS against the original's ZENABLE 0), the same row TRAINING, City and Arctic already carry. The sky pixels are not measured. `MASHED_NO_SKYSLOTS=1` restores the old parse. `" + R + "`. Remaining: U-9719. "
             "| none | none | none |")

U9715_NEW = ("| U-9715 | ~~structural~~ **RESOLVED 2026-10-16** (rsky: Forest's object BSP is a second world; cause corrected) | `Object_Bsp_Filename( 0 , \"Trees.bsp\" )` (Forest COURSE.LUA only) | "
             "`Trees.bsp` is a full RpWorld loaded with the world-BSP loader `0x0042a640` into `course + 0x10040` (handler `0x0047a610`, names at `(slot+0xe)*0x40`); Forest's hook record (`0x005f33f8`, Course_Id 34) slot 2 `0x0044df00` renders clump 0 (`Water.dff`) and then `FUN_0044dce0` (`0x0044dce0`): end the camera update, move the camera to the object world (`0x004e4350` / `0x004e4320`), cull mode NONE, `RpWorldRender`, back. The trees are that world's 6 visible sectors (bark / Bush2 / Tree'sBack / midtree / OakTreeSprite3 material meshes, `verify/rsky_20261016/bsp_binmesh.py`), NOT placed copies of the excluded clumps: slot 0's six `FUN_00479f30` tree sets (`0x0044dc00`: clumps 2, 3, 3, 4, 5, 5 by Mts 0, 1, 6, 4, 3, 5) are lone atomics with a frame and no world, clump or tie call (`0x00479e80`) and nothing renders them; the original's Forest frames contain no 157-triangle bark, 48-triangle OakTreeSprite3 or 8-triangle Bush2 draw. r10's reading of C23b (up to 6 placed copies) was wrong. The port builds the object world as the main world (`RaceSubmit_SetObjectWorld`), draws it at the slot-2 point, walks it back-to-front (U-9720), and does not draw clumps 2..5. Forest matched triangles 70.1 / 70.1 / 68.5 % -> 91.9 / 91.9 / 87.7 %, C23b rows 20 -> 0, instrument gates pass. The sector mesh order was the second finding: RenderWare draws a sector's meshes in its BinMesh PLG (`0x50E`) order, not material list order (`Track::Sector::mesh_order`); applied to every world, it removes the ORDER rows of Forest, Sands, Dump, Warzone and City (see U-9709). `MASHED_NO_OBJBSP=1` / `MASHED_NO_SECTOR_BINMESH=1` restore the old behaviour. `" + R + "`. Remaining: U-9720. "
             "| none | none | none |")

CHG_ENTRY = ("2026-10-16  rsky (work/rsky): U-9714 and U-9715 RESOLVED on the default build (base 1fa71b277). Sky_Filename: 4 slots, optional slot argument, slot-ordered draw "
             "(0x0047a1e0, 0x00426d20/0x00426d90); Forest object BSP Trees.bsp as a second world drawn by slot 2 (0x0044df00 -> 0x0044dce0), trees are its sectors, the six "
             "FUN_00479f30 tree sets are never rendered (r10's C23b reading corrected); world sectors draw in BinMesh PLG (0x50E) order. 13 tracks x 3 rows re-scored: rows "
             "1211 -> 1147, 0 RED, C23 / C23b 0; Forest matched triangles 70 -> 92 %. G-CTRL unchanged (2b4d47bb / 356821c8 / f7a155e1). Neustein +105 I-NAME closed by a new "
             "capture whose probe names the car shadow render textures. New U-9719, U-9720; U-9702 / U-9709 / U-9691 extended.  " + R + "\n")

APPEND = {
    "U-9702": " [rsky] The Neustein +105 I-NAME footnote is closed by a new original capture taken with the probe naming the car shadow render textures (`@shadowrt0/1`): shot rows 730 / 770 / 805 pass every gate (I-NAME 125/125, 122/122, 77/77) and score 21 / 21 / 22 rows, 0 RED, on the final build (`" + R + "` section 4).",
    "U-9709": " [rsky] The City `thickgirders` / `Path2` / `railings` ORDER rows (and Dump `Wetroad` / `WetEntrance`, Warzone `ruin` / `girders` / `rope`, Sands `distanttrees`, Forest `RockWall2` / `Rocks2`) were NOT tie order: they vanish when a world sector draws its meshes in BinMesh PLG (`0x50E`) order instead of material list order (`" + R + "`, MASHED_NO_SECTOR_BINMESH=1 brings them back). What remains of U-9709 is the within-sector order of tied DFF atomics (TRAINING ROAD.DFF), which no scored frame shows as a row.",
    "U-9691": " [rsky] The `#WxH:21` runtime textures are the car SHADOW render-to-texture: FUN_0041f8f0 (`0x0041f8f0`) picks `(&DAT_0063e490)[b]` as the camera and the RwTexture `(&DAT_0063d854)[b]` (b = rec+0x298 == 2) as its target, FUN_00420420 (`0x00420420`) hands the textures to FUN_004224d0 (`0x004224d0`), and FUN_004228f0 (`0x004228f0`, call `0x00422a5e`) draws the shadow quads with RenderState 1 = that raster. The probe now names them `@shadowrt0/1` (`re/frida/camera_probe.py`).",
}


def main() -> int:
    ap = argparse.ArgumentParser(); ap.add_argument("--dry-run", action="store_true"); a = ap.parse_args()
    u = UNC.read_bytes().decode("utf-8")
    if "| U-9720 |" in u:
        print("U-9720 already present: the transaction was applied, nothing to do"); return 1
    eol = "\r\n"
    sep = "|----|------|-------|-----------|------------------|--------------------|--------|" + eol
    i = u.index(sep) + len(sep)
    u = u[:i] + "".join("| " + " | ".join(r) + " |" + eol for r in ROWS) + u[i:]
    for uid, new in (("U-9714", U9714_NEW), ("U-9715", U9715_NEW)):
        j = u.index(f"| {uid} |")
        k = u.index(eol, j)
        u = u[:j] + new + u[k:]
    for uid, txt in APPEND.items():
        j = u.index(f"| {uid} |")
        k = u.index(eol, j)
        cols = u[j:k].split(" | ")
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
    print("applied: 2 rows, U-9714 / U-9715 closed, 2 rows extended, 1 CHANGELOG entry")
    return 0


if __name__ == "__main__":
    sys.exit(main())
