# RACE-LOAD-LUA-2: emit the `extra_pre` rules.tsv lines a C unit (BIND-CUNCLEAN-AS-C) needs when its matched source declares
# globals with MGLOBAL: in C the source preamble makes them `extern T n` (unresolved in the exe), so the wrapper replaces the
# preamble (MATCH_PREAMBLE) and turns each global into a #define at its original address, the form RACE-LOAD-LUA used for
# 0x0047b860 / 0x0047b8d0 (state/raceload/rules.tsv). Prints the lines; the row appends them to its own profile.
#   py -3.12 verify/racelua2_20261122/cunit_preamble.py <rva> [<rva> ...]
import re, sys, json
from pathlib import Path

W = Path(__file__).resolve().parents[2]
PRE = [
    "#define MATCH_PREAMBLE 1   // C unit: the source preamble is replaced by these lines (its MGLOBAL is `extern T n` in C, unresolved in the exe)",
    "typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;",
    "typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;",
    "typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;",
    "typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;",
    "typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef unsigned char bool;",
    "#define MGLOBAL(T, n, a) enum { mglobal_##n }   // the name is a #define at its original address below",
    "#define MGLOBALA(T, n, N, a) enum { mglobala_##n }",
]
plan = json.load(open(W / 're/tools/bind/state/raceload/plan.json'))
for rva in sys.argv[1:]:
    src = plan[rva][1]
    text = open(W / src, encoding='utf-8', errors='replace').read().splitlines()
    pre_ok = any('MATCH_PREAMBLE' in l for l in text)
    out = list(PRE) if pre_ok else []
    for i, l in enumerate(text, 1):
        m = re.match(r'\s*MGLOBAL\(\s*(.+?)\s*,\s*(\w+)\s*,\s*(0x[0-9a-fA-F]+)\s*\);', l)
        if m:
            t, n, a = m.groups()
            out.append('#define %s (*(%s *)%su)   // MGLOBAL %s:%d' % (n, t, a.lower(), src, i))
            continue
        m = re.match(r'\s*MGLOBALA\(\s*(.+?)\s*,\s*(\w+)\s*,\s*(\w+)\s*,\s*(0x[0-9a-fA-F]+)\s*\);', l)
        if m:
            t, n, N, a = m.groups()
            out.append('#define %s (*(%s (*)[%s])%su)   // MGLOBALA %s:%d' % (n, t, N, a.lower(), src, i))
    if not pre_ok:
        sys.stderr.write('%s: no MATCH_PREAMBLE guard in %s, no lines\n' % (rva, src)); continue
    for l in out:
        print('extra_pre\t%s\t%s' % (rva, l))
