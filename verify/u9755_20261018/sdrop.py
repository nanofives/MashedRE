import csv, struct
f = lambda u: struct.unpack('<f', struct.pack('<I', int(u)))[0]
rows = list(csv.DictReader(open('o_sdrop1.msd.spawndrop.csv')))
for r in rows[:12]:
    k = r['kind']
    if k == 'in':
        print('in  n=%s x=%.9g y=%.9g (0x%08x) z=%.9g caller=0x%08x' % (r['n'], f(r['a']), f(r['b']), int(r['b']), f(r['c']), int(r['d'])))
    elif k == 'cb':
        print('cb  n=%s dist=%.9g best_before=%.9g' % (r['n'], f(r['a']), f(r['b'])))
    else:
        b = f(r['a'])
        print('out n=%s best=%.9g (0x%08x) -> drop=best-3=%.9g' % (r['n'], b, int(r['a']), b - 3.0))
