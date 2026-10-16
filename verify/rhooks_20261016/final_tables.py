"""rhooks: per-frame and per-cite comparison of two score runs. Usage: final_tables.py <base_score.txt> <arm_score.txt>
<base_dir> <base_prefix> <arm_dir> <arm_prefix>"""
import re, sys, json, collections
from pathlib import Path
def load(p):
    d = {}
    for l in open(p, encoding="utf-8", errors="replace"):
        m = re.match(r"(\d\d_\w+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+([\d.]+)%\s+\|\s+(\d+)\s+(\d+)\s+(\d+)", l)
        if m: d[(m.group(1), int(m.group(2)))] = m.groups()
    return d
b, a = load(sys.argv[1]), load(sys.argv[2])
print("| track | frame | matched draws (base -> rhooks) | matched triangles % | rows | RED |")
print("|---|---:|---|---|---|---:|")
mb = ma = na = rb = ra = red = 0; tb = ta = tt = 0
for k in sorted(b):
    x, y = b[k], a[k]
    print(f"| {k[0][3:]} | {k[1]} | {x[3]} -> {y[3]} of {x[2]} | {x[6]} -> {y[6]} | {x[7]} -> {y[7]} | {y[9]} |")
    mb += int(x[3]); ma += int(y[3]); na += int(x[2]); rb += int(x[7]); ra += int(y[7]); red += int(y[9])
    tb += int(x[5]); ta += int(y[5]); tt += int(x[4])
print(f"| **all** | {len(b)} | {mb} -> {ma} of {na} | {100*tb/tt:.1f} -> {100*ta/tt:.1f} | {rb} -> {ra} | {red} |")
def tally(root, pre):
    c = collections.Counter()
    for td in sorted(p for p in Path(root).iterdir() if p.is_dir()):
        for f in td.glob(pre + "*.json"):
            if f.name.startswith("cites"): continue
            for r in json.loads(f.read_text())["rows"]:
                c[r.get("cite") or "UNCITED"] += 1
    return c
cb, ca = tally(sys.argv[3], sys.argv[4]), tally(sys.argv[5], sys.argv[6])
print()
for k in sorted(set(cb) | set(ca)):
    if cb[k] != ca[k]: print(f"  {k:24s} {cb[k]:4d} -> {ca[k]:4d}")
print("  total", sum(cb.values()), "->", sum(ca.values()))
