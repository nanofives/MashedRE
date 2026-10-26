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
    print('  car %d calls=%s emits=%s emit/call=%.3f emits/1000fr=%.1f cols=%s ids=%s' % (
        c, r['calls'], r['emits'], em / ca if ca else 0, 1000.0 * em / r['frames'] if r['frames'] else 0, r['cols'], r['ids']))
pooled = {}
for c in range(4):
    for k, v in orig[c]['cols'].items(): pooled[k] = pooled.get(k, 0) + v
print('  pooled colours:', dict(sorted(pooled.items(), key=lambda kv: -kv[1])))
arms = [('F0', sys.argv[2]), ('F1', sys.argv[3])] + ([('F2', sys.argv[4])] if len(sys.argv) > 4 else [])
for name, path in arms:
    P = port(path)
    print(name)
    for s, r in P.items():
        ca = sum(r['calls']); em = sum(r['emits'])
        rate = 1000.0 * (em if name != 'F0' else r['skids']) / r['frames']
        orate = 1000.0 * sum(orig[s]['emits']) / orig[s]['frames'] if orig[s]['frames'] else 0
        print('  slot %d frames=%d skids=%d calls=%s emits=%s emit/call=%.3f per1000=%.1f orig_per1000=%.1f ratio=%s' % (
            s, r['frames'], r['skids'], r['calls'], r['emits'], em / ca if ca else 0, rate, orate,
            ('%.2f' % (rate / orate)) if orate else 'inf'))
