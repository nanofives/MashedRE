"""Pivot a run_h4 arm's per-car step/gates CSVs into one wide per-frame CSV for collateral.py (csv:
channels key on `frame` alone). Read-only on inputs. usage: py -3.12 pivot_arm.py <step.csv> <gates.csv> <out.csv>"""
import csv, sys
rows = {}
for tag, path in (('s', sys.argv[1]), ('g', sys.argv[2])):
    for r in csv.DictReader(open(path, newline='')):
        f = int(r['frame']); v = r.get('v', '?')
        d = rows.setdefault(f, {})
        for k, val in r.items():
            if k in ('frame', 'v'): continue
            d['%s.v%s.%s' % (tag, v, k)] = val
cols = sorted({k for d in rows.values() for k in d})
with open(sys.argv[3], 'w', newline='') as fh:
    w = csv.writer(fh); w.writerow(['frame'] + cols)
    for f in sorted(rows):
        w.writerow([f] + [rows[f].get(c, '') for c in cols])
print('wrote', sys.argv[3], len(rows), 'frames', len(cols), 'fields')
