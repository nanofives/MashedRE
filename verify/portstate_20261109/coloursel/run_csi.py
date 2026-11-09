"""run_csi.py [--no-gates] -- PORT-COLOURSEL-ICON arms 2-4 (PREREG_COLOURSEL_ICON.md section 4). Run inside test-slot +
mashed_lock.py run. Gates: run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_SSM B_SSM_K0 B_SSM_K1 B_SSM_K26. Then four nav-demo
walks (MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1, PORT-TOP-04D's settings baseline restored before each, PORT-TOP-04D's
knob_boot.py): hand OFF, hand ON, matched (MASHED_ORIG_TOP=1 MASHED_TOP04D_DATAZERO=1) OFF, matched ON."""
import os, shutil, subprocess, sys

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
D = 'D:/MashedWT/PORT-COLOURSEL-ICON_data'
R = 'D:/MashedWT/PORT-COLOURSEL-ICON_root'
BASE = 'D:/MashedWT/PORT-TOP-04D_data/settings_baseline.bin'
env = dict(os.environ, PORTTOP_OUT=D, PYTHONWARNINGS='ignore')
if '--no-gates' not in sys.argv:
    os.makedirs(D + '/arms', exist_ok=True)
    e = dict(env, A4_ROOT=R, A4_OUT=D + '/arms')
    rc = subprocess.call(['py', '-3.12', 'verify/savelink_20261023/run_arms.py', 'H_DEF', 'B_DEF_K0', 'B_DEF_K1', 'B_DEF_K26',
                          'H_CSI', 'B_CSI_K0', 'B_CSI_K1', 'B_CSI_K26'], cwd=W, env=e)
    print('run_arms rc', rc, flush=True)
for spec in (['walk_hand_off'], ['walk_hand_on', 'MASHED_COLOURSEL_ICON=1'],
             ['walk_matched_off', 'MASHED_ORIG_TOP=1', 'MASHED_TOP04D_DATAZERO=1'],
             ['walk_matched_on', 'MASHED_ORIG_TOP=1', 'MASHED_TOP04D_DATAZERO=1', 'MASHED_COLOURSEL_ICON=1']):
    lab = spec[0]
    shutil.copyfile(BASE, R + '/mashed_re_settings.bin')
    out = D + '/' + lab
    os.makedirs(out, exist_ok=True)
    subprocess.call(['py', '-3.12', 'verify/porttop_20261108/knob_boot.py', lab, R, '300'] + spec[1:] +
                    ['MASHED_NAV_DEMO=1', 'MASHED_DETERMINISTIC=1', 'MASHED_VERIFY_OUT=' + out], cwd=W, env=env)
