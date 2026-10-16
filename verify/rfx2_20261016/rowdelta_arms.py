"""rfx: rows new / gone between two arms (PREREG_rfx.md section 4 rule R2), per frame.

    py -3.12 rowdelta_arms.py <before_arm> <after_arm> [--tex REGEX]

Arms are log/rfx/<arm>/<track>/diff_<K>.json (run_arm.py). Row identity is d1r1's rowdelta.py key:
(kind, texture, detail without counts) for STATE/TEX1/FOG, (kind, texture) otherwise.
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed") / "log" / "rfx2"


def key(r):
    if r["kind"] in ("STATE", "TEX1"):
        return (r["kind"], r["tex"], re.sub(r"^\d+ matched draws", "", r["detail"]))
    if r["kind"] == "FOG":
        return (r["kind"], re.sub(r"\d+ matched draws, ", "", r["detail"]).split(",")[0])
    return (r["kind"], r["tex"])


def main() -> int:
    a, b = sys.argv[1], sys.argv[2]
    rx = re.compile(sys.argv[sys.argv.index("--tex") + 1]) if "--tex" in sys.argv else None
    new_total = 0
    for fb in sorted((ROOT / a).glob("*/diff_*.json")):
        fa = ROOT / b / fb.parent.name / fb.name
        if not fa.exists():
            continue
        rb = {key(r): r for r in json.loads(fb.read_text())["rows"]}
        ra = {key(r): r for r in json.loads(fa.read_text())["rows"]}
        new = [k for k in ra if k not in rb]
        gone = [k for k in rb if k not in ra]
        new_total += len(new)
        tag = f"{fb.parent.name}:{fb.stem.split('_')[-1]}"
        print(f"{tag:<16} before {len(rb):>3} after {len(ra):>3}  gone {len(gone):>3}  NEW {len(new)}")
        for k in gone:
            if rx is None or rx.search(rb[k]["tex"]):
                print(f"    gone {rb[k]['kind']} {rb[k]['tex']} [{rb[k].get('cite')}]")
        for k in new:
            print(f"    NEW  {ra[k]['kind']} {ra[k]['tex']} [{ra[k].get('cite')}]: {ra[k].get('detail', '')[:200]}")
    print(f"NEW ROWS TOTAL: {new_total}")
    return 0 if new_total == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
