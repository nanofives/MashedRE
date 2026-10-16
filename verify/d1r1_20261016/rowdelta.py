"""d1r1 collateral review: rows present in the AFTER diff that the BEFORE diff does not have
(PREREG section 4 rule 2), plus rows that went away, per frame.

    py -3.12 rowdelta.py <log/d1r1 dir> <before prefix, e.g. diff_stand_> <after prefix>

A row is identified by (kind, texture, detail-without-counts) for STATE/TEX1/FOG (the state
tuples are in the detail) and by (kind, texture) otherwise.
"""
import json
import re
import sys
from pathlib import Path


def key(r):
    if r["kind"] in ("STATE", "TEX1"):
        return (r["kind"], r["tex"], re.sub(r"^\d+ matched draws", "", r["detail"]))
    if r["kind"] == "FOG":
        return (r["kind"], re.sub(r"\d+ matched draws, ", "", r["detail"]).split(",")[0])
    return (r["kind"], r["tex"])


def main() -> int:
    base, pb, pa = Path(sys.argv[1]), sys.argv[2], sys.argv[3]
    new_total = 0
    for fb in sorted(base.glob(f"orig_*/{pb}*.json")):
        fa = fb.with_name(fb.name.replace(pb, pa, 1))
        if not fa.exists():
            continue
        rb = {key(r): r for r in json.loads(fb.read_text())["rows"]}
        ra = {key(r): r for r in json.loads(fa.read_text())["rows"]}
        new = [k for k in ra if k not in rb]
        gone = [k for k in rb if k not in ra]
        new_total += len(new)
        tag = f"{fb.parent.name[5:]}:{fb.stem.split('_')[-1]}"
        print(f"{tag:<16} before {len(rb):>3} after {len(ra):>3}  gone {len(gone):>3}  NEW {len(new)}")
        for k in new:
            print(f"    NEW {ra[k]['kind']} {ra[k]['tex']}: {ra[k]['detail'][:200]}")
    print(f"NEW ROWS TOTAL: {new_total}")
    return 0 if new_total == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
