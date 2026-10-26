"""Whole-file gamesave.bin compare, region by region (D4-WSF-SAVE-RT, verify/wsfsave_20261026).

Regions follow SerializeToBuffer 0x00404ee0 (see the PREREG's table):
  magic 0x0..0x3 | block 0x4..0x24a3f (ghost buffer, 0x928f dwords) | table 0x24a40..0x24d0f (13 x 12 dwords at 0x007f0a40)
  | span_rest 0x24d10..0x24f53 minus the 0x24efc dword | state 0x24efc (DAT_008a95ac) | gather 0x24f54..0x24f5f
  (12 bytes from 0x007f105c stride 0x4c) | tail 0x24f60..0x24f9f (no writer)

    py -3.12 re/tools/save_fullfile.py A.bin B.bin [--label X]
"""
import argparse, hashlib, sys
from pathlib import Path

SIZE = 0x24FA0
REGIONS = [("magic", 0x0, 0x4), ("block", 0x4, 0x24A40), ("table", 0x24A40, 0x24D10),
           ("span_rest", 0x24D10, 0x24F54), ("state", 0x24EFC, 0x24F00), ("gather", 0x24F54, 0x24F60),
           ("tail", 0x24F60, SIZE)]


def region_of(i):
    if 0x24EFC <= i < 0x24F00:
        return "state"
    for name, lo, hi in REGIONS:
        if name != "state" and lo <= i < hi:
            return name
    return "beyond"


def compare(a, b):
    n = max(len(a), len(b))
    diffs = [i for i in range(n) if i >= len(a) or i >= len(b) or a[i] != b[i]]
    per = {}
    for i in diffs:
        per.setdefault(region_of(i), []).append(i)
    return n, diffs, per


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a")
    ap.add_argument("b")
    ap.add_argument("--label", default="")
    x = ap.parse_args()
    a, b = Path(x.a).read_bytes(), Path(x.b).read_bytes()
    n, diffs, per = compare(a, b)
    print(f"{x.label} A={Path(x.a).name} ({len(a)} B, sha {hashlib.sha256(a).hexdigest()[:16]}) "
          f"B={Path(x.b).name} ({len(b)} B, sha {hashlib.sha256(b).hexdigest()[:16]})")
    print(f"  compared {n} bytes, differing {len(diffs)}")
    for name, lo, hi in REGIONS:
        d = per.get(name, [])
        nzA = sum(1 for i in range(lo, min(hi, len(a))) if a[i])
        nzB = sum(1 for i in range(lo, min(hi, len(b))) if b[i])
        extra = ""
        if d:
            extra = " first " + ",".join(f"{i:#x}:{a[i] if i < len(a) else None}->{b[i] if i < len(b) else None}"
                                         for i in d[:6])
        print(f"  {name:9s} {lo:#07x}..{hi - 1:#07x} differ {len(d):6d}  nonzero A {nzA:6d} B {nzB:6d}{extra}")
    if "beyond" in per:
        print(f"  beyond    differ {len(per['beyond'])}")
    if len(a) == SIZE and len(b) == SIZE:
        ca, cb = (int.from_bytes(v[0x24EFC:0x24F00], "little") for v in (a, b))
        print(f"  state dword A {ca} B {cb}; gather A {a[0x24F54:0x24F60].hex()} B {b[0x24F54:0x24F60].hex()}")
    return 0 if not diffs else 1


if __name__ == "__main__":
    sys.exit(main())
