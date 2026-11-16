"""PORT-OT-RACE checkpoint 3: list the planned binds whose matched source is already #included by a non-generated TU of
mashedmod/exe_sources.rsp (an existing exe body of the same source, which exe_body_index.py misses when the TU keeps
the name FUN_<rva> with no rename #define). Binding a second copy would put the rva in the exe twice.
usage: py -3.12 same_source_exe.py <plan listing from rl_bind_gen.py (dry run stdout)>
Run from the worktree root. Prints: rva, source, the existing TU, the symbol it compiles the rva under."""
import os, re, sys
src_root = 'mashedmod/src/mashed_re'
tus = []
for line in open('mashedmod/exe_sources.rsp', encoding='utf-8'):
    line = line.strip().strip('"').replace(chr(92), '/')
    if line.endswith('.cpp') or line.endswith('.c'):
        tus.append(line)
inc = {}   # source path -> [(tu, text)]
for tu in tus:
    p = os.path.join(src_root, tu)
    if not os.path.exists(p):
        continue
    t = open(p, encoding='utf-8', errors='replace').read()
    if t.startswith('// PORT-OT-RACE (2026-11-16): exe-only port body'):
        continue
    for m in re.finditer(r'#include\s+"(?:\.\./)+(re/match/[^"]+)"', t):
        inc.setdefault(m.group(1), []).append((tu, t))
n = 0
for l in open(sys.argv[1], encoding='utf-8', errors='replace'):
    p = l.split()
    if len(p) < 3 or p[1] != 'bind':
        continue
    rva, s = p[0], p[2].replace(chr(92), '/')
    for tu, t in inc.get(s, []):
        m = re.search(r'#define\s+FUN_%s\s+(\w+)' % rva, t)
        print(rva, s, tu, m.group(1) if m else 'FUN_%s' % rva)
        n += 1
print('total', n)
