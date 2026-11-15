"""Name the exe function that holds a return address from a [code-trap] line (MSVC .map, Rva+Base column).
usage: py -3.12 verify/portorigtop_20261113/rwaudio_632ba8/map_ret.py <file.map> <va hex> [<va hex> ...]"""
import re, sys

ROW = re.compile(r'^\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s', re.I)


def main():
    rows = []
    for line in open(sys.argv[1], encoding='latin-1'):
        m = ROW.match(line)
        if m:
            rows.append((int(m.group(2), 16), m.group(1)))
    rows.sort()
    for a in sys.argv[2:]:
        va = int(a, 16)
        best = None
        for addr, name in rows:
            if addr <= va:
                best = (addr, name)
            else:
                break
        print('0x%08x  %s + 0x%x' % (va, best[1], va - best[0]) if best else '0x%08x  ?' % va)


if __name__ == '__main__':
    main()
