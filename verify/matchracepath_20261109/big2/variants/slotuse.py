"""slotuse.py <one-side listing> [frame]: frame-relative [esp+X] uses per slot (linear push tracking)."""
import re, sys, collections
L = open(sys.argv[1]).read().splitlines()
dep = 0; u = collections.defaultdict(list)
for i, s in enumerate(L):
    s = s.strip()
    for x in re.findall(r'\[esp \+ (?:\w+\*\d+ \+ )?(0x[0-9a-f]+)\]', s):
        u[int(x, 16) - dep].append(i)
    if s.startswith('push'): dep += 4
    elif s.startswith('pop'): dep -= 4
    else:
        m = re.match(r'(add|sub) esp, (0x[0-9a-f]+|\d+)', s)
        if m and i > 0:
            v = int(m.group(2), 0); dep += (-v if m.group(1) == 'add' else v)
    if s.startswith('push edi') and i < 12: base = dep
for x in sorted(u):
    r = u[x]
    print(f"{x:#06x} n={len(r):3d} first={r[0]:4d} last={r[-1]:4d}")
