#!/usr/bin/env python3
"""G-COAST: list the 0xd04 record dwords that change at the car-car contact frame and change back 10 +-1 frames later.
Contact frame = the first frame where +0x9e4 (speed) drops by more than 30% from the previous frame while > 500.
Usage: py -3.12 coast_fields.py <car.msd> [--show 0xaec,0x9b0,...]"""
import struct, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as M

path = sys.argv[1]
show = [int(x, 16) for x in sys.argv[3].split(',')] if len(sys.argv) > 3 and sys.argv[2] == '--show' else []
fr = list(M.parse(path))
f32 = lambda p, o: struct.unpack_from('<f', p, o)[0]
i32 = lambda p, o: struct.unpack_from('<i', p, o)[0]
ci = None
for k in range(1, len(fr)):
    a, b = f32(fr[k - 1][1], 0x9e4), f32(fr[k][1], 0x9e4)
    if a > 500 and b < 0.7 * a:
        ci = k; break
if ci is None:
    print('no contact frame found'); sys.exit(1)
print('contact at msd index %d frame %d: speed %.3f -> %.3f' % (ci, fr[ci][0], f32(fr[ci - 1][1], 0x9e4), f32(fr[ci][1], 0x9e4)))
n = len(fr[ci][1]) // 4
pre, at = fr[ci - 1][1], fr[ci][1]
hits = []
for w in range(n):
    o = w * 4
    if pre[o:o + 4] == at[o:o + 4]:
        continue
    # returns to a different regime 9..11 frames later: find the first frame after ci where the dword changes again
    vals = [fr[ci + d][1][o:o + 4] for d in range(0, 14) if ci + d < len(fr)]
    hold = 0
    for d in range(1, len(vals)):
        if vals[d] != vals[d - 1]:
            break
        hold = d
    hits.append((o, hold))
const = [h for h in hits if 8 <= h[1] <= 11]
print('%d dwords change at the contact; %d of them then hold constant for 8..11 frames:' % (len(hits), len(const)))
for o, hold in const:
    print('  +0x%03x hold=%d  i=%d f=%g' % (o, hold, i32(at, o), f32(at, o)))
# counters stepping by the clock: values that decrease by 50 per frame from the contact
print('dwords that step by -50 (int) per frame for >= 5 frames after the contact:')
for w in range(n):
    o = w * 4
    s = [i32(fr[ci + d][1], o) for d in range(0, 12) if ci + d < len(fr)]
    if len(s) >= 6 and all(s[d + 1] - s[d] == -50 for d in range(5)):
        print('  +0x%03x' % o, s)
for o in show:
    print('+0x%03x:' % o, ' '.join('%d:%g/%d' % (fr[ci + d][0], f32(fr[ci + d][1], o), i32(fr[ci + d][1], o)) for d in range(-2, 14) if 0 <= ci + d < len(fr)))
