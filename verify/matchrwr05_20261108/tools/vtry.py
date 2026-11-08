"""vtry.py <src.c> <0xaddr> <spec.py>
spec.py defines OLD (snippet in src, LF) and VARIANTS (dict name -> replacement).
Writes scratch/<va>_<name>.c with LF endings and scores each with match.py.
--apply NAME : write that variant back into src preserving CRLF endings."""
import sys, subprocess, os, re
src, addr, spec = sys.argv[1], sys.argv[2], sys.argv[3]
apply = sys.argv[5] if len(sys.argv) > 5 and sys.argv[4] == '--apply' else None
ns = {}
exec(open(spec).read(), ns)
raw = open(src, newline='').read()
crlf = '\r\n' in raw
s = raw.replace('\r\n', '\n')
old = ns['OLD'].strip('\n')
if old not in s:
    sys.exit('OLD not found')
WT = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR05'
S = 'D:/MashedWT/MATCH-RWR05_data/scratch/'
va = addr.lower().replace('0x', '').zfill(8)
if apply:
    t = s.replace(old, ns['VARIANTS'][apply].strip('\n'))
    for eo, en in ns.get('EXTRA', []):
        t = t.replace(eo, en, 1)
    if crlf:
        t = t.replace('\n', '\r\n')
    open(src, 'w', newline='').write(t)
    print('applied', apply)
    sys.exit()
for k, v in ns['VARIANTS'].items():
    p = S + va + '_' + k + '.c'
    t = s.replace(old, v.strip('\n'))
    for eo, en in ns.get('EXTRA', []):
        t = t.replace(eo, en, 1)
    open(p, 'w', newline='').write(t)
    r = subprocess.run([sys.executable, WT + '/re/tools/match/match.py', p, '--only', addr],
                       capture_output=True, text=True, cwd=WT)
    line = [l for l in (r.stdout + r.stderr).splitlines() if '%' in l or 'rror' in l]
    print(k, '|', line[-1] if line else (r.stdout + r.stderr)[-300:])
