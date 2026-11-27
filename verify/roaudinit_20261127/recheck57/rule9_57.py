"""RULE 9 on the rebased tree (run 57 recheck): match.py --bytes and relocheck.py on every re/match source that an exe TU
changed by this branch (git diff d20fd3984 HEAD -- mashedmod/src/mashed_re/Match) includes.
usage: py -3.12 rule9_57.py > rule9_57.txt"""
import re, subprocess, sys, pathlib
WT = pathlib.Path(__file__).resolve().parents[3]
names = subprocess.run(['git', 'diff', '--name-only', 'd20fd3984', 'HEAD', '--', 'mashedmod/src/mashed_re/Match'],
                       cwd=WT, capture_output=True, text=True).stdout.split()
srcs = []
for n in names:
    p = WT / n
    if not p.exists():
        continue
    for s in sorted(set(re.findall(r're/match/[\w/]+\.c', p.read_text(errors='replace')))):
        if s not in srcs:
            srcs.append(s)
for s in srcs:
    m = subprocess.run([sys.executable, 're/tools/match/match.py', s, '--bytes'], cwd=WT, capture_output=True, text=True)
    bl = [l.strip() for l in (m.stdout + m.stderr).splitlines() if 'bytes:' in l]
    r = subprocess.run([sys.executable, 're/tools/match/relocheck.py', s], cwd=WT, capture_output=True, text=True)
    rl = [l.strip() for l in (r.stdout + r.stderr).splitlines() if l.startswith('0x')]
    print('%s | %s | %s' % (s, bl[-1] if bl else 'NO BYTES LINE rc=%d' % m.returncode,
                            rl[-1] if rl else 'NO RELOCHECK LINE rc=%d' % r.returncode), flush=True)
