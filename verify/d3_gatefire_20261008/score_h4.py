# Scorer for PREREG_H4.md section 2. Reads the H4_<arm>.{ptrace.log,step.csv,gates.csv}.
import csv, sys
from pathlib import Path
from collections import Counter

D = Path(__file__).resolve().parent


def ptrace(arm):
    p = D / f"H4_{arm}.ptrace.log"
    return p.read_bytes().splitlines() if p.exists() else None


def step(arm):
    p = D / f"H4_{arm}.step.csv"
    if not p.exists():
        return None, None
    with open(p, newline="") as f:
        r = csv.DictReader(f)
        rows = {(row["frame"], row["seq"], row["v"]): row for row in r}
        return r.fieldnames, rows


def gates(arm):
    p = D / f"H4_{arm}.gates.csv"
    if not p.exists():
        return []
    with open(p, newline="") as f:
        return list(csv.DictReader(f))


def step_cmp(a, b, skip=()):
    ca, ra = step(a)
    cb, rb = step(b)
    if ra is None or rb is None:
        return "MISSING"
    cols = [c for c in ca if c in cb and c not in skip]
    keys = set(ra) & set(rb)
    only = len(set(ra) ^ set(rb))
    diff = Counter()
    for k in keys:
        for c in cols:
            if ra[k][c] != rb[k][c]:
                diff[c] += 1
    return dict(shared=len(keys), only_one=only, ncols=len(cols),
                cols_differing=dict(diff.most_common(10)))


def pt_cmp(a, b):
    la, lb = ptrace(a), ptrace(b)
    if la is None or lb is None:
        return "MISSING"
    first = next((i for i, (x, y) in enumerate(zip(la, lb)) if x != y), None)
    if first is None and len(la) != len(lb):
        first = min(len(la), len(lb))
    return dict(lines=(len(la), len(lb)), identical=(la == lb), first_diff_line=first)


print("H4-FLOOR  ptrace A vs A2:", pt_cmp("A", "A2"))
print("H4-FLOOR  step   A vs A2:", step_cmp("A", "A2"))
for arm in "A A2 B C D".split():
    g = gates(arm)
    v0 = Counter(r["slot_state"] for r in g if r["v"] == "0")
    base = Counter(r["p5f2770"] for r in g)
    # w_fire is g_wireFire[v], a CUMULATIVE per-car counter: take the max per car and
    # the first frame at which each car's counter rises.
    fire, first = {}, {}
    for r in g:
        v, w = r["v"], int(r["w_fire"] or 0)
        if w > fire.get(v, 0):
            fire[v] = w
            first.setdefault(v, int(r["frame"]))
    print(f"H4-ARM/FIRE {arm}: rows={len(g)} v0 slot_state={dict(v0)} p5f2770={dict(base)} "
          f"w_fire_max={fire} first_fire_frame={first}")
print("H4-C0-INERT ptrace A vs B:", pt_cmp("A", "B"))
print("H4-C0-INERT step   A vs B (ss_* excluded):",
      step_cmp("A", "B", skip=("ss_base", "ss_v", "ss_raw")))
print("H4-C0-INERT step   A vs B (all cols):", step_cmp("A", "B"))
r = pt_cmp("C", "D")
print("H4-C0-B2  ptrace C vs D:", r)
if isinstance(r, dict) and r["first_diff_line"] is not None:
    lc, ld = ptrace("C"), ptrace("D")
    i = r["first_diff_line"]
    for j in (i, i + 1, i + 120, i + 1200):
        if j < min(len(lc), len(ld)):
            print(f"  line {j}\n    C: {lc[j][:400]!r}\n    D: {ld[j][:400]!r}")
