# s18sum.py <drawlist_diff.json> [--all] -- screen 18/24 widget summary of a drawlist_diff --json output.
# Original-side rows are bucketed by their caller (rets[1]):
#   plate : the record path FUN_0043c5b0 (0x0043c5b0..0x0043d2a0): plate, fade, borders, left border, Button cap
#   arrow : FUN_0043af10 arrow calls, rets 0x43b3e3 / 0x43b206 (right, FUN_004739f0) and 0x43b595 (left, FUN_00473870)
#   other : everything else (background checker 0x42e89c is counted apart, as sumdiff32.py does)
# Standalone-only (extra) rows carry no original caller, so they are bucketed by geometry: a row inside the
# list region x 40..330, y 70..300 is "plate/arrow region", else "other".
# Optional --map <mashed_re.map>: standalone rows whose first return resolves to LogoOverlayDraw (the
# background checker, as in sumdiff32.py) are counted as checker, not region.
import json, sys, re, bisect
d = json.load(open(sys.argv[1]))
syms = []
if '--map' in sys.argv:
    mp = open(sys.argv[sys.argv.index('--map') + 1]).read()
    base = int(re.search(r'Preferred load address is ([0-9a-fA-F]+)', mp).group(1), 16)
    for m in re.finditer(r'^ 0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f ', mp, re.M):
        syms.append((int(m.group(2), 16) - base, m.group(1)))
    syms.sort()
addrs = [a for a, _ in syms]
def sym(off):
    i = bisect.bisect_right(addrs, off) - 1
    return syms[i][1] if i >= 0 else '?'
ARROW = {'0x43b3e3', '0x43b206', '0x43b595'}
def rets(r): return r.get('rets') or []
def bucket_a(a):
    r = rets(a)
    if len(r) < 2: return 'other'
    c = int(r[1], 16)
    if r[1] == '0x42e89c': return 'checker'
    if r[1] in ARROW: return 'arrow'
    if 0x0043c5b0 <= c < 0x0043d2a0: return 'plate'
    return 'other'
def bucket_b(b):
    r = rets(b)
    if syms and r and 'LogoOverlayDraw' in sym(int(r[0], 16)): return 'checker'
    if 40 <= b['x'] <= 330 and 70 <= b['y'] <= 300: return 'region'
    return 'other'
def fmt(r): return f"x={r['x']:.3f} y={r['y']:.3f} w={r['w']:.3f} h={r['h']:.3f} col={'/'.join(f'{c:08x}' for c in r['colors'][:4])} ret={','.join(rets(r)[:2])}"
for k, fr in d.items():
    m = fr['matched'] if isinstance(fr['matched'], int) else len(fr['matched'])
    cnt = {}; lines = []
    for kind in ('mismatched', 'missing', 'extra'):
        for r in fr[kind]:
            a = r.get('a', r) if kind == 'mismatched' else r
            b = r.get('b') if kind == 'mismatched' else None
            bk = bucket_b(a) if kind == 'extra' else bucket_a(a)
            cnt[(kind, bk)] = cnt.get((kind, bk), 0) + 1
            if bk in ('plate', 'arrow', 'region'):
                lines.append(f"  [{bk:6s}] {kind:10s} {r.get('kind', '') if kind == 'mismatched' else ''} {fmt(a)}" + (f"\n                       B: {fmt(b)}" if b else ""))
    wred = sum(v for (kd, bk), v in cnt.items() if bk in ('plate', 'arrow', 'region'))
    print(f"{k}: matched={m} plate/arrow RED={wred} | " + ", ".join(f"{kd}:{bk}={v}" for (kd, bk), v in sorted(cnt.items())))
    if '--all' in sys.argv or k == list(d)[0]:
        for s in lines: print(s)
