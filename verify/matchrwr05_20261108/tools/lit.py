"""lit.py <src.c> <0xrva> [--apply]: replace float DAT_ globals with float literals (value read from the unpatched exe).
Tries: all consts literal, and each const alone. Prints scores; --apply writes the best if it beats the current."""
import sys, re, struct, subprocess, itertools
import pefile
WT = 'C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-RWR05'
S = 'D:/MashedWT/MATCH-RWR05_data/scratch/'
pe = pefile.PE('C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched', fast_load=True)
src, addr = sys.argv[1], sys.argv[2]
apply = '--apply' in sys.argv
raw = open(src, newline='').read()
crlf = '\r\n' in raw
s = raw.replace('\r\n', '\n')
va = addr.lower().replace('0x', '').zfill(8)
def lit(a, typ):
    if typ == 'double':
        v = struct.unpack('<d', pe.get_data(a - 0x400000, 8))[0]
        r = repr(v)
        return r if ('.' in r or 'e' in r) else r + '.0'
    v = struct.unpack('<f', pe.get_data(a - 0x400000, 4))[0]
    # shortest repr that round-trips through float32
    for p in range(1, 18):
        t = '%.*g' % (p, v)
        if struct.unpack('<f', struct.pack('<f', float(t)))[0] == v:
            break
    if not any(c in t for c in '.e'): t += '.0'
    if 'inf' in t or 'nan' in t: return None
    return t + 'f'
decls = re.findall(r'MGLOBAL\((float|double), (_?DAT_([0-9a-f]{8})), 0x[0-9a-f]+\);', s)
consts = {}
for typ, name, hexa in decls:
    l = lit(int(hexa, 16), typ)
    if l: consts[name] = l
def score(text, tag):
    p = S + va + '_lit_' + tag + '.c'
    open(p, 'w', newline='').write(text)
    r = subprocess.run([sys.executable, WT + '/re/tools/match/match.py', p, '--only', addr], capture_output=True, text=True, cwd=WT)
    m = re.search(r': ([\d.]+)% \(regs', r.stdout)
    return (float(m.group(1)) if m else -1.0), p
def subst(text, names):
    body_i = text.index('// MATCH:')
    head, body = text[:body_i], text[body_i:]
    for n in names:
        lv = consts[n]
        body = re.sub(r'\*\(float \*\)&' + n + r'\b', lv, body)
        body = re.sub(r'\(float\)' + n + r'\b', lv, body)
        body = re.sub(r'\b' + n + r'\b', '(' + lv + ')' if lv.startswith('-') else lv, body)
    return head + body
base, _ = score(s, 'base')
print('consts', consts, 'base', base)
best = (base, None, s)
names = list(consts)
cands = [('all', names)] + [(n, [n]) for n in names] if len(names) > 1 else [('all', names)]
if 2 < len(names) <= 5:
    for k in range(2, len(names)):
        for c in itertools.combinations(names, k):
            cands.append(('+'.join(x[-6:] for x in c), list(c)))
for tag, ns in cands:
    t = subst(s, ns)
    sc, p = score(t, tag.replace('+', '_'))
    print(tag, sc)
    if sc > best[0]:
        best = (sc, tag, t)
if apply and best[1]:
    t = best[2]
    if crlf: t = t.replace('\n', '\r\n')
    open(src, 'w', newline='').write(t)
    print('APPLIED', best[1], best[0])
