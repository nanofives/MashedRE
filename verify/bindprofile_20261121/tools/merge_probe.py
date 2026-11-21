"""BIND-PROFILE-DATA: 3-way merge probes for the append-only tables (synthetic sides built from the committed file).
Run from the worktree root: py -3.12 verify/bindprofile_20261121/tools/merge_probe.py <scratch dir>
Cases per file: (1) both sides append a different row, (2) both append the same row, (3) one side edits row R, the
other appends, (4) both edit row R differently. Tools: `git merge-file --union` (what merge=union does) and
re/tools/tracker_merge3.py allow (the train's keyed merge). Prints the outcome and duplicate keys per case."""
import os, subprocess, sys

out = sys.argv[1]
os.makedirs(out, exist_ok=True)


def key(l):
    s = l.strip()
    return None if not s or s.startswith('#') else s.split()[0]


def dups(text):
    ks = [key(l) for l in text.splitlines()]
    ks = [k for k in ks if k]
    return sorted({k for k in ks if ks.count(k) > 1})


def probe(path, row_of, edit_key):
    base = open(path, encoding='utf-8').read().replace('\r\n', '\n')
    lines = base.splitlines()
    i = next(n for n, l in enumerate(lines) if key(l) == edit_key)
    a, b = row_of('A'), row_of('B')
    ed1 = lines[:i] + [lines[i] + ' EDIT1'] + lines[i + 1:]
    ed2 = lines[:i] + [lines[i] + ' EDIT2'] + lines[i + 1:]
    cases = {'1 different appends': (lines + [a], lines + [b]),
             '2 same append': (lines + [a], lines + [a]),
             '3 edit vs append': (ed1, lines + [b]),
             '4 edit vs edit': (ed1, ed2)}
    for name, (o, t) in cases.items():
        for tool in ('union', 'keyed'):
            fo, fb, ft = [os.path.join(out, 'x.%s' % s) for s in ('ours', 'base', 'theirs')]
            open(fo, 'w', newline='\n').write('\n'.join(o) + '\n')
            open(fb, 'w', newline='\n').write(base)
            open(ft, 'w', newline='\n').write('\n'.join(t) + '\n')
            if tool == 'union':
                r = subprocess.run(['git', 'merge-file', '--union', fo, fb, ft], capture_output=True, text=True)
            else:
                r = subprocess.run([sys.executable, 're/tools/tracker_merge3.py', 'allow', fo, fb, ft], capture_output=True, text=True)
            res = open(fo).read()
            print('%-40s %-26s %-6s exit %d, dup keys %s%s' % (os.path.basename(path), name, tool, r.returncode,
                  dups(res) or 'none', ', EDIT lines %d' % res.count(' EDIT')))


probe('re/match/PROVISIONAL.tsv', lambda s: '0x00ABC00%s\tFUN\tsrc\tflags\t1\tNOT-PROVEN\tx\tx\tx\tx\tprovisional\tE' % ('1' if s == 'A' else '2'), '0x00495870')
probe('mashedmod/exe_sources.rsp', lambda s: '"Match\\PortTop_00ABC00%s.cpp"' % ('1' if s == 'A' else '2'), '"Match\\PortTop_00426e10.cpp"')
probe('re/tools/dual_copy_allowlist.txt', lambda s: 'ORIGTOP-ONLY DUP-IN-TARGET:00abc00%s:a.cpp,b.cpp   # X' % ('1' if s == 'A' else '2'),
      'ORIGTOP-ONLY')
probe('re/tools/bind/state/_shared/rules.tsv', lambda s: 'prov_bind\t00abc00%s' % ('1' if s == 'A' else '2'), 'prov_bind')
