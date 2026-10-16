"""rsky-verify: the cites of the merged tree 2dec160eb = rhooks' cites (cites_rhooks.json, the newest merged set) minus the two groups
rsky fixed (C23-SKY-SLOTS, C23b-OBJECT-BSP), with C10's texture pattern also covering the name the probe now gives the car shadow render
textures (rsky's cites_rsky.json does the same to r10's v3). Nothing is added.

    py -3.12 verify\\rskyverify_20261016\\make_cites_merged.py
"""
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
src = json.loads((HERE.parent / "rhooks_20261016" / "cites_rhooks.json").read_text())
rsky = {e["id"]: e for e in json.loads((HERE.parent / "rsky_20261016" / "cites_rsky.json").read_text())}
out = []
for e in src:
    if e["id"] in ("C23-SKY-SLOTS", "C23b-OBJECT-BSP"):
        continue
    if e["id"] == "C10-RUNTIME-TEX":
        e = dict(e)
        e["tex"] = rsky["C10-RUNTIME-TEX"]["tex"]
        e["note"] = rsky["C10-RUNTIME-TEX"]["note"]
        e["status"] = rsky["C10-RUNTIME-TEX"]["status"]
    out.append(e)
(HERE / "cites_merged.json").write_text(json.dumps(out, indent=1))
print(len(src), "->", len(out))
