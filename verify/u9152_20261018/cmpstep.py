"""u9152: first differing step-dump row between two arms, ignoring the +0x968 half columns (rec_968..rec_9a4).
usage: cmpstep.py <a.step.csv> <b.step.csv>"""
import csv, sys
a = list(csv.reader(open(sys.argv[1]))); b = list(csv.reader(open(sys.argv[2])))
h = a[0]
skip = {j for j, n in enumerate(h) if n.startswith('rec_') and 0x968 <= int(n[4:], 16) <= 0x9a4} if all(
    c in '0123456789abcdef' for n in h if n.startswith('rec_') for c in n[4:]) else set()
print('rows', len(a), len(b), 'skipped cols', sorted(h[j] for j in skip))
for i, (x, y) in enumerate(zip(a, b)):
    d = [(h[j], x[j], y[j]) for j in range(min(len(x), len(y))) if j not in skip and x[j] != y[j]]
    if d:
        r = dict(zip(h, x)); print('first diff row', i, 'frame', r.get('frame'), 'v', r.get('v'), d[:10]); break
else:
    print('no difference outside the half (over the common length)')
