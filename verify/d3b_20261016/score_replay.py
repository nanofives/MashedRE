"""G-REPLAY-LOOK scorer (d3b): per file, per car, the 220-call window from the first c4 != 0, rows with ai_mode 0 and
an original look point that is not (0, 0) (probe-zero rows = the original left ControlStep before writing it).
usage: py -3.12 score_replay.py <LR_*.look.csv> [...]"""
import csv, math, sys
for p in sys.argv[1:]:
    rows = [r for r in csv.DictReader(open(p, newline='')) if not r['file'].startswith('#')]
    print(p)
    tot = [0, 0, 0, 0, 0]
    for f in ('o_reg_r1.', 'o_reg_r13.', 'o_reg_r19.'):
        out = []
        for v in '123':
            R = [r for r in rows if f in r['file'] and r['v'] == v]
            i0 = next(i for i, r in enumerate(R) if int(r['c4']) != 0)
            W = [r for r in R[i0:i0 + 220] if r['ai_mode'] == '0' and not (float(r['orig_x']) == 0 and float(r['orig_z']) == 0)]
            d = lambda r, m: math.hypot(float(r[m + '_x']) - float(r['orig_x']), float(r[m + '_z']) - float(r['orig_z']))
            s = (len(W), sum(int(r['eq_lut']) for r in W), sum(d(r, 'lut') < 1e-5 for r in W),
                 sum(int(r['eq_ex']) for r in W), sum(d(r, 'ex') < 1e-5 for r in W))
            tot = [a + b for a, b in zip(tot, s)]
            out.append('car%s %d: mode1 bit %d near %d | exact bit %d near %d' % ((v,) + s))
        print('  ' + f + ' ' + '; '.join(out))
    print('  TOTAL n=%d mode1 bit %d (%.1f %%) near %d | exact bit %d near %d' % (tot[0], tot[1], 100.0 * tot[1] / tot[0], tot[2], tot[3], tot[4]))
