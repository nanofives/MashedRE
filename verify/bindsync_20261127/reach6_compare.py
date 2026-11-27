"""BIND-SYNC-REMOVE-STALE: in the scratch checkout of REACH-6 (fdee19028, detached), put back REACH-6's BASE state for the
hand-edited parts (before the hand edits), run the new binder, and show `git diff fdee19028` of the result.

  py -3.12 reach6_compare.py <scratch root> <new porttop_bind.py> <A|B>

Base inputs (A = the rebased branch, B = REACH-5's pre-rebase row naming the deleted TU, 37aca6283^):
  mashedmod/exe_sources.rsp, re/tools/dual_copy_allowlist.txt, Match/PortTop_004ccef0.cpp   from 6b7da6d8e
      (REACH-6's commit before 100ba0cc5 hand-removed the rsp line and the file; 6cb4697a2 later pruned the allowlist line)
  BINDINGS.tsv = fdee19028's with the 0x004ccef0 row put back to the row before the hand edit:
      A: 100ba0cc5's row (missing PortTop_AStop_004ccef0), B: 37aca6283^'s row (near-match PortTop_Bound_004ccef0 ...
         Match/PortTop_004ccef0.cpp)
Tool steps: sync --profile audio; bindings --profile audio --step RO-AUDIOINIT-45D460-REACH-6 --rva 004ccef0;
            bindings --profile audio (must be 0/0).
"""
import os, shutil, subprocess, sys

root, tool, case = sys.argv[1], os.path.abspath(sys.argv[2]), sys.argv[3]
os.chdir(root)
REF = 'fdee19028'
B = 'verify/porttop_20261108/BINDINGS.tsv'
TU = 'mashedmod/src/mashed_re/Match/PortTop_004ccef0.cpp'


def git(*a, raw=False):
    r = subprocess.run(['git'] + list(a), capture_output=True)
    if r.returncode:
        sys.exit('git %s: %s' % (' '.join(a), r.stderr.decode()))
    return r.stdout if raw else r.stdout.decode()


def show(rev, path):
    return git('show', '%s:%s' % (rev, path), raw=True)


# reset the scratch tree to REACH-6's tip, then the new tool on top
git('checkout', REF, '--', '.')
for f in git('ls-files', '--others', '--exclude-standard').split():
    if f.startswith('mashedmod/src/mashed_re/Match/'):
        os.remove(f)
shutil.copyfile(tool, 're/tools/bind/porttop_bind.py')

# base state of the hand-edited parts
for p in ('mashedmod/exe_sources.rsp', 're/tools/dual_copy_allowlist.txt', TU):
    open(p, 'wb').write(show('6b7da6d8e', p))
row_rev = {'A': '100ba0cc5', 'B': '37aca6283^'}[case]
base_row = [l for l in show(row_rev, B).split(b'\n') if l.startswith(b'0x004ccef0\t')][0]
lines = show(REF, B).split(b'\n')
lines = [base_row if l.startswith(b'0x004ccef0\t') else l for l in lines]
open(B, 'wb').write(b'\n'.join(lines))
print('base 0x004ccef0 row:', base_row.decode().rstrip('\r'))
print('base diff vs %s (the hand edits undone):' % REF)
print(git('diff', '--stat', REF, '--', '.', ':!re/tools/bind/porttop_bind.py'))

for cmd in (['sync', '--profile', 'audio'],
            ['bindings', '--profile', 'audio', '--step', 'RO-AUDIOINIT-45D460-REACH-6', '--rva', '004ccef0'],
            ['bindings', '--profile', 'audio']):
    r = subprocess.run([sys.executable, 're/tools/bind/porttop_bind.py'] + cmd, capture_output=True, text=True)
    print('$ porttop_bind.py', ' '.join(cmd))
    print(r.stdout.strip(), r.stderr.strip())

d = git('diff', REF, '--', '.', ':!re/tools/bind/porttop_bind.py')
st = [l for l in git('status', '--short', '--', '.').splitlines() if 're/tools/bind/porttop_bind.py' not in l]
print('git diff %s after the tool (porttop_bind.py excluded): %d bytes' % (REF, len(d)))
print(d)
print('git status (porttop_bind.py excluded): %d lines' % len(st))
print('\n'.join(st))
# status 'M' under core.autocrlf=true can be line endings only: compare the raw working bytes with the committed blob
raw_same = True
for l in st:
    f = l[3:].strip()
    if l.startswith(' D') or not os.path.exists(f):
        print('  %s: missing in the working tree' % f); raw_same = False; continue
    w = git('hash-object', '--no-filters', f).strip()
    b = git('rev-parse', '%s:%s' % (REF, f)).strip()
    print('  %s: raw working bytes %s, %s blob %s -> %s' % (f, w, REF, b, 'SAME' if w == b else 'DIFFERENT'))
    raw_same &= w == b
gone = not os.path.exists(TU)
print('%s absent after the tool: %s (absent in %s too)' % (TU, gone, REF))
print('RESULT %s: %s' % (case, 'IDENTICAL byte for byte' if not d and raw_same and gone else 'DIFFERENT'))
