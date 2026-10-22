import re, sys, collections
# summary of phys_c4_*_selftest.log lines: counts per (air, state, xfok, noo!=0, nov!=0, nvs!=0, npad!=0, ndiff!=0)
for fn in sys.argv[1:]:
    c = collections.Counter(); n = 0
    for ln in open(fn):
        if not ln.startswith('['): continue
        n += 1
        g = lambda k: (re.search(r'\b' + k + r'=(\d+)', ln) or [None, '-'])[1]
        key = tuple((k, g(k) if k in ('air', 'state', 'xfok') else (g(k) if g(k) == '-' else str(g(k) != '0')))
                    for k in ('air', 'state', 'xfok', 'noo', 'nov', 'nvs', 'npad', 'ndiff'))
        c[key] += 1
    print(fn, 'calls', n)
    for k, v in sorted(c.items()): print('  ' + ' '.join('%s=%s' % kv for kv in k), v)
