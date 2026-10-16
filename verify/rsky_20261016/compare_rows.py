"""rsky: compare the divergence rows of two scoring runs (baseline vs after).

    py -3.12 verify\\rsky_20261016\\compare_rows.py <baseline dir> <after dir> [--tracks 9,2] [--prefix diff_]

Row identity (PREREG_rsky.md A3): (kind, texture, cite) of a frame, with the row's draw count `n` compared on the
rows both sides have. Prints, per frame: rows REMOVED (in baseline only), rows ADDED (in after only), and rows
whose `n` changed. A frame with nothing printed has the same row identities and counts.
"""
import argparse
import json
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from run13 import TRACKS, shots_of  # noqa: E402


def load(d: Path, name: str, prefix: str, k: int):
    f = d / name / f"{prefix}{k:05d}.json"
    if not f.exists():
        return None
    rows = json.loads(f.read_text())["rows"]
    out = {}
    for r in rows:
        key = (r["kind"], r.get("tex", ""), r.get("cite", "RED"))
        out[key] = out.get(key, 0) + r.get("n", 0)
    return out, json.loads(f.read_text())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base"); ap.add_argument("after")
    ap.add_argument("--tracks", default=",".join(str(t) for t in TRACKS if t < 20))
    ap.add_argument("--prefix", default="diff_")
    a = ap.parse_args()
    tot = Counter()
    for t in [int(x) for x in a.tracks.split(",")]:
        name = TRACKS[t][0]
        for k in shots_of(t):
            b = load(Path(a.base), name, a.prefix, k)
            c = load(Path(a.after), name, a.prefix, k)
            if b is None or c is None:
                print(f"{name} {k}: MISSING record ({'base' if b is None else 'after'})")
                continue
            (rb, jb), (rc, jc) = b, c
            removed = sorted(set(rb) - set(rc))
            added = sorted(set(rc) - set(rb))
            changed = sorted(x for x in set(rb) & set(rc) if rb[x] != rc[x])
            hdr = (f"{name} {k}: rows {len(jb['rows'])}->{len(jc['rows'])} matched {jb['matched']}->{jc['matched']}")
            if not (removed or added or changed):
                tot["same"] += 1
                continue
            print(hdr)
            for x in removed:
                print(f"   REMOVED {x} n={rb[x]}")
                tot["removed"] += 1
            for x in added:
                print(f"   ADDED   {x} n={rc[x]}")
                tot["added"] += 1
            for x in changed:
                print(f"   CHANGED {x} n {rb[x]}->{rc[x]}")
                tot["changed"] += 1
    print(dict(tot))


if __name__ == "__main__":
    main()
