"""rsky: the per-track table of the RESULT (baseline arm against the final arm, same cites).

    py -3.12 verify\\rsky_20261016\\make_tables.py <base dir> <after dir> [--prefix rs_] > tables_rsky.md
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from run13 import TRACKS, shots_of  # noqa: E402


def frame(d: Path, name: str, pre: str, k: int):
    j = json.loads((d / name / f"{pre}{k:05d}.json").read_text())
    txt = (d / name / f"{pre}{k:05d}.txt").read_text()
    m = re.search(r"matched (\d+) draws \((\d+) tri\)", txt)
    mt = int(m.group(2)) if m else 0
    return {"rows": len(j["rows"]), "red": sum(1 for r in j["rows"] if "cite" not in r),
            "matched": j["matched"], "na": j["na"], "tri": j["tri_a"], "mtri": mt}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base"); ap.add_argument("after"); ap.add_argument("--prefix", default="rs_")
    ap.add_argument("--tracks", default=",".join(str(t) for t in TRACKS if t < 20))
    a = ap.parse_args()
    print("| track | rows base -> after | RED base -> after | matched draws (3 rows) base -> after | matched triangles % base -> after |")
    print("|---|---|---|---|---|")
    tb = ta = rb = ra = 0
    for t in [int(x) for x in a.tracks.split(",")]:
        name = TRACKS[t][0]
        fb = [frame(Path(a.base), name, a.prefix, k) for k in shots_of(t)]
        fa = [frame(Path(a.after), name, a.prefix, k) for k in shots_of(t)]
        rows_b, rows_a = [f["rows"] for f in fb], [f["rows"] for f in fa]
        tb += sum(rows_b); ta += sum(rows_a)
        rb += sum(f["red"] for f in fb); ra += sum(f["red"] for f in fa)
        pct = lambda fs: ", ".join(f"{100 * f['mtri'] / max(1, f['tri']):.1f}" for f in fs)
        print(f"| {name.split('_', 1)[1]} | {sum(rows_b)} -> {sum(rows_a)} | {sum(f['red'] for f in fb)} -> {sum(f['red'] for f in fa)} | "
              f"{'/'.join(str(f['matched']) for f in fb)} -> {'/'.join(str(f['matched']) for f in fa)} | {pct(fb)} -> {pct(fa)} |")
    print(f"| **all** | {tb} -> {ta} | {rb} -> {ra} | | |")


if __name__ == "__main__":
    main()
