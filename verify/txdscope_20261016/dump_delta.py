"""txdscope: what the scoped lookup changed in the Dump draw files.

    py -3.12 verify\\txdscope_20261016\\dump_delta.py <base dir> <after dir>

Per Dump frame: events in each arm, events whose record differs (same index), grouped by (field -> before -> after) for the
differing scalar fields, and the texture names / ids the before-arm had where the after-arm has none.
"""
import json
import sys
from collections import Counter
from pathlib import Path


def load(p):
    return json.loads(Path(p).read_text())


def main():
    a, b = Path(sys.argv[1]), Path(sys.argv[2])
    for k in (735, 775, 810):
        fa = a / "10_Dump" / "stand" / "d1r2" / f"stand_row_{k:05d}.draws.json"
        fb = b / "10_Dump" / "stand" / "d1r2" / f"stand_row_{k:05d}.draws.json"
        ja, jb = load(fa), load(fb)
        ea, eb = ja["events"], jb["events"]
        print(f"== row {k}: events base={len(ea)} after={len(eb)}")
        if ea:
            print("   keys of an event:", sorted(ea[0].keys()) if isinstance(ea[0], dict) else type(ea[0]))
        n = min(len(ea), len(eb))
        changed = Counter()
        examples = {}
        for i in range(n):
            x, y = ea[i], eb[i]
            if x == y:
                continue
            if isinstance(x, dict) and isinstance(y, dict):
                for key in sorted(set(x) | set(y)):
                    if x.get(key) != y.get(key):
                        sig = (key, json.dumps(x.get(key))[:60], json.dumps(y.get(key))[:60])
                        changed[sig] += 1
                        examples.setdefault(sig, i)
        print(f"   differing events (same index): {sum(1 for i in range(n) if ea[i] != eb[i])}")
        for sig, c in changed.most_common(25):
            print(f"   x{c:<4} {sig[0]}: {sig[1]} -> {sig[2]}   (first at event {examples[sig]})")


if __name__ == "__main__":
    main()
