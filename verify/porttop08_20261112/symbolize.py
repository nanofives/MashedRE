"""PORT-TOP-08: map mashed_re.exe addresses to the nearest preceding public symbol in mashedmod/build/mashed_re.map.

usage: py -3.12 verify/porttop08_20261112/symbolize.py <map> <hexaddr> [<hexaddr> ...]
       py -3.12 verify/porttop08_20261112/symbolize.py <map> --tsv <watch.tsv>   (symbolizes pc and bt columns)
"""
import bisect
import re
import sys


def load(mp):
    syms = []
    pat = re.compile(r'^\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(f\s+)?(\S+)', re.I)
    for ln in open(mp, encoding='latin-1'):
        m = pat.match(ln)
        if m:
            syms.append((int(m.group(2), 16), m.group(1), m.group(4)))
    syms.sort()
    return syms


def sym(syms, keys, a):
    i = bisect.bisect_right(keys, a) - 1
    if i < 0:
        return '?'
    va, name, obj = syms[i]
    return '%s+0x%x [%s]' % (name, a - va, obj)


def main():
    syms = load(sys.argv[1])
    keys = [s[0] for s in syms]
    if sys.argv[2] == '--tsv':
        for ln in open(sys.argv[3], encoding='utf-8'):
            f = ln.rstrip('\n').split('\t')
            if f[0] == 'seq':
                continue
            print('#%s t=%s %s -> %s  pc=%s %s' % (f[0], f[1], f[3], f[4], f[2], sym(syms, keys, int(f[2], 16))))
            for b in f[6].split(','):
                if b:
                    print('    ret %s %s' % (b, sym(syms, keys, int(b, 16))))
        return
    for a in sys.argv[2:]:
        print(a, sym(syms, keys, int(a, 16)))


if __name__ == '__main__':
    main()
