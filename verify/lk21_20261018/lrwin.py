"""Lookahead replay (run_arms LR_ arms: the original's aistep rows through the port's SplineLookahead) by (b)-window call.
Per file and car: window calls (first c4 != 0) where the replayed target misses the original's, mode-0 rows only
(the replay computes the mode-0 lookahead on every row). With --car V --k0 A --k1 B prints those rows in full.
usage: lrwin.py <look.csv> [--col eq_lut|eq_ex] [--car V --k0 A --k1 B --file SUBSTR]"""
import csv, sys, collections, os


def arg(n, d):
    a = sys.argv
    return a[a.index(n) + 1] if n in a else d


rows = collections.defaultdict(list)
for r in csv.DictReader(open(sys.argv[1], newline='')):
    if not (r.get('v') or '').strip().isdigit():
        continue   # the replay's trailing summary lines
    rows[(os.path.basename(r['file']).split('.')[0], int(r['v']))].append(r)
col = arg('--col', 'eq_lut')
car = int(arg('--car', 0)); k0, k1 = int(arg('--k0', 0)), int(arg('--k1', 220)); fsub = arg('--file', '')
for (f, v), rs in sorted(rows.items()):
    i0 = next((i for i, r in enumerate(rs) if int(r['c4']) != 0), None)
    if i0 is None:
        continue
    w = rs[i0:i0 + 220]
    if car:
        if v != car or fsub not in f:
            continue
        print('%s car %d' % (f, v))
        for k in range(k0, min(k1, len(w))):
            r = w[k]
            print('  %3d mode=%s orig=(%.4f,%.4f) ex=(%.4f,%.4f) lut=(%.4f,%.4f) eq_ex=%s eq_lut=%s march o/ex/lut=%s/%s/%s' % (
                k, r['ai_mode'], float(r['orig_x']), float(r['orig_z']), float(r['ex_x']), float(r['ex_z']),
                float(r['lut_x']), float(r['lut_z']), r['eq_ex'], r['eq_lut'], r['march_orig'], r['march_ex'], r['march_lut']))
        continue
    miss = [k for k, r in enumerate(w) if r['ai_mode'] == '0' and r[col] == '0']
    print('%-10s car %d: %s misses %d %s' % (f, v, col, len(miss), miss[:20]))
