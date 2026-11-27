"""Recheck run 57: inside the blocks firstdiv counts at record 0 (json blocks_at_record), the dwords that differ from ref1
at tick 1 in one run and not in the other (raw values, no masks, no PTR classification).
usage: py -3.12 words57.py <fd.json> <ref.data.bin> <old run.data.bin> <new run.data.bin>"""
import json, struct, sys
LO = 0x005ea000
blocks = [int(b, 16) for b in json.load(open(sys.argv[1]))['blocks_at_record']]
ref, old, new = (open(p, 'rb').read() for p in sys.argv[2:5])
for base in blocks:
    for off in range(0, 1024, 4):
        i = base + off - LO
        r, o, n = (struct.unpack_from('<I', x, i)[0] for x in (ref, old, new))
        if (r != o) != (r != n) or (r != n and o != n and abs(o - n) > 0x1000):
            print('0x%08x ref=0x%08x old=0x%08x new=0x%08x' % (base + off, r, o, n))
