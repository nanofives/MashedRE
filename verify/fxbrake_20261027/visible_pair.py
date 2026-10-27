"""D4-FX-BRAKEBYTE-WRITER: per-call pairing of the FX-visible brake byte, port step dump vs original aistep captures.

Usage: py -3.12 visible_pair.py <port step.csv> <orig aistep.csv> [<orig aistep.csv> ...]

Both sides cut to the (b) window (220 calls from the first c4 != 0). Visible byte = visible_c5.py's model:
port replay(N) = ai_override(N-1) != 0 or ai_override(N) != 0 (dump after the countdown),
original replay(N) = ai_override(N) != 0 (probe inside FUN_00416250, before the countdown at 0x004187e2).
Prints, per slot and original file, calls where the step byte, the replay flag and the visible byte agree.
"""
import csv
import sys
from collections import defaultdict


def series(path, orig):
    seq = defaultdict(list)
    with open(path, newline="") as f:
        for r in csv.DictReader(f):
            seq[int(r["v"])].append((int(r["c5"]), int(float(r["ai_override"])), int(r["c4"])))
    out = {}
    for v, allr in seq.items():
        i0 = next((i for i, x in enumerate(allr) if x[2] != 0), None)
        if i0 is None:
            continue
        prev = allr[i0 - 1][1] if i0 > 0 else 0
        vis, res = 0, []
        for c5, ov, _ in allr[i0:i0 + 220]:
            rep = (ov != 0) if orig else (prev != 0 or ov != 0)
            vis = vis if rep else c5
            res.append((c5, rep, vis))
            prev = ov
        out[v] = res
    return out


def main(port, origs):
    p = series(port, False)
    for o in origs:
        q = series(o, True)
        parts = []
        for v in (1, 2, 3):
            a, b = p.get(v, []), q.get(v, [])
            n = min(len(a), len(b))
            eq = [sum(a[i][k] == b[i][k] for i in range(n)) for k in range(3)]
            parts.append(f"v={v} n={n} step {eq[0]} replay {eq[1]} visible {eq[2]}")
        print(f"{o.replace(chr(92), '/').split('/')[-1]:30s} " + " | ".join(parts))


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2:])
