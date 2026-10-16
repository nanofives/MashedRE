"""print record fields by capture INDEX (a8_launch's d counts indices, not frame ids).
usage: msd_win.py <msd> <lo> <hi> <off:type> ..."""
import struct, sys
path, lo, hi = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
specs = [(int(a.split(':')[0], 0), a.split(':')[1]) for a in sys.argv[4:]]
b = open(path, 'rb').read()
assert b[:4] == b'MSD1'
rec, base, _ = struct.unpack_from('<III', b, 4)
off = 16; frames = []
while off + 4 + rec <= len(b):
    fi, = struct.unpack_from('<I', b, off); frames.append((fi, off + 4)); off += 4 + rec
frames.sort()
fmt = {'f': '<f', 'i': '<i', 'u': '<I'}
print('idx   frame  ' + '  '.join('%s:%s' % (hex(o), t) for o, t in specs))
for j in range(lo, min(hi + 1, len(frames))):
    fi, po = frames[j]
    print('%4d %6d  %s' % (j, fi, '  '.join('%.9g' % struct.unpack_from(fmt[t], b, po + o)[0] for o, t in specs)))
