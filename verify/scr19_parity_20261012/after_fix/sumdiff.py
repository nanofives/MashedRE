import json, sys, collections, bisect, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're', 'tools'))
# --map <mashed_re.map>: key the LogoOverlayDraw checker on its map symbol instead of the
# fixed return address 0xb8202 (the address moves with every rebuild). Without --map the
# address fallback below applies.
SYM = None
if '--map' in sys.argv:
    from drawlist_diff import load_msvc_map
    SYM = load_msvc_map(sys.argv[sys.argv.index('--map') + 1])
def sym_of(ret):
    if not SYM: return None
    addrs, names = SYM
    i = bisect.bisect_right(addrs, int(ret, 16)) - 1
    return names[i] if i >= 0 else None
def is_logo_overlay(r):
    ret = (r.get('rets') or [''])[0]
    if SYM and ret: return sym_of(ret) == 'LogoOverlayDraw'
    return ret == '0xb8202'
d = json.load(open(sys.argv[1]))
CHECK = '0x42e89c'
def caller(r):
    rets = r.get('rets') or []
    return rets[1] if len(rets) > 1 else '?'
def fmt(r):
    return f"x={r['x']:.2f} y={r['y']:.2f} w={r['w']:.2f} h={r['h']:.2f} col={r['colors'][0]:08x} ret={','.join((r.get('rets') or [])[:2])}"
for k, fr in d.items():
    m = fr['matched'] if isinstance(fr['matched'], int) else len(fr['matched'])
    chk = 0; rows = []
    for kind in ('mismatched', 'missing', 'extra'):
        for r in fr[kind]:
            a = r.get('a', r) if kind == 'mismatched' else r
            b = r.get('b') if kind == 'mismatched' else None
            if kind != 'extra' and caller(a) == CHECK: chk += 1; continue
            if kind == 'extra' and is_logo_overlay(r): chk += 1; continue
            s = f"  {kind:10s} A: {fmt(a)}"
            if b: s += f"\n             B: {fmt(b)}"
            rows.append(s)
    print(f"{k}: matched={m} checker(0x474464/0x42e89c)={chk} other={len(rows)}")
    if '--all' in sys.argv or k == list(d)[0]:
        print("\n".join(rows))
