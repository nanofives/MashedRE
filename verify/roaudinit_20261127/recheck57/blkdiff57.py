"""Recheck run 57: the differing dwords at tick 1 (ref1 ckpt201 vs run ckpt00) inside the given 1 KiB blocks (raw, no masks).
usage: py -3.12 blkdiff57.py <ref.data.bin> <run.data.bin> 0x<block> [...]"""
import struct, sys
LO = 0x005ea000
a = open(sys.argv[1], 'rb').read()
b = open(sys.argv[2], 'rb').read()
for blk in sys.argv[3:]:
    base = int(blk, 16)
    out = []
    for off in range(0, 1024, 4):
        addr = base + off
        i = addr - LO
        x = struct.unpack_from('<I', a, i)[0]
        y = struct.unpack_from('<I', b, i)[0]
        if x != y:
            out.append('0x%08x ref=0x%08x run=0x%08x' % (addr, x, y))
    print('%s: %d dwords differ' % (blk, len(out)))
    for l in out[:12]:
        print('   ' + l)
