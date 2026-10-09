"""U-9201 step 2 scorer (PREREG_U9201.md §4). Read-only. usage: py -3.12 score_u9201.py
For r in 1, 2: port arm L2_R<r> (original run r's QPC ticks replayed) vs o_u9201_r<r>.csv.
Frame alignment: original frame f = (clk_0ff4 - clk at its first updated phase-3 sample) / 50;
port frame f = trace row p3f = f + 1 (the first frame's two chain calls are rows 0 and 1).
Q1 first-frame target within 0.05; Q2 every common frame within 0.05 (on failure: the first frame,
and the first +0x964..+0x9a0 field that differs by > 0.05 there); Q3 'released after N',
N = run frames + 1 (r1 690 -> 691, r2 740 -> 741), +-3."""
import csv, math, re
FIELDS = ['s964', 's968', 's96c', 's970', 's974', 's978', 's97c', 's980', 's984', 's988', 's98c', 's990', 'c99c', 'c9a0']
LEN = {1: 690, 2: 740}
ok_all = True
for r in (1, 2):
    o = [x for x in csv.DictReader(open('o_u9201_r%d.csv' % r)) if x['substate'] == '3' and x['cpx'] not in ('0', '0.0')]
    c0 = int(o[0]['clk_0ff4'])
    of = {}
    for x in o: of[(int(x['clk_0ff4']) - c0) // 50] = x
    tr = [x for x in csv.DictReader(open('L2_R%d.trace.csv' % r)) if x['ba8c'] == '3']
    pf = {int(x['p3f']) - 1: x for x in tr if int(x['p3f']) >= 1}
    common = sorted(set(of) & set(pf))
    def d(f):
        a, b = of[f], pf[f]
        return math.dist((float(a['cpx']), float(a['cpy']), float(a['cpz'])), (float(b['tx']), float(b['ty']), float(b['tz'])))
    q1 = d(0); worst = max(common, key=d)
    first_bad = next((f for f in common if d(f) > 0.05), None)
    log = open('L2_R%d.log' % r, encoding='utf-8', errors='replace').read()
    m = re.findall(r"phase 3 released after (\d+) frames", log)
    q3 = len(m) == 1 and abs(int(m[0]) - (LEN[r] + 1)) <= 3
    print('== r%d  common frames %d' % (r, len(common)))
    print('  Q1 first-frame |port - orig| = %.4f  %s' % (q1, 'PASS' if q1 <= 0.05 else 'FAIL'))
    print('  Q2 max over frames = %.4f at f=%d  %s' % (d(worst), worst, 'PASS' if first_bad is None else 'FAIL (first f=%d, d=%.4f)' % (first_bad, d(first_bad))))
    for f in (0, 1, 2, 60, 300, 600):
        if f in of and f in pf:
            print('    f%3d orig (%.4f, %.4f, %.4f) port (%.4f, %.4f, %.4f)' % (f, float(of[f]['cpx']), float(of[f]['cpy']), float(of[f]['cpz']),
                  float(pf[f]['tx']), float(pf[f]['ty']), float(pf[f]['tz'])))
    if first_bad is not None:
        a, b = of[first_bad], pf[first_bad]
        diffs = [(k, float(a[k]), float(b[k])) for k in FIELDS if abs(float(a[k]) - float(b[k])) > 0.05]
        print('    fields > 0.05 at f=%d: %s' % (first_bad, diffs[:6]))
        a0, b0 = of[0], pf[0]
        print('    f0 fields orig/port: ' + ' '.join('%s %.3f/%.3f' % (k, float(a0[k]), float(b0[k])) for k in FIELDS))
        print('    f0 pairs orig %s,%s port %s,%s' % (a0['c994'], a0['c998'], b0['c994'], b0['c998']))
    print('  Q3 released after %s (expected %d +-3)  %s' % (m, LEN[r] + 1, 'PASS' if q3 else 'FAIL'))
    ok_all &= (q1 <= 0.05) and first_bad is None and q3
print('H2', 'HOLDS' if ok_all else 'FAILS (see rows)')
