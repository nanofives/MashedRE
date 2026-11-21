"""List generated bulk wrappers whose matched source does not define a function named FUN_<rva> (thunk_FUN_ ... names)."""
import re, glob
for f in sorted(glob.glob('mashedmod/src/mashed_re/Match/PortTop_0*.cpp')):
    t = open(f, encoding='utf-8').read()
    if 'RACE-BIND-C4-BULK' not in t[:200]:
        continue
    rva = re.search(r'PortTop_([0-9a-f]{8})\.cpp', f).group(1)
    s = re.search(r'#include "(?:\.\./)+(re/match/[^"]+)"', t).group(1)
    st = open(s, encoding='utf-8', errors='replace').read()
    if not re.search(r'^[A-Za-z_][^\n;=(]*\bFUN_%s\s*\([^;{]*\)\s*\{' % rva, st, re.I | re.M):
        defs = re.findall(r'^[A-Za-z_][^\n;=(]*\b((?:thunk_)?FUN_[0-9a-f]{8})\s*\([^;{]*\)\s*\{', st, re.I | re.M)
        print(rva, s, defs[:4])
