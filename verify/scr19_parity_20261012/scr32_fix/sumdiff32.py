# Widget-row summary for a drawlist_diff --json output. The standalone's checker rows
# are identified by the map symbol LogoOverlayDraw in their annotated return address
# (not a literal 0xb8202, which moves with every rebuild); the original's by caller 0x42e89c.
# Widget rows = original caller in 0x430000-0x432fff, or a standalone row whose pair is one.
import json, re, sys, bisect
d = json.load(open(sys.argv[1]))
# argv[2]: the standalone's mashed_re.map. Standalone rets are module offsets (VA - load address).
syms = []
mp = open(sys.argv[2]).read()
base = int(re.search(r'Preferred load address is ([0-9a-fA-F]+)', mp).group(1), 16)
for m in re.finditer(r'^ 0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f ', mp, re.M):
    syms.append((int(m.group(2), 16) - base, m.group(1)))
syms.sort(); addrs = [a for a, _ in syms]
def sym(off):
    i = bisect.bisect_right(addrs, off) - 1
    return syms[i][1] if i >= 0 else '?'
def rets(r): return r.get('rets') or []
def fmt(r): return f"x={r['x']:.2f} y={r['y']:.2f} w={r['w']:.2f} h={r['h']:.2f} col={r['colors'][0]:08x} ret={','.join(rets(r)[:2])}"
for k, fr in d.items():
    m = fr['matched'] if isinstance(fr['matched'], int) else len(fr['matched'])
    chk = 0; wid = []; oth = []
    for kind in ('mismatched', 'missing', 'extra'):
        for r in fr[kind]:
            a = r.get('a', r) if kind == 'mismatched' else r
            b = r.get('b') if kind == 'mismatched' else None
            blob = json.dumps(r)
            if (kind != 'extra' and len(rets(a)) > 1 and rets(a)[1] == '0x42e89c') or (kind == 'extra' and rets(a) and 'LogoOverlayDraw' in sym(int(rets(a)[0], 16))):
                chk += 1; continue
            s = f"  {kind:10s} {fmt(a)}" + (f"\n             B: {fmt(b)}" if b else "")
            isw = kind != 'extra' and len(rets(a)) > 1 and rets(a)[1][:5] in ('0x430', '0x431', '0x432')
            (wid if isw else oth).append(s)
    print(f"{k}: matched={m} checker={chk} widget_red={len(wid)} other={len(oth)}")
    if '--all' in sys.argv or k == list(d)[0]:
        for s in wid: print("  [widget]" + s)
        for s in oth: print("  [other] " + s)
