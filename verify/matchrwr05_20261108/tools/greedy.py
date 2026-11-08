"""greedy.py <src.c> <0xrva> <spec.py>: spec defines TEMPLATE (with {0},{1},.. slots) and SLOTS (list of option lists).
Greedy coordinate search over slots (2 passes). Writes best to scratch/<va>_greedy.c and prints its score."""
import sys, re, subprocess
WT = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR05'
S = 'D:/MashedWT/MATCH-RWR05_data/scratch/'
src, addr, spec = sys.argv[1:4]
ns = {}; exec(open(spec).read(), ns)
s = open(src, newline='').read().replace('\r\n', '\n')
va = addr.lower().replace('0x', '').zfill(8)
pre = s[:s.index(ns['CUT'])]
def score(choice):
    body = ns['TEMPLATE'].format(*[ns['SLOTS'][i][c] for i, c in enumerate(choice)])
    p = S + va + '_greedy_tmp.c'
    open(p, 'w', newline='').write(pre + body)
    r = subprocess.run([sys.executable, WT + '/re/tools/match/match.py', p, '--only', addr], capture_output=True, text=True, cwd=WT)
    m = re.search(r': ([\d.]+)% \(regs', r.stdout)
    return (float(m.group(1)) if m else -1.0), pre + body
cur = [0] * len(ns['SLOTS'])
best, text = score(cur)
print('start', best)
for _ in range(2):
    for i, opts in enumerate(ns['SLOTS']):
        for c in range(len(opts)):
            if c == cur[i]: continue
            t = cur[:]; t[i] = c
            sc, tx = score(t)
            if sc > best:
                best, cur, text = sc, t, tx
                print('slot', i, '->', c, best)
open(S + va + '_greedy.c', 'w', newline='').write(text)
print('best', best, cur)
