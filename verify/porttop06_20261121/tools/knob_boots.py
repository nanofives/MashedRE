"""PORT-TOP-06 knob boots: four runs of verify/porttop_20261108/knob_boot.py (VS2022 exe and object-build exe, sub-knob
MASHED_OT_RACEFRAME off and on), each wrapped as test-slot (outside) + mashed_lock.py run (inside). Exes are the copies
kept in the data dir (sha256 in sha_vs5.txt / sha_ddk2.txt). Run from the worktree root."""
import os, subprocess, sys

DATA = 'D:/MashedWT/PORT-TOP-06_data'
ROOT = 'D:/MashedWT/PORT-TOP-06_root'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
BASE = ['MASHED_ORIG_TOP=1', 'MASHED_OT_RACELOAD=1', 'MASHED_DET_FRAMES=3000']
runs = [('knob_vs_off', 'mashed_re_vs5.exe', []), ('knob_vs_on', 'mashed_re_vs5.exe', ['MASHED_OT_RACEFRAME=1']),
        ('knob_ddk_off', 'mashed_re_ddk2.exe', []), ('knob_ddk_on', 'mashed_re_ddk2.exe', ['MASHED_OT_RACEFRAME=1'])]
sel = sys.argv[1:] or [r[0] for r in runs]
for i, (label, exe, extra) in enumerate([r for r in runs if r[0] in sel], 1):
    env = dict(os.environ, KNOB_EXE='%s/%s' % (DATA, exe), PORTTOP_OUT=DATA)
    cmd = ['py', '-3.12', SLOT, 'py', '-3.12', LOCK, 'run', '--label', 'PORT-TOP-06', '--',
           'py', '-3.12', 'verify/porttop_20261108/knob_boot.py', label, ROOT, '900'] + BASE + extra
    print('[%d/%d] %s %s %s' % (i, len(sel), label, exe, ' '.join(BASE + extra)), flush=True)
    r = subprocess.run(cmd, env=env, capture_output=True, text=True, errors='replace')
    open('%s/%s.out' % (DATA, label), 'w', encoding='utf-8').write(r.stdout + r.stderr)
    print('[%d/%d] %s rc=%d' % (i, len(sel), label, r.returncode), flush=True)
    for l in (r.stdout + r.stderr).splitlines():
        if any(k in l for k in ('rc=', 'self-test', 'PORT-TOP-06', 'returned', 'stop', 'FATAL', 'test-slot', 'test-hub')):
            print('    ' + l[:260], flush=True)
