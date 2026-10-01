#!/usr/bin/env python3
"""a12_mult.py - D2 attempt 12 step 3, side leg. Two things:

(1) the ESP-delta walk over A6a that resolves WHICH frame slot each `[esp+N]`
    reference means, and
(2) the running-original value of the RwV3dLength result latched at 0x0046820f,
    from scenario_launch --mag-probe (which records the RETURN site).

This exists because the candidate "the clamp's multiplicand at 0x004687db is the
length computed at 0x0046820a" was raised from an ESP-NAIVE grep and is REFUTED
by (1): with the frame address formed correctly as `esp_delta + displacement`,
0x0046820f writes frame -212 while the clamp reads frame -208. Both numbers are
kept so the refutation is reproducible rather than asserted.

SIGN NOTE, and it cost one wrong conclusion. The frame address of `[esp+N]` is
`esp_delta + N`, NOT `N - esp_delta`. The first version of this file used the
latter and so reported 0x004686a9 (the post-W1 RwV3dLength result) as a
DIFFERENT slot from the clamp's read at 0x004687db. They are the same slot: at
0x004686a9 the address is ESP+0x24, and the `add esp,4` at 0x004686c4 re-labels
it ESP+0x20. Ghidra agrees (`fVar5`, one assignment, pcaddr=004686a9).

Usage:
  a12_mult.py --dis <a6a.txt from re/tools/disasm_va.py> [--mag <...magprobe.csv>]
"""
import collections, csv, math, re, statistics, sys


def esp_walk(path):
    d = 0
    rows = []
    for ln in open(path):
        m = re.match(r'(0x[0-9a-f]+)\s+((?:[0-9a-f]{2} )+)\s*(.*)', ln)
        if not m:
            continue
        a = int(m.group(1), 16)
        ins = m.group(3).strip()
        rows.append((a, d, ins))
        if ins.startswith('push'):
            d -= 4
        elif ins.startswith('pop '):
            d += 4
        elif ins.startswith('sub esp,'):
            d -= int(ins.split(',')[1].strip(), 0)
        elif ins.startswith('add esp,'):
            d += int(ins.split(',')[1].strip(), 0)
    return rows


def main(argv):
    a = {}
    i = 0
    while i < len(argv):
        a[argv[i][2:]] = argv[i + 1]
        i += 2

    rows = esp_walk(a['dis'])
    REF = re.compile(r'\[esp(?: \+ (0x[0-9a-f]+))?\]')
    for target, name in ((-208, "the clamp's slot, read at 0x004687db"),
                         (-212, 'what 0x0046820f actually writes')):
        print('=== every A6a access resolving to frame %d (%s) ===' % (target, name))
        for addr, d, ins in rows:
            for mm in REF.finditer(ins):
                off = int(mm.group(1), 16) if mm.group(1) else 0
                if d + off == target:
                    print('  0x%08x  esp_delta %5d  %s' % (addr, d, ins))
        print('')

    if 'mag' not in a:
        return
    mr = list(csv.DictReader(open(a['mag'], newline='')))
    print('\n=== running original: RwV3dLength results by RETURN site ===')
    print('  ', collections.Counter(r['site'] for r in mr).most_common(10))

    def mg(r):
        return math.sqrt(float(r['vx']) ** 2 + float(r['vy']) ** 2 + float(r['vz']) ** 2)

    frames, cur = [], None
    for r in mr:
        if r['site'] == '00467673':
            if cur is not None:
                frames.append(cur)
            cur = dict(sp=float(r['rec_speed']), gnd=float(r['gnd']), vals=[])
        elif cur is not None and r['site'] == '0046820f':
            cur['vals'].append(mg(r))
    if cur:
        frames.append(cur)
    print('  frames %d   calls/frame %s'
          % (len(frames), sorted(collections.Counter(len(f['vals']) for f in frames).items())))
    print('  band          n | med LAST |v| @0046820f | med +0x9e4 | ratio')
    for lo, hi in ((40, 70), (70, 100), (100, 150), (150, 260), (260, 500),
                   (500, 1000), (1000, 1500), (1500, 2000)):
        b = [f for f in frames if lo <= f['sp'] < hi and abs(f['gnd'] - 4.0) < 1e-6
             and f['vals']]
        if len(b) < 10:
            continue
        lv = statistics.median([f['vals'][-1] for f in b])
        sp = statistics.median([f['sp'] for f in b])
        print('  %-13s %4d | %21.3f | %10.2f | %6.4f'
              % ('%d-%d' % (lo, hi), len(b), lv, sp, lv / sp))


if __name__ == '__main__':
    main(sys.argv[1:])
