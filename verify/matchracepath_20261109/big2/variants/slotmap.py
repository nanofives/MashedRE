"""slotmap.py <match --show listing>: maps ours [esp+X] frame offsets to the original's, from aligned rows.
Tracks push/pop/add esp/sub esp per side (linear; jumps ignored, so depth after a call block is approximate)."""
import re, sys, collections
L = open(sys.argv[1]).read().splitlines()[1:]
dep = [0, 0]; m = collections.defaultdict(collections.Counter)
def upd(s, i):
    s = s.strip()
    if s.startswith('push'): dep[i] += 4
    elif s.startswith('pop'): dep[i] -= 4
    else:
        mm = re.match(r'(add|sub) esp, (0x[0-9a-f]+|\d+)', s)
        if mm:
            v = int(mm.group(2), 0); dep[i] += (-v if mm.group(1) == 'add' else v)
def offs(s, i):
    return [int(x, 16) - dep[i] for x in re.findall(r'\[esp \+ (?:\w+\*\d+ \+ )?(0x[0-9a-f]+)\]', s)]
for l in L:
    if not l.startswith('   ') or '|' not in l: continue
    a, _, b = l[4:].partition('|')
    oa, ob = offs(a, 0), offs(b, 1)
    if len(oa) == len(ob):
        for x, y in zip(oa, ob): m[x][y] += 1
    upd(a, 0); upd(b, 1)
for x in sorted(m):
    print(f"ours {x:#06x} -> " + ", ".join(f"{y:#06x}x{n}" for y, n in m[x].most_common(4)))
