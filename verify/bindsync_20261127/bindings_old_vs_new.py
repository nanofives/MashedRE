"""BIND-SYNC-REMOVE-STALE: for every profile, the BINDINGS.tsv the head's binder (287c10369, copied to
re/tools/bind/_old_pb.py) writes and the one this branch's binder writes, from the same tree. The file is restored
after each run.   py -3.12 verify/bindsync_20261127/bindings_old_vs_new.py   (from the worktree root)"""
import subprocess, sys

B = 'verify/porttop_20261108/BINDINGS.tsv'
PROFILES = ('audio', 'bulk', 'gates', 'raceframe', 'raceload', 'renderframe')
base = open(B, 'rb').read()
bad = 0
for i, p in enumerate(PROFILES, 1):
    out = {}
    for name, tool in (('old', 're/tools/bind/_old_pb.py'), ('new', 're/tools/bind/porttop_bind.py')):
        try:
            r = subprocess.run([sys.executable, tool, 'bindings', '--profile', p], capture_output=True, text=True)
            out[name] = (open(B, 'rb').read(), r.stdout.strip())
        finally:
            open(B, 'wb').write(base)
    same = out['old'][0] == out['new'][0]
    bad += not same
    print('[%d/%d] %s old: %s | new: %s | BINDINGS.tsv written %s' % (i, len(PROFILES), p, out['old'][1], out['new'][1],
                                                                  'IDENTICAL' if same else 'DIFFERENT'))
print('bindings old vs new: %s' % ('PASS' if not bad else 'FAIL %d' % bad))
sys.exit(1 if bad else 0)
