"""PORT-OT-RACE: append to rltools/cpp_unclean.txt every generated PortTop_<rva>.cpp TU that a VS2022 build log shows a
compile error for (first error kept). usage: py -3.12 unclean_from_log.py <build.log>"""
import os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
log = open(sys.argv[1], encoding='mbcs', errors='replace').read().splitlines()
pat = re.compile(r'error (C\d+): (.*?) \((?:compilando archivo de origen|compiling source file) Match\\PortTop_([0-9a-f]{8})\.cpp\)')
first = {}
for l in log:
    m = pat.search(l)
    if m and m.group(3) not in first:
        first[m.group(3)] = m.group(1) + ' ' + m.group(2)[:110]
p = os.path.join(HERE, 'cpp_unclean.txt')
have = {}
if os.path.exists(p):
    for l in open(p, encoding='utf-8'):
        if l.strip() and not l.startswith('#'):
            have[l.split(chr(9))[0].strip()] = l.rstrip(chr(10))
new = 0
with open(p, 'a', encoding='utf-8', newline='\n') as f:
    if not have and os.path.getsize(p) == 0 if os.path.exists(p) else True:
        f.write('# PORT-OT-RACE checkpoint 2: matched sources that fail to compile as C++ (MATCH_PORT, VS2022) in the exe build; first error.\n')
    for k in sorted(first):
        if k not in have:
            f.write('%s\t%s\n' % (k, first[k])); new += 1
print('unclean +', new, 'total', len(have) + new)
