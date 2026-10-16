#!/usr/bin/env python3
"""Rule 4 (no-op control): two arms' standalone draw records, byte for byte and event for
event with the wall-clock field `t` removed.

    py -3.12 noop_check.py <arm A> <arm B>
"""
import json
import sys
from pathlib import Path

LOG = Path(__file__).resolve().parents[2] / "log" / "rstates"
SHOTS = {"training": (884, 924, 959), "arctic": (1051, 1091, 1126), "city": (761, 801, 836)}


def strip(doc):
    return [{k: v for k, v in e.items() if k != "t"} for e in doc["events"]]


def main(argv):
    a_arm, b_arm = argv[0], argv[1]
    n_same = n = 0
    for t, ks in SHOTS.items():
        for k in ks:
            fa = LOG / a_arm / t / "d1r2" / f"stand_row_{k:05d}.draws.json"
            fb = LOG / b_arm / t / "d1r2" / f"stand_row_{k:05d}.draws.json"
            if not (fa.exists() and fb.exists()):
                print(f"{t}:{k} missing"); continue
            ra, rb = fa.read_bytes(), fb.read_bytes()
            ea, eb = strip(json.loads(ra)), strip(json.loads(rb))
            diff = [i for i, (x, y) in enumerate(zip(ea, eb)) if x != y]
            same = ea == eb
            n += 1; n_same += same
            what = "bytes-identical" if ra == rb else ("events-identical(excl t)" if same else
                   f"DIFFER at events {diff[:6]} (n={len(diff)}, lens {len(ea)}/{len(eb)})")
            print(f"{t}:{k} {what}")
            for i in diff[:3]:
                print(f"    A {json.dumps(ea[i])[:160]}\n    B {json.dumps(eb[i])[:160]}")
    print(f"identical (excl t): {n_same}/{n}")


if __name__ == "__main__":
    main(sys.argv[1:])
