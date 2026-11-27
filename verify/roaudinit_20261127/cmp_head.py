"""Compare this worktree's ddkobj_summary.json with log/orch/run56_headcheck (new DIFFERENT / not-in-map / identical deltas).
usage: py -3.12 cmp_head.py <summary.json>"""
import json, sys
H = r'C:\Users\maria\Desktop\Proyectos\Mashed\log\orch\run56_headcheck\ddkobj_summary.json'
h = json.load(open(H))
n = json.load(open(sys.argv[1]))
for k in ('identical', 'different', 'not_in_map', 'functions', 'units', 'units_linked', 'refusals', 'skips',
          'dual_body_rvas', 'provisional_units_skipped', 'provisional_functions_vs2022_in_linked_units'):
    hv, nv = h.get(k), n.get(k)
    hv = len(hv) if isinstance(hv, (list, dict)) else hv
    nv = len(nv) if isinstance(nv, (list, dict)) else nv
    print('%-48s head %-6s now %-6s %s' % (k, hv, nv, '' if hv == nv else '<-- changed'))
for k in ('different_rvas', 'not_in_map_rvas', 'dual_body_rvas'):
    hs, ns = h.get(k), n.get(k)
    if isinstance(hs, list) and isinstance(ns, list):
        hs = set(map(str, hs)); ns = set(map(str, ns))
        print(k, 'new:', sorted(ns - hs), 'gone:', sorted(hs - ns))
for k in ('refusals', 'skips'):
    print(k, 'head', json.dumps(h.get(k))[:600], 'now', json.dumps(n.get(k))[:600])
pf = 'provisional_functions_vs2022_in_linked_units'
print(pf, 'head', h.get(pf), 'now', n.get(pf))
