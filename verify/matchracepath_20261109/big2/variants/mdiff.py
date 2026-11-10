"""mdiff.py <listing>: rows whose mnemonics differ (structure), with row index."""
import sys
L = open(sys.argv[1]).read().splitlines()[1:]
lo = int(sys.argv[2]) if len(sys.argv) > 2 else 0; hi = int(sys.argv[3]) if len(sys.argv) > 3 else 10**9
for i, l in enumerate(L):
    if not (lo <= i < hi) or not l.startswith('   !'): continue
    a, _, b = l[4:].partition('|'); a = a.strip(); b = b.strip()
    if (a.split(' ')[0] if a else '') != (b.split(' ')[0] if b else ''):
        print(i, f"{a[:44]:44} | {b[:44]}")
