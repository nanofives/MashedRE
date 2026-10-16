"""r10: summarise the render-site chains captured by bt_capture.py (orig_row_*.draws.json 'bt' fields).

    py -3.12 bt_summary.py [--depth 6] [--tracks 1,2]

Per (texture, chain of the first --depth return addresses) prints the tracks, the number of draws and the frames.
Chains are MASHED.exe VAs (image base 0x400000). Only the chain matters here, never the draw counts (new original run).
"""
import argparse, glob, json, importlib.util
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("d3", HERE.parent.parent / "re" / "tools" / "drawlist3d.py")
d3 = importlib.util.module_from_spec(spec); spec.loader.exec_module(d3)


def clean_chain(bt):
    """MASHED.exe VAs only (0x401000..0x5d0000), immediate duplicates collapsed: the raw chain also carries the
    d3d9 shim frame, a repeated site and stale stack words."""
    out = []
    for x in bt:
        if not x.startswith("0x"):
            continue
        v = int(x, 16)
        if not (0x401000 <= v < 0x5d0000):
            continue
        if out and out[-1] == x:
            continue
        out.append(x)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--depth", type=int, default=6)
    ap.add_argument("--tracks", default="")
    ap.add_argument("--src", default="bt")
    a = ap.parse_args()
    g = defaultdict(lambda: {"tracks": set(), "n": 0, "tri": 0})
    for f in sorted(glob.glob(str(HERE / a.src / "[0-9][0-9]_*" / "orig_row_*.draws.json"))):
        p = Path(f); trk = p.parent.name
        if a.tracks and trk.split("_")[0].lstrip("0") not in a.tracks.split(","):
            continue
        doc = json.loads(p.read_text())
        for e in d3.cut_frame(doc):
            if "bt" not in e:
                continue
            key = (d3.clean(e.get("tex")), tuple(clean_chain(e["bt"])[:a.depth]))
            x = g[key]; x["tracks"].add(trk[3:]); x["n"] += 1; x["tri"] += e.get("tri") or 0
    for (tex, ch), x in sorted(g.items()):
        print(f"{tex:<18} n={x['n']:<4} tri={x['tri']:<6} {','.join(sorted(x['tracks'])):<40} {' <- '.join(ch)}")


if __name__ == "__main__":
    main()
