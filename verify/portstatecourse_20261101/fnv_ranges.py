"""MATCH-PORTSTATE-COURSE P2 witness: FNV-1a 32 of the four seeded ranges, read out of MASHED.exe.unpatched.

Usage: py -3.12 verify/portstatecourse_20261101/fnv_ranges.py [path-to-MASHED.exe.unpatched] [mashed_re.log ...]
Prints the file-side hashes and, for each log given, compares its [portstate-course] lines field by field.
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT = os.path.normpath(os.path.join(HERE, '..', '..', 'original', 'MASHED.exe.unpatched'))
RANGES = [('recs', 0x005f2570, 0x104), ('ptrs', 0x005f2728, 0x34), ('hook', 0x005f33f8, 0x3a8), ('count', 0x005f37a0, 4)]


def sections(b):
    pe = struct.unpack_from('<I', b, 0x3c)[0]
    nsec = struct.unpack_from('<H', b, pe + 6)[0]
    opt = struct.unpack_from('<H', b, pe + 20)[0]
    base = struct.unpack_from('<I', b, pe + 24 + 28)[0]
    o = pe + 24 + opt
    out = []
    for _ in range(nsec):
        vs, va, rs, ro = struct.unpack_from('<IIII', b, o + 8)
        out.append((base + va, vs, ro, rs))
        o += 40
    return out


def read(b, secs, a, n):
    for va, vs, ro, rs in secs:
        if va <= a and a + n <= va + rs:   # file-backed only
            return b[ro + a - va: ro + a - va + n]
    raise SystemExit('range 0x%08x+0x%x is not file-backed' % (a, n))


def fnv(d):
    h = 2166136261
    for c in d:
        h = ((h ^ c) * 16777619) & 0xffffffff
    return h


def main():
    args = sys.argv[1:]
    exe = args.pop(0) if args and not args[0].lower().endswith('.log') else DEFAULT
    b = open(exe, 'rb').read()
    secs = sections(b)
    want = {k: '%08x' % fnv(read(b, secs, a, n)) for k, a, n in RANGES}
    print('file', exe, ' '.join('%s=%s' % (k, want[k]) for k, _, _ in RANGES))
    bad = 0
    for log in args:
        for line in open(log, encoding='utf-8', errors='replace'):
            if '[portstate-course]' not in line:
                continue
            got = dict(re.findall(r'(\w+)=(\S+)', line))
            res = ['%s %s' % (k, 'OK' if got.get(k) == want[k] else 'MISMATCH %s' % got.get(k)) for k, _, _ in RANGES]
            bad += sum('MISMATCH' in r for r in res)
            print(os.path.basename(log), ' '.join(res), 'sel=%s name=%s' % (got.get('sel'), got.get('name')))
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
