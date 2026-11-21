"""RACE-BIND-C4-BULK: one regeneration cycle for a checkpoint.
usage (worktree root): py -3.12 verify/bindbulk_20261121/tools/regen.py <roots.tsv> [--keep rsp_base]
1. delete the wrappers of the previous run (tools/newfiles_cp.txt), restore exe_sources.rsp and dual_copy_allowlist.txt
   to the committed state EXCEPT for the lines kept by earlier checkpoints (they are committed, so git checkout is exact)
2. bulk_bind_gen.py <roots> --write
3. append the new wrappers + the stops file to exe_sources.rsp
4. seed dual_copy_allowlist.txt with the new ORIGTOP-ONLY findings (citation line)
5. run the lint.
The stops file name is PortTop_BulkStops.cpp for checkpoint 1; checkpoint n uses --stops-suffix (see bulk_bind_gen.py)."""
import os, subprocess, sys
T = 'verify/bindbulk_20261121/tools'
roots_tsv = sys.argv[1]
roots = [l.split('\t')[0] for l in open(roots_tsv, encoding='utf-8').read().splitlines()[1:] if l.startswith('0x')]
nf = os.path.join(T, 'newfiles_cp.txt')
if os.path.exists(nf):
    for f in open(nf).read().split('\n'):
        if f.strip() and os.path.exists(f):
            os.remove(f)
for f in ('mashedmod/exe_sources.rsp', 're/tools/dual_copy_allowlist.txt'):
    subprocess.run(['git', 'checkout', '--', f], check=True)
env = dict(os.environ, PYTHONIOENCODING='utf-8')
r = subprocess.run([sys.executable, os.path.join(T, 'bulk_bind_gen.py'), os.getcwd(), ','.join(roots), '--write'],
                   capture_output=True, text=True, encoding='utf-8', env=env)
open(os.path.join(T, 'last_plan.txt'), 'w', encoding='utf-8').write(r.stdout)
print(r.stdout.splitlines()[0]); print(r.stdout.splitlines()[-1]); print(r.stderr[-500:])
subprocess.run([sys.executable, os.path.join(T, 'refs_gen.py')], check=True)
open(nf, 'a').write('mashedmod/src/mashed_re/Match/PortTop_BulkRefs.cpp\n')
files = [l.strip() for l in open(nf).read().split('\n') if l.strip()]
raw = open('mashedmod/exe_sources.rsp', 'rb').read().decode('utf-8')
nl = '\r\n'
if not raw.endswith(nl): raw += nl
raw += nl.join('"%s"' % f.replace('mashedmod/src/mashed_re/', '').replace('/', '\\') for f in files) + nl
open('mashedmod/exe_sources.rsp', 'wb').write(raw.encode('utf-8'))
have = set(l.split('#')[0].strip() for l in open('re/tools/dual_copy_allowlist.txt', encoding='utf-8'))
seed = subprocess.run([sys.executable, 'scripts/lint_rva_bodies.py', '--seed'], capture_output=True, text=True, encoding='utf-8', env=env).stdout
cite = ('   # RACE-BIND-C4-BULK (L-111 (a), L-126), 2026-11-21: the matched C4 body runs only on MASHED_ORIG_TOP + MASHED_OT_RACELOAD '
        '(race-load census, verify/bindbulk_20261121/tools/bulk_bind_gen.py); deleted at the ORIG_TOP flip. '
        'Evidence: verify/bindbulk_20261121/RESULT_RACE-BIND-C4-BULK.md.')
new = []
for l in seed.splitlines():
    k = l.split('#')[0].strip()
    if (k.startswith('ORIGTOP-ONLY ') or k.startswith('DUP-') or k.startswith('CROSS-')) and k not in have:
        new.append(k + cite)
raw = open('re/tools/dual_copy_allowlist.txt', 'rb').read().decode('utf-8')
nl = '\r\n' if '\r\n' in raw else '\n'
if not raw.endswith(nl): raw += nl
raw += nl.join(new) + nl
open('re/tools/dual_copy_allowlist.txt', 'wb').write(raw.encode('utf-8'))
print('allowlist +', len(new))
out = subprocess.run([sys.executable, 'scripts/lint_rva_bodies.py'], capture_output=True, text=True, encoding='utf-8', env=env)
print('\n'.join(out.stdout.splitlines()[-3:]))
