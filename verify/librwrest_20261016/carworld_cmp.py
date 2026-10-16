"""librwrest W2 helper: per draw, compare the D3DTS_WORLD the recorder stored for draws whose
stage-0 texture matches a regex, between two draw records (e.g. original vs standalone, or base vs
post standalone). Draws are paired in recorded order within each (texture, triangle count) key.

    py -3.12 carworld_cmp.py <a.draws.json> <b.draws.json> [--tex REGEX] [--out out.txt]
"""
import argparse
import json
import re
from collections import defaultdict
from pathlib import Path


def draws(p):
    j = json.loads(Path(p).read_text())
    if isinstance(j, dict) and "events" in j:   # d1r1 recorder layout (both sides)
        return [e for e in j["events"] if e.get("e") == "d"]
    if isinstance(j, dict):
        for k in ("draws", "d"):
            if k in j:
                return j[k]
        for v in j.values():
            if isinstance(v, list) and v and isinstance(v[0], dict):
                return v
    return j


def key_of(d):
    t = d.get("tex") or d.get("t0") or "-"
    if isinstance(t, list):
        t = t[0] if t else "-"
    n = d.get("tri") or d.get("ntri") or d.get("n")
    if isinstance(n, list):
        n = n[1] if len(n) > 1 else n[0]
    return str(t), n


def world(d):
    for k in ("world", "W", "w", "wm"):
        if k in d:
            return d[k]
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a"); ap.add_argument("b")
    ap.add_argument("--tex", default="^Advantage")
    ap.add_argument("--out", default="")
    a = ap.parse_args()
    rx = re.compile(a.tex, re.I)
    ga, gb = defaultdict(list), defaultdict(list)
    da, db = draws(a.a), draws(a.b)
    for d in da:
        k = key_of(d)
        if rx.search(k[0] or ""):
            ga[k].append(d)
    for d in db:
        k = key_of(d)
        if rx.search(k[0] or ""):
            gb[k].append(d)
    out = [f"a={a.a} ({len(da)} draws) b={a.b} ({len(db)} draws) tex~{a.tex}"]
    for k in sorted(set(ga) | set(gb), key=lambda x: (x[0], str(x[1]))):
        la, lb = ga.get(k, []), gb.get(k, [])
        out.append(f"key {k}: a={len(la)} b={len(lb)}")
        for i in range(min(len(la), len(lb))):
            wa, wb = world(la[i]), world(lb[i])
            if wa is None or wb is None:
                out.append(f"  #{i}: no world matrix field")
                continue
            d = max(abs(x - y) for x, y in zip(wa, wb))
            out.append(f"  #{i}: max|dW|={d:.6f}  a.pos=({wa[12]:.3f},{wa[13]:.3f},{wa[14]:.3f})"
                       f" b.pos=({wb[12]:.3f},{wb[13]:.3f},{wb[14]:.3f})")
    s = "\n".join(out)
    if a.out:
        Path(a.out).write_text(s + "\n")
    print(s)


if __name__ == "__main__":
    main()
