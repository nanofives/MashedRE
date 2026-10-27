r"""D4-FX-INPUTS scorer. Original census json (orig_fx_inputs.py) and port fx_debug.txt files (MASHED_FX_DEBUG, ig= field).
usage: py -3.12 fx_inputs_score.py --orig <orig.json> [--port NAME=<fx_debug.txt> ...]
Original rows are cumulative snapshots, the last one is the run total; the per-snapshot delta of act shows when each car
stops passing the outer gates. Port: the last line per slot is cumulative.
Rates: emits per 1000 active frames are in verify/fxgates_20261026 (fx_score.py); here the inputs only."""
import argparse, json, re

ap = argparse.ArgumentParser()
ap.add_argument('--orig'); ap.add_argument('--port', action='append', default=[])
a = ap.parse_args()

def pct(n, d): return '%5.1f%%' % (100.0 * n / d) if d else '   - '

if a.orig:
    rows = json.load(open(a.orig))['rows']; last = rows[-1]
    print('ORIGINAL frames=%d snapshots=%d slots=%s' % (last['frames'], len(rows), last['slots']))
    for c in range(4):
        k = last['cnt'][c * 16:c * 16 + 8]; g = last['g'][c * 8:c * 8 + 6]
        act = k[0]
        print('  car %d outer: slot<0=%d 9e0<=1=%d slow=%d pass=%d cfc=%d frozen_vel=%d' % ((c,) + tuple(g)))
        if 'gd' in last:
            dd = last['gd'][c * 8:c * 8 + 8]
            print('        rec+0x4==0 frames=%d: slot<0=%d 9e0<=1=%d slow=%d pass=%d cfc=%d frozen_vel=%d |v|=0:%d'
                  % (dd[6], dd[0], dd[1], dd[2], dd[3], dd[4], dd[5], dd[7]))
        print('        counted=%d skid=%d (%s) legA b0c>100=%d (%s) legB rng&brk=%d (%s) brk>40=%d (%s) rng=%d (%s) cfc=%d frozen=%d'
              % (act, k[1], pct(k[1], act), k[2], pct(k[2], act), k[3], pct(k[3], act), k[4], pct(k[4], act), k[5], pct(k[5], act), k[6], k[7]))
        print('        b0c bins(25)=%s brake classes [0,1..40,41..fe,ff]=%s' % (last['hb'][c * 16:c * 16 + 9], last['hk'][c * 4:c * 4 + 4]))
        prev = 0; ser = []
        for r in rows:
            ser.append(r['cnt'][c * 16] - prev); prev = r['cnt'][c * 16]
        print('        counted per 10 s snapshot: %s' % ser)

for spec in a.port:
    name, _, path = spec.partition('=')
    lastl = {}
    for ln in open(path):
        m = re.match(r'\[s(\d)\].*og_frames=(\d+) og_calls=(\d+)/(\d+) og_emits=(\d+)/(\d+) ig=([\d,]+)', ln)
        if m: lastl[int(m.group(1))] = m
    print('PORT %s (%s)' % (name, path))
    for s in sorted(lastl):
        m = lastl[s]; ig = [int(x) for x in m.group(7).split(',')]
        act = ig[3]
        skid_fr = int(m.group(4)) // 3
        print('  slot %d frames=%s outer: slot<0=%d 9e0<=1=%d slow=%d pass=%d cfc=%d | alive-active=%d' % (s, m.group(2), ig[0], ig[1], ig[2], ig[3], ig[11], ig[4]))
        print('        skid frames=%d (%s) skid&alive=%d legA b0c>100=%d (%s) legB rng&brk=%d (%s) brk>40=%d (%s) rng=%d (%s) frozen=%d (%s)'
              % (skid_fr, pct(skid_fr, act), ig[5], ig[6], pct(ig[6], act), ig[7], pct(ig[7], act), ig[8], pct(ig[8], act), ig[9], pct(ig[9], act), ig[10], pct(ig[10], act)))
        af = int(m.group(3)) + skid_fr   # the D4-FX-GATES denominator: calls[non-skid] + calls[skid]/3
        print('        emits=%s/%s active_fr=%d per1000active=%.1f' % (m.group(5), m.group(6), af, 1000.0 * (int(m.group(5)) + int(m.group(6))) / af if af else 0))
        if len(ig) >= 15:
            aa = ig[4] + ig[14]   # alive active frames: +0xcfc == 0 passed, plus +0xcfc != 0 past the roll
            print('        ALIVE: active_fr=%d skid_fr=%d (%s) calls=%d emits=%d per1000active=%.1f'
                  % (aa, ig[5] + ig[14], pct(ig[5] + ig[14], aa), ig[12], ig[13], 1000.0 * ig[13] / aa if aa else 0))
