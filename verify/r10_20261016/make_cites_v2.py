"""r10: build cites_r10_v2.json from the pre-registered union (cites_r10.json) + the changes made after the first
scoring run (diff0_). Every change is listed in CHANGES below with the evidence file that backs it.

An entry may carry "tracks": [names] (score_tracks.py drops it for other tracks; drawlist3d.py has no such key)
and "status": "MAPPED" (cause traced to code / data in this lane or a sibling) or "OPEN" (the row is explained only
as far as the note says, a U-row carries the rest). "status" and "evidence" are read by the report, not by the differ.
"""
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
base = json.loads((HERE / "cites_r10.json").read_text())
out = []


def by_id(i):
    return [c for c in base if c["id"] == i]


# ---- 1. entries of the union that are restricted or widened --------------------------------------------------
for c in base:
    c = dict(c)
    if c["id"] == "C10-RUNTIME-TEX":
        c["tex"] = r"^#\d+x\d+:21$"
        c["note"] += (" [r10] Same site on nine more tracks, original-side chain for '#64x64:21' (27 draws, Dump, Egypt, "
                      "Forest, Highway, Neustein, Roundabout, Sands, SuperG, Warzone): 0x004e11d9 <- 0x004d4129 <- "
                      "0x004cd3d7 <- 0x00422a5e (bt/*/orig_row_*.draws.json, bt_summary.py). Texture size differs per "
                      "track (256x256 TRAINING, 64x64 elsewhere), the call chain does not.")
        c["status"] = "OPEN"; c["evidence"] = "bt_summary.py"
    elif c["id"] == "C12-CITYPLACE":
        c["tracks"] = ["City"]          # City only: the bark/Grass/building rows are rprops' Clump_Mts_Pair copies
        c["status"] = "MAPPED"
    elif c["id"] == "C15-SKY":
        c["tracks"] = ["Arctic"]        # the note is Arctic's two-copy sky pass; r10 tracks use C23-SKY-SLOTS
        c["status"] = "MAPPED"
    elif c["id"] == "C16-TEX1-WORLD":
        c["tex"] = r"^(CliffEdge|CliffEdge_Small|Dirt|RoadTarmac|Wall|WetRoad|Ice|IceRoad1|IceRoad2|water)$"
        c["note"] += (" [r10] SuperG Ice / IceRoad1 / IceRoad2: original chain 0x004cb321 <- 0x0049aa7a (the sector loop "
                      "the note above names for the 'Ic?' materials), textures owned by graphics.bsp. Storm 'water' "
                      "(stage-1 'water2'): chain 0x00540d4a <- 0x005436ca <- 0x004ea957 <- 0x004e568d, the MatFX DUAL "
                      "pipeline of U-9692's TRAINING half (Water.dff, Clump_Exclude_From_World). The standalone binds "
                      "stage 0 only on all four.")
        c["status"] = "MAPPED"; c["evidence"] = "bt_summary.py, tex_owner.py"
    elif c["id"] == "C07-WEAPONS":
        c["tex"] = r"^(mine|oil|mortar|missile|PowerUpModelsFinal|machinegun|shotgun|depthcharge)$"
        c["note"] += (" [r10] machinegun / shotgun / depthcharge (weapon pickup models, Roundabout, SuperG, Warzone, Sands): "
                      "chain ... <- 0x004e66ab <- 0x00458bd9 <- 0x0045b99b. 0x00458bd9 and the C06 PUGlow site 0x00458b3f "
                      "are both inside FUN_00458b10 (0x00458b10, 219 bytes), so the pickup function draws the glow and "
                      "the model. The standalone draws a pickup only as the generated '@gen:pickup' sprite.")
        c["status"] = "MAPPED"; c["evidence"] = "bt_summary.py, decomp_pc.py 0x00458bd9 0x00458b3f"
    out.append(c)

# ---- 2. new entries ------------------------------------------------------------------------------------------
NEW = [
    dict(id="C11b-WATER-STATIC", kind=["EXTRA"], tex=r"^wet_watercol00$", tracks=["Sands", "Forest"], status="MAPPED",
         evidence="evidence_water_pairs.txt",
         note="Static side of C11. The standalone binds the static 'wet_watercol00' where the original rebinds the "
              "animated waterNN texture (C11); in every frame the B-only draws equal the cited A-only waterNN draws in "
              "draw count, triangle count and AABB (evidence_water_pairs.txt). Water.dff is a Clump_Exclude_From_World clump."),
    dict(id="C23-SKY-SLOTS", kind=["MISSING"], tex=r"^(skyCylTop|Sky3|Sky4|sky)$",
         tracks=["Neustein", "Roundabout", "Warzone"], status="MAPPED",
         evidence="tex_owner.py, TrackRenderer.cpp:2121-2135, 1882",
         note="Sky_Filename handling. The standalone's COURSE.LUA parse calls load_prop(sk, &sky_, ...) once per "
              "'Sky_Filename( %*d , \"x\" )' line into the single Prop sky_, and Prop::rw_model is assigned on every call "
              "(TrackRenderer.cpp:2121-2135, 1882), so with two sky slots only the last stays: Neustein draws skyCyl "
              "(SkyDome_01, slot 1) and the original also skyCylTop (SkyDome_02, slot 0); Warzone draws horizon "
              "(HorDome, slot 1) and the original also sky (SkyDome, slot 0). Roundabout's line is the one-argument form "
              "Sky_Filename(\"skydome.dff\"), which neither sscanf pattern matches, so the standalone draws no sky texture "
              "and the original draws Sky3 and Sky4 (skydome.dff). Original draw site: 0x004ec6bb ... <- 0x004c0ee7 <- "
              "0x004e66ab <- 0x00426da2 (FUN_00426d90, the generic sky-slot draw of rprops item 6). U-9714."),
    dict(id="C24-IM3D-TRACKFX", kind=["MISSING"], tex=r"^(TreeSprite|OakTreeSprite|wfall|flames3|barrel_shad|crane_shad)$",
         status="MAPPED", evidence="bt_summary.py, bt_slots.py, decomp_pc.py",
         note="RwIm3D / DrawPrimitive effects issued by game code that the standalone has no draw site for (C04, C09 "
              "family). TreeSprite and OakTreeSprite: 0x00536129 <- 0x005356d4 <- 0x0047cd47 <- slot 10 of the track's "
              "hook record (Neustein 0x0044af75, Sands 0x00450a4b, Warzone 0x00452dee; slot 10 entries 0x0044af70, "
              "0x00450a20, 0x00452da0), the leaf-sprite renderer of U-9701. wfall: 0x00536129 <- 0x005356d4 <- 0x0048e7f6 "
              "(FUN_0048e7b0, the function that also draws 'flatshad', C04) <- 0x0040bdf8. flames3: ... <- 0x00475e66 / "
              "0x00475e7a (FUN_00475e50) <- 0x0040be20 / 0x00410c2d. barrel_shad / crane_shad (2-triangle ground quads): "
              "0x004e11d9 <- 0x004d4129 <- 0x004cd3d7 <- 0x0044be4f (FUN_0044bd50) / 0x0044bf7f (FUN_0044be70) <- "
              "0x0044d806 / 0x0044d810, inside Roundabout slot 2 (0x0044d6f0). U-9701."),
    dict(id="C01c-CULL-A", kind=["MISSING", "MOVED"], tex=r".*", all_offscreen=True, status="MAPPED",
         evidence="drawlist3d.Frustum",
         note="The original submits draws that lie wholly outside the view frustum (row's every draw fails one plane, "
              "Frustum.outside); the standalone does not draw them. They rasterise nothing (A-side twin of C01)."),
    dict(id="C22-SLOT-PLACEMENT", kind=["MISSING", "EXTRA", "MOVED"], tex=r"^(trunk|fence|road|girders|crater|office-block_plain)$",
         tracks=["Warzone"], status="OPEN", evidence="tex_owner.py, texcompare.py, bt_slots.py, TrackRenderer.cpp:2278-2372",
         note="Copies of Clump_Exclude_From_World clumps (CrossTree, Fence_1/2, OakTrunk, Crater_N, Chunk, Pylon_L/H) that the "
              "original instantiates and draws from its per-track hook-table slots (Warzone record 0x005f3560; 'road' chain "
              "... <- 0x00479f96 <- 0x00452dc5 / 0x00452dd0, slot 10 entry 0x00452da0). The standalone keeps every such clump as one "
              "instance (rw_kind kInstAfterWorld, TrackRenderer.cpp:2362) and ports a track's slots only for Arctic "
              "(Render/ArcticTrackNodeSlot0.cpp) plus the Clump_Mts_Pair copies. Measured: trunk 30 original draws vs 1 "
              "standalone draw at the world origin, fence 16 vs 2, road 103 vs 10. Ownership of each texture by an excluded "
              "clump: tex_owner.py. The per-copy placement code of each slot is not decoded. U-9713."),
    dict(id="C22-SLOT-PLACEMENT", kind=["MISSING", "EXTRA", "MOVED"], tex=r"^traffic3$", tracks=["Highway"], status="OPEN",
         evidence="tex_owner.py, texcompare.py, bt_slots.py",
         note="Highway traffic: 45 truck / lorry clumps are Clump_Exclude_From_World (tex_owner.py), drawn by Highway slot 2 "
              "(entry 0x0044e470, return 0x0044e4ee, 49 of 61 captured draws). The standalone draws 96 draws of the "
              "texture, 92 at the world origin, the original 18 to 29 placed. The placement of a lorry (its path) is not "
              "decoded. U-9713."),
    dict(id="C22-SLOT-PLACEMENT", kind=["MISSING", "EXTRA", "MOVED"], tex=r"^(comp2|girders|gravel2|rustycable|Camstand)$",
         tracks=["Roundabout"], status="OPEN", evidence="tex_owner.py, texcompare.py",
         note="Roundabout excluded clumps (crane, railcar, Gib, Girders, Girders02, Pylon_H/L, Frame_H/L, Handrail, Hrail) "
              "whose placement is the track's hook-table slots (record 0x005f34d0). Four Clump_Mts_Pair uses are ported "
              "(rprops), the rest of the copies are not: the row's counts differ in both directions "
              "(comp2 28 original draws vs 29, girders 17 vs 8) and 1 draw sits at the origin. U-9713."),
    dict(id="C22-SLOT-PLACEMENT", kind=["MISSING", "EXTRA", "MOVED"], tex=r"^(camstand|concrete3|roadnew)$",
         tracks=["Sands"], status="OPEN", evidence="tex_owner.py, texcompare.py",
         note="Sands excluded clumps Jump, Debris, Bridgehafbroke, BridgeBroke, BridgeWhole (lua clumps 7, 10, 18, 19, 20). The "
              "standalone draws more copies than the original (camstand 11 vs 7, concrete3 12 vs 9, roadnew 10 vs 7); "
              "three of the four B-only camstand draws are off-screen. [UNCERTAIN] whether the three bridge states are "
              "drawn together by the standalone and selected by a slot in the original: not decoded. U-9713."),
    dict(id="C23b-OBJECT-BSP", kind=["MISSING", "EXTRA", "MOVED"], tex=r"^(Bush2|OakTreeSprite3|Tree'sBack|midtree|ads|bark)$",
         tracks=["Forest"], status="OPEN", evidence="tex_owner.py, texcompare.py, grep of mashedmod/src",
         note="Forest's trees are Object_Bsp_Filename(0,\"Trees.bsp\") plus the excluded clumps FarTree / NearTree / "
              "MidTree / Bushes (Mts FarTree, NearTree, MidTree, Bushes, MidTree2, NearTree2). A case-insensitive search of "
              "mashedmod/src for 'Object_Bsp' and 'Trees.bsp' finds nothing, so the standalone loads no object BSP and "
              "draws each excluded clump once at the origin (Bush2 8 tri, midtree 2 tri, Tree'sBack 14 tri, "
              "OakTreeSprite3 48 tri, bark 157 tri). The original draws 4 to 6 placed copies per texture and frame through the "
              "default sector path (chain ... <- 0x004e51cd <- 0x004e568d, no 0x004842ab callback layer). U-9715."),
    dict(id="C26-RWP-OBJECTS", kind=["MISSING", "EXTRA", "MOVED"], tex=r"^(Sign02|Trucktyre|interactiveHut|post)$",
         tracks=["Roundabout", "Warzone", "Neustein"], status="OPEN", evidence="tex_owner.py, texcompare.py",
         note="Physics objects (RWP_Object: Sign02, Trucktyre, Hut01 and Wall01-04, Post_1). The standalone has no reference "
              "to Physics_Scenario_Add_Objects / Physics_Scenario_Trigger_Add_Objects (0 hits in mashedmod/src) and "
              "places every RWP_Object Mts record; the original draws the objects the scenario added at their physics pose. "
              "[UNCERTAIN] which scenario the captured race ran and the pose source: not decoded. U-9716."),
    dict(id="C28-ANIM-CLUMP", kind=["MISSING"], tex=r"^train$", tracks=["Dump"], status="OPEN",
         evidence="tex_owner.py, course_lua/dump_COURSE.LUA",
         note="Dump Train.dff (clump 0, not Clump_Exclude_From_World) is driven by SetClumpAnim( LINEAR, 0, 3 ) with "
              "General_Anim_Filename(3,\"Train.anm\"). 'SetClumpAnim' has 0 hits in mashedmod/src and the standalone draws "
              "no 'train' texture; the original draws one 2112-triangle mesh per frame. U-9717."),
    dict(id="C25-CARLOD-MISSING", kind=["MISSING"], tex=r"^(AdvantageSmall|Glass)$", tracks=["Neustein", "Warzone"],
         status="OPEN", evidence="texcompare.py",
         note="The original draws a car part (AdvantageSmall 4 draws vs the standalone's 3 on Neustein row 1169, Glass 4 vs 2 "
              "on Warzone row 659) that the standalone does not. C02's side of the same selection, per-part visibility "
              "inputs S (+0x298) and F (+0x294) of FUN_00420870. U-9706."),
    dict(id="C29-RESIDUAL", kind=["EXTRA", "MOVED"], tex=r"^(-|Advantage|RWObjShad)$",
         tracks=["Egypt", "Neustein", "Dump", "Warzone"], status="OPEN", evidence="texcompare.py",
         note="Single rows with no mapped cause. Egypt row 740: one B-only 6-triangle ground patch with an unnamed "
              "#128x64:21 texture that the standalone also draws, and matches, in rows 780 and 815. Neustein: one thin "
              "untextured mesh drawn at the origin (AABB 0.07 x 2.5 x 0.09) where the original has it with AABB minimum (7.70, 2.67, 9.23). "
              "Dump row 735: two Advantage parts offset by 0.0123 (tolerance 0.01). Warzone row 659: RWObjShad 10 triangles "
              "vs the original's 6 (U-9710 covers cylinder shadows, which this row is not shown to be). U-9718."),
]
# order: sibling entries first (so their ids win), then the generic offscreen cite, then the new cause groups
generic = [n for n in NEW if n["id"] in ("C01c-CULL-A",)]
rest = [n for n in NEW if n["id"] not in ("C01c-CULL-A",)]
final = out + generic + rest
(HERE / "cites_r10_v2.json").write_text(json.dumps(final, indent=1))
print(len(final), "entries")
