r"""D4-FX-STATE7: car 0's rec+0x4 / rec+0x10 / rec+0xcfc runs in an original MSD1 capture (statediff car 0, base 0x008815a0).
Prints each run of equal (rec+0x4, rec+0x10, rec+0xcfc) of 3+ frames, or any run with rec+0xcfc != 0.
usage: py -3.12 msd_car0.py <x.msd> [...]"""
import os, struct, sys
for path in sys.argv[1:]:
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1'
    rec, = struct.unpack_from('<I', b, 4)
    off, runs, n = 16, [], 0
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        p = off + 4
        k = (struct.unpack_from('<i', b, p + 0x4)[0], struct.unpack_from('<i', b, p + 0x10)[0], struct.unpack_from('<i', b, p + 0xcfc)[0])
        if runs and runs[-1][0] == k: runs[-1][2] = fi; runs[-1][3] += 1
        else: runs.append([k, fi, fi, 1])
        off += 4 + rec; n += 1
    print('== %s  %d frames' % (os.path.basename(path), n))
    for k, a, z, c in runs:
        if c >= 3 or k[2]: print('  +0x4=%d +0x10=%d +0xcfc=%d  frames %d..%d (%d)' % (k + (a, z, c)))
