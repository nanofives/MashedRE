r"""D4-FX-STATE7: wheel-colour mix behind FUN_0040c690's roll threshold, port against original.
Original: orig_fx_state7.py <rep>.E.json (last snapshot), per car and colour: calls [non-skid, skid], emits [non-skid, skid].
Port: fx_debug.txt last "[sN col] key:cns/csk:ens/esk ..." line per slot (TrackRenderer::EmitCarFxOrigGates census).
Prints per car and colour: share of calls, emits per call (non-skid / skid), and per car the total emits per call. Also
"port at the original's mix": the port's per-colour emit rates weighted by the original's mean per-colour call counts.
usage: py -3.12 colour_score.py <port fx_debug.txt> <rep>.E.json [...]"""
import json, re, sys

def port_hist(path):
    h = {}
    for ln in open(path, errors='replace'):
        m = re.match(r'\[s(\d) col\](.*)', ln)
        if not m: continue
        d = {}
        for t in m.group(2).split():
            k, c, e = t.split(':')
            cn, cs = (int(x) for x in c.split('/')); en, es = (int(x) for x in e.split('/'))
            d['0x' + k.lower().lstrip('0')] = [cn, cs, en, es]
        h[int(m.group(1))] = d
    return h

def orig_hist(path):
    s = json.load(open(path))['snaps'][-1]['cars']
    return {c: s[c] for c in range(4)}

P = port_hist(sys.argv[1])
O = [(p.replace('\\', '/').split('/')[-1].replace('.E.json', ''), orig_hist(p)) for p in sys.argv[2:]]
def fmt(n):
    tot = n[0] + n[1]
    return 'c %d/%d e %d/%d  r %.3f/%.3f' % (n[0], n[1], n[2], n[3], n[2] / n[0] if n[0] else 0, n[3] / n[1] if n[1] else 0)
for c in range(4):
    keys = sorted(set(P.get(c, {})) | {k for _, o in O for k in o[c]})
    print('car %d' % c)
    for k in keys:
        pn = P.get(c, {}).get(k, [0, 0, 0, 0])
        print('  %-11s port %-40s | %s' % (k, fmt(pn), ' | '.join('%s %s' % (n, fmt(o[c].get(k, [0, 0, 0, 0]))) for n, o in O)))
    def tot(d):
        t = [sum(v[i] for v in d.values()) for i in range(4)]
        return t, (t[2] + t[3]) / (t[0] + t[1]) if t[0] + t[1] else 0
    pt, pr = tot(P.get(c, {}))
    print('  total       port calls %d emits %d per call %.3f | %s' % (pt[0] + pt[1], pt[2] + pt[3], pr,
          ' | '.join('%s calls %d emits %d per call %.3f' % (n, tot(o[c])[0][0] + tot(o[c])[0][1], tot(o[c])[0][2] + tot(o[c])[0][3], tot(o[c])[1]) for n, o in O)))
    # port per-colour rates at the original's mean per-colour call counts (keys the port never saw keep the original's rate)
    exp = calls = 0.0
    for k in keys:
        mc = [sum(o[c].get(k, [0, 0, 0, 0])[i] for _, o in O) / len(O) for i in range(4)]
        pn = P.get(c, {}).get(k)
        for s in (0, 1):
            if not mc[s]: continue
            rate = (pn[2 + s] / pn[s]) if pn and pn[s] else (mc[2 + s] / mc[s])
            exp += mc[s] * rate; calls += mc[s]
    if calls: print('  port rates at the original mean mix: %.1f emits over %.1f calls = %.3f per call' % (exp, calls, exp / calls))
