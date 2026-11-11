"""RW-SIZING item 4: RW 3.x struct layouts against the vendored librw, field by field.

  py -3.12 verify/rwsizing_20261111/rw_layout.py

Inputs (measured, not hand-counted): MSVC 19.44 x86 `/d1reportAllClassLayout` dumps of two probes in this folder,
extracted to layout_rw3.txt / layout_librw.txt:
  probe_rw3.cpp   + probe_rw3.bat    gta-reversed-modern's RW headers (rwcore.h, rpworld.h; RW 3.6 SDK headers as
                                     vendored by gta-reversed, /std:c++17)
  probe_librw.cpp + probe_librw.bat  mashedmod/deps/librw/rw.h with librw's own build defines (/DRW_D3D9 /DNDEBUG)
Writes RW_LAYOUT.tsv. Field status:
  same      librw has the field (alias table below) at the same offset and the same width
  moved     librw has it at another offset
  width     same offset, different width (e.g. RwRaster.nOffsetX short vs rw::Raster.offsetX int)
  absent    librw's struct has no such field
game_reads cites evidence that matched game code or the ORIG_TOP route reads the field inline (RESULT files, the
taint lists in RW_INLINE_READERS.tsv). [UNCERTAIN] Mashed links RW 3.7.0.2, the headers are 3.6: the offsets the RESULT
files measured on Mashed (atomic +0x18 geometry, raster +0x22 privateFlags, camera +0x60 frameBuffer, frame +0x98 /
+0x9c / +0xa0, RwGlobals +0x20 / +0x34 / +0x60 / +0x108 / +0x10c / +0x11c) all agree with the 3.6 layout, but the
fields no RESULT measured are header values only.
"""
from __future__ import annotations

import collections
import csv
import pathlib
import re

HERE = pathlib.Path(__file__).resolve().parent

PAIRS = [
    ("RwObject", "rw::Object", {}),
    ("RwObjectHasFrame", "rw::ObjectWithFrame", {"lFrame": "inFrame", "sync": "syncCB"}),
    ("RwFrame", "rw::Frame", {"inDirtyListLink": "inDirtyList", "modelling": "matrix"}),
    ("RwMatrixTag", "rw::Matrix", {}),
    ("RwRaster", "rw::Raster", {"cpPixels": "pixels", "nOffsetX": "offsetX", "nOffsetY": "offsetY", "cType": "type",
                                "cFlags": "flags", "cFormat": "format"}),
    ("RwTexture", "rw::Texture", {"lInDictionary": "inDict"}),
    ("RwTexDictionary", "rw::TexDictionary", {"texturesInDict": "textures", "lInInstance": "inGlobalList"}),
    ("RwImage", "rw::Image", {"cpPixels": "pixels"}),
    ("RwCamera", "rw::Camera", {"projectionType": "projection", "beginUpdate": "beginUpdateCB",
                                "endUpdate": "endUpdateCB", "recipViewWindow": None}),
    ("RpAtomic", "rw::Atomic", {"repEntry": None, "inClumpLink": "inClump", "renderCallBack": "renderCB",
                                "interpolator": None, "renderFrame": None, "pad": None,
                                "llWorldSectorsInAtomic": None}),
    ("RpGeometry", "rw::Geometry", {"lockedSinceLastInst": "lockedSinceInst", "preLitLum": "colors",
                                    "mesh": "meshHeader", "repEntry": "instData", "morphTarget": "morphTargets"}),
    ("RpClump", "rw::Clump", {"atomicList": "atomics", "lightList": "lights", "cameraList": "cameras",
                              "inWorldLink": "inWorld", "callback": None}),
    ("RpMaterial", "rw::Material", {"pad": None}),
    ("RpLight", "rw::Light", {"WorldSectorsInLight": None, "lightFrame": None, "pad": None}),
    ("RpWorld", "rw::World", {"clumpList": "clumps", "lightList": "localLights", "directionalLightList": "globalLights"}),
    ("RpWorldSector", None, {}),
    ("RwGlobals", "rw::Engine", {"curCamera": "currentCamera", "curWorld": "currentWorld",
                                 "dirtyFrameList": "frameDirtyList", "dOpenDevice": None}),
]

# fields game code (or matched RW code on the ORIG_TOP route) reads inline, with the evidence
V = "verify/portorigtop_20261109/"
READS = {
    ("RpAtomic", "geometry"): V + "RESULT_ATOMIC-VIEW.md (atomic +0x18; 0x0048fef0, 0x0040baa0 view)",
    ("RpGeometry", "flags"): "re/match/s15/0048fef0.c (geometry +0x08 |= 0x40)",
    ("RpGeometry", "matList"): V + "RESULT_ATOMIC-VIEW-2.md / RESULT_PORT-OT-HUD.md (geometry +0x20 / +0x24 materials)",
    ("RpMaterial", "texture"): V + "RESULT_ATOMIC-VIEW.md (material +0x00, 0x0040baa0)",
    ("RwObject", "parent"): V + "RESULT_ATOMIC-VIEW-2.md (atom +0x04 frame; frame +0x04 parent)",
    ("RwObject", "flags"): V + "RESULT_ATOMIC-VIEW-2.md (atom +0x02 via 0x004b5240)",
    ("RwFrame", "child"): V + "RESULT_ATOMIC-VIEW-2.md (frame +0x98, 0x004c1210 / 0x004c1040 / 0x004c15c0)",
    ("RwFrame", "next"): V + "RESULT_ATOMIC-VIEW-2.md (frame +0x9c)",
    ("RwFrame", "root"): V + "RESULT_ATOMIC-VIEW-2.md (frame +0xa0)",
    ("RwTexture", "dict"): V + "RESULT_CLUMP-TEXREAD.md (texture +0x04)",
    ("RwTexture", "filterAddressing"): "RW_INLINE_READERS.tsv T1 (+0x50 on the 0x004c5a00 return)",
    ("RwRaster", "privateFlags"): V + "RESULT_RWG-RASTER-BIND.md:58 (raster +0x22 bit 0, 0x004d52d0)",
    ("RwRaster", "width"): V + "RESULT_RWG-RASTER-BIND.md (0x004cdca0(raster+0xc, raster+0x10, 0x20))",
    ("RwRaster", "height"): V + "RESULT_RWG-RASTER-BIND.md (raster +0x10)",
    ("RwCamera", "frameBuffer"): "RW_INLINE_READERS.tsv T1 (+0x60 on the 0x004c1d30 return); RESULT_RWG-RASTER-BIND.md",
    ("RwCamera", "zBuffer"): "RW_INLINE_READERS.tsv T1 (+0x64 on the 0x004c1d30 return)",
    ("RwCamera", "viewWindow"): "RW_INLINE_READERS.tsv T3 (+0x68 / +0x6c, 0x004c1c80 callers)",
    ("RwObjectHasFrame", "object"): "RW_INLINE_READERS.tsv T1 (+0x04 on the 0x004c1d30 / 0x004e7420 return)",
    ("RwGlobals", "curCamera"): "RW_INLINE_READERS.tsv T2 (+0x00, 10 functions)",
    ("RwGlobals", "dOpenDevice"): "RW_INLINE_READERS.tsv T2 (+0x18 z 14, +0x20 RenderStateSet 65, +0x24 3, +0x30 "
                                  "Im2DRenderPrimitive 13, +0x34 2 functions); RESULT_PORT-OT-RWG-SLOTS.md",
    ("RwGlobals", "stdFunc"): V + "RESULT_RWG-RASTER-BIND.md (+0x60 = stdFunc[6])",
    ("RwGlobals", "stringFuncs"): "RW_INLINE_READERS.tsv T2 (+0xc4 / +0xcc / +0xd4 / +0xf4)",
    ("RwGlobals", "memoryFuncs"): "RW_INLINE_READERS.tsv T2 (+0x108 malloc 4, +0x10c free 9)",
    ("RwGlobals", "memoryFree"): V + "RESULT_PORT-OT-HUD.md (+0x11c RwGlobalsFreeListFree)",
    ("RwGlobals", "memoryAlloc"): V + "RESULT_PORT-OT-HUD-2.md (+0x118)",
}


def parse(path):
    out, cur = {}, None
    for line in open(path, encoding="utf-8"):
        m = re.match(r"^class (\S+)\s+size\((\d+)\):", line)
        if m:
            cur = m.group(1)
            out[cur] = {"size": int(m.group(2)), "f": []}
            continue
        m = re.match(r"^\s*(\d+)\s*\|\s*(?!\|)(.*)$", line)
        if cur and m and "| |" not in line:
            name = m.group(2).strip().split()[-1]
            out[cur]["f"].append((int(m.group(1)), name))
    for c in out.values():
        fs = c["f"]
        c["w"] = {n: ((fs[i + 1][0] if i + 1 < len(fs) else c["size"]) - o) for i, (o, n) in enumerate(fs)}
    return out


def main():
    A = parse(HERE / "layout_rw3.txt")
    Bl = parse(HERE / "layout_librw.txt")
    rows = []
    for t3, tl, alias in PAIRS:
        a = A[t3]
        b = Bl.get(tl) if tl else None
        bmap = {n: o for o, n in b["f"]} if b else {}
        for o, n in a["f"]:
            ln = alias.get(n, n) if n in alias else n
            if b is None or ln is None or ln not in bmap:
                st, lo = "absent", "-"
            else:
                lo = bmap[ln]
                if lo != o:
                    st = "moved"
                elif b["w"][ln] != a["w"][n]:
                    st = "width"
                else:
                    st = "same"
            rows.append(dict(rw3_type=t3, rw3_size=a["size"], rw3_off="0x%02x" % o, rw3_field=n, width=a["w"][n],
                             librw_type=tl or "-", librw_size=b["size"] if b else "-",
                             librw_field=ln if (b and ln in bmap) else "-",
                             librw_off=("0x%02x" % lo) if lo != "-" else "-", status=st,
                             game_reads=READS.get((t3, n), "")))
    with open(HERE / "RW_LAYOUT.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, list(rows[0].keys()), delimiter="\t")
        w.writeheader()
        w.writerows(rows)
    print("type\tsize RW3/librw\tfields\tsame\tmoved\twidth\tabsent\tread-by-game (status)")
    for t3, tl, _ in PAIRS:
        rs = [r for r in rows if r["rw3_type"] == t3]
        c = collections.Counter(r["status"] for r in rs)
        rd = [r for r in rs if r["game_reads"]]
        print("%s\t%s/%s\t%d\t%d\t%d\t%d\t%d\t%s" % (t3, rs[0]["rw3_size"], rs[0]["librw_size"], len(rs), c["same"],
                                                   c["moved"], c["width"], c["absent"],
                                                   " ".join("%s:%s" % (r["rw3_field"], r["status"]) for r in rd)))
    rd = [r for r in rows if r["game_reads"]]
    c = collections.Counter(r["status"] for r in rd)
    print("\ntotal fields", len(rows), collections.Counter(r["status"] for r in rows).most_common())
    print("game-read fields", len(rd), c.most_common(), " types with a game-read field not 'same':",
          len({r["rw3_type"] for r in rd if r["status"] != "same"}))


if __name__ == "__main__":
    main()
