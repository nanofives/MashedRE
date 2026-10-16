#!/usr/bin/env python3
"""Build verify/rprops_20261016/cites_rprops.json from d1r1's cites with the notes rprops changes.

d1r1's file is left untouched. Only the `note` text of C12, C13, C15 and C20 changes (and one C15
entry's matcher); patterns, ids and kinds are as d1r1 wrote them, so the verdicts stay comparable.
"""
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
src = json.loads((HERE.parent / "d1r1_20261016" / "cites_d1r1.json").read_text(encoding="utf8"))

NOTES = {
    "C12-CITYPLACE":
        "City repeated props. rprops F-PAIR places the 27 TreeTrunks.mts trunks and 6 Standard.mts buildings "
        "(Clump_Mts_Pair, Course::CreateFromDescription 0x00479330), so the bark/Grass and building rows match. "
        "What is left: (a) the 'oaktreesprite' leaf sprites (City record slot 0 0x0044b3c0 -> FUN_0047c330 / "
        "FUN_0047c560, OakLeaf.mts), not ported; (b) building copies wholly outside the view frustum, which "
        "the original culls and librw does not (counted as off-screen in the row). U-9695, U-9700.",
    "C13-PROPPLACE":
        "Camera-men (entries 1..N of the track's camera table, helpers 0x00448dc0 etc.) and the Arctic "
        "freighter / fishing boats (0x00448770 / 0x00448880 placement, 0x00448cb0 / 0x00448e20 per-frame tick) "
        "are placed by rprops F-CAM / F-SHIPS. A row left here is yaw (the camera-man yaw is run-time state "
        "rewritten by 0x00441d40, U-9698) or ship tick phase (U-9699).",
    "C15-SKY":
        "Arctic sky. rprops F-SKY draws the 86-triangle sky mesh the way 0x004491e0 (rotated copy, Arctic "
        "only) and 0x00426d20 (unrotated copy) do, with the root-frame offset discarded, so the sky rows match. "
        "The remaining 'World' / '-' pair is NOT a sky-pass draw: the 22-triangle mesh is an ordinary world-sector "
        "draw (backtrace 0x0049aa7a, Z test and write on) that the standalone binds an unnamed 256x256 A8R8G8B8 "
        "texture to instead of the original's 'World' 256x256 R5G6B5. U-9696.",
    "C20-STATE-SKY":
        "Sky draw state. The original draws the sky with depth test and write off (FUN_004492b0, "
        "RwRenderStateSet 8 and 6 = 0), blend per the raster alpha flag, cull whatever precedes it. The librw "
        "sky pass expresses depth-test-off as ZENABLE 1 / ZFUNC ALWAYS / ZWRITE 0 (rw::ZTESTENABLE 0), the same "
        "visible behaviour in a different encoding; alpha test and cull are librw's defaults (C19).",
}

out = []
for e in src:
    e = dict(e)
    if e["id"] in NOTES and ("detail" not in e or e["id"] != "C15-SKY" or e.get("kind") != "EXTRA"):
        e["note"] = NOTES[e["id"]]
    elif e["id"] == "C15-SKY" and e.get("kind") == "EXTRA":
        e["note"] = ("Same 22 triangles as the original's Arctic 'World' mesh (AABB equal): a world-sector draw "
                     "the standalone textures with an unnamed 256x256 A8R8G8B8 raster. Not the sky pass. U-9696.")
    out.append(e)
(HERE / "cites_rprops.json").write_text(
    "[\n" + ",\n".join(" " + json.dumps(e, ensure_ascii=False) for e in out) + "\n]\n", encoding="utf8")
print("wrote", len(out), "entries")
