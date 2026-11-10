"""run_csb.py [--no-gates] -- PORT-COLOURSEL-BAR-Y arms 2-4 (PREREG_COLOURSEL_BAR_Y.md section 3). Run inside test-slot +
mashed_lock.py run. Gates: H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_CSB B_CSB_K0 B_CSB_K1 B_CSB_K26 H_CSIB B_CSIB_K0. Then four
nav-demo walks on the hand path (PORT-TOP-04D's settings baseline restored before each, its knob_boot.py): OFF, BAR_Y ON,
ICON ON, both ON."""
import os, shutil, subprocess, sys

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
D = 'D:/MashedWT/PORT-COLOURSEL-BAR-Y_data'
R = 'D:/MashedWT/PORT-COLOURSEL-BAR-Y_root'
BASE = 'D:/MashedWT/PORT-TOP-04D_data/settings_baseline.bin'
env = dict(os.environ, PORTTOP_OUT=D, PYTHONWARNINGS='ignore')
if '--no-gates' not in sys.argv:
    os.makedirs(D + '/arms', exist_ok=True)
    e = dict(env, A4_ROOT=R, A4_OUT=D + '/arms')
    rc = subprocess.call(['py', '-3.12', 'verify/savelink_20261023/run_arms.py', 'H_DEF', 'B_DEF_K0', 'B_DEF_K1', 'B_DEF_K26',
                          'H_CSB', 'B_CSB_K0', 'B_CSB_K1', 'B_CSB_K26', 'H_CSIB', 'B_CSIB_K0'], cwd=W, env=e)
    print('run_arms rc', rc, flush=True)
for spec in (['walk_off'], ['walk_bar', 'MASHED_COLOURSEL_BAR_Y=1'], ['walk_icon', 'MASHED_COLOURSEL_ICON=1'],
             ['walk_both', 'MASHED_COLOURSEL_BAR_Y=1', 'MASHED_COLOURSEL_ICON=1']):
    lab = spec[0]
    shutil.copyfile(BASE, R + '/mashed_re_settings.bin')
    out = D + '/' + lab
    os.makedirs(out, exist_ok=True)
    subprocess.call(['py', '-3.12', 'verify/porttop_20261108/knob_boot.py', lab, R, '300'] + spec[1:] +
                    ['MASHED_NAV_DEMO=1', 'MASHED_DETERMINISTIC=1', 'MASHED_VERIFY_OUT=' + out], cwd=W, env=env)
