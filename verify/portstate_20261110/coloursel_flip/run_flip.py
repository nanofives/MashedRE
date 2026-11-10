"""run_flip.py [--no-gates] [--no-caps] -- PORT-COLOURSEL-ICON-FLIP evidence (L-112 (a)). Run inside test-slot + mashed_lock.py
run, from the worktree root. Gates on the flipped exe: H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 (+ H_NOCSIX, the opt-out stack).
12 captures OFF/ON/OFF/ON through verify/porttop_20261108/capflake_runs.py (spec OFF@0 = MASHED_ORIG_TOP unset, pin 0),
OFF = MASHED_NO_COLOURSEL_ICON=1 MASHED_NO_COLOURSEL_BORDER=1 in the env, ON = the new default."""
import os, subprocess, sys

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
D = 'D:/MashedWT/PORT-COLOURSEL-ICON-FLIP_data'
R = 'D:/MashedWT/PORT-COLOURSEL-ICON-FLIP_root'
EXE = D + '/exe/mashed_re.exe'
base = dict(os.environ, PYTHONWARNINGS='ignore', KNOB_EXE=EXE)
for k in ('MASHED_NO_COLOURSEL_ICON', 'MASHED_NO_COLOURSEL_BORDER', 'MASHED_COLOURSEL_ICON', 'MASHED_COLOURSEL_BORDER'):
    base.pop(k, None)
if '--no-gates' not in sys.argv:
    os.makedirs(D + '/arms', exist_ok=True)
    e = dict(base, A4_ROOT=R, A4_OUT=D + '/arms', A4_EXE=EXE)
    rc = subprocess.call(['py', '-3.12', 'verify/savelink_20261023/run_arms.py', 'H_DEF', 'B_DEF_K0', 'B_DEF_K1', 'B_DEF_K26',
                          'H_NOCSIX'], cwd=W, env=e)
    print('run_arms rc', rc, flush=True)
if '--no-caps' not in sys.argv:
    off = {'MASHED_NO_COLOURSEL_ICON': '1', 'MASHED_NO_COLOURSEL_BORDER': '1'}
    for n, side in enumerate(('off', 'on', 'off', 'on'), 1):
        arm = 'c%d_%s' % (n, side)
        e = dict(base, CAPFLAKE_DATA=D + '/caps', CAPFLAKE_ROOT=R, **(off if side == 'off' else {}))
        rc = subprocess.call(['py', '-3.12', 'verify/porttop_20261108/capflake_runs.py', arm, 'OFF@0'], cwd=W, env=e)
        print('[%d/4] caps %s rc %d' % (n, arm, rc), flush=True)
