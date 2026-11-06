"""MATCH-T13-SEED port-side seed arms (PREREG verify/t13seed_20261102/PREREG_MATCH-T13-SEED.md).
usage: py -3.12 p_arms.py NAME ...   NAME = P_{OFF|ON}_{SANDS|CITY|NEU|TRAIN}
Base env = verify/savelink_20261023/run_arms.py base_env, plus MASHED_ROUND_RULE=10, the track, the seed log.
Explicit PID only."""
import hashlib, os, shutil, subprocess, sys, time
D = 'D:/MashedWT/MATCH-T13-SEED_data'
EXE = os.environ.get('A4_EXE', D + '/exe_g1/mashed_re.exe')
ROOT = os.path.normpath('D:/MashedWT/MATCH-T13-SEED_root')
OUT = os.path.join(D, 'parms'); os.makedirs(OUT, exist_ok=True)
TRK = {'SANDS': 'sands', 'CITY': 'City', 'NEU': 'Neustein', 'TRAIN': 'training'}
FRAMES = os.environ.get('P_FRAMES', '2400')


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8]


for name in sys.argv[1:]:
    _, knob, trk = name.split('_')
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_MUTE='1', MASHED_TRACK_VIEW=TRK[trk], MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='10', MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=FRAMES, MASHED_RULE10_SEED_LOG='1')
    if knob == 'ON':
        e['MASHED_RULE10_SEED_DIFF'] = '1'
    log = os.path.join(ROOT, 'mashed_re.log')
    if os.path.exists(log): os.remove(log)
    t0 = time.time()
    p = subprocess.Popen([EXE], cwd=ROOT, env=e)
    print(f'=== {name} pid={p.pid} exe={sha8(EXE)}', flush=True)
    try:
        rc = p.wait(600)
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait()
    dst = os.path.join(OUT, name + '.log')
    if os.path.exists(log): shutil.copy2(log, dst)
    print(f'    exit={rc} secs={time.time() - t0:.0f}', flush=True)
    if os.path.exists(dst):
        for l in open(dst, encoding='latin-1'):
            if l.startswith('[rule10-seed]'):
                print('    ' + l.rstrip(), flush=True)
