"""PORT-TOP-08: for each RVA, the exported port name of its matched body, the TU that owns it, and whether that TU is
in exe_sources.rsp. usage: py -3.12 verify/porttop08_20261112/resolve_names.py <rva> [<rva> ...]"""
import glob
import os
import re
import sys

SRC = 'mashedmod/src/mashed_re'
BS = chr(92)
exe = set()
for ln in open('mashedmod/exe_sources.rsp', encoding='latin-1'):
    s = ln.strip().strip('"').replace(BS, '/')
    if s.endswith('.cpp'):
        exe.add(s)
files = glob.glob(SRC + '/**/*.cpp', recursive=True)
txt = {f: open(f, encoding='latin-1').read() for f in files}
for r in sys.argv[1:]:
    r = r.lower().replace('0x', '')
    hits = []
    for f, t in txt.items():
        rel = os.path.relpath(f, SRC).replace(BS, '/')
        for m in re.finditer(r'#define\s+FUN_%s\s+(\w+)' % r, t, re.I):
            n = m.group(1)
            own = re.search(r'RH_ScopedInstall\(\s*%s\s*,\s*0x%s\s*\)' % (n, r), t, re.I) or \
                re.search(r'#define\s+FUN_%s\s+%s\b[^\n]*\n(?:[^\n]*\n){0,80}?#include\s+"[^"]*/%s\.c"' % (r, n, r), t, re.I)
            if own:
                hits.append('%s %s %s' % (n, rel, 'EXE' if rel in exe else 'asi'))
        if not any(rel in h for h in hits):
            m = re.search(r'RH_ScopedInstall\(\s*(\w+)\s*,\s*0x%s\s*\)' % r, t, re.I)
            if m:
                hits.append('%s %s %s (install)' % (m.group(1), rel, 'EXE' if rel in exe else 'asi'))
    src = ' '.join(sorted(glob.glob('re/match/*/%s.c' % r))) or '-'
    print('0x%s | %s | %s' % (r, ' ; '.join(hits) or 'NONE', src))
