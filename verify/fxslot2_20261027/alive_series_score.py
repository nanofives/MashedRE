r"""D4-FX-SLOT2 scorer for orig_alive_series.py output.
usage: py -3.12 alive_series_score.py <out_prefix>
Prints: run-length transitions of ba8c / phase / alive mask / +0x10 mask / slot words; per car the frame and AI call
index of every alive edge; the event list (FUN_00422fd0 / FUN_0040eee0 / FUN_0040d590 entries); ctrl byte 0xa
non-zero frames per car; per round segment (between ba8c edges into 3) the count of frames with car alive and with
ctrl byte 5 > 0x40."""
import json, struct, sys

P = sys.argv[1]
raw = open(P + '.rows.bin', 'rb').read()
N = len(raw) // 24
rows = [raw[i * 24:(i + 1) * 24] for i in range(N)]
ev = json.load(open(P + '.events.json'))
def s8(b): return b - 256 if b > 127 else b
def aic(r, c): return r[16 + 2 * c] | (r[17 + 2 * c] << 8)

print('frames', N, 'events', len(ev))
print('-- transitions (frame: ba8c phase alive +0x10 slots aic)')
prev = None
for f, r in enumerate(rows):
    key = (r[0], r[1], r[2], r[3], tuple(s8(x) for x in r[4:8]))
    if key != prev:
        print('  f=%5d ba8c=%d ph=%d alive=%s m10=%s slots=%s aic=%s' % (f, r[0], r[1], format(r[2], '04b')[::-1],
              format(r[3], '04b')[::-1], list(key[4]), [aic(r, c) for c in range(4)]))
        prev = key
print('-- events')
for e in ev:
    f = e['frame']; r = rows[min(f, N - 1)] if N else None
    a = [x if x < 0x80000000 else x for x in e['args']]
    print('  f=%5d %s args=%s aic@f=%s' % (f, e['fn'], a, [aic(r, c) for c in range(4)] if r else None))
print('-- per car alive spans (frame range, AI calls at start/end), ctrl5>0x40 frames, ctrl0xa!=0 frames')
for c in range(4):
    spans = []; st = None
    for f, r in enumerate(rows):
        al = (r[2] >> c) & 1
        if al and st is None: st = f
        if not al and st is not None: spans.append((st, f - 1)); st = None
    if st is not None: spans.append((st, N - 1))
    out = []
    for a, b in spans:
        brk = sum(1 for f in range(a, b + 1) if rows[f][8 + c] > 0x40)
        out.append('[%d..%d n=%d aic %d..%d brk>40=%d]' % (a, b, b - a + 1, aic(rows[a], c), aic(rows[b], c), brk))
    ca = sum(1 for r in rows if r[12 + c] != 0)
    print('  car %d spans=%d %s ctrl0xa!=0 frames=%d' % (c, len(spans), ' '.join(out), ca))
