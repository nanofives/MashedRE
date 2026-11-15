# Containing hooks.csv function (greatest RVA <= site) for each site.  Usage: py -3.12 owner.py hooks.csv 0x... ...
import csv, sys
rows = []
with open(sys.argv[1], newline='', encoding='utf-8', errors='replace') as f:
    for r in csv.DictReader(f):
        try: rows.append((int(r['rva'], 16), r['name'], r['confidence']))
        except ValueError: pass
rows.sort()
for s in sys.argv[2:]:
    t = int(s, 16); best = None
    for r in rows:
        if r[0] <= t: best = r
        else: break
    print(f'{t:#010x} -> {best[0]:#010x} {best[1]} {best[2]}')
