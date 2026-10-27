"""Per-frame decel of a killed car in a matched |v| band (D4-FX-INPUTS attempt 3, PREREG_killdamage.md).

usage: py -3.12 decel_band.py <file.msd> [lo] [hi]   (default band 25..70)
For the first rec+0x4 1->0 episode: every consecutive pair of dead frames with lo <= |v|_prev <= hi and |v|_next > 0
gives one decel sample |v|_prev - |v|_next. Prints n, median, min, max, the wheel+0x54 values (+0x1f8 + w*0xc4)
and rec+0x2c / +0x30 on the first sample frame.
"""
import math, statistics, struct, sys
path = sys.argv[1]; lo = float(sys.argv[2]) if len(sys.argv) > 2 else 25.0; hi = float(sys.argv[3]) if len(sys.argv) > 3 else 70.0
b = open(path, 'rb').read(); assert b[:4] == b'MSD1'
rec, = struct.unpack_from('<I', b, 4); fr = []; off = 16
while off + 4 + rec <= len(b):
    fi, = struct.unpack_from('<I', b, off); p = b[off + 4: off + 4 + rec]; off += 4 + rec
    fr.append((fi, struct.unpack_from('<i', p, 4)[0], math.sqrt(sum(c * c for c in struct.unpack_from('<fff', p, 0x9b0))), p))
k0 = next(k for k in range(1, len(fr)) if fr[k][1] == 0 and fr[k - 1][1] != 0)
s, first = [], None
for k in range(k0, len(fr) - 1):
    if fr[k][1] != 0 or fr[k + 1][1] != 0: break
    a, c = fr[k][2], fr[k + 1][2]
    if lo <= a <= hi and c > 0.0:
        s.append(a - c); first = first or fr[k]
print(f'{path}: kill frame {fr[k0][0]} |v| {fr[k0][2]:.4f} band {lo}..{hi} n={len(s)}', end='')
if s:
    p = first[3]
    print(f' median {statistics.median(s):.4f} min {min(s):.4f} max {max(s):.4f} | wheel+0x54 '
          + '/'.join(f'{struct.unpack_from("<f", p, 0x1f8 + w * 0xc4)[0]:.4g}' for w in range(4))
          + f' | +0x2c {struct.unpack_from("<i", p, 0x2c)[0]} +0x30 {struct.unpack_from("<i", p, 0x30)[0]}')
else: print()
