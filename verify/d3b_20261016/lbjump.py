"""d3b G-SPLIT: the heading jump of the live body matrix between the last sample of one step and the next step's
A6a entry (outside both substeps), against the new step's wheel steer +0x1a8 (A4 writes it before A6a) and the
previous step's. usage: py -3.12 lbjump.py <x.latbracket.csv>"""
import csv, math, statistics, sys
rows = list(csv.DictReader(open(sys.argv[1], newline='')))
def head(r):
    i = int(r['i9a8'])
    if r['site'] == '3':      # solver entry: FUN_0046e9e0 has just written the OTHER buffer
        i = 1 - i
    x, z = (r['at0x'], r['at0z']) if i == 0 else (r['at1x'], r['at1z'])
    return math.degrees(math.atan2(float(x), float(z)))
steps, cur = [], None
for r in rows:
    if r['site'] == '0':
        if cur: steps.append(cur)
        cur = [r]
    elif cur is not None:
        cur.append(r)
groups = {}
for a, b in zip(steps, steps[1:]):
    if float(a[0]['speed']) < 100:          # parked / countdown steps carry no information
        continue
    j = head(b[0]) - head(a[-1])
    new0 = float(b[0]['s1a8']) == 0.0
    old0 = float(a[0]['s1a8']) == 0.0
    groups.setdefault((old0, new0), []).append(j)
for k in sorted(groups):
    v = groups[k]
    big = sum(abs(x) > 0.01 for x in v)
    print('prev steer0=%-5s new steer0=%-5s n=%4d  |jump|>0.01 deg on %4d  median %+.4f  min %+.4f  max %+.4f' % (
        k[0], k[1], len(v), big, statistics.median(v), min(v), max(v)))

if len(sys.argv) > 2:
    for n, (a, b) in enumerate(zip(steps, steps[1:])):
        if float(a[0]['speed']) < 100: continue
        j = head(b[0]) - head(a[-1])
        if abs(j) > 0.01:
            print('step %4d seq %s jump %+.4f  steer prev %.4f new %.4f  spd %.1f  a148 %.6f' % (
                n, b[0]['seq'], j, float(a[0]['s1a8']), float(b[0]['s1a8']), float(b[0]['speed']), float(b[0]['a148'])))
