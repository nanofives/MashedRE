"""retv.py <src> <0xaddr> [param_index ...] : try 'return <param>' variants for a void-typed function.
For each param index (default all pointer/int params) writes scratch/<va>_ret<k>.c and scores it.
--apply K writes variant K back to src (CRLF preserved)."""
import sys, subprocess, re
WT = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR04'
S = 'D:/MashedWT/MATCH-RWR04_data/scratch/'
src, addr = sys.argv[1], sys.argv[2]
apply = None
args = sys.argv[3:]
if args and args[0] == '--apply':
    apply = int(args[1]); args = []
va = addr.lower().replace('0x', '').zfill(8)
raw = open(src, newline='').read()
crlf = '\r\n' in raw
s = raw.replace('\r\n', '\n')
m = re.search(r'^void\s+(FUN_%s)\s*\(([^)]*)\)\s*\{' % va, s, re.M)
if not m:
    sys.exit('no void sig')
params = [p.strip() for p in m.group(2).split(',')]
# function body: from m.end() to matching brace
i = m.end(); depth = 1
while depth:
    c = s[i]
    depth += (c == '{') - (c == '}')
    i += 1
body = s[m.end():i - 1]


def variant(k):
    p = params[k]
    pm = re.match(r'(.*?)(\w+)$', p)
    ptype, pname = pm.group(1).strip(), pm.group(2)
    b = re.sub(r'\breturn\s*;', 'return %s;' % pname, body)
    if not re.search(r'return\s+%s;\s*$' % pname, b):
        b = b.rstrip() + '\n  return %s;\n' % pname
    sig = '%s %s(%s)\n{' % (ptype, m.group(1), m.group(2))
    return s[:m.start()] + sig + b + '}' + s[i:]


if apply is not None:
    t = variant(apply)
    if crlf:
        t = t.replace('\n', '\r\n')
    open(src, 'w', newline='').write(t)
    print('applied ret', apply)
    sys.exit()
ks = [int(a) for a in args] if args else range(len(params))
for k in ks:
    p = S + va + '_ret%d.c' % k
    open(p, 'w', newline='').write(variant(k))
    r = subprocess.run([sys.executable, WT + '/re/tools/match/match.py', p, '--only', addr],
                       capture_output=True, text=True, cwd=WT)
    line = [l for l in (r.stdout + r.stderr).splitlines() if '%' in l or 'rror' in l]
    print('ret%d' % k, params[k], '|', line[-1] if line else (r.stdout + r.stderr)[-300:])
