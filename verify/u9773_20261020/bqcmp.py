"""u9773: classifier-entry (kind Q) rows, original --batch-probe vs port MASHED_BATCH_DUMP, for one car. Aligns on the
offset that gives the longest run of equal (query, count, entries, wheel points) rows, then prints the first rows that differ
and which fields. usage: bqcmp.py <orig.batchprobe.csv> <port.batch.csv> [n_show]"""
import csv, sys
K = ('cx', 'cy', 'cz', 'r', 'count', 'entries', 'wpos')
O = [r for r in csv.DictReader(open(sys.argv[1])) if r['kind'] == 'Q']
P = [r for r in csv.DictReader(open(sys.argv[2])) if r['kind'] == 'Q']
key = lambda r: tuple(r[k] for k in K)
idx = {}
for i, r in enumerate(O): idx.setdefault(key(r), []).append(i)
best = (0, 0, 0)
for j in range(0, min(len(P), 400), 7):
    for i in idx.get(key(P[j]), [])[:50]:
        n = 0
        while i + n < len(O) and j + n < len(P) and key(O[i + n]) == key(P[j + n]): n += 1
        if n > best[0]: best = (n, i - j, j)
n, off, j0 = best
print('best run %d rows, orig row = port row %+d (from port row %d)' % (n, off, j0))
shown = 0
for j in range(j0, len(P)):
    i = j + off
    if i >= len(O): break
    d = [k for k in K if O[i][k] != P[j][k]]
    if d:
        print('port Q row %d / orig %d: differs in %s' % (j, i, d))
        for k in d:
            if k == 'wpos':
                a, b = O[i][k].split(';'), P[j][k].split(';')
                import struct
                fl = lambda h: struct.unpack('<f', bytes.fromhex(h)[::-1])[0]
                print('   wpos orig', ' '.join('%.7g' % fl(h) for h in a))
                print('   wpos port', ' '.join('%.7g' % fl(h) for h in b))
            elif k == 'entries':
                print('   entries orig n=%d port n=%d' % (len(O[i][k].split('|')), len(P[j][k].split('|'))))
            else: print('   %s %s / %s' % (k, O[i][k], P[j][k]))
        shown += 1
        if shown >= int(sys.argv[3] if len(sys.argv) > 3 else 3): break
