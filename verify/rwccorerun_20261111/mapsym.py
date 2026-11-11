"""Resolve exe-relative offsets (the [RWC_CORE] AV lines) against an MSVC link map.

usage: py -3.12 verify/rwccorerun_20261111/mapsym.py <map> <off> [<off> ...]   (offsets hex, e.g. 0x1A0B6A or +0x1A0B6A)
"""
import bisect
import re
import sys


def load(path):
    base = None
    syms = []
    for line in open(path, encoding='latin-1'):
        m = re.match(r'\s*Preferred load address is ([0-9a-fA-F]+)', line)
        if m:
            base = int(m.group(1), 16)
        m = re.match(r'\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+(?:f\s+)?(?:i\s+)?(\S+)', line)
        if m and base is not None:
            va = int(m.group(2), 16)
            if va >= base:
                syms.append((va - base, m.group(1), m.group(3)))
    syms.sort()
    return syms


def main():
    syms = load(sys.argv[1])
    keys = [s[0] for s in syms]
    for a in sys.argv[2:]:
        off = int(a.lstrip('+'), 16)
        i = bisect.bisect_right(keys, off) - 1
        s = syms[i] if i >= 0 else (0, '?', '?')
        print('+0x%X  %s+0x%X  (%s)' % (off, s[1], off - s[0], s[2]))


if __name__ == '__main__':
    main()
