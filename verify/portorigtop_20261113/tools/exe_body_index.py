"""Index rva -> exe body symbol for every TU in mashedmod/exe_sources.rsp.
Body = a file that #includes re/match/<d>/<rva>.c and #defines FUN_<rva> <sym>, or RH_ScopedInstall(sym, 0x<rva>) / an
extern "C" definition right under a `// 0x<rva>` comment."""
import re, os, sys, json
root = sys.argv[1]
src = os.path.join(root, 'mashedmod', 'src', 'mashed_re')
files = []
for line in open(os.path.join(root, 'mashedmod', 'exe_sources.rsp'), encoding='utf-8'):
    line = line.strip().strip('"')
    if line.endswith('.cpp') or line.endswith('.c'):
        files.append(line.replace('\\', '/'))
idx = {}
def add(rva, sym, f, how):
    idx.setdefault(rva, []).append((sym, f, how))
for f in files:
    p = os.path.join(src, f)
    if not os.path.exists(p):
        continue
    t = open(p, encoding='utf-8', errors='replace').read()
    incs = set(m.lower() for m in re.findall(r'#include\s+"[^"]*re/match/[^/"]+/([0-9a-fA-F]{8})\.c"', t))
    for m in re.finditer(r'#define\s+FUN_([0-9a-fA-F]{8})\s+(\w+)', t):
        r = m.group(1).lower()
        if r in incs:
            add(r, m.group(2), f, 'match-include')
    for m in re.finditer(r'RH_ScopedInstall\(\s*([\w:]+)\s*,\s*0x([0-9a-fA-F]{8})', t):
        add(m.group(2).lower(), m.group(1), f, 'rh')
json.dump(idx, open(sys.argv[2], 'w'), indent=0)
print(len(idx), 'rvas indexed from', len(files), 'files')
