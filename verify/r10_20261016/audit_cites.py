"""r10: per cite id audit of a scored run: rows, draws, tracks, status, and the signature checks that make the
sibling track-specific cites (C13, C14, C20) valid on a new track (small moves, same state difference).

    py -3.12 audit_cites.py [--prefix diff1_] [--cites cites_r10_v2.json]
"""
import argparse, glob, json, re
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ap = argparse.ArgumentParser(); ap.add_argument("--prefix", default="diff2_"); ap.add_argument("--cites", default="cites_r10_v3.json"); ap.add_argument("--root", default=".")
a = ap.parse_args()
status = {}
for c in json.loads((HERE / a.cites).read_text()):
    status.setdefault(c["id"], c.get("status", "MAPPED (sibling lane)"))
agg = defaultdict(lambda: {"rows": 0, "tracks": set(), "frames": set(), "kinds": defaultdict(int)})
bad = []
for f in sorted(glob.glob(str((HERE / a.root).resolve() / "[0-9][0-9]_*" / f"{a.prefix}[0-9]*.json"))):
    p = Path(f); trk = p.parent.name.split("_", 1)[1]; k = p.stem.split("_")[-1]
    for r in json.loads(p.read_text())["rows"]:
        cid = r.get("cite", "RED")
        g = agg[cid]; g["rows"] += 1; g["tracks"].add(trk); g["frames"].add(f"{trk}:{k}"); g["kinds"][r["kind"]] += 1
        if cid in ("C13-PROPPLACE", "C14-ANIM"):
            m = re.search(r"moved by up to ([\d.]+)", r["detail"])
            if r["kind"] == "MOVED" and (not m or float(m.group(1)) > 0.25):
                bad.append((trk, k, cid, r["tex"], r["detail"][:90]))
        if cid == "C20-STATE-SKY" and "differ in Z,ZF" not in r["detail"]:
            bad.append((trk, k, cid, r["tex"], r["detail"][:90]))
print(f"{'cite':<24}{'status':<26}{'rows':>5} {'frames':>6}  kinds / tracks")
for cid, g in sorted(agg.items(), key=lambda kv: (kv[0] == "RED", kv[0])):
    print(f"{cid:<24}{status.get(cid, '-')[:25]:<26}{g['rows']:>5} {len(g['frames']):>6}  {dict(g['kinds'])} / {','.join(sorted(g['tracks']))[:70]}")
print("SIGNATURE VIOLATIONS (C13/C14 moved > 0.25, C20 not Z,ZF):", bad if bad else "none")
