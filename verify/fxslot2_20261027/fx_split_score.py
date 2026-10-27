r"""D4-FX-SLOT2: FX emits per 1000 active frames, all frames and alive-only, original (orig_fx_gates_count.py json, last
snapshot) against the port's fx_debug.txt (fx_inputs_score.py fields). Active frames = calls[non-skid] + calls[skid] / 3,
the D4-FX-GATES denominator (verify/fxgates_20261026/fx_score.py); alive = the same on rec+0x4 != 0 calls.
usage: py -3.12 fx_split_score.py <port fx_debug.txt> <orig.json> [<orig.json> ...]"""
import json, re, sys

def rate(e, a): return 1000.0 * e / a if a else 0.0

pl = {}
for ln in open(sys.argv[1], errors='replace'):
    m = re.match(r'\[s(\d)\].*og_frames=(\d+) og_calls=(\d+)/(\d+) og_emits=(\d+)/(\d+) ig=([\d,]+)', ln)
    if m: pl[int(m.group(1))] = m
port = {}
for s, m in pl.items():
    ig = [int(x) for x in m.group(7).split(',')]
    af = int(m.group(3)) + int(m.group(4)) // 3; em = int(m.group(5)) + int(m.group(6))
    aa = ig[4] + ig[14]; ae = ig[13]
    port[s] = (af, em, aa, ae)
print('PORT %s' % sys.argv[1])
for s in sorted(port):
    af, em, aa, ae = port[s]
    print('  slot %d all: active=%d emits=%d per1000=%.1f | alive: active=%d emits=%d per1000=%.1f | dead: active=%d emits=%d per1000=%.1f'
          % (s, af, em, rate(em, af), aa, ae, rate(ae, aa), af - aa, em - ae, rate(em - ae, af - aa)))
for path in sys.argv[2:]:
    r = json.load(open(path))['rows'][-1]
    print('ORIGINAL %s frames=%d' % (path, r['frames']))
    for c in range(4):
        ca, em = r['calls'][2 * c:2 * c + 2], r['emits'][2 * c:2 * c + 2]
        aca, aem = r['acalls'][2 * c:2 * c + 2], r['aemits'][2 * c:2 * c + 2]
        af = ca[0] + ca[1] // 3; aa = aca[0] + aca[1] // 3
        e, ae = sum(em), sum(aem)
        print('  car %d all: active=%d emits=%d per1000=%.1f | alive: active=%d emits=%d per1000=%.1f | dead: active=%d emits=%d per1000=%.1f'
              % (c, af, e, rate(e, af), aa, ae, rate(ae, aa), af - aa, e - ae, rate(e - ae, af - aa)))
