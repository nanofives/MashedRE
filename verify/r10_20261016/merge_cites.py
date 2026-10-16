"""r10: build cites_r10.json = de-duplicated union of the merged sibling cite files.
rfx2's order first (it is the newest superset), then any entry the others carry that it lacks."""
import json
from pathlib import Path
V = Path(__file__).resolve().parent.parent
SRC = ["rfx2_20261016/cites_rfx2.json", "rstates_20261016/cites_rstates.json", "rprops_20261016/cites_rprops.json",
       "rfx_20261016/cites_rfx.json", "d1r1_20261016/cites_d1r1.json"]
seen, out = set(), []
def key(c): return json.dumps({k: c.get(k) for k in ("id", "kind", "tex", "detail", "all_offscreen", "src")}, sort_keys=True)
for s in SRC:
    for c in json.loads((V / s).read_text()):
        if key(c) in seen: continue
        seen.add(key(c)); c = dict(c); c["_from"] = s.split("/")[0]; out.append(c)
(Path(__file__).resolve().parent / "cites_r10.json").write_text(json.dumps(out, indent=1))
print(len(out), "entries")
