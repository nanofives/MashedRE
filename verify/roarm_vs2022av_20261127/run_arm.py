"""RO-ARM-VS2022-AV: one knob-off arm = test-slot (outside) + mashed_lock.py run (inside) + run_arms.py <arm>.
usage: py -3.12 verify/roarm_vs2022av_20261127/run_arm.py <exe-name-in-data-dir> <arm>   (worktree root)"""
import os, subprocess, sys
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
D = 'D:/MashedWT/RO-ARM-VS2022-AV_data'
exe, arm = sys.argv[1], sys.argv[2]
env = dict(os.environ, A4_OUT=D + '/arms_' + exe[:-4], A4_ROOT='D:/MashedWT/RO-ARM-VS2022-AV_root', A4_EXE=D + '/' + exe, A4_RO_CKPT='1')
env.pop('A4_PLAN', None)
cmd = ['py', '-3.12', SLOT, 'py', '-3.12', LOCK, 'run', '--label', 'RO-ARM-VS2022-AV', '--', 'py', '-3.12', 'verify/savelink_20261023/run_arms.py', arm]
r = subprocess.run(cmd, env=env, capture_output=True, text=True, errors='replace')
open('%s/%s.%s.arms.out' % (D, exe[:-4], arm), 'w', encoding='utf-8').write(r.stdout + r.stderr)
print(exe, arm, 'rc=%d' % r.returncode, flush=True)
