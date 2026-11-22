"""KNOBON-BOOTS-G5: each replay arm = test-slot (outside) + mashed_lock.py run (inside) + verify/savelink_20261023/run_arms.py <arm>.
usage: py -3.12 verify/knobonboots_20261121/run_arms_wrapped.py <arm> [<arm> ...]   (worktree root)"""
import os, subprocess, sys
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
D = 'D:/MashedWT/KNOBON-BOOTS-G5_data'
env = dict(os.environ, A4_OUT=D + '/arms', A4_ROOT='D:/MashedWT/KNOBON-BOOTS-G5_root', A4_EXE=D + '/' + os.environ.get('G5_EXE', 'mashed_re_vs.exe'), A4_RO_CKPT='1')
env.pop('A4_PLAN', None)
for i, arm in enumerate(sys.argv[1:], 1):
    cmd = ['py', '-3.12', SLOT, 'py', '-3.12', LOCK, 'run', '--label', 'KNOBON-BOOTS-G5', '--', 'py', '-3.12', 'verify/savelink_20261023/run_arms.py', arm]
    print('[%d/%d] %s' % (i, len(sys.argv) - 1, arm), flush=True)
    r = subprocess.run(cmd, env=env, capture_output=True, text=True, errors='replace')
    open('%s/%s.%s.arms.out' % (D, os.environ.get('G5_EXE', 'mashed_re_vs.exe')[:-4], arm), 'w', encoding='utf-8').write(r.stdout + r.stderr)
    print('[%d/%d] %s rc=%d' % (i, len(sys.argv) - 1, arm, r.returncode), flush=True)
