r"""D4-FX-REBASE: per-state, per-car summary of orig_fx_rebase.py <prefix>.D.rows.bin (132 B per FUN_0040ca00 call).
Per (DAT_0063ba8c state, car): frames, alive frames, frames with |rec+0x9b0|^2 >= 62500.0 (the 0x0040cae0 gate,
[0x005ccd20]), frames whose position (+0x958, +0x960) moved since the previous row, ctrl byte 4 classes
(0 / 1..0x40 / 0x41..0xfe / 0xff), ctrl byte 5 > 0x40, rec+0xcfc != 0, (int)rec+0x9e0 > 1.
Then, for every car alive in state 7, the first 12 state-7 rows and every 600th after.
usage: py -3.12 series_score.py <prefix>.D.rows.bin [...]"""
import math, struct, sys

ROW = 132
for path in sys.argv[1:]:
    b = open(path, 'rb').read()
    n = len(b) // ROW
    print('%s rows=%d' % (path.replace('\\', '/').split('/')[-1], n))
    st = {}
    prev = [None] * 4
    s7 = {c: [] for c in range(4)}
    for i in range(n):
        r = b[i * ROW:(i + 1) * ROW]
        state, phase, alive = r[0], r[1], r[2]
        for c in range(4):
            q = r[4 + c * 32:4 + (c + 1) * 32]
            v2, px, pz, f9e0 = struct.unpack_from('<4f', q, 0)
            cfc = struct.unpack_from('<i', q, 16)[0]
            ctrl = q[20:28]
            al = (alive >> c) & 1
            k = st.setdefault((state, c), dict(fr=0, al=0, fast=0, mv=0, c4=[0, 0, 0, 0], c5=0, cfc=0, g9e0=0, d=0.0))
            k['fr'] += 1; k['al'] += al
            if v2 >= 62500.0: k['fast'] += 1
            if prev[c] is not None and (px, pz) != prev[c]:
                k['mv'] += 1; k['d'] += math.hypot(px - prev[c][0], pz - prev[c][1])
            prev[c] = (px, pz)
            t = ctrl[4]
            k['c4'][0 if t == 0 else 1 if t <= 0x40 else 2 if t < 0xff else 3] += 1
            if ctrl[5] > 0x40: k['c5'] += 1
            if cfc: k['cfc'] += 1
            if f9e0 >= 2.0: k['g9e0'] += 1
            if state == 7 and al: s7[c].append((i, math.sqrt(max(v2, 0.0)), px, pz, ctrl.hex(), cfc, f9e0))
    print('  state car | frames alive | v2>=62500 moved dist | c4 [0,1..40,41..fe,ff] | c5>40 cfc!=0 9e0>=2')
    for (s, c), k in sorted(st.items()):
        print('  %5d %3d | %6d %5d | %6d %6d %9.1f | %s | %5d %5d %5d' % (s, c, k['fr'], k['al'], k['fast'], k['mv'], k['d'],
              k['c4'], k['c5'], k['cfc'], k['g9e0']))
    for c in range(4):
        rows = s7[c]
        if not rows: continue
        print('  car %d alive in state 7: %d rows, first at frame %d' % (c, len(rows), rows[0][0]))
        for j, (i, v, px, pz, cx, cfc, f) in enumerate(rows):
            if j < 12 or j % 600 == 0:
                print('    f%-6d |v|=%9.2f pos=(%10.2f, %10.2f) ctrl0..7=%s cfc=%d 9e0=%.2f' % (i, v, px, pz, cx, cfc, f))
