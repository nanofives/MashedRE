#!/usr/bin/env python3
"""R-NOOP: the opt-out arm (MASHED_NO_PROPPLACE=1 MASHED_NO_SKYFIX=1) must reproduce the base arm's draw
records and BMPs byte for byte. Usage: py -3.12 rnoop.py <base_arm> <noop_arm> [--out file]"""
import hashlib
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
LOG = WT / "log" / "rprops"


def h(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()[:16]


def main():
    base, noop = sys.argv[1], sys.argv[2]
    lines, n, t = [], 0, 0
    for trk in ("training", "arctic", "city"):
        d = LOG / f"{base}_{trk}" / "d1r2"
        for f in sorted(list(d.glob("*.draws.json")) + list(d.glob("*.bmp"))):
            g = LOG / f"{noop}_{trk}" / "d1r2" / f.name
            same = g.exists() and h(f) == h(g)
            t += 1
            n += same
            lines.append(f"{trk}/{f.name} base={h(f)} noop={h(g) if g.exists() else '-'} "
                         f"{'IDENTICAL' if same else 'DIFFERENT'}")
    lines.append(f"R-NOOP: {n}/{t} files byte-identical (draw records and BMPs), "
                 f"MASHED_NO_PROPPLACE=1 MASHED_NO_SKYFIX=1 vs the unchanged base build")
    txt = "\n".join(lines)
    print(lines[-1])
    if "--out" in sys.argv:
        Path(sys.argv[sys.argv.index("--out") + 1]).write_text(txt + "\n")


if __name__ == "__main__":
    main()
