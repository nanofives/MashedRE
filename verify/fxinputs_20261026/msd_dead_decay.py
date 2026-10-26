"""Per-frame dead-car decay from an MSD1 capture (original or port).

usage: py -3.12 msd_dead_decay.py <file.msd> [n_after] [--from-frame F]
--from-frame F: treat frame_idx F..end as one dead episode (the knob-OFF control, whose +0x4 never drops).

Prints the frames around each rec+0x4 1->0 transition: rec+0x4, rec+0x10 (i32), |rec+0x9b0..+0x9b8|,
rec+0x9e4, rec+0xb0c, and a summary per dead episode: frames to |v| == 0, frames with |v|^2 >= 62500
(the FX outer gate, [0x005ccd20]), frames bit-frozen (payload +0x9b0..+0x9b8 equal to the previous frame).
"""
import math, struct, sys

argv = sys.argv[1:]
from_frame = None
if '--from-frame' in argv:
    k = argv.index('--from-frame'); from_frame = int(argv[k + 1]); del argv[k:k + 2]
path = argv[0]
nafter = int(argv[1]) if len(argv) > 1 else 40
b = open(path, 'rb').read()
assert b[:4] == b'MSD1'
rec, = struct.unpack_from('<I', b, 4)
fr, off = [], 16
while off + 4 + rec <= len(b):
    fi, = struct.unpack_from('<I', b, off)
    p = b[off + 4: off + 4 + rec]
    a4, a10 = struct.unpack_from('<ii', p, 0x4)[0], struct.unpack_from('<i', p, 0x10)[0]
    v = struct.unpack_from('<fff', p, 0x9b0)
    fr.append(dict(f=fi, a4=a4, a10=a10, v=v, raw=p[0x9b0:0x9bc],
                   s=struct.unpack_from('<f', p, 0x9e4)[0], b0c=struct.unpack_from('<f', p, 0xb0c)[0]))
    off += 4 + rec
print(f'{len(fr)} frames, frame_idx {fr[0]["f"]}..{fr[-1]["f"]}')
eps, i = [], 0
if from_frame is not None:
    i0 = next(k for k, x in enumerate(fr) if x['f'] >= from_frame)
    eps.append((i0, len(fr))); i = len(fr)
while i < len(fr):
    if fr[i]['a4'] == 0 and (i == 0 or fr[i - 1]['a4'] != 0):
        j = i
        while j < len(fr) and fr[j]['a4'] == 0:
            j += 1
        eps.append((i, j))
        i = j
    else:
        i += 1
for (i, j) in eps:
    seg = fr[i:j]
    mag = [math.sqrt(sum(c * c for c in x['v'])) for x in seg]
    tz = next((k for k, m in enumerate(mag) if m == 0.0), None)
    gate = sum(1 for m in mag if m * m >= 62500.0)
    frozen = sum(1 for k in range(1, len(seg)) if seg[k]['raw'] == seg[k - 1]['raw'] and mag[k] != 0.0)
    zero = sum(1 for m in mag if m == 0.0)
    print(f'== dead episode frames {seg[0]["f"]}..{seg[-1]["f"]} ({len(seg)} captured) rec+0x10 at start {seg[0]["a10"]} '
          f'| |v| at kill {mag[0]:.4f} | first |v|==0 after {tz} frames | gate-pass {gate} | zero {zero} | frozen-nonzero {frozen}')
    for k in range(max(0, i - 2), min(len(fr), i + nafter)):
        x = fr[k]
        m = math.sqrt(sum(c * c for c in x['v']))
        print(f'  f={x["f"]:6d} +4={x["a4"]} +10={x["a10"]} |v|={m:10.4f} 9e4={x["s"]:10.4f} b0c={x["b0c"]:8.3f}')
