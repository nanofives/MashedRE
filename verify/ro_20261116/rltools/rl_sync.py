"""PORT-OT-RACE: add the generated TUs (rltools/newfiles_rl.txt) to exe_sources.rsp and the lint's new ORIGTOP-ONLY keys
to the allowlist. Run from the worktree root. A copy of verify/portorigtop_20261113/tools/sync_rsp_allowlist.py with
this row's file list and allowlist text."""
import os, subprocess, sys
S = os.path.dirname(os.path.abspath(__file__))
p = 'mashedmod/exe_sources.rsp'
raw = open(p, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
t = raw.decode()
add = []
for f in open(os.path.join(S, 'newfiles_rl.txt')).read().split():
    e = '"Match\\%s"' % os.path.basename(f)
    if e not in t:
        add.append(e)
if add:
    if not t.endswith(nl): t += nl
    t += nl.join(add) + nl
    open(p, 'wb').write(t.encode())
print('rsp +', len(add))
seed = subprocess.run([sys.executable, 'scripts/lint_rva_bodies.py', '--seed'], capture_output=True, text=True).stdout
p = 're/tools/dual_copy_allowlist.txt'
raw = open(p, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
t = raw.decode()
have = set(l.split('#')[0].strip() for l in t.splitlines())
new, bad = [], []
for l in seed.splitlines():
    k = l.split('#')[0].strip()
    if not k or l.startswith('#') or k in have: continue
    if not k.startswith('ORIGTOP-ONLY '):
        bad.append(l); continue
    new.append(k + '   # PORT-OT-RACE (L-111 (a), L-126), 2026-11-16: the matched / replay-C4 body runs only on MASHED_ORIG_TOP + '
               'MASHED_OT_RACELOAD (race-load closure of 0x0040d270, verify/ro_20261116/rltools/rl_bind_gen.py); deleted at the ORIG_TOP flip. '
               'Evidence: verify/ro_20261116/RESULT_PORT-OT-RACE.md.')
if new:
    if not t.endswith(nl): t += nl
    t += nl.join(new) + nl
    open(p, 'wb').write(t.encode())
print('allowlist +', len(new))
for b in bad: print('UNTAGGABLE', b[:250])
