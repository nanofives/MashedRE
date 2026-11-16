# RWC-FRAME-LADDER-CHECK: original-side write watch of 0x0067eca4 / 0x0067eca8 (detorig.py boot mode, tick clock) against
# the core's per-call ladder log (PortTop_RwcPhaseLadder_0043dfd0, "ladder step call" lines).
# Usage: py -3.12 ladder_compare.py <orig_dir> [<orig_dir> ...] -- <core.log> [<core.log> ...]
# Original ladder call index = the tick of the 0x0043dfd0 eca8 write (one write site per call: 0x0043e0c6 / 0x0043e0d9 in
# phase 1, 0x0043e112 / 0x0043e120 in phase 2); the value kept is the last write of that tick. pc in watch.jsonl is the
# address after the writing instruction (x86 data breakpoints trap after the write).
import json, re, struct, sys

# pc after the write -> writing instruction (verify/rwcframeclump_20261113/MEMREFS_67eca4_r49.txt)
SITES = {0x433253: 0x00433249, 0x43e0cc: 0x0043e0c6, 0x43e0e3: 0x0043e0d9, 0x43e0e9: 0x0043e0e3,
         0x43e118: 0x0043e112, 0x43e126: 0x0043e120, 0x43e130: 0x0043e126,
         0x432b0f: 0x00432b0a, 0x432b25: 0x00432b1f}
B = 0x005ea000


def orig(d, warp=300):
    hits = [json.loads(l) for l in open(d + '/watch.jsonl')]
    pre = [h for h in hits if h['tick'] < warp]
    per = {}
    for h in pre:
        if int(h['pc'], 16) in (0x43e0cc, 0x43e0e3, 0x43e0e9, 0x43e118, 0x43e126, 0x43e130):
            per[h['tick']] = (int(h['values'][0], 16), int(h['values'][1], 16) & 0xffffffff)
    trans = []
    for h in pre:
        if int(h['pc'], 16) in (0x433253, 0x43e0e9, 0x43e130):
            trans.append((h['tick'], int(h['values'][0], 16), SITES[int(h['pc'], 16)]))
    return hits, pre, per, trans


def core(f):
    per = {}
    trans = []
    for l in open(f, errors='replace'):
        m = re.search(r'ladder step call (\d+) presents (\d+): DAT_0067eca4 (\d+) -> (\d+), DAT_0067eca8 = (-?\d+), '
                      r'DAT_007f1004 = [\d.]+ \(([0-9A-F]+)\), DAT_0067ecac = (\d+), DAT_0067e9f8 = (-?\d+), '
                      r'DAT_0067ed78 = ?([0-9A-F ]+), DAT_00898ac0 = ([0-9A-F ]+)', l)
        if not m:
            continue
        c, p, a, b, fade = (int(m.group(i)) for i in range(1, 6))
        per[c] = (b, fade & 0xffffffff, p, m.group(6), int(m.group(7)), int(m.group(8)), m.group(9).strip(), m.group(10).strip())
        if a != b:
            trans.append((c, p, a, b))
    return per, trans


def main():
    i = sys.argv.index('--')
    odirs, clogs = sys.argv[1:i], sys.argv[i + 1:]
    O = [orig(d) for d in odirs]
    C = [core(f) for f in clogs]
    seqs = [[(h['tick'], h['pc'], tuple(h['values'])) for h in o[0]] for o in O]
    print('original runs: ' + ', '.join('%s %d hits (%d before the warp tick 300)' % (d, len(o[0]), len(o[1])) for d, o in zip(odirs, O)))
    print('original runs identical (tick, pc, values) across all hits: %s' % all(s == seqs[0] for s in seqs))
    print('core logs identical (call, phase, fade, present): %s' % all(
        {k: v[:3] for k, v in c[0].items()} == {k: v[:3] for k, v in C[0][0].items()} for c in C))
    o = O[0]
    print('\noriginal transitions of 0x0067eca4 (before the warp): tick, new value, writer')
    for t in o[3]:
        print('  tick %d -> %d at 0x%08x (ladder call %d)' % (t[0], t[1], t[2], t[0] + 1))
    print('core transitions: ' + '; '.join('call %d (present %d) %d -> %d' % t for t in C[0][1]))
    print('\nper-call compare (original tick t = ladder call t+1): call, orig (phase, fade), core (phase, fade), match')
    bad = 0
    calls = sorted(set(k + 1 for k in o[2]) | set(C[0][0]))
    for c in calls:
        ov = o[2].get(c - 1)
        cv = C[0][0].get(c)
        ok = ov is not None and cv is not None and ov == cv[:2]
        bad += not ok
        print('  %3d  %-14s %-14s %s' % (c, ov, cv[:2] if cv else None, 'match' if ok else 'MISMATCH'))
    print('per-call mismatches: %d of %d' % (bad, len(calls)))
    # screen build of 0x0043d2a0(DAT_0067ecac, 0): original checkpoints tick 16 (ckpt204) and 17 (ckpt205)
    for d in odirs:
        a = open(d + '/ckpt204.data.bin', 'rb').read()
        b = open(d + '/ckpt205.data.bin', 'rb').read()
        rd = lambda x, va, n: ' '.join('%08x' % struct.unpack_from('<I', x, va - B + 4 * k)[0] for k in range(n))
        print('\n%s 0x0043d2a0 screen build, checkpoint tick 16 -> tick 17:' % d)
        for va, n in ((0x0067ecac, 1), (0x0067e9f8, 1), (0x0067ed78, 4), (0x00898ac0, 4), (0x007f1004, 1)):
            print('  0x%08x  %s  ->  %s' % (va, rd(a, va, n), rd(b, va, n)))
    cv = C[0][0][max(C[0][0])]
    print('\ncore at ladder call %d: DAT_007f1004 %s, DAT_0067ecac %d, DAT_0067e9f8 %d, DAT_0067ed78 %s, DAT_00898ac0 %s'
          % ((max(C[0][0]),) + cv[3:]))


main()
