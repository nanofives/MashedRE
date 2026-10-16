"""rsky: cites_rsky.json = cites_r10_v3.json minus the two groups this lane fixed (C23-SKY-SLOTS, C23b-OBJECT-BSP), with C10's
texture pattern also covering the name the probe now gives the car shadow render textures. Nothing is added: no group, no entry.

    py -3.12 verify\\rsky_20261016\\make_cites_rsky.py
"""
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
SRC = HERE.parent / "r10_20261016" / "cites_r10_v3.json"
c = json.loads(SRC.read_text())
keep = [e for e in c if e.get("id") not in ("C23-SKY-SLOTS", "C23b-OBJECT-BSP")]
for e in keep:
    if e.get("id") == "C10-RUNTIME-TEX":
        e["tex"] = r"^(#\d+x\d+:21|@shadowrt\d)$"
        e["note"] += (" [rsky] IDENTITY ESTABLISHED: these are the car SHADOW render-to-texture (FUN_0041f8f0 0x0041f8f0 renders the car "
                      "into the RwTexture (&DAT_0063d854)[b]; FUN_004228f0 0x004228f0 draws the shadow quads with it, call 0x00422a5e). "
                      "The probe (re/frida/camera_probe.py) now names them @shadowrt0/1 in new captures, so the pattern covers both "
                      "spellings; the standalone has no render-to-texture shadow (C04 / C04b are its stand-ins). U-9691.")
        e["status"] = "MAPPED"
(HERE / "cites_rsky.json").write_text(json.dumps(keep, indent=1))
print(len(c), "->", len(keep), [e["tex"] for e in keep if e.get("id") == "C10-RUNTIME-TEX"])
