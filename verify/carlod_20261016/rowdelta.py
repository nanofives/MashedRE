"""carlod row delta between two arms (PREREG section 3 rules 1-2): per frame, rows the AFTER arm
has that the BEFORE arm lacks (NEW) and rows that went away (GONE).

    py -3.12 rowdelta.py <before arm> <after arm>      (arms under log/carlod/)

Row identity is d1r1's rowdelta.py key: (kind, tex, detail without counts) for STATE / TEX1,
(kind, first FOG pair) for FOG, (kind, tex) otherwise. Exit 1 when any NEW row exists.
"""
import json
import re
import sys
from pathlib import Path

LOG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\log\carlod")


def key(r):
    if r["kind"] in ("STATE", "TEX1"):
        return (r["kind"], r["tex"], re.sub(r"^\d+ matched draws", "", r["detail"]))
    if r["kind"] == "FOG":
        return (r["kind"], re.sub(r"\d+ matched draws, ", "", r["detail"]).split(",")[0])
    return (r["kind"], r["tex"])


def main() -> int:
    a, b = LOG / sys.argv[1], LOG / sys.argv[2]
    new_total = 0
    for fa in sorted(a.glob("diff_*.json")):
        fb = b / fa.name
        if not fb.exists():
            continue
        ra = {key(r): r for r in json.loads(fa.read_text())["rows"]}
        rb = {key(r): r for r in json.loads(fb.read_text())["rows"]}
        new = [k for k in rb if k not in ra]
        gone = [k for k in ra if k not in rb]
        new_total += len(new)
        tag = fa.stem[5:]
        print(f"{tag:<16} before {len(ra):>3} after {len(rb):>3}  gone {len(gone):>3}  NEW {len(new)}")
        for k in gone:
            r = ra[k]
            print(f"    gone {r['kind']} {r['tex']} [{r.get('cite')}]: {r['detail'][:150]}")
        for k in new:
            r = rb[k]
            print(f"    NEW  {r['kind']} {r['tex']} [{r.get('cite')}]: {r['detail'][:200]}")
    print(f"NEW ROWS TOTAL: {new_total}")
    return 0 if new_total == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
