"""car-v speed (+0x9e4) and own position per AI step, first N rows from the first substate==6 row."""
import csv, sys
path, v, n = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
rows = [r for r in csv.DictReader(open(path, newline='')) if r.get('v') == str(v)]
i6 = next(i for i, r in enumerate(rows) if r['substate'] == '6')
for k, r in enumerate(rows[max(0, i6 - 2): i6 + n]):
    print(k - 2, r['frame'], r['clk_0ff4'], r['substate'], 'spd=%s' % r['rec_9e4'], 'own=(%s,%s)' % (r['own_x'], r['own_z']),
          'c0=%s c1=%s' % (r['c0'], r['c1']))
