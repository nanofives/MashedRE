import json, sys, collections
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
            if kind == 'extra' and 'LogoOverlayDraw' in json.dumps(r): chk += 1; continue
            if kind == 'extra' and (r.get('rets') or [''])[0] == '0xb8202': chk += 1; continue
            s = f"  {kind:10s} A: {fmt(a)}"
            if b: s += f"\n             B: {fmt(b)}"
            rows.append(s)
    print(f"{k}: matched={m} checker(0x474464/0x42e89c)={chk} other={len(rows)}")
    if '--all' in sys.argv or k == list(d)[0]:
        print("\n".join(rows))
