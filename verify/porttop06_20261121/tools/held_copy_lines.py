"""PORT-TOP-06: plain (untagged) dual_copy_allowlist.txt lines for the static held copies the raceframe closure compiles
twice (same precedent as RWC-CORE-RUN's 0x004d5480 lines). For each finding: the `static` definition line in every source
whose wrapper the finding names. Appends only lines whose key is not present. Run from the worktree root."""
import re, subprocess, sys

ALLOW = 're/tools/dual_copy_allowlist.txt'
seed = subprocess.run([sys.executable, 'scripts/lint_rva_bodies.py', '--seed'], capture_output=True, text=True).stdout
have = set(l.split('#')[0].strip() for l in open(ALLOW, encoding='utf-8'))
new = []
for l in seed.splitlines():
    k = l.split('#')[0].strip()
    if not k or k in have or k.startswith('ORIGTOP-ONLY') or l.startswith('#'):
        continue
    check, rva, files = k.split(':', 2)
    where = []
    for f in files.split(','):
        p = 'mashedmod/src/mashed_re/' + f
        t = open(p, encoding='utf-8').read()
        if not t.startswith('// PORT-TOP-06'):
            where.append('%s (asi-only, not this row)' % f)
            continue
        src = re.search(r'#include "(?:\.\./)+(re/match/[^"]+)"', t).group(1)
        s = open(src, encoding='utf-8').read()
        m = re.search(r'^[ \t]*static\b[^\n;]*\bFUN_%s\s*\(' % rva, s, re.M | re.I)
        if not m:
            sys.exit('not a static held copy: %s %s %s' % (k, f, src))
        where.append('%s:%d' % (src, s[:m.start()].count('\n') + 1))
    new.append('%s   # PORT-TOP-06 (L-111 (a)), 2026-11-21: 0x%s is a static held copy (file-local) in each source, '
               'held there for its DDK codegen: %s. Both PortTop TUs are exe-only and run only under MASHED_ORIG_TOP + '
               'MASHED_OT_RACEFRAME; deleted at the ORIG_TOP flip. Evidence: verify/porttop06_20261121/RESULT_PORT-TOP-06.md'
               % (k, rva, ', '.join(where)))
raw = open(ALLOW, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
t = raw.decode()
if new:
    if not t.endswith(nl): t += nl
    open(ALLOW, 'wb').write((t + nl.join(new) + nl).encode())
for n in new:
    print(n[:200])
print('held-copy lines +', len(new))
