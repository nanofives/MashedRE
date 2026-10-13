# mscompare.py <sweep_dir> <base.map> <t4.map> -- text4 collateral: record multisets base vs t4 per screen.
# For every sweep/sa<S>_base.json + sa<S>_t4.json pair, all four frames (f200..f203), each record is keyed by
# its decoded geometry/colours/uv/tex/blend (drawlist_diff.decode_raw_blob, rounded to 1e-4) and counted.
# Records whose first return resolves to LogoOverlayDraw (the animated checker, as sumdiff32.py treats it)
# are counted apart, as are the font (tex 9) records. Prints per screen: total records each side, and the
# non-checker records only in base / only in t4, with up to 6 examples of each.
import json, sys, bisect
from collections import Counter
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[3] / "re" / "tools"))
import drawlist_diff as dd

sw = Path(sys.argv[1]); maps = {"base": dd.load_msvc_map(sys.argv[2]), "t4": dd.load_msvc_map(sys.argv[3])}

def sym(rets, m):
    addrs, names = m
    out = []
    for r in rets[:2]:
        v = int(r, 16); k = bisect.bisect_right(addrs, v) - 1
        out.append(names[k] if k >= 0 else "?")
    return out

def key(rec):
    d = dd.decode_raw_blob(rec["v"])
    return (round(d["x"], 4), round(d["y"], 4), round(d["w"], 4), round(d["h"], 4),
            tuple(d["colors"]), tuple(d["uv"]), tuple(rec.get("s") or ()))

def load(path, m):
    data = json.load(open(path))
    rest, chk, font = Counter(), 0, 0
    for lab, recs in data.items():
        for r in recs:
            s = sym(r.get("r", []), m)
            if any("LogoOverlayDraw" in x for x in s):
                chk += 1
            elif (r.get("s") or [None])[0] == 9:
                font += 1; rest[key(r)] += 1
            else:
                rest[key(r)] += 1
    return rest, chk, font

screens = sorted({int(p.stem[2:].split("_")[0]) for p in sw.glob("sa*_base.json")})
for S in screens:
    pb, pt = sw / f"sa{S}_base.json", sw / f"sa{S}_t4.json"
    if not (pb.exists() and pt.exists()):
        print(f"s{S}: missing capture"); continue
    rb, cb, fb = load(pb, maps["base"]); rt, ct, ft = load(pt, maps["t4"])
    ob, ot = rb - rt, rt - rb
    nb, nt = sum(ob.values()), sum(ot.values())
    print(f"s{S}: records base {sum(rb.values())+cb} t4 {sum(rt.values())+ct} | checker {cb}/{ct} | font {fb}/{ft} | "
          f"non-checker only-base {nb} only-t4 {nt}")
    for tag, c in (("  -base", ob), ("  +t4  ", ot)):
        for k, n in list(c.items())[:6]:
            print(f"{tag} x{n} x={k[0]} y={k[1]} w={k[2]} h={k[3]} col={'/'.join(f'{v:08x}' for v in k[4][:2])} s={k[-1]}")
