"""PORT-TOP-RENDERFRAME-492E90 knob boots (arms of verify/rwcframemovie_20261116/run_arms.sh, this row's root and data
folder, plus the sub-knob arms). Call inside test-slot -> mashed_lock.py run. knob_boot.py kills only the PID it spawns.
  off      MASHED_ORIG_TOP=1 (hand RenderFrame; the sub-knob has no effect off the RW core frame route)
  frameT   + MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=100000 MASHED_OT_TIMER=1   (sub-knob unset)
  rfT      frameT + MASHED_OT_RENDERFRAME=1   (the direct callees of 0x00492e90 call the walked closure)
  rfoff    off + MASHED_OT_RENDERFRAME=1      (must equal off: the knob only acts on the RW core frame route)
prefix vs_ / ddk_ picks the exe. Usage (worktree root): py -3.12 verify/porttoprenderframe_20261121/tools/run_arms.py vs_off ...
"""
import os, subprocess, sys

DATA = 'D:/MashedWT/PORT-TOP-RENDERFRAME-492E90_data'
ROOT = 'D:/MashedWT/PORT-TOP-RENDERFRAME-492E90_root'
COMMON = ['MASHED_ORIG_TOP=1', 'MASHED_WIN_POS=primary-bl', 'MASHED_DET_FRAMES=600']
FRAME = ['MASHED_RWC_CORE=1', 'MASHED_RWC_FRAME=1', 'MASHED_RWC_FRAME_N=100000', 'MASHED_OT_TIMER=1']
ARMS = {'off': (180, []), 'frameT': (240, FRAME), 'rfT': (240, FRAME + ['MASHED_OT_RENDERFRAME=1']),
        'rfoff': (180, ['MASHED_OT_RENDERFRAME=1'])}
rc = 0
for a in sys.argv[1:]:
    b, k = a.split('_', 1)
    tmo, extra = ARMS[k]
    env = dict(os.environ, PORTTOP_OUT=DATA)
    if b == 'ddk':
        env['KNOB_EXE'] = os.path.abspath('mashedmod/build/ddkobj/mashed_re_ddk.exe')
    r = subprocess.run(['py', '-3.12', 'verify/porttop_20261108/knob_boot.py', a, ROOT, str(tmo)] + COMMON + extra, env=env)
    print('ARM', a, 'knob_boot rc', r.returncode, flush=True)
    rc |= r.returncode != 0
sys.exit(0)
