"""rhooks: cites_rhooks.json = verify/r10_20261016/cites_r10_v3.json with this lane's changes (written after the arm
scorings, like r10's v2/v3; nothing is removed, every change is listed in RESULT_rhooks.md section 4)."""
import json
from pathlib import Path
HERE = Path(__file__).resolve().parent
src = json.loads((HERE.parent / "r10_20261016" / "cites_r10_v3.json").read_text())
out = []
for c in src:
    c = dict(c)
    if c["id"] == "C22-SLOT-PLACEMENT" and c.get("tracks") == ["Warzone"]:
        c["status"] = "PORTED"
        c["note"] = ("[rhooks] Ported (Render/TrackHookSlots.cpp): Warzone slot 0 0x004527f0 groups, slot 2 0x00452d10, "
                     "slot 10 0x00452da0, the crater switch entries (FUN_00452620), the Pylon LOD hook (0x00452aba). "
                     "No row of this group remains on the 3 frames. Original note: " + c["note"])
    elif c["id"] == "C22-SLOT-PLACEMENT" and c.get("tracks") == ["Sands"]:
        c["status"] = "PORTED"
        c["note"] = ("[rhooks] Ported: Sands slot 2 0x00450910 and the switch table (FUN_00450300: BridgeWhole ON, "
                     "Bridgehafbroke / BridgeBroke / Jump OFF). No row remains. Original note: " + c["note"])
    elif c["id"] == "C22-SLOT-PLACEMENT" and c.get("tracks") == ["Highway"]:
        c["note"] = ("[rhooks] The excluded truck / lorry / Jet clumps are no longer drawn at the origin (no draw site "
                     "outside slot 2's traffic array 0x00890060 and the DAT_005f9748-gated Jet). The traffic itself (slots "
                     "0 / 1 0x0044f970 / 0x0044eeb0) is not ported: one MISSING traffic3 row per frame, U-9721. Original "
                     "note: " + c["note"])
    elif c["id"] == "C22-SLOT-PLACEMENT" and c.get("tracks") == ["Roundabout"]:
        c["note"] = ("[rhooks] The LOD hooks (Pylon 17<-18 r60, Frame 19<-20 r30, FUN_00557a60) and the slot-2 states are "
                     "ported and match on every frame. What remains: the three railcar clones of clump 2 (comp2 716 + "
                     "gravel2, FUN_0044c8b0 / FUN_0044c4f0: in the RpWorld, on the course spline course+0x100b8, scene "
                     "physics bodies, U-9719) and the crane clump 1's pose (slot 1 FUN_0044cb00, two atomics not drawn "
                     "by the original, U-9720). Original note: " + c["note"])
    elif c["id"] == "C26-RWP-OBJECTS":
        c["note"] = ("[rhooks] The race-reset scenario visibility FUN_00471cf0 is ported (TrackRenderer.cpp, "
                     "MASHED_NO_SCENARIOVIS): the walls / Hut_Signs extras are gone. What remains: Roundabout Trucktyre "
                     "AABB 0.0115 off, Neustein Post_1 (49 records, none drawn by the original, cause not decoded), "
                     "U-9716. Original note: " + c["note"])
    elif c["id"] == "C28-ANIM-CLUMP":
        c["kind"] = ["MISSING", "MOVED"]
        c["status"] = "PORTED"
        c["note"] = ("[rhooks] SetClumpAnim ported (FUN_0047ad40 records, FUN_00426700 tick on the copters' units, "
                     "FUN_004b47e0 / FUN_00532b80 for LINEAR = 1). The train is drawn; on all three frames it is 0.0571 "
                     "ahead in x, the train's motion over one tick (2.28 per 40 rows): a one-tick phase, U-9723. "
                     "Original note: " + c["note"])
    elif c["id"] == "C29-RESIDUAL":
        c["note"] = ("[rhooks] The Neustein part is resolved: the thin untextured mesh at the origin was the Trunk base "
                     "clump 3, which the original never draws (its copies are slot-2 groups). Original note: " + c["note"])
    out.append(c)
out.append({
    "id": "C30-TXD-SCOPE", "kind": ["EXTRA"], "tex": "^trainroof$", "tracks": ["Dump"], "status": "MAPPED",
    "evidence": "piz contents (TRAINROOF only in CITY.TXD of dump.piz), dump COURSE.LUA line 52, TrackRenderer.cpp:1419-1421",
    "note": ("Once the train is drawn (C28), its roof material: the original draws those 32 triangles UNTEXTURED (the "
             "matched-AABB A-side draw sits in C03-UNTEXTURED), the standalone with 'trainroof'. The texture exists only "
             "in CITY.TXD inside dump.piz; COURSE.LUA names Texture_Dictionary_Filename(\"dump.txd\"); the standalone "
             "resolves materials across every TXD in the piz (TrackRenderer.cpp:1419-1421). U-9722.")})
(HERE / "cites_rhooks.json").write_text(json.dumps(out, indent=1))
print(len(src), "->", len(out))
