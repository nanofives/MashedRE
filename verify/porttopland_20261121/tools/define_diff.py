"""PORT-TOP-LAND-06-RF: for every changed generated wrapper under mashedmod/src/mashed_re/Match/ (working tree against
<ref>), the `#define FUN_<rva> <symbol>` pairs that changed, comments ignored. Stop files are listed by count only.
py -3.12 verify/porttopland_20261121/tools/define_diff.py [ref]"""
import re, subprocess, sys

ref = sys.argv[1] if len(sys.argv) > 1 else 'HEAD'
files = subprocess.run(['git', 'diff', '--name-only', ref, '--', 'mashedmod/src/mashed_re/Match/'], capture_output=True,
                       text=True).stdout.split()


def defs(text):
    return {m.group(1): m.group(2) for m in re.finditer(r'^#define (FUN_[0-9a-fA-F]{8}) (\S+)', text, re.M)}


for f in files:
    old = subprocess.run(['git', 'show', '%s:%s' % (ref, f)], capture_output=True, text=True, errors='replace').stdout
    try:
        new = open(f, encoding='utf-8', errors='replace').read()
    except FileNotFoundError:
        new = ''
    a, b = defs(old), defs(new)
    ch = ['%s %s -> %s' % (k, a.get(k, '-'), b.get(k, '-')) for k in sorted(set(a) | set(b)) if a.get(k) != b.get(k)]
    if f.endswith('Stops.cpp'):
        so = set(re.findall(r'_Stop_([0-9a-f]{8})|Stop_([0-9a-f]{8})\(', old))
        sn = set(re.findall(r'_Stop_([0-9a-f]{8})|Stop_([0-9a-f]{8})\(', new))
        print('%s: stop file, %d -> %d stop definitions' % (f, len(so), len(sn)))
    elif ch:
        print('%s: %s' % (f, '; '.join(ch)))
    else:
        print('%s: comments / text only' % f)
