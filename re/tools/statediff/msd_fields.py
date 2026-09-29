#!/usr/bin/env python3
"""msd_fields.py - print arbitrary vehicle-record fields per frame from one MSD1
capture (re/frida/scenario_launch.py --statediff-out). Format: FORMAT.md.

Usage:
  py -3.12 re/tools/statediff/msd_fields.py <msd> <spec> [<spec> ...]
           [--every N] [--first N] [--distinct]

<spec> is <offset>:<type>, offset hex (0x...) or decimal, type in {f,i,u}.
  py -3.12 re/tools/statediff/msd_fields.py cap.msd 0x490:i 0x494:i 0x498:f 0x49c:f
--distinct prints the distinct value tuples with counts instead of a frame table.
"""
import struct, sys, collections

def parse(path):
    b = open(path, 'rb').read()
    assert b[:4] == b'MSD1', 'not MSD1'
    rec, base, _ = struct.unpack_from('<III', b, 4)
    off = 16
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off)
        yield fi, b[off + 4: off + 4 + rec]
        off += 4 + rec

def main(argv):
    path = argv[0]
    specs, every, first, distinct = [], 1, 0, False
    i = 1
    while i < len(argv):
        a = argv[i]
        if a == '--every': every = int(argv[i + 1]); i += 2; continue
        if a == '--first': first = int(argv[i + 1]); i += 2; continue
        if a == '--distinct': distinct = True; i += 1; continue
        o, t = a.split(':')
        specs.append((int(o, 0), t)); i += 1
    fmt = {'f': '<f', 'i': '<i', 'u': '<I'}
    rows, n = [], 0
    for fi, p in parse(path):
        vals = tuple(struct.unpack_from(fmt[t], p, o)[0] for o, t in specs)
        rows.append((fi, vals)); n += 1
        if first and n >= first and not distinct: break
    if distinct:
        c = collections.Counter(v for _, v in rows)
        for v, k in c.most_common():
            print('%6d  %s' % (k, '  '.join('%g' % x for x in v)))
        print('frames', len(rows))
        return
    hdr = '  '.join('%s:%s' % (hex(o), t) for o, t in specs)
    print('frame  ' + hdr)
    for j, (fi, vals) in enumerate(rows):
        if j % every: continue
        print('%5d  %s' % (fi, '  '.join('%g' % x for x in vals)))

if __name__ == '__main__':
    main(sys.argv[1:])
