#!/usr/bin/env python3
"""Collateral rule 2 (PREREG_rstates.md section 3): rows in arm AFTER that arm BEFORE lacks, per
frame, with d1r1's row key (verify/d1r1_20261016/rowdelta.py: kind+texture+state detail for
STATE/TEX1, kind+texture otherwise).

    py -3.12 rowdelta2.py <before arm> <after arm> [tracks]
"""
import json
import re
import sys
from pathlib import Path

LOG = Path(__file__).resolve().parents[2] / "log" / "rstates"
SHOTS = {"training": (884, 924, 959), "arctic": (1051, 1091, 1126), "city": (761, 801, 836)}


def key(r):
    if r["kind"] in ("STATE", "TEX1"):
        return (r["kind"], r["tex"], re.sub(r"^\d+ matched draws", "", r["detail"]))
    if r["kind"] == "FOG":
        return (r["kind"], re.sub(r"\d+ matched draws, ", "", r["detail"]).split(",")[0])
    return (r["kind"], r["tex"])


def main(argv):
    before, after = argv[0], argv[1]
    tracks = argv[2].split(",") if len(argv) > 2 else list(SHOTS)
    new_total = red_total = 0
    for t in tracks:
        for k in SHOTS[t]:
            fb = LOG / before / t / f"diff_{k:05d}.json"
            fa = LOG / after / t / f"diff_{k:05d}.json"
            if not (fb.exists() and fa.exists()):
                print(f"{t}:{k} missing diff ({fb.exists()}, {fa.exists()})")
                continue
            rb = {key(r): r for r in json.loads(fb.read_text())["rows"]}
            ra = {key(r): r for r in json.loads(fa.read_text())["rows"]}
            new = [x for x in ra if x not in rb]
            gone = [x for x in rb if x not in ra]
            red = [x for x in ra if not ra[x].get("cite")]
            new_total += len(new); red_total += len(red)
            print(f"{t}:{k:<6} before {len(rb):>3} after {len(ra):>3}  gone {len(gone):>3}  "
                  f"NEW {len(new)}  RED {len(red)}")
            for x in new:
                print(f"    NEW {ra[x]['kind']} {ra[x]['tex']} [{ra[x].get('cite')}]: "
                      f"{ra[x]['detail'][:170]}")
    print(f"NEW ROWS TOTAL: {new_total}   RED TOTAL: {red_total}")
    return 0 if new_total == 0 else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
