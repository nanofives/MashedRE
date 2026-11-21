"""Resolve exe-relative offsets against an MSVC .map (RACE-RWCORE-RACELOAD-ENGINE step 1).

Usage: py -3.12 mapsym.py <map> <off> [<off> ...]   (offsets hex, exe-relative, e.g. 0x66EC or +0x66EC)
Prints, per offset: the nearest preceding public symbol, its object, and the distance.
"""
import bisect
import re
import sys


def load(path):
    base = None
    syms = []
    pat = re.compile(r'^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(f\s+)?(?:i\s+)?(\S+)\s*$')
    for line in open(path, encoding='latin-1'):
        if base is None and 'Preferred load address is' in line:
            base = int(line.split()[-1], 16)
            continue
        m = pat.match(line)
        if m:
            name, va, _, obj = m.group(1), int(m.group(2), 16), m.group(3), m.group(4)
            syms.append((va, name, obj))
    syms.sort()
    return base, syms


def main():
    base, syms = load(sys.argv[1])
    vas = [s[0] for s in syms]
    for a in sys.argv[2:]:
        off = int(a.lstrip('+'), 16)
        va = base + off
        i = bisect.bisect_right(vas, va) - 1
        if i < 0:
            print(f'+0x{off:X} va {va:08X}: before the first symbol')
            continue
        s = syms[i]
        print(f'+0x{off:X} va {va:08X}: {s[1]}+0x{va - s[0]:X}  ({s[2]})')


if __name__ == '__main__':
    main()
