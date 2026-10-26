r"""D4-FX-GATES scorer (PREREG_fxgates.md section 4, P2..P5).
usage: py -3.12 fx_score.py <orig.json> <F0.fx_debug.txt> <F1.fx_debug.txt> [<F2.fx_debug.txt>]
Port: the last per-slot line of each fx_debug file (cumulative counters). Original: the last sample of the capture.
Rates are per 1000 frames: original frames = FUN_0040ca00 entries; port frames = og_frames (knob ON) or
12 x the slot's line count (knob OFF, one line per 12 EmitCarFx calls)."""
import json, re, sys

def port(path):
    last, n = {}, {}
    for ln in open(path, errors='replace'):
        m = re.match(r'\[s(\d)\]', ln)
        if not m: continue
        s = int(m.group(1)); n[s] = n.get(s, 0) + 1
        kv = dict(re.findall(r'(\w+)=([-\d./]+)', ln)); last[s] = kv
    out = {}
    for s, kv in sorted(last.items()):
        calls = [int(x) for x in kv.get('og_calls', '0/0').split('/')]
        emits = [int(x) for x in kv.get('og_emits', '0/0').split('/')]
        fr = int(kv.get('og_frames', '0')) or 12 * n[s]
        out[s] = dict(frames=fr, skids=int(kv['skids']), calls=calls, emits=emits)
    return out

o = json.load(open(sys.argv[1]))['rows'][-1]
orig = {c: dict(frames=o['frames'], calls=o['calls'][2 * c:2 * c + 2], emits=o['emits'][2 * c:2 * c + 2], cols=o['cols'][c],
                ids={i: n for i, n in enumerate(o['ids'][32 * c:32 * c + 32]) if n}) for c in range(4)}
print('ORIGINAL frames=%d phase=%d [0x00636adc]=%d course=%d' % (o['frames'], o['phase'], o['w636adc'], o['course']))
for c in range(4):
    r = orig[c]; ca = sum(r['calls']); em = sum(r['emits'])
    act = r['calls'][0] + r['calls'][1] / 3.0
    print('  car %d calls=%s emits=%s emit/call=%.3f emits/1000fr=%.1f active_fr=%.0f emits/1000active=%.1f cols=%s ids=%s' % (
        c, r['calls'], r['emits'], em / ca if ca else 0, 1000.0 * em / r['frames'] if r['frames'] else 0, act,
        1000.0 * em / act if act else 0, r['cols'], r['ids']))
pooled = {}
for c in range(4):
    for k, v in orig[c]['cols'].items(): pooled[k] = pooled.get(k, 0) + v
print('  pooled colours:', dict(sorted(pooled.items(), key=lambda kv: -kv[1])))
# active frames: one FUN_0040c690 call per gated frame, three when skid (0x0040cbd5), so calls[0] + calls[1] / 3.
# Knob OFF has no gate counters: it borrows F1's active frames (same simulation, the G-CTRL hashes are identical).
ACT = {}
arms = [('F0', sys.argv[2]), ('F1', sys.argv[3])] + ([('F2', sys.argv[4])] if len(sys.argv) > 4 else [])
for name, path in arms:
    P = port(path)
    print(name)
    if name == 'F1':
        for s_, r_ in P.items(): ACT[s_] = r_['calls'][0] + r_['calls'][1] / 3.0
    for s, r in P.items():
        ca = sum(r['calls']); em = sum(r['emits'])
        rate = 1000.0 * (em if name != 'F0' else r['skids']) / r['frames']
        orate = 1000.0 * sum(orig[s]['emits']) / orig[s]['frames'] if orig[s]['frames'] else 0
        print('  slot %d frames=%d skids=%d calls=%s emits=%s emit/call=%.3f per1000=%.1f orig_per1000=%.1f ratio=%s' % (
            s, r['frames'], r['skids'], r['calls'], r['emits'], em / ca if ca else 0, rate, orate,
            ('%.2f' % (rate / orate)) if orate else 'inf'))
        oa = orig[s]['calls'][0] + orig[s]['calls'][1] / 3.0
        pa = (r['calls'][0] + r['calls'][1] / 3.0) if name != 'F0' else None
        if name == 'F0' or pa is None: pa = None
        a = pa if pa else None
        print('         per1000active=%s orig_per1000active=%.1f' % (
            ('%.1f' % (1000.0 * (em if name != 'F0' else r['skids']) / (a if a else 1))) if a else 'see F0 below', 1000.0 * sum(orig[s]['emits']) / oa if oa else 0))

if 'F1' in [a for a, _ in arms]:
    P0 = port(sys.argv[2])
    print('F0 per 1000 active frames (F1 denominators):')
    for s_, r_ in P0.items():
        a = ACT.get(s_, 0)
        oa = orig[s_]['calls'][0] + orig[s_]['calls'][1] / 3.0
        print('  slot %d skids=%d active=%.0f per1000active=%.1f orig=%.1f' % (s_, r_['skids'], a, 1000.0 * r_['skids'] / a if a else 0,
              1000.0 * sum(orig[s_]['emits']) / oa if oa else 0))
