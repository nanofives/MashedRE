"""PREREG_d3b amendment 3 G-SPLIT reducer: per physics step of the chosen car, the heading change of the LIVE body
matrix (buffer +0x9a8, at-row = +0x928 + idx*0x40 + 0x20) between consecutive lat-bracket samples.
Sites: 0 = A6a entry (car-filtered), 2 = A6b entry (unfiltered, one per car), 1 = substep-loop entry, 3 = solver
0x0046f6c0 entry (car-filtered, --lat-bracket-solver). A step = the samples from one site-0 row to the next.
Each interval is printed as '<from>><to>:<dheading deg>'. Steer flag = +0x1a8 at the step's A6a entry (A4 writes it
before A6a, so it is this step's wheel steer).
usage: py -3.12 lbsplit.py <x.latbracket.csv> [first_step] [last_step]"""
import csv, math, sys

rows = list(csv.DictReader(open(sys.argv[1], newline='')))
a = int(sys.argv[2]) if len(sys.argv) > 2 else 0
b = int(sys.argv[3]) if len(sys.argv) > 3 else 10 ** 9


def head(r, live=True):
    i = int(r['i9a8']) if live else 1 - int(r['i9a8'])
    x, z = (float(r['at0x']), float(r['at0z'])) if i == 0 else (float(r['at1x']), float(r['at1z']))
    return math.degrees(math.atan2(x, z))


def fwdhead(r):
    return math.degrees(math.atan2(float(r['fwdx']), float(r['fwdz'])))


steps, cur = [], None
for r in rows:
    if r['site'] == '0':
        if cur:
            steps.append(cur)
        cur = [r]
    elif cur is not None:
        cur.append(r)
if cur:
    steps.append(cur)
print('steps', len(steps), 'site pattern of step 50:', ''.join(r['site'] for r in steps[min(50, len(steps) - 1)]))
for n, s in enumerate(steps):
    if not (a <= n <= b) or n + 1 >= len(steps):
        continue
    seq = s + [steps[n + 1][0]]
    parts, tot = [], 0.0
    for p, q in zip(seq, seq[1:]):
        d = head(q) - head(p)
        tot += d
        parts.append('%s>%s:%+.4f' % (p['site'], q['site'], d))
    r0 = s[0]
    print('%4d steer1a8 %+.4f spd %7.1f head %9.4f fwd %9.4f a148 %.6f | tot %+.4f | %s' % (
        n, float(r0['s1a8']), float(r0['speed']), head(r0), fwdhead(r0), float(r0['a148']), tot, ' '.join(parts)))
