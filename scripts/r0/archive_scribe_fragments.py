#!/usr/bin/env python3
"""R0 scribe-fragment archival (Phase R0, 2026-06-09).

70 one-row re/SCRIBE_QUEUE_<batch>_s<N>.md fragments were never back-annotated
after their batches were centrally swept. For each fragment: parse every
rvas= list, verify each RVA is accounted for in hooks.csv (confidence C2+ or a
third-party-library row kept at C1 by convention). Fully-accounted fragments
move to re/archive/scribe_fragments/; any straggler stays put and is reported.
"""
import csv
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEST = ROOT / "re" / "archive" / "scribe_fragments"


def load_hooks():
    conf, subsys = {}, {}
    with open(ROOT / "hooks.csv", newline="", encoding="utf-8") as f:
        for row in csv.reader(f):
            if not row or row[0].startswith("#") or row[0] == "rva":
                continue
            conf[row[0].lower()] = row[3]
            subsys[row[0].lower()] = row[2]
    return conf, subsys


def main() -> int:
    conf, subsys = load_hooks()
    frags = sorted(ROOT.glob("re/SCRIBE_QUEUE_*_s*.md"))
    DEST.mkdir(parents=True, exist_ok=True)
    moved, kept = 0, []
    for p in frags:
        text = p.read_text(encoding="utf-8")
        rvas = []
        for grp in re.findall(r"rvas(?:_c2)?=([0-9a-fA-Fx,]+)", text):
            rvas += [r.lower().removeprefix("0x")
                     for r in grp.split(",") if r.strip()]
        unaccounted = [r for r in rvas
                       if not (conf.get(r) in ("C2", "C3", "C4")
                               or subsys.get(r, "").startswith(
                                   "third-party-library"))]
        if unaccounted:
            kept.append((p.name, unaccounted[:5], len(rvas)))
        else:
            shutil.move(str(p), DEST / p.name)
            moved += 1
    print(f"fragments={len(frags)} archived={moved} kept={len(kept)}")
    for name, miss, total in kept:
        print(f"KEPT {name}: {len(miss)}+ unaccounted of {total}: {miss}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
