"""KNOBON-BOOTS-G5: knob boots of the VS2022 exe, each = test-slot (outside) + mashed_lock.py run (inside) + verify/porttop_20261108/knob_boot.py.
usage: py -3.12 verify/knobonboots_20261121/run_boots.py [label ...]   (run from the worktree root)"""
import os, subprocess, sys
DATA = 'D:/MashedWT/KNOBON-BOOTS-G5_data'
ROOT = 'D:/MashedWT/KNOBON-BOOTS-G5_root'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
BASE = ['MASHED_ORIG_TOP=1', 'MASHED_OT_RACELOAD=1', 'MASHED_DET_FRAMES=3000']
CORE = ['MASHED_RWC_CORE=1']
runs = {
 'rl_d110': ['MASHED_OT_RL_D110=1'],
 'rl_d110_count': ['MASHED_OT_RL_D110=1', 'MASHED_OT_RL_STOPS=count'],
 'titletick': ['MASHED_OT_TITLETICK=1'],
 'titletick_count': ['MASHED_OT_TITLETICK=1', 'MASHED_OT_RL_STOPS=count'],
 'raceframe_rwcframe': ['MASHED_OT_RACEFRAME=1'] + CORE + ['MASHED_RWC_FRAME=1', 'MASHED_RWC_FRAME_N=100000'],
 'raceframe_norwcframe': ['MASHED_OT_RACEFRAME=1'] + CORE,
 'raceframe_rwcframe_count': ['MASHED_OT_RACEFRAME=1'] + CORE + ['MASHED_RWC_FRAME=1', 'MASHED_RWC_FRAME_N=100000', 'MASHED_OT_RL_STOPS=count'],
 'raceframe_norwcframe_count': ['MASHED_OT_RACEFRAME=1'] + CORE + ['MASHED_OT_RL_STOPS=count'],
}
sel = sys.argv[1:] or list(runs)
for i, label in enumerate(sel, 1):
    extra = runs[label]
    env = dict(os.environ, KNOB_EXE=DATA + '/mashed_re_vs.exe', PORTTOP_OUT=DATA)
    cmd = ['py', '-3.12', SLOT, 'py', '-3.12', LOCK, 'run', '--label', 'KNOBON-BOOTS-G5', '--',
           'py', '-3.12', 'verify/porttop_20261108/knob_boot.py', label, ROOT, '900'] + BASE + extra
    print('[%d/%d] %s %s' % (i, len(sel), label, ' '.join(BASE + extra)), flush=True)
    r = subprocess.run(cmd, env=env, capture_output=True, text=True, errors='replace')
    open('%s/%s.out' % (DATA, label), 'w', encoding='utf-8').write(r.stdout + r.stderr)
    print('[%d/%d] %s rc=%d' % (i, len(sel), label, r.returncode), flush=True)
    for l in (r.stdout + r.stderr).splitlines():
        if 'rc=' in l or 'test-hub' in l:
            print('    ' + l[:260], flush=True)
