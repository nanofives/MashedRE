"""Variant runner for MATCH-RWT-S4: vrun.py <base.c> <variants.txt> [toolchain]
variants.txt: blocks separated by lines '@@ name'; each block replaces everything from the
'// MATCH:' line of base.c onward (the block must contain its own // MATCH: line). Prints one score line each
and writes each probe to <variants dir>/<stem>_<name>.c"""
import os, subprocess, sys
base, vfile = sys.argv[1], sys.argv[2]
tc = sys.argv[3] if len(sys.argv) > 3 else None
head = open(base, encoding='utf-8').read().split('// MATCH:')[0]
txt = open(vfile, encoding='utf-8').read()
outdir = os.path.dirname(os.path.abspath(vfile))
stem = os.path.splitext(os.path.basename(vfile))[0]
pre = ''
blocks = txt.split('\n@@ ')
if blocks[0].startswith('@@ '):
    blocks[0] = blocks[0][3:]
else:
    pre = blocks.pop(0)  # shared declarations for every variant
for b in blocks:
    name, body = b.split('\n', 1)
    name = name.strip()
    p = os.path.join(outdir, f'{stem}_{name}.c')
    open(p, 'w', encoding='utf-8').write(head + pre + '\n' + body)
    cmd = ['py', '-3.12', 're/tools/match/match.py', '--bytes', p]
    if tc:
        cmd[4:4] = ['--toolchain', tc]
    r = subprocess.run(cmd, capture_output=True, text=True)
    lines = [l for l in (r.stdout + r.stderr).splitlines() if '%' in l or 'error' in l.lower() or 'bytes:' in l]
    print(f'{name:12s}', ' | '.join(l.strip() for l in lines[:3]))
