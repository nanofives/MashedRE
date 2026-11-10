"""run_lr.py [--no-gates] -- PORT-COLOURSEL-LEFTRIGHT arms 2-6 (PREREG_COLOURSEL_LEFTRIGHT.md section 3). Run inside test-slot
+ mashed_lock.py run, from the worktree root. Gates: H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_CSIX B_CSIX_K0. Walks (nav demo, hand
path, PORT-TOP-04D settings baseline): default, ICON, BORDER, ICON+BORDER. Draw stream: the ICON+BORDER walk with
MASHED_DBG_DRAWSTREAM=250:300 (colour select = frames 268..295). Left/right: MASHED_NAV_KEYS e,e,e,r and e,e,e,r,r,r,r,r,r,l
with ICON+BORDER, MASHED_DET_FRAMES=700, draw stream 690:692 (the icon quad's x gives the choice)."""
import os, shutil, subprocess, sys

W = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
D = 'D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_data'
R = 'D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_root'
BASE = 'D:/MashedWT/PORT-TOP-04D_data/settings_baseline.bin'
env = dict(os.environ, PORTTOP_OUT=D, PYTHONWARNINGS='ignore')
if '--no-gates' not in sys.argv:
    os.makedirs(D + '/arms', exist_ok=True)
    e = dict(env, A4_ROOT=R, A4_OUT=D + '/arms')
    rc = subprocess.call(['py', '-3.12', 'verify/savelink_20261023/run_arms.py', 'H_DEF', 'B_DEF_K0', 'B_DEF_K1', 'B_DEF_K26',
                          'H_CSIX', 'B_CSIX_K0'], cwd=W, env=e)
    print('run_arms rc', rc, flush=True)
ICON, BORD = 'MASHED_COLOURSEL_ICON=1', 'MASHED_COLOURSEL_BORDER=1'
runs = [['walk_def'], ['walk_icon', ICON], ['walk_border', BORD], ['walk_both', ICON, BORD],
        ['stream_both', ICON, BORD, 'MASHED_DBG_DRAWSTREAM=250:300', 'MASHED_DBG_DRAWSTREAM_OUT=' + D + '/stream_both.json']]
for spec in runs:
    lab = spec[0]
    shutil.copyfile(BASE, R + '/mashed_re_settings.bin')
    out = D + '/' + lab
    os.makedirs(out, exist_ok=True)
    subprocess.call(['py', '-3.12', 'verify/porttop_20261108/knob_boot.py', lab, R, '300'] + spec[1:] +
                    ['MASHED_NAV_DEMO=1', 'MASHED_DETERMINISTIC=1', 'MASHED_VERIFY_OUT=' + out], cwd=W, env=env)
for lab, keys in (('keys_r1', 'e,e,e,r'), ('keys_r6l', 'e,e,e,r,r,r,r,r,r,l')):
    shutil.copyfile(BASE, R + '/mashed_re_settings.bin')
    subprocess.call(['py', '-3.12', 'verify/porttop_20261108/knob_boot.py', lab, R, '300', ICON, BORD,
                     'MASHED_NAV_KEYS=' + keys, 'MASHED_DETERMINISTIC=1', 'MASHED_DET_FRAMES=700',
                     'MASHED_DBG_DRAWSTREAM=690:692', 'MASHED_DBG_DRAWSTREAM_OUT=' + D + '/' + lab + '.json'], cwd=W, env=env)
