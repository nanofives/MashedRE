"""r10: aggregate drawlist_diff rows over all tracks/frames.

    py -3.12 aggregate_rows.py [--prefix diff0_] [--red-only] [--by cite|tex]

Prints, per (cite-or-RED, kind, texture), the tracks and frames it appears on, row/draw/triangle counts and one
detail sample. Used to group RED rows into cause groups and to audit which tracks each existing cite was applied on.
"""
import argparse
import glob
import json
import re
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent


def nums(detail):
    m = re.search(r"(?:A|B)-only (\d+) draws (\d+) tri \((\d+) offscreen\)", detail)
    if m:
        return int(m.group(1)), int(m.group(2)), int(m.group(3))
    m = re.match(r"(\d+) draws", detail)
    return (int(m.group(1)), 0, 0) if m else (1, 0, 0)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--prefix", default="diff0_")
    ap.add_argument("--red-only", action="store_true")
    ap.add_argument("--cited-only", action="store_true")
    ap.add_argument("--tracks", default="")
    a = ap.parse_args()
    groups = defaultdict(lambda: {"frames": set(), "rows": 0, "draws": 0, "tri": 0, "off": 0, "sample": ""})
    for f in sorted(glob.glob(str(HERE / "[0-9][0-9]_*" / f"{a.prefix}[0-9]*.json"))):
        p = Path(f)
        trk = p.parent.name
        if a.tracks and trk.split("_")[0].lstrip("0") not in a.tracks.split(","):
            continue
        k = p.stem.split("_")[-1]
        for r in json.loads(p.read_text())["rows"]:
            c = r.get("cite")
            if a.red_only and c:
                continue
            if a.cited_only and not c:
                continue
            key = (c or "RED", r["kind"], r["tex"])
            g = groups[key]
            g["frames"].add(f"{trk.split('_')[1]}:{k}")
            d, t, o = nums(r["detail"])
            g["rows"] += 1; g["draws"] += d; g["tri"] += t; g["off"] += o
            if not g["sample"]:
                g["sample"] = r["detail"][:200]
    for (c, kind, tex), g in sorted(groups.items(), key=lambda kv: (kv[0][0] != "RED", kv[0])):
        trks = sorted({x.split(":")[0] for x in g["frames"]})
        print(f"{c:<22} {kind:<8} {tex[:30]:<30} frames={len(g['frames']):>2} tracks={','.join(trks)} "
              f"rows={g['rows']} draws={g['draws']} tri={g['tri']} off={g['off']}")
        print(f"      {g['sample']}")


if __name__ == "__main__":
    main()
