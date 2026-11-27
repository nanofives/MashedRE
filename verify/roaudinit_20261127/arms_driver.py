"""RO-AUDIOINIT-45D460-REACH-5 arms, run inside test-slot -> mashed_lock.py run:
1. knob-on boots vs_init_audio / ddk_init_audio (audiotick_stops_arms.py, REACH-3's knob set),
2. the RO arm RO_ROAUDINIT-pr1 on the object build (run_arms.py, stacks/ROAUDINIT.txt).
usage: py -3.12 arms_driver.py [knob] [ro]"""
import os, subprocess, sys
WT = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-AUDIOINIT-45D460-REACH-5'
STEP = 'RO-AUDIOINIT-45D460-REACH-5'
ROOT = 'D:/MashedWT/%s_root' % STEP
D = 'D:/MashedWT/%s_data' % STEP
want = set(sys.argv[1:]) or {'knob', 'ro'}
rc_all = 0
if 'knob' in want:
    e = dict(os.environ, ARMS_STEP=STEP, ARMS_ROOT=ROOT,
             ARMS_EXTRA='MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1 '
                        'MASHED_OT_AUDIOTHREAD_STANDIN=1 MASHED_ORIG_TOP_RWMOD=1')
    with open(os.path.join(D, 'arms_knob.out'), 'w') as f:
        rc = subprocess.call([sys.executable, 'verify/portorigtop_20261113/audiotick_stops_arms.py',
                              'vs_init_audio', 'ddk_init_audio'], cwd=WT, env=e, stdout=f, stderr=subprocess.STDOUT)
    print('[driver] knob rc=%d' % rc, flush=True)
    rc_all |= rc
if 'ro' in want:
    e = dict(os.environ, A4_EXE=os.path.join(WT, r'mashedmod\build\ddkobj\mashed_re_ddk.exe'), A4_ROOT=ROOT,
             A4_OUT=os.path.join(D, 'ro'))
    with open(os.path.join(D, 'roarm.out'), 'w') as f:
        rc = subprocess.call([sys.executable, '-W', 'ignore', 'verify/savelink_20261023/run_arms.py', 'RO_ROAUDINIT-pr1'],
                             cwd=WT, env=e, stdout=f, stderr=subprocess.STDOUT)
    print('[driver] ro rc=%d' % rc, flush=True)
    rc_all |= rc
sys.exit(rc_all)
